%define api.pure full
%lex-param { void * scanner }
%parse-param { void * scanner }{ NCBI::Logging::AWS_LogLines * lib }

%define parse.trace
%define parse.error verbose

%name-prefix "aws_"

%{
#define YYDEBUG 1

#include <stdint.h>

#include "log_lines.hpp"
#include "aws_parser.hpp"
#include "aws_scanner.hpp"
#include "helper.hpp"

using namespace std;
using namespace NCBI::Logging;

void aws_error( yyscan_t locp, NCBI::Logging::AWS_LogLines * lib, const char* msg );

%}

%code requires
{
#include "types.h"
#include "log_lines.hpp"

extern void aws_get_scanner_input( void * yyscanner, t_str & str );
extern void aws_start_URL( void * yyscanner );
extern void aws_start_UserAgent( void * yyscanner );
extern void aws_pop_state( void * yyscanner );

using namespace NCBI::Logging;
}

%union
{
    t_str s;
    t_timepoint tp;
    t_request req;
    t_agent agent;
}

%token<s> STR STR1 MONTH IPV4 IPV6 METHOD VERS QSTR DASH I64 AMPERSAND EQUAL PERCENT SLASH QMARK
%token<s> PATHSTR PATHEXT ACCESSION SPACE 
%token COLON QUOTE OB CB 
%token UNRECOGNIZED
%token<s> OS SRA_TOOLKIT LIBCVERSION AGENTSTR SRATOOLVERS PHIDVALUE

%type<tp> time
%type<s> ip referer method qstr_list 
%type<s> aws_owner aws_bucket aws_requester aws_request_id aws_operation aws_error
%type<s> aws_version_id aws_host_id aws_cipher aws_auth aws_host_hdr aws_tls_vers
%type<s> result_code aws_bytes_sent aws_obj_size aws_total_time aws_turnaround_time
%type<req> request aws_key url_token url_list url key_token
%type<agent> agent vdb_agent vdb_agent_token

%start line

%%

line
    : log_aws       { YYACCEPT; }
    | log_aws_err   { YYACCEPT; }
    ;

log_aws
    : aws_owner SPACE
      aws_bucket SPACE
      time SPACE
      ip SPACE
      aws_requester SPACE
      aws_request_id SPACE
      aws_operation SPACE
      { aws_start_URL( scanner ); } aws_key SPACE
      request SPACE
      result_code SPACE
      aws_error SPACE
      aws_bytes_sent SPACE
      aws_obj_size SPACE
      aws_total_time SPACE
      aws_turnaround_time SPACE
      referer 
      { aws_start_UserAgent( scanner ); } 
      SPACE agent 
      { aws_pop_state( scanner ); }
      SPACE
      aws_version_id SPACE
      aws_host_id SPACE
      aws_cipher SPACE
      aws_auth SPACE
      aws_host_hdr SPACE
      aws_tls_vers
    {
        LogAWSEvent ev;
        ev . owner = $1;
        ev . bucket = $3;
        ev . time = $5;
        ev . ip = $7;
        ev . requester = $9;
        ev . request_id = $11;
        ev . operation = $13;

        ev . request = $18; // this has to happen before we use the key-decomposition
        // combine data from aws_key and request
        ev . key = $16 . path;
        if ( $16 . accession . n > 0 )
        {
            ev . request . accession = $16 . accession;
            ev . request . filename  = $16 . filename;
            ev . request . extension = $16 . extension;
        }

        ev . res_code           = $20;
        ev . error              = $22;
        ev . res_len            = $24;
        ev . obj_size           = $26;
        ev . total_time         = $28;
        ev . turnaround_time    = $30;
        ev . referer            = $32;
        ev . agent              = $35;
        ev . version_id         = $38;
        ev . host_id            = $40;
        ev . cipher_suite       = $42;
        ev . auth_type          = $44;
        ev . host_header        = $46;
        ev . tls_version        = $48;
        
        lib -> acceptLine( ev );
    }
    ;

