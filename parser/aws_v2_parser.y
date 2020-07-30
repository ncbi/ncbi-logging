%define api.pure full
%lex-param { void * scanner }
%parse-param { void * scanner }{ NCBI::Logging::LogAWSEvent * lib }

%define parse.trace
%define parse.error verbose

%name-prefix "aws_"

%{
#define YYDEBUG 1

#include <stdint.h>

//#include "AWS_Interface.hpp"
#include "aws_v2_parser.hpp"
#include "aws_v2_scanner.hpp"
#include "helper.hpp"

using namespace std;
using namespace NCBI::Logging;

void aws_error( yyscan_t locp, NCBI::Logging::LogAWSEvent * lib, const char* msg );

#define SET_VALUE( selector, source ) ( ( ( LogAWSEvent * ) lib ) -> set( (selector), (source) ) )
#define REPORT( selector, source ) ( ( ( LogAWSEvent * ) lib ) -> set( (selector), (source) ) )

%}

%code requires
{
#include "types.h"
#include "AWS_Interface.hpp"

extern void aws_get_scanner_input( void * yyscanner, t_str & str );

extern void aws_start_URL( void * yyscanner );
extern void aws_start_UserAgent( void * yyscanner );
extern void aws_start_TLS_vers( void * yyscanner );
extern void aws_start_host_id( void * yyscanner );
extern void aws_start_time( void * yyscanner );
extern void aws_start_ipaddr( void * yyscanner );
extern void aws_start_rescode( void * yyscanner );
extern void aws_start_referer( void * yyscanner );

extern void aws_pop_state( void * yyscanner );

using namespace NCBI::Logging;
}

%union
{
    t_str s;
    t_request req;
    t_agent agent;
}

%token<s> STR STR1 MONTH IPV4 IPV6 METHOD VERS QSTR DASH I64 AMPERSAND EQUAL PERCENT SLASH QMARK
%token<s> PATHSTR PATHEXT ACCESSION SPACE TLS_VERSION X_AMZ_ID_2 S3_EXT_REQ_ID TIME_FMT RESULTCODE
%token COLON QUOTE OB CB 
%token UNRECOGNIZED
%token<s> OS SRA_TOOLKIT LIBCVERSION AGENTSTR SRATOOLVERS PHIDVALUE

%type<s> time
%type<s> ip referer method qstr_list dash string_or_dash
%type<s> aws_owner aws_bucket aws_requester aws_request_id aws_operation aws_error
%type<s> aws_version_id aws_host_id aws_sig aws_cipher aws_auth aws_host_hdr aws_tls_vers
%type<s> result_code aws_bytes_sent aws_obj_size aws_total_time aws_turnaround_time
%type<s> x_amz_id_2
%type<req> request aws_key aws_quoted_key url_token url_list url key_token
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
      { aws_start_time( scanner ); } time SPACE
      { aws_start_ipaddr( scanner ); } ip SPACE
      aws_requester SPACE
      aws_request_id SPACE
      aws_operation SPACE
      { aws_start_URL( scanner ); } aws_quoted_key SPACE
      request SPACE
      { aws_start_rescode( scanner ); } result_code SPACE
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
      { aws_start_host_id( scanner ); } 
      aws_host_id 
      { aws_pop_state( scanner ); /* the following space is consumed by aws_host_id */ }
      aws_sig SPACE
      aws_cipher SPACE
      aws_auth SPACE
      aws_host_hdr SPACE
      { aws_start_TLS_vers( scanner ); } aws_tls_vers { aws_pop_state( scanner ); }
    {
        // numbers may be shifted!!!!

        // combine data from aws_key and request
        if ( $18 . path . n == 1 && $18 . path . p[ 0 ] == '-' )
        {
            t_str empty;
            EMPTY_TSTR( empty );
            SET_VALUE( LogAWSEvent::key, empty );
        }
        else
        {
            SET_VALUE( LogAWSEvent::key, $18 . path );
        }

        t_request req = $20;
        if ( $18 . accession . n > 0 )
        {
            req . accession = $18 . accession;
            req . filename  = $18 . filename;
            req . extension = $18 . extension;
        }

        lib -> setRequest( req );
    }
    ;

dash
    : DASH                  { EMPTY_TSTR($$); }
    | QUOTE DASH QUOTE      { EMPTY_TSTR($$); }
    ;

string_or_dash
    : STR
    | STR1
    | dash
    ;

log_aws_err
    : aws_owner error       {  YYABORT; }
    ;

aws_owner      : string_or_dash             { SET_VALUE( LogAWSEvent::owner, $1 ); };
aws_bucket     : string_or_dash             { SET_VALUE( LogAWSEvent::bucket, $1 ); };
aws_requester  : string_or_dash             { SET_VALUE( LogAWSEvent::requester, $1 ); };
aws_request_id : string_or_dash             { SET_VALUE( LogAWSEvent::request_id, $1 ); };
aws_operation  : string_or_dash             { SET_VALUE( LogAWSEvent::operation, $1 ); };
aws_error      : string_or_dash             { SET_VALUE( LogAWSEvent::error, $1 ); };
aws_version_id : string_or_dash             { SET_VALUE( LogAWSEvent::version_id, $1 ); };
aws_sig        : string_or_dash             { SET_VALUE( LogAWSEvent::sig_ver, $1 ); };
aws_cipher     : string_or_dash             { SET_VALUE( LogAWSEvent::cipher_suite, $1 ); };
aws_auth       : string_or_dash             { SET_VALUE( LogAWSEvent::auth_type, $1 ); };
aws_host_hdr   : string_or_dash             { SET_VALUE( LogAWSEvent::host_header, $1 ); };

