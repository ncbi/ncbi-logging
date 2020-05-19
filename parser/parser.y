%define api.pure full
%lex-param { void * scanner }
%parse-param { void * scanner }{ parselib * lib }

%define parse.trace
%define parse.error verbose

%name-prefix "log1_"

%{
#define YYDEBUG 1

#include <stdint.h>
#include "parser-functions.h"
#include "parselib.h"
#include "log1_parser.h"
#include "log1_scanner.h"

void log1_error( yyscan_t *locp, parselib *lib, const char* msg );
%}

%code requires
{
#include "types.h"
#include "parselib.h"
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

%start logging

%%

logging
    : %empty
    | logging line
    ;

line
    : newline
    | log_v1 newline
    | error newline                 { yyerrok; }
    ;

newline
    : CR
    | LF
    ;

log_v1
    : ip DASH user time request result_code result_len req_time referer agent forwarded port req_len
    {
        t_event ev;
        ev . ip = $1;
        ev . user = $3;
        ev . t = $4;
        ev . req = $5;
        ev . res_code = $6;
        ev . res_len = $7;
        ev . req_time = $8;
        ev . referer = $9;
        ev . agent = $10;
        ev . forwarded = $11;
        ev . port = $12;
        ev . req_len = $13;
        parselib_event( lib, &ev );
    }
    ;

ip
    : IPV4
    | IPV6
    ;

user
    : DASH                          { $$.p = NULL; $$.n = 0;}
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
    | agent_list QSTR               { $$.n += $2.n; }
    | agent_list SPACE              { $$.n += 1; }
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

void log1_error( yyscan_t * locp, parselib * lib, const char * msg )
{
    parselib_error( lib, msg );
}