log_aws_err
    : aws_owner error
    {
        LogAWSEvent ev;
        ev . owner = $1;
        aws_get_scanner_input( scanner, ev . unparsed );
        lib -> rejectLine( ev );
    }
    ;

aws_owner
    : STR
    | STR1
    ;

aws_bucket
    : STR
    ;

aws_requester
    : STR
    | STR1
    | DASH  { EMPTY_TSTR($$); }
    ;

aws_request_id
    : STR
    | STR1
    | DASH  { EMPTY_TSTR($$); }
    ;

aws_operation
    : STR
    | STR1
    | DASH  { EMPTY_TSTR($$); }
    ;

key_token
    : SLASH         { InitRequest( $$ ); $$ . path = $1; }
    | EQUAL         { InitRequest( $$ ); $$ . path = $1; }
    | AMPERSAND     { InitRequest( $$ ); $$ . path = $1; }
    | QMARK         { InitRequest( $$ ); $$ . path = $1; }
    | PERCENT       { InitRequest( $$ ); $$ . path = $1; }
    | ACCESSION     { InitRequest( $$ ); $$ . path = $1; $$ . accession = $1; }
    | PATHSTR       { InitRequest( $$ ); $$ . path = $1; $$ . filename = $1; }
    | PATHEXT       { InitRequest( $$ ); $$ . path = $1; $$ . extension = $1; }
    ;

aws_key
    : key_token
        {
            $$ = $1;
            if ( $1 . accession . n > 0 )
            {
                $$ . filename  = $1 . accession;
            }
        }    
    | aws_key key_token    
        { 
            $$ . path . n += $2 . path . n;

            // clear the filename and extension after every slash - to make sure we catch only the last
            // filename and extension in case of middle segments existing
            if ( $2 . path . n == 1 && $2 . path . p[ 0 ] == '/' )
            {
                $$ . filename . n = 0;
                $$ . extension . n = 0;
            }
            else if ( $2 . accession . n > 0 )
            {
                // we will use the last non-empty accession-looking token
                $$ . accession = $2 . accession;
                $$ . filename  = $2 . accession;
            }
            else if ( $2 . filename . n > 0 )
            {
                $$ . filename  = $2 . filename;
                $$ . extension . n = 0;
            }
            else if ( $2 . extension . n > 0 )
            {
                $$ . extension = $2 . extension;
            }
        }
    ;

aws_error
    : STR
    | STR1
    | DASH  { EMPTY_TSTR($$); }
    ;

aws_bytes_sent
    : STR
    | DASH  { EMPTY_TSTR($$); }
    ;

aws_obj_size
    : STR
    | DASH  { EMPTY_TSTR($$); }
    ;

aws_total_time
    : STR
    | DASH  { EMPTY_TSTR($$); }
    ;

aws_turnaround_time
    : STR
    | DASH  { EMPTY_TSTR($$); }
    ;

aws_version_id
    : STR
    | STR1
    | DASH  { EMPTY_TSTR($$); }
    ;

aws_host_id
    : STR
    | STR1
    | DASH  { EMPTY_TSTR($$); }
    ;

aws_cipher
    : STR SPACE STR     { $$ = $1; $$.n += ( $3.n + 1 ); }
    | STR1 SPACE STR    { $$ = $1; $$.n += ( $3.n + 1 ); }
    | STR SPACE STR1    { $$ = $1; $$.n += ( $3.n + 1 ); }
    | STR1 SPACE STR1   { $$ = $1; $$.n += ( $3.n + 1 ); }   
    | DASH              { EMPTY_TSTR($$); }
    ;

aws_auth
    : STR
    | STR1
    | DASH  { EMPTY_TSTR($$); }
    ;

aws_host_hdr
    : STR
    | STR1
    | DASH  { EMPTY_TSTR($$); }
    ;

