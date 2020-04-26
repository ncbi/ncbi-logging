%{

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

extern int yylex();
extern int yyparse();
extern int yylineno;
extern char * yytext;
extern FILE* yyin;

void yyerror( const char * s );
uint8_t month_int( const char * s );

%}

%union {
    struct t_str
    {
        char * p;
        int n;
	} s;
	
	int64_t i64;

    struct t_timepoint
    {
        uint8_t day;
        uint8_t month;
        uint32_t year;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
        int32_t offset;
    } tp;
}

%token<s> QSTR
%token<s> MONTH
%token<s> IPV4
%token<s> FLOAT
%token<i64> I64;
%token DOT DASH SLASH COLON QUOTE OB CB PORT RL NEWLINE

%start logging

%%

logging:                { printf( "start\n\n" ); }
	   | logging line
;

line: NEWLINE
    | log NEWLINE   { printf( "done\n\n" ); }
;

log: IPV4 DASH DASH time QUOTE QSTR QUOTE QUOTE QSTR QUOTE I64 I64 FLOAT QUOTE QSTR QUOTE QUOTE QSTR QUOTE QUOTE QSTR QUOTE PORT I64 RL I64
    {
        printf( "ip\t=%.*s\n", $<s>1.n, $<s>1.p  );
        printf( "t\t=[%.02u/%.02u/%.04u:%.02u:%.02u:%.02u]\n",
                $<tp>4.day, $<tp>4.month, $<tp>4.year, $<tp>4.hour, $<tp>4.minute, $<tp>4.second );
        printf( "dom\t=%.*s\n", $<s>6.n, $<s>6.p );
        printf( "url\t=%.*s\n", $<s>9.n, $<s>9.p );
        printf( "res\t=%d\n", $11 );
        printf( "len\t=%d\n", $12 );
        printf( "fac\t=%.*s\n", $<s>13.n, $<s>13.p );
        printf( "agnt\t=%.*s\n", $<s>18.n, $<s>18.p );
        printf( "port\t=%d\n", $24 );
        printf( "rl\t=%d\n", $26 );
    }
;

time:
    OB I64 SLASH MONTH SLASH I64 COLON I64 COLON I64 COLON I64 I64 CB
    {
        $<tp>$.day = $2;
        $<tp>$.month = month_int( $<s>4.p );
        $<tp>$.year = $6;
        $<tp>$.hour = $8;
        $<tp>$.minute = $10;
        $<tp>$.second = $12;
        $<tp>$.offset = $13;
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

void yyerror( const char* s )
{
	fprintf( stderr, "#%d: %s at >'%s'<\n", yylineno, s, yytext );
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
