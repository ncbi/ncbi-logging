%define api.pure full
%lex-param { void * scanner }
%parse-param { void * scanner }{ NCBI::Logging::AWSReceiver * lib }

%define parse.trace
%define parse.error verbose

%name-prefix "aws_"

%{
#define YYDEBUG 1

#include <stdint.h>

#include "aws_parser.hpp"
#include "aws_scanner.hpp"

using namespace std;
using namespace NCBI::Logging;

void aws_error( yyscan_t locp, NCBI::Logging::AWSReceiver * lib, const char* msg );

#define SET_VALUE( selector, source ) ( lib -> set( (selector), (source) ) )

const t_str EmptyTSTR = { "", 0, false };

%}

%code requires
{
#include "types.h"
#include "AWS_Interface.hpp"

extern void aws_get_scanner_input( void * yyscanner, t_str & str );

extern void aws_start_URL( void * yyscanner );
extern void aws_start_key( void * yyscanner );
extern void aws_start_UserAgent( void * yyscanner );
extern void aws_start_TLS_vers( void * yyscanner );
extern void aws_start_host_id( void * yyscanner );
extern void aws_start_time( void * yyscanner );
extern void aws_start_ipaddr( void * yyscanner );
extern void aws_start_rescode( void * yyscanner );
extern void aws_start_referer( void * yyscanner );
extern void aws_start_rest_of( void * yyscanner );

extern void aws_pop_state( void * yyscanner );

using namespace NCBI::Logging;
}

%union
{
    t_str s;
}

%token<s> STR STR1 MONTH IPV4 IPV6 METHOD VERS QSTR DASH I64 AMPERSAND EQUAL PERCENT SLASH QMARK
%token<s> PATHSTR PATHEXT ACCESSION SPACE TLS_VERSION X_AMZ_ID_2 S3_EXT_REQ_ID TIME_FMT RESULTCODE
%token COLON QUOTE OB CB
%token UNRECOGNIZED

%type<s> time
%type<s> ip referer method qstr_list dash string_or_dash
%type<s> aws_owner aws_bucket aws_requester aws_request_id aws_operation aws_error
%type<s> aws_version_id aws_host_id aws_sig aws_cipher aws_auth aws_host_hdr aws_tls_vers
%type<s> result_code aws_bytes_sent aws_obj_size aws_total_time aws_turnaround_time
%type<s> x_amz_id_2
%type<s> request aws_key url
%type<s> agent vdb_agent

%start line

%%

line
    : log_aws       { YYACCEPT; }
    | log_aws_err   { YYABORT; }
    ;

log_aws
    : aws_owner SPACE
      aws_bucket SPACE
      { aws_start_time( scanner ); } time SPACE
      { aws_start_ipaddr( scanner ); } ip SPACE
      aws_requester SPACE
      aws_request_id SPACE
      aws_operation SPACE
      { aws_start_key( scanner ); } aws_key SPACE
      request SPACE
      { aws_start_rescode( scanner ); } result_code SPACE
      aws_error SPACE
      aws_bytes_sent SPACE
      aws_obj_size SPACE
      aws_total_time SPACE
      aws_turnaround_time SPACE
      referer SPACE
      { aws_start_UserAgent( scanner ); } agent SPACE
      aws_version_id SPACE
      { aws_start_host_id( scanner ); }
      aws_host_id
      { aws_pop_state( scanner ); /* the following space is consumed by aws_host_id */ }
      aws_sig SPACE
      aws_cipher SPACE
      aws_auth SPACE
      aws_host_hdr SPACE
      { aws_start_TLS_vers( scanner ); } aws_tls_vers { aws_pop_state( scanner ); }
      rest_of_line
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
    : aws_owner error
    ;

aws_owner      : string_or_dash             { SET_VALUE( AWSReceiver::owner, $1 ); };
aws_bucket     : string_or_dash             { SET_VALUE( AWSReceiver::bucket, $1 ); };
aws_requester  : string_or_dash             { SET_VALUE( AWSReceiver::requester, $1 ); };
aws_request_id : string_or_dash             { SET_VALUE( AWSReceiver::request_id, $1 ); };
aws_operation  : string_or_dash             { SET_VALUE( AWSReceiver::operation, $1 ); };
aws_error      : string_or_dash             { SET_VALUE( AWSReceiver::error, $1 ); };
aws_version_id : string_or_dash             { SET_VALUE( AWSReceiver::version_id, $1 ); };
aws_sig        : string_or_dash             { SET_VALUE( AWSReceiver::sig_ver, $1 ); };
aws_cipher     : string_or_dash             { SET_VALUE( AWSReceiver::cipher_suite, $1 ); };
aws_auth       : string_or_dash             { SET_VALUE( AWSReceiver::auth_type, $1 ); };
aws_host_hdr   : string_or_dash             { SET_VALUE( AWSReceiver::host_header, $1 ); };

aws_key
    : PATHSTR               {
                                if ( $1 . n == 1 && $1 . p[ 0 ] == '-' )
                                {
                                    SET_VALUE( AWSReceiver::key, EmptyTSTR );
                                }
                                else
                                {
                                    SET_VALUE( AWSReceiver::key, $1 );
                                    lib -> key_for_postprocess = string( $1.p, $1.n );
                                }
                            }
    | QUOTE DASH QUOTE      { SET_VALUE( AWSReceiver::key, EmptyTSTR ); }
    ;

