%define api.pure full
%lex-param { void * scanner }
%parse-param { void * scanner }{ NCBI::Logging::OP_LogLines * lib }

%define parse.trace
%define parse.error verbose

%name-prefix "op_"

%{
#define YYDEBUG 1

#include <stdint.h>
#include "log_lines.hpp"
#include "op_parser.hpp"
#include "op_scanner.hpp"

using namespace std;
using namespace NCBI::Logging;

void op_error( yyscan_t locp, NCBI::Logging::OP_LogLines * lib, const char* msg );
#define EMPTY_TSTR(t) do { t.p = NULL; t.n = 0; } while (false)

%}

%code requires
{
#include "types.h"
#include "log_lines.hpp"
#include "helper.hpp"
using namespace NCBI::Logging;
}

%union
{
    t_str s;
    int64_t i64;
    t_timepoint tp;
    t_request req;
}

%token<s> STR MONTH IPV4 IPV6 FLOAT METHOD VERS QSTR QSTR_ESC
%token<i64> I64
%token DOT DASH SLASH COLON QUOTE OB CB PORT RL CR LF SPACE  
%token UNRECOGNIZED

%type<tp> time
%type<req> server_and_request request
%type<s> ip user req_time referer agent agent_list forwarded vers_opt method server
%type<s> quoted_list quoted_list_body quoted_list_elem 
%type<i64> result_code result_len port req_len

%start line

%%

line
    : log_onprem        { YYACCEPT; }
    | log_onprem_err    { YYACCEPT; }
    ;

log_onprem
    : ip DASH user time server_and_request result_code result_len req_time referer agent forwarded port req_len
    {
        LogOPEvent ev;
        ev . ip = $1;
        ev . user = $3;
        ev . time = $4;
        ev . request = $5;
        ev . res_code = $6;
        ev . res_len = $7;
        ev . req_time = $8;
        ev . referer = $9;
        ev . agent = $10;
        ev . forwarded = $11;
        ev . port = $12;
        ev . req_len = $13;

        lib -> acceptLine( ev );
    }
    ;

log_onprem_err
    : ip error
    {
        LogOPEvent ev;
        ev . ip = $1;
        lib -> rejectLine( ev );
    }
    ;

ip
    : IPV4
    | IPV6
    ;

user
    : DASH                          { EMPTY_TSTR($$); }
    | STR                           { $$ = $1; }
    ;

vers_opt
    : SPACE VERS    { $$ = $2; }
    | SPACE         { EMPTY_TSTR($$); }

    | %empty        { EMPTY_TSTR($$); }
    ;

method
    : METHOD        { $$ = $1; }
    ;

server 
    : QUOTE QSTR QUOTE  { $$ = $2; }
    | STR               { $$ = $1; }
    ;

request
    : QUOTE method SPACE QSTR vers_opt QUOTE
    {
        EMPTY_TSTR($$.server);
        $$.method = $2;
        $$.path   = $4;
        $$.vers   = $5;
    }
    | QUOTE method QUOTE
    {
        EMPTY_TSTR($$.server);
        $$.method = $2;
        EMPTY_TSTR($$.path);
        EMPTY_TSTR($$.vers);
    }
    ;

server_and_request 
    : server request
    {
        $$ = $2;
        $$.server = $1;
    }
    | server quoted_list
    { 
        $$.server = $1;
        $$.method = $2; 
        EMPTY_TSTR($$.path);
        EMPTY_TSTR($$.vers);
    }    
    | request
    {
        EMPTY_TSTR($$.server);
        $$ = $1;
    }
    ;

quoted_list
    : QUOTE quoted_list_body QUOTE  { $$ = $2; }
    | QUOTE QUOTE                   { EMPTY_TSTR($$); }
    ;

quoted_list_elem
    : QSTR      { $$ = $1; }
    | VERS      { $$ = $1; }
    | QSTR_ESC  { $$ = $1; }
    ;

quoted_list_body
    : quoted_list_elem                          { $$ = $1; }
    | quoted_list_body SPACE quoted_list_elem   { $$.n += 1 + $3.n; $$.escaped = $1.escaped || $3.escaped; }
    | quoted_list_body SPACE METHOD             { $$.n += 1 + $3.n; $$.escaped = $1.escaped || $3.escaped; }
    | quoted_list_body METHOD                   { $$.n += $2.n; $$.escaped = $1.escaped || $2.escaped; }
    ;

result_code
    : I64
    ;

result_len
    : I64
    ;

req_time
    : FLOAT
    ;

referer
    : QUOTE QSTR QUOTE              { $$ = $2; }
    ;

agent
    : QUOTE agent_list QUOTE    { $$ = $2; }
    | QUOTE QUOTE               { EMPTY_TSTR($$); }
    ;

agent_list
    : QSTR                { $$ = $1; }
    | agent_list QSTR     { $$.n += $2.n; $$.escaped = $1.escaped || $2.escaped; }
    | agent_list SPACE    { $$.n += 1;    $$.escaped = $1.escaped; }
    ;

forwarded
    : QUOTE QSTR QUOTE              { $$ = $2; }
    ;

port
    : PORT I64                      { $$ = $2; }
    ;

req_len
    : RL I64                        { $$ = $2; }
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

void op_error( yyscan_t locp, NCBI::Logging::OP_LogLines * lib, const char * msg )
{
    // TODO: find a way to comunicate the syntax error to the consumer...
    // t_str msg_p = { msg, (int)strlen( msg ) };
    // lib -> unrecognized( msg_p );
}
