%define api.pure full
%lex-param { void * scanner }
%parse-param { void * scanner }{ parselib * lib }

%define parse.trace
%define parse.error verbose

%name-prefix "log1_"

%{
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "parser-functions.h"
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
%type<s> ip user server req_time referer agent agent_list forwarded
%type<i64> result_code result_len port req_len

%start logging

%%

logging
    : %empty                        { printf( "start\n\n" ); }
    | logging line
    ;

line
    : newline
    | log_v1 newline                { printf( "done ( v1 )\n\n" ); }
    | log_v2 newline                { printf( "done ( v2 )\n\n" ); }
    | error newline                 { yyerrok; }
    ;

newline
    : CR
    | LF
    | CR LF
    ;

log_v1
    : ip DASH user time server request result_code result_len req_time referer agent forwarded port req_len
    {
        t_event ev;
        ev . ip = $1;
        ev . user = $3;
        ev . t = $4;
        ev . server = $5;
        ev . req = $6;
        ev . res_code = $7;
        ev . res_len = $8;
        ev . req_time = $9;
        ev . referer = $10;
        ev . agent = $11;
        ev . forwarded = $12;
        ev . port = $13;
        ev . req_len = $14;
        parselib_event( lib, &ev );
    }
    ;

log_v2
    : ip DASH user time request result_code result_len req_time referer agent forwarded port req_len
    {
        t_event ev;
        ev . ip = $1;
        ev . user = $3;
        ev . t = $4;
        ev . server . p = NULL; ev . server . n = 0;
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
    | STR                           { $$.p = $1.p; $$.n = $1.n; }
    ;

server
    : QUOTE QSTR QUOTE              { $$.p = $2.p; $$.n = $2.n; }
    | STR
    ;

request
    : QUOTE METHOD SPACE QSTR SPACE VERS QUOTE
    {
        $$.method.p = $2.p; $$.method.n = $2.n;
        $$.path.p = $4.p; $$.path.n = $4.n;
        $$.params.p = $4.p; $$.params.n = 0;
        $$.vers.p = $6.p; $$.vers.n = $6.n;
    }
    | QUOTE METHOD SPACE QSTR QMARK QSTR SPACE VERS QUOTE
    {
        $$.method.p = $2.p; $$.method.n = $2.n;
        $$.path.p = $4.p; $$.path.n = $4.n;
        $$.params.p = $6.p; $$.params.n = $6.n;
        $$.vers.p = $8.p; $$.vers.n = $8.n;
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
    : QUOTE QSTR QUOTE              { $$.p = $2.p; $$.n = $2.n; }
    ;

agent
    : QUOTE agent_list QUOTE        { $$.p = $2.p; $$.n = $2.n; }
    ;

agent_list
    : QSTR                          { $$.p = $1.p; $$.n = $1.n; }
    | agent_list QSTR               { $$.n += $2.n; }
    | agent_list SPACE              { $$.n += 1; }
    ;

forwarded
    : QUOTE QSTR QUOTE              { $$.p = $2.p; $$.n = $2.n; }
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

void log1_error( yyscan_t * locp, parselib * lib, const char* msg )
{
    fprintf( stderr, "err: %s\n", msg );
}