aws_bytes_sent
    : STR                   { SET_VALUE( AWSReceiver::res_len, $1 ); }
    | dash                  { SET_VALUE( AWSReceiver::res_len, $1 ); }
    ;

aws_obj_size
    : STR                   { SET_VALUE( AWSReceiver::obj_size, $1 ); }
    | dash                  { SET_VALUE( AWSReceiver::obj_size, $1 ); }
    ;

aws_total_time
    : STR                   { SET_VALUE( AWSReceiver::total_time, $1 ); }
    | dash                  { SET_VALUE( AWSReceiver::total_time, $1 ); }
    ;

aws_turnaround_time
    : STR                   { SET_VALUE( AWSReceiver::turnaround_time, $1 ); }
    | dash                  { SET_VALUE( AWSReceiver::turnaround_time, $1 ); }
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
            SET_VALUE( AWSReceiver::host_id, $$ );
        }
    | x_amz_id_2
        {
            $$ = $1;
            // trim the trailing space
            $$ . n --;
            SET_VALUE( AWSReceiver::host_id, $$ );
        }
    | S3_EXT_REQ_ID SPACE
        {
            SET_VALUE( AWSReceiver::host_id, $1 );
        }
    | dash SPACE
        {
            SET_VALUE( AWSReceiver::host_id, $1 );
        }
    ;

aws_tls_vers
    : TLS_VERSION           { SET_VALUE( AWSReceiver::tls_version, $1 ); }
    | dash                  { SET_VALUE( AWSReceiver::tls_version, $1 ); }
    ;

ip
    : IPV4      { SET_VALUE( AWSReceiver::ip, $1 ); }
    | IPV6      { SET_VALUE( AWSReceiver::ip, $1 ); }
    | dash      { SET_VALUE( AWSReceiver::ip, $1 ); }
    ;

method
    : METHOD    { SET_VALUE( AWSReceiver::method, $1 ); }
    | dash      { SET_VALUE( AWSReceiver::method, $1 ); }
    ;

url
    : SPACE { aws_start_URL( scanner ); } PATHSTR
    {
        SET_VALUE( AWSReceiver::path, $3 );
        lib -> url_for_postprocess = string( $3.p, $3.n );
    }
    ;

request
    : QUOTE method url SPACE VERS QUOTE
    {
        SET_VALUE( AWSReceiver::vers, $5 );
    }
    | QUOTE method url SPACE QUOTE
    {
        SET_VALUE( AWSReceiver::vers, EmptyTSTR );
    }
    | QUOTE method QUOTE
    {
        SET_VALUE( AWSReceiver::path, EmptyTSTR );
        SET_VALUE( AWSReceiver::vers, EmptyTSTR );
    }
    | QUOTE method url
        { /* SPACE does that in the above branches, here have to pop state explicitly */
            aws_pop_state( scanner );
            SET_VALUE( AWSReceiver::vers, EmptyTSTR );
        }
        QUOTE { }
    | DASH
    {
        SET_VALUE( AWSReceiver::method, EmptyTSTR );
        SET_VALUE( AWSReceiver::path, EmptyTSTR );
        SET_VALUE( AWSReceiver::vers, EmptyTSTR );
    }
    ;

result_code
    : RESULTCODE        { SET_VALUE( AWSReceiver::res_code, $1 ); }
    | dash              { SET_VALUE( AWSReceiver::res_code, $1 ); }
    ;

qstr_list
    : QSTR            { $$ = $1; }
    | qstr_list QSTR  { $$ = $1; MERGE_TSTR ( $$, $2 ); }
    ;

referer
    : QUOTE { aws_start_referer( scanner ); }
        qstr_list QUOTE
        {
            SET_VALUE( AWSReceiver::referer, $3 );
            aws_pop_state( scanner ); // out of QUOTED into the global state
        }
    | string_or_dash
        {
            SET_VALUE( AWSReceiver::referer, $1 );
        }
    ;

vdb_agent
    : STR               { $$ = $1; }
    | vdb_agent STR     { $$ = $1; MERGE_TSTR( $$, $2 ); }
    ;

agent
    : QUOTE vdb_agent QUOTE
        {
            lib -> set( ReceiverInterface::agent, $2 );
            lib -> agent_for_postprocess = string( $2.p, $2.n );
        }
    | QUOTE QUOTE       { lib -> set( ReceiverInterface::agent, EmptyTSTR ); }
    | DASH              { lib -> set( ReceiverInterface::agent, EmptyTSTR ); }
    ;

time
    : TIME_FMT              { SET_VALUE( AWSReceiver::time, $1 ); }
    | dash                  { SET_VALUE( AWSReceiver::time, $1 ); }
    ;

rest_of_line
    : SPACE { aws_start_rest_of( scanner ); } STR
        { SET_VALUE( AWSReceiver::_extra, $3 ); lib->reportField("Extra fields discovered"); }
    | %empty
    ;

%%

void aws_error( yyscan_t locp, NCBI::Logging::AWSReceiver * lib, const char * msg )
{
    // intentionally left empty, we communicate errors via rejected lines
}