aws_tls_vers
    : STR
    | STR1
    | DASH  { EMPTY_TSTR($$); }
    ;

ip
    : IPV4
    | IPV6
    | DASH  { EMPTY_TSTR($$); }
    ;

method
    : METHOD        { $$ = $1; }
    ;

qstr_list
    : QSTR                  { $$ = $1; }
    | qstr_list SPACE QSTR  { $$ = $1; $$.n += 1 + $3.n; $$.escaped = $1.escaped || $3.escaped; }
    ;

 /* 
    typedef enum { acc_before = 0, acc_inside, acc_after } eAccessionMode; (defined in log_lines.hpp)

        for a url_token node, set to 
            'acc_inside' if it is an ACCESSION
            'acc_after'  if it is a delimiter

        for a url_list node, describes the state of URL parsing:
            'acc_before' - no accession has been seen yet
            'acc_inside' - we are between the first accession and the following delimiter,
                            capture the filename and extension tokens
            'acc_after'  - we are past delimiter following an accession, 
                            no further action necessary
 */

url_token
    : SLASH         { InitRequest( $$ ); $$ . path = $1; }
    | EQUAL         { InitRequest( $$ ); $$ . path = $1; $$.accession_mode = acc_after; }
    | AMPERSAND     { InitRequest( $$ ); $$ . path = $1; $$.accession_mode = acc_after; }
    | QMARK         { InitRequest( $$ ); $$ . path = $1; $$.accession_mode = acc_after; }
    | PERCENT       { InitRequest( $$ ); $$ . path = $1; }
    | ACCESSION     
        { 
            InitRequest( $$ ); 
            $$ . path = $1; 
            $$ . accession = $1; 
            $$ . accession_mode = acc_inside; 
        }
    | PATHSTR       { InitRequest( $$ ); $$ . path = $1; $$ . filename = $1; }
    | PATHEXT       { InitRequest( $$ ); $$ . path = $1; $$ . extension = $1; }
    ;

 /* This is a collection of url tokens and accessions. 
    We are looking for the first accession and filename/extension that follow it.*/
url_list
    :  url_token               
        { 
            $$ = $1; 
            if ( $1 . accession_mode == acc_after )            
            {   /* a delimiter seen before an accession */
                $$ . accession_mode = acc_before;
            }
        }
    |  url_list url_token
        { 
            $$ = $1; 
            MERGE_TSTR( $$ . path, $2 . path );
            switch ( $$.accession_mode )
            {
            case acc_before:
                if ( $2.accession_mode == acc_inside )
                {
                    $$ . accession = $2 . accession;
                    $$ . accession_mode = acc_inside;
                }
                break;
            case acc_inside:
                switch ( $2 . accession_mode )
                {
                case acc_inside:
                    $$ . filename  = $2 . accession;
                    break;
                case acc_after:
                    $$ . accession_mode = acc_after;
                    break;
                default:
                    if ( $2 . filename . n > 0 )    $$ . filename  = $2 . filename;
                    if ( $2 . extension . n > 0 )   $$ . extension = $2 . extension;
                }
                break;
            }
        }
    ;

url
    : SPACE { aws_start_URL( scanner ); } url_list { $$ = $3; }
    ;

request
    : QUOTE method url SPACE VERS QUOTE
    {
        $$ = $3;
        $$.method = $2;
        $$.vers   = $5;
    }
    | QUOTE method url SPACE QUOTE
    {
        $$ = $3;
        $$.method = $2;
     }
    | QUOTE method url QUOTE
    {
        $$ = $3;
        $$.method = $2;
    }
    | QUOTE method QUOTE 
    {
        InitRequest( $$ );
        $$.method = $2;
    }
    | QUOTE { aws_start_URL( scanner ); } url_list QUOTE
    {
        $$ = $3;
    }
    | DASH                          
    {
        InitRequest( $$ );
    }
    ;

