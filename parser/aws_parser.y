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
using namespace NCBI::Logging;
}

%union
{
    t_str s;
    int64_t i64;
    t_timepoint tp;
    t_request req;
}

%token<s> STR STR1 MONTH IPV4 IPV6 FLOAT METHOD VERS QSTR DASH
%token<i64> I64
%token DOT SLASH COLON QUOTE OB CB PORT RL CR LF SPACE QMARK

%type<tp> time
%type<req> request
%type<s> ip referer agent agent_list params_opt
%type<s> aws_owner aws_bucket aws_requester aws_request_id aws_operation aws_key aws_error
%type<s> aws_version_id aws_host_id aws_cipher aws_auth aws_host_hdr aws_tls_vers
%type<i64> result_code aws_bytes_sent aws_obj_size aws_total_time aws_turnaround_time

%start line

%%

line
    : log_aws       { return 0; }
    ;

log_aws
    : aws_owner aws_bucket time ip aws_requester aws_request_id aws_operation
      aws_key request result_code aws_error aws_bytes_sent aws_obj_size
      aws_total_time aws_turnaround_time referer agent aws_version_id
      aws_host_id aws_cipher aws_auth aws_host_hdr aws_tls_vers
    {
        LogAWSEvent ev;
        ev . owner = $1;
        ev . bucket = $2;
        ev . time = $3;
        ev . ip = $4;
        ev . requester = $5;
        ev . request_id = $6;
        ev . operation = $7;
        ev . key = $8;
        ev . request = $9;
        ev . res_code = $10;
        ev . error = $11;
        ev . res_len = $12;
        ev . obj_size = $13;
        ev . total_time = $14;
        ev . turnaround_time = $15;
        ev . referer = $16;
        ev . agent = $17;
        ev . version_id = $18;
        ev . host_id = $19;
        ev . cipher_suite = $20;
        ev . auth_type = $21;
        ev . host_header = $22;
        ev . tls_version = $23;
        lib -> acceptLine( ev );
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
    ;

aws_request_id
    : STR
    | STR1
    ;

aws_operation
    : STR
    | STR1
    ;

aws_key
    : STR
    | STR1
    ;

aws_error
    : DASH                          { $$.p = NULL; $$.n = 0; }
    | STR
    | STR1
    ;

aws_bytes_sent
    : I64
    | DASH                          { $$ = 0; }
    ;

aws_obj_size
    : I64
    | DASH                          { $$ = 0; }
    ;

aws_total_time
    : I64
    | DASH                          { $$ = 0; }
    ;

aws_turnaround_time
    : I64
    | DASH                          { $$ = 0; }
    ;

aws_version_id
    : STR
    | STR1
    | DASH
    ;

aws_host_id
    : STR
    | STR1
    ;

aws_cipher
    : STR STR          { $$ = $1; $$.n += ( $2.n + 1 ); }
    | STR1 STR         { $$ = $1; $$.n += ( $2.n + 1 ); }
    | STR STR1         { $$ = $1; $$.n += ( $2.n + 1 ); }
    | STR1 STR1        { $$ = $1; $$.n += ( $2.n + 1 ); }    
    ;

aws_auth
    : STR
    ;

aws_host_hdr
    : STR
    | STR1
    ;

aws_tls_vers
    : STR
    | STR1
    ;

ip
    : IPV4
    | IPV6
    ;

params_opt
    : QMARK QSTR    { $$ = $2; }
    | %empty        { $$.p = NULL; $$.n = 0; }
    ;

request :
    QUOTE QSTR QUOTE QUOTE METHOD SPACE QSTR params_opt SPACE VERS QUOTE
    {
        $$.server = $2;
        $$.method = $5;
        $$.path   = $7;
        $$.params = $8;
        $$.vers   = $10;
    }
    |
    STR QUOTE METHOD SPACE QSTR params_opt SPACE VERS QUOTE
    {
        $$.server = $1;
        $$.method = $3;
        $$.path   = $5;
        $$.params = $6;
        $$.vers   = $8;
    }
    |
    QUOTE METHOD SPACE QSTR params_opt SPACE VERS QUOTE
    {
        $$.server.p = NULL; $$.server.n = 0;
        $$.method = $2;
        $$.path   = $4;
        $$.params = $5;
        $$.vers   = $7;
    }
    ;

result_code
    : I64
    ;

referer
    : QUOTE QSTR QUOTE              { $$ = $2; }
    | DASH                          { $$.p = NULL; $$.n = 0; }
    | STR
    | STR1
    ;

agent
    : QUOTE agent_list QUOTE        { $$ = $2; }
    ;

agent_list
    : QSTR                          { $$ = $1; }
    | agent_list QSTR               { $$.n += $2.n; $$.escaped = $1.escaped || $2.escaped; }
    | agent_list SPACE              { $$.n += 1;    $$.escaped = $1.escaped; }    
    ;

time
    : OB I64 SLASH MONTH SLASH I64 COLON I64 COLON I64 COLON I64 I64 CB
    {
        $$.day = $2;
        $$.month = month_int( &($4) );
        $$.year = $6;
        $$.hour = $8;
        $$.minute = $10;
        $$.second = $12;
        $$.offset = $13;
    }
    ;

%%

void aws_error( yyscan_t locp, NCBI::Logging::AWS_LogLines * lib, const char * msg )
{
    // TODO: find a way to comunicate the syntax error to the consumer...
    // t_str msg_p = { msg, (int)strlen( msg ) };
    // lib -> unrecognized( msg_p );
}