key_token
    : SLASH         { InitRequest( $$ ); $$ . path = $1; }
    | ACCESSION     { InitRequest( $$ ); $$ . path = $1; $$ . accession = $1; }
    | PATHSTR       { InitRequest( $$ ); $$ . path = $1; $$ . filename = $1; }
    | PATHEXT       { InitRequest( $$ ); $$ . path = $1; $$ . extension = $1; }
    | EQUAL  
        { 
            lib -> reportField( "Unexpected '=' in key" ); 
            InitRequest( $$ ); $$ . path = $1; 
        }
    | AMPERSAND
        { 
            lib -> reportField( "Unexpected '&' in key" ); 
            InitRequest( $$ ); $$ . path = $1; 
        }
    | PERCENT
        { 
            lib -> reportField( "Unexpected '%' in key" ); 
            InitRequest( $$ ); $$ . path = $1; 
        }
    | QMARK         
        { 
            lib -> reportField( "Unexpected '?' in key" ); 
            InitRequest( $$ ); $$ . path = $1; 
        }
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

aws_quoted_key
    : aws_key               { $$ = $1; }
    | QUOTE DASH QUOTE      { InitRequest( $$ ); }
    ;

aws_bytes_sent
    : STR                   { SET_VALUE( LogAWSEvent::res_len, $1 ); }
    | dash                  { SET_VALUE( LogAWSEvent::res_len, $1 ); }
    ;

aws_obj_size
    : STR                   { SET_VALUE( LogAWSEvent::obj_size, $1 ); }
    | dash                  { SET_VALUE( LogAWSEvent::obj_size, $1 ); }
    ;

aws_total_time
    : STR                   { SET_VALUE( LogAWSEvent::total_time, $1 ); }
    | dash                  { SET_VALUE( LogAWSEvent::total_time, $1 ); }
    ;

aws_turnaround_time
    : STR                   { SET_VALUE( LogAWSEvent::turnaround_time, $1 ); }
    | dash                  { SET_VALUE( LogAWSEvent::turnaround_time, $1 ); }
    ;

x_amz_id_2: X_AMZ_ID_2 SPACE
        { 
            $$ = $1;
            MERGE_TSTR( $$ , $2 );
        }
    ;

aws_host_id 
    : x_amz_id_2 S3_EXT_REQ_ID SPACE
        { 
            $$ = $1; // keep the space between the 2 parts of the Id
            MERGE_TSTR( $$ , $2 );
            SET_VALUE( LogAWSEvent::host_id, $$ );
        }
    | x_amz_id_2
        {
            $$ = $1;
            // trim the trailing space
            $$ . n --;
            SET_VALUE( LogAWSEvent::host_id, $$ );
        }
    | S3_EXT_REQ_ID SPACE
        {
            SET_VALUE( LogAWSEvent::host_id, $1 );
        }
    | dash SPACE
        {
            SET_VALUE( LogAWSEvent::host_id, $1 );
        }
    ;

aws_tls_vers 
    : TLS_VERSION           { SET_VALUE( LogAWSEvent::tls_version, $1 ); }
    | dash                  { SET_VALUE( LogAWSEvent::tls_version, $1 ); }
    ;

ip
    : IPV4      { SET_VALUE( LogAWSEvent::ip, $1 ); }
    | IPV6      { SET_VALUE( LogAWSEvent::ip, $1 ); }
    | dash      { SET_VALUE( LogAWSEvent::ip, $1 ); }
    ;

method
    : METHOD  
    | dash
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
    | QUOTE method QUOTE 
    {
        InitRequest( $$ );
        $$.method = $2;
    }
    | QUOTE method url 
        { /* SPACE does that in the above branches, here have to pop state explicitly */
            aws_pop_state( scanner ); 
        } 
      QUOTE
        { 
            $$ = $3;
            $$.method = $2;
        }
    | DASH { InitRequest( $$ );}
    ;

result_code
    : RESULTCODE        { SET_VALUE( LogAWSEvent::res_code, $1 ); }
    | dash              { SET_VALUE( LogAWSEvent::res_code, $1 ); }
    ;

qstr_list
    : QSTR            { $$ = $1; }
    | qstr_list QSTR  { $$ = $1; MERGE_TSTR ( $$, $2 ); }
    ;

referer
    : QUOTE { aws_start_referer( scanner ); } 
        qstr_list QUOTE                 
        { 
            SET_VALUE( LogAWSEvent::referer, $3 ); 
            aws_pop_state( scanner ); // out of QUOTED into the global state
        }
    | string_or_dash                        
        { 
            SET_VALUE( LogAWSEvent::referer, $1 ); 
        }
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
    | vdb_agent OS
    {
        $$ = $1;
        MERGE_TSTR( $$ . original, $2 );
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
            $$ . vdb_os = $2;
            lib -> setAgent( $$ );
        }
    | QUOTE vdb_agent QUOTE
        { 
            $$ = $2;
            lib -> setAgent( $$ );
        }
    | QUOTE QUOTE                                           
        { 
            InitAgent( $$ ); 
            lib -> setAgent( $$ );
        }
    | dash 
        { 
            InitAgent( $$ ); 
            lib -> setAgent( $$ );
        }
    ;

time
    : TIME_FMT              { SET_VALUE( LogAWSEvent::time, $1 ); }
    | dash                  { SET_VALUE( LogAWSEvent::time, $1 ); }
    ;

%%

void aws_error( yyscan_t locp, NCBI::Logging::LogAWSEvent * lib, const char * msg )
{
    // intentionally left empty, we communicate errors via rejected lines
}