result_code
    : STR
    | DASH  { EMPTY_TSTR($$); }
    ;

referer
    : QUOTE qstr_list QUOTE { $$ = $2; }
    | STR
    | STR1
    | DASH                  { EMPTY_TSTR($$); }
    ;

vdb_agent_token
    : SRA_TOOLKIT   { InitAgent( $$ ); $$.original = $1; } 
    | SRATOOLVERS
        { 
            InitAgent( $$ ); 
            $$.original = $1; 
            const char * dot = strchr( $1 . p, '.' );
            $$ . vdb_tool . p = $1 . p;
            $$ . vdb_tool . n = dot - $1 . p;
            /* skip the leading dot */
            $$ . vdb_release . p = dot + 1; 
            $$ . vdb_release . n = $1 . n - ( dot - $1 . p ) - 1; 
        } 
    | LIBCVERSION   { InitAgent( $$ ); $$.original = $1; $$.vdb_libc = $1; }
    | PHIDVALUE     { InitAgent( $$ ); $$.original = $1; $$.vdb_phid_compute_env = $1; }
    | SPACE         { InitAgent( $$ ); $$.original = $1; } 
    | AGENTSTR      { InitAgent( $$ ); $$.original = $1; } 
    ;

vdb_agent
    : vdb_agent_token           { $$ = $1; }
    | vdb_agent vdb_agent_token 
    { 
        $$ = $1;
        MERGE_TSTR( $$ . original, $2 . original );
        if ( $2 . vdb_phid_compute_env . n > 0 )
        {
            $$ . vdb_phid_compute_env . p = $2 . vdb_phid_compute_env . p + 5;
            $$ . vdb_phid_compute_env . n = 3;

            /*
                the whole token is either 14 or 15 chars long!
                guid is either 3 or 4 chars long, the other parts have fixed length.
            */
            int guid_len = $2 . vdb_phid_compute_env . n - 11;
            $$ . vdb_phid_guid . p = $2 . vdb_phid_compute_env . p + 8;
            $$ . vdb_phid_guid . n = guid_len;

            $$ . vdb_phid_session_id . p = $$ . vdb_phid_guid . p + guid_len;
            $$ . vdb_phid_session_id . n = 3;
        }
        else if ( $2 . vdb_libc . n > 0 )
        {
            $$ . vdb_libc . p = $2 . vdb_libc . p + 5;
            $$ . vdb_libc . n = $2 . vdb_libc . n - 5;
        }
        else if ( $2 . vdb_tool . n > 0 )
        {
            $$ . vdb_tool = $2 . vdb_tool;
            $$ . vdb_release = $2 . vdb_release;
        }
    }
    ;

agent
    : QUOTE OS vdb_agent QUOTE       
        { 
            t_agent temp;
            InitAgent( temp ); 
            temp . original = $2;
            MERGE_TSTR( temp . original, $3 . original );
            $$ = $3;
            $$ . original = temp . original;

            $$.vdb_os = $2;
        }
    | QUOTE vdb_agent QUOTE
        { 
            $$ = $2;
        }
    | QUOTE QUOTE                                           
        { 
            InitAgent( $$ ); 
        }
    | DASH
        { 
            InitAgent( $$ ); 
        }
    ;

time
    : OB I64 SLASH MONTH SLASH I64 COLON I64 COLON I64 COLON I64 I64 CB
    {
        $$.day = atoi( $2.p );
        $$.month = month_int( &($4) );
        $$.year = atoi( $6.p );
        $$.hour = atoi( $8.p );
        $$.minute = atoi( $10.p );
        $$.second = atoi( $12.p );
        $$.offset = atoi( $13.p );
    } 
    ;

%%

void aws_error( yyscan_t locp, NCBI::Logging::AWS_LogLines * lib, const char * msg )
{
    // intentionally left empty, we communicate errors rejected lines
}
