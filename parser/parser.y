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
}

%token<s> STR QSTR MONTH IPV4 FLOAT
%token<i64> I64
%token DOT DASH SLASH COLON QUOTE OB CB PORT RL CR LF

%type<tp> time
%type<s> quoted ip user host request gzip_ratio referer agent
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
    ;

newline
    : CR
    | LF
    | CR LF
    ;

log_v1
    : ip DASH user time host request result_code result_len gzip_ratio referer agent quoted port req_len
    {
        printf( "ip\t=%.*s\n", $1.n, $1.p  );
        if ( $3.n > 0 )
            printf( "user\t=%.*s\n", $3.n, $3.p );
        else
            printf( "user\t=none\n" );
        printf( "t\t=[%.02u/%.02u/%.04u:%.02u:%.02u:%.02u]\n",
                $4.day, $4.month, $4.year, $4.hour, $4.minute, $4.second );
        printf( "host\t=%.*s\n", $5.n, $5.p );
        printf( "url\t=%.*s\n", $6.n, $6.p );
        printf( "res\t=%d\n", $7 );
        printf( "len\t=%d\n", $8 );
        printf( "gzip\t=%.*s\n", $<s>9.n, $<s>9.p );
        printf( "ref\t=%.*s\n", $<s>10.n, $<s>10.p );
        printf( "agnt\t=%.*s\n", $<s>11.n, $<s>11.p );
        printf( "port\t=%d\n", $13 );
        printf( "rl\t=%d\n", $14 );
    }
    ;

log_v2
    : ip DASH user time request result_code result_len gzip_ratio referer agent quoted port req_len
    {
        printf( "ip\t=%.*s\n", $1.n, $1.p  );
        if ( $3.n > 0 )
            printf( "user\t=%.*s\n", $3.n, $3.p );
        else
            printf( "user\t=none\n" );
        printf( "t\t=[%.02u/%.02u/%.04u:%.02u:%.02u:%.02u]\n",
                $4.day, $4.month, $4.year, $4.hour, $4.minute, $4.second );
        printf( "host\t=none\n" );
        printf( "url\t=%.*s\n", $5.n, $5.p );
        printf( "res\t=%d\n", $6 );
        printf( "len\t=%d\n", $7 );
        printf( "gzip\t=%.*s\n", $<s>8.n, $<s>8.p );
        printf( "ref\t=%.*s\n", $<s>9.n, $<s>9.p );
        printf( "agnt\t=%.*s\n", $<s>10.n, $<s>10.p );
        printf( "port\t=%d\n", $12 );
        printf( "rl\t=%d\n", $13 );
    }
    ;

ip
    : IPV4
    ;

user
    : DASH                          { $$.p = NULL; $$.n = 0;}
    | STR                           { $$.p = $1.p; $$.n = $1.n; }
    ;

host
    : quoted
    | STR
    ;

request
    : quoted
    ;

result_code
    : I64
    ;

result_len
    : I64
    ;

gzip_ratio
    : FLOAT
    ;

referer
    : quoted
    ;

agent
    : quoted
    ;

port
    : PORT I64                      { $$ = $2; }
    ;

req_len
    : RL I64                        { $$ = $2; }
    ;

quoted
    : QUOTE QSTR QUOTE              { $$.p = $2.p; $$.n = $2.n; }
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
    exit( 1 );
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
