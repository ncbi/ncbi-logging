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
//extern void aws_stop_URL( void * yyscanner );

using namespace NCBI::Logging;
}

%union
{
    t_str s;
    t_timepoint tp;
    t_request req;
}

%token<s> STR STR1 MONTH IPV4 IPV6 FLOAT METHOD VERS QSTR DASH I64
%token<s> PATHSTR PATHEXT ACCESSION 
%token DOT SLASH COLON QUOTE OB CB PORT RL CR LF SPACE QMARK
%token UNRECOGNIZED

%type<tp> time
%type<s> ip referer agent agent_list method qstr_list 
%type<s> aws_owner aws_bucket aws_requester aws_request_id aws_operation aws_error
%type<s> aws_version_id aws_host_id aws_cipher aws_auth aws_host_hdr aws_tls_vers
%type<s> result_code aws_bytes_sent aws_obj_size aws_total_time aws_turnaround_time
%type<req> request aws_key url_token

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
      referer SPACE
      agent SPACE
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
        ev . agent              = $34;
        ev . version_id         = $36;
        ev . host_id            = $38;
        ev . cipher_suite       = $40;
        ev . auth_type          = $42;
        ev . host_header        = $44;
        ev . tls_version        = $46;
        
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

url_token
    : SLASH     
        { 
            InitRequest( $$ );
            $$ . path . p = "/", $$ . path . n = 1;
        }
    | ACCESSION  
        { 
            InitRequest( $$ );
            $$ . path = $1; 
            $$ . accession = $1; 
        }
    | PATHSTR   
        { 
            InitRequest( $$ );
            $$ . path = $1; 
            $$ . filename = $1; 
        }
    | PATHEXT
        { 
            InitRequest( $$ );
            $$ . path = $1; 
            $$ . extension = $1; 
        }
    ;

aws_key
    : url_token             { $$ = $1; }    
    | aws_key url_token    
        { 
            $$ . path . n += $2 . path . n;
            // clear the filename and extension after every slash - to make sure we catch only the last
            // filename and extension in case of middle segments existing
            if ( $2 . path . n == 1 && $2 . path . p[ 0 ] == '/' )
            {
                $$ . filename . n = 0;
                $$ . extension . n = 0;
            }
            else
            {
                if ( $2 . accession . n > 0 ) $$ . accession = $2 . accession; // we will use the last non-empty accession-looking token
                if ( $2 . filename . n > 0 )
                {
                    $$ . filename  = $2 . filename;
                    $$ . extension . n = 0;
                }
                if ( $2 . extension . n > 0 ) $$ . extension = $2 . extension;
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

request
    : QUOTE method SPACE qstr_list SPACE VERS QUOTE
    {
        InitRequest( $$ );
        $$.method = $2;
        $$.path   = $4;
        $$.vers   = $6;
    }
    | QUOTE method SPACE qstr_list SPACE QUOTE
    {
        InitRequest( $$ );
        $$.method = $2;
        $$.path   = $4;
     }
    | QUOTE method SPACE qstr_list QUOTE
    {
        InitRequest( $$ );
        $$.method = $2;
        $$.path   = $4;
    }
    | QUOTE method QUOTE 
    {
        InitRequest( $$ );
        $$.method = $2;
    }
    | QUOTE qstr_list QUOTE
    {
        InitRequest( $$ );
        $$.path = $2;
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

agent
    : QUOTE agent_list QUOTE        { $$ = $2; }
    | DASH  { EMPTY_TSTR($$); }
    ;

agent_list
    : QSTR                          { $$ = $1; }
    | agent_list QSTR               { $$ = $1; $$.n += $2.n; $$.escaped = $1.escaped || $2.escaped; }
    | agent_list SPACE              { $$ = $1; $$.n += 1;    $$.escaped = $1.escaped; }    
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
