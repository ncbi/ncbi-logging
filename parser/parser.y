%locations
%define parse.error verbose

%{
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

extern int yylex();
extern int yyparse();
extern int yylineno;
extern char * yytext;
extern FILE* yyin;

void yyerror( const char* msg );
uint8_t month_int( const char * s );

%}

%code requires {
#include "types.h"
}

%union {
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
        printf( "ip\t=%.*s\n", $1.n, $1.p  );
        if ( $3.n > 0 )
            printf( "user\t=%.*s\n", $3.n, $3.p );
        else
            printf( "user\t=none\n" );
        printf( "time\t=[%.02u/%.02u/%.04u:%.02u:%.02u:%.02u]\n",
                $4.day, $4.month, $4.year, $4.hour, $4.minute, $4.second );
        printf( "server\t=%.*s\n", $5.n, $5.p );

        printf( "req.method\t=%.*s\n", $6.method.n, $6.method.p );
        printf( "req.path\t=%.*s\n", $6.path.n, $6.path.p );
        printf( "req.params\t=%.*s\n", $6.params.n, $6.params.p );
        printf( "req.vers\t=%.*s\n", $6.vers.n, $6.vers.p );

        printf( "result\t=%d\n", $7 );
        printf( "body-len\t=%d\n", $8 );
        printf( "req-time\t=%.*s\n", $<s>9.n, $<s>9.p );
        printf( "referer\t=%.*s\n", $<s>10.n, $<s>10.p );
        printf( "agent\t=%.*s\n", $<s>11.n, $<s>11.p );
        printf( "port\t=%d\n", $13 );
        printf( "req-len\t=%d\n", $14 );
    }
    ;

log_v2
    : ip DASH user time request result_code result_len req_time referer agent forwarded port req_len
    {
        printf( "ip\t=%.*s\n", $1.n, $1.p  );
        if ( $3.n > 0 )
            printf( "user\t=%.*s\n", $3.n, $3.p );
        else
            printf( "user\t=none\n" );
        printf( "time\t=[%.02u/%.02u/%.04u:%.02u:%.02u:%.02u]\n",
                $4.day, $4.month, $4.year, $4.hour, $4.minute, $4.second );
        printf( "server\t=none\n" );

        printf( "req.method\t=%.*s\n", $5.method.n, $5.method.p );
        printf( "req.path\t=%.*s\n", $5.path.n, $5.path.p );
        printf( "req.params\t=%.*s\n", $5.params.n, $5.params.p );
        printf( "req.vers\t=%.*s\n", $5.vers.n, $5.vers.p );

        printf( "result\t=%d\n", $6 );
        printf( "body-len\t=%d\n", $7 );
        printf( "req-time\t=%.*s\n", $<s>8.n, $<s>8.p );
        printf( "referer\t=%.*s\n", $<s>9.n, $<s>9.p );
        printf( "agent\t=%.*s\n", $<s>10.n, $<s>10.p );
        printf( "port\t=%d\n", $12 );
        printf( "req-len\t=%d\n", $13 );
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
        $$.month = month_int( $4.p );
        $$.year = $6;
        $$.hour = $8;
        $$.minute = $10;
        $$.second = $12;
        $$.offset = $13;
    }
    ;

%%

int main()
{
    yyin = stdin;

    do
    {
        yyparse();
    } while( !feof( yyin ) );

    return 0;
}

void yyerror( const char* msg )
{
    fprintf( stderr, "#%d: %s at >'%s'<\n", yylineno, msg, yytext );
//    fprintf( stderr, "at: %d.%d ... %d.%d\n", p->first_line, p->first_column, p->last_line, p->last_column );
}

uint8_t month_int( const char * s )
{
    if ( s[ 0 ] == 'A' ) /* Apr, Aug */
    {
        if ( s[ 1 ] == 'p' && s[ 2 ] == 'r' ) return 4;
        if ( s[ 1 ] == 'u' && s[ 2 ] == 'g' ) return 8;
    }
    else if ( s[ 0 ] == 'D' ) /* Dec */
    {
        if ( s[ 1 ] == 'e' && s[ 2 ] == 'c' ) return 12;
    }
    else if ( s[ 0 ] == 'F' ) /* Feb */
    {
        if ( s[ 1 ] == 'e' && s[ 2 ] == 'b' ) return 2;
    }
    else if ( s[ 0 ] == 'J' ) /* Jan, Jun, Jul */
    {
        if ( s[ 1 ] == 'a' && s[ 2 ] == 'n' ) return 1;
        if ( s[ 1 ] == 'u' )
        {
            if ( s[ 2 ] == 'n' ) return 6;
            if ( s[ 2 ] == 'l' ) return 7;
        }
    }
    else if ( s[ 0 ] == 'M' ) /* Mar, May */
    {
        if ( s[ 1 ] == 'a' )
        {
            if ( s[ 2 ] == 'r' ) return 3;
            if ( s[ 2 ] == 'y' ) return 5;
        }
    }
    else if ( s[ 0 ] == 'N') /* Nov */
    {
        if ( s[ 1 ] == 'o' && s[ 2 ] == 'v' ) return 11;
    }
    else if ( s[ 0 ] == 'O' ) /* Oct */
    {
        if ( s[ 1 ] == 'c' && s[ 2 ] == 't' ) return 10;
    }
    else if ( s[ 0 ] == 'S' ) /* Sep */
    {
        if ( s[ 1 ] == 'e' && s[ 2 ] == 'p' ) return 9;
    }
    return 0;
}
