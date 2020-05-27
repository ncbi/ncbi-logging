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

%token<s> STR MONTH IPV4 IPV6 FLOAT METHOD VERS QSTR
%token<i64> I64
%token DOT DASH SLASH COLON QUOTE OB CB PORT RL CR LF SPACE QMARK

%type<tp> time
%type<req> request
%type<s> ip user req_time referer agent agent_list forwarded params_opt
%type<i64> result_code result_len port req_len

%start line

%%

line
    : log_onprem        { return 0; }
    | log_onprem_err    { return 0; }
    ;

log_onprem
    : ip DASH user time request result_code result_len req_time referer agent forwarded port req_len
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
    : DASH                          { $$.p = NULL; $$.n = 0; }
    | STR                           { $$ = $1; }
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
    : QUOTE agent_list QUOTE        { $$ = $2; }
    ;

agent_list
    : QSTR                          { $$ = $1; }
    | agent_list QSTR               { $$.n += $2.n; $$.escaped = $1.escaped || $2.escaped; }
    | agent_list SPACE              { $$.n += 1;    $$.escaped = $1.escaped; }
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
