%{
#include "url_parser.hpp"

//TODO: convert into a function
#define MAKE_STR() \
    do { \
        yylval -> s . str . p = yytext; \
        yylval -> s . str . n = yyleng; \
        yylval -> s . str . escaped = false; \
        EMPTY_TSTR( yylval -> s . accession ); \
        EMPTY_TSTR( yylval -> s . filename ); \
        EMPTY_TSTR( yylval -> s . extension ); \
    } while(false)

#define RETURN( token ) do { MAKE_STR(); return token; } while ( false )

#define POP() yy_pop_state( yyscanner )
#define PUSH( state ) yy_push_state( state, yyscanner )
%}

%option noyywrap yylineno
%option reentrant bison-bridge
%option prefix="url_"
%option stack
%option debug

ACCESSION   [DES]R[RZ][0-9]{6,9}
SLASH       (\/|%2F|%2f)
QMARK       "?"
QUERY_SEP   [&;]
EQUAL       "="
HASH        "#"
PERCENT     "%"
DOT         \.

CMNCHARS    [A-Za-z0-9\-_~!*'();:@+$,\[\] ^{}]
PATHCHARS   {CMNCHARS}|{QUERY_SEP}|{EQUAL}|{URL_BYTE}
PATHSTR     {PATHCHARS}+

QUERYCHARS  [A-Za-z0-9\-_~@:?<>]
    /* %2f and %2F (/) are excluded because they are a special token (SLASH) */
HEXCHAR     [0-9A-Fa-f]
HEXCHAR_1   [013-9A-Fa-f]
HEXCHAR_2   [0-9A-Ea-e]
URL_BYTE_1  %{HEXCHAR_1}{HEXCHAR_2}
URL_BYTE_2  %{HEXCHAR_1}{HEXCHAR}
URL_BYTE_3  %{HEXCHAR}{HEXCHAR_2}
URL_BYTE    {URL_BYTE_1}|{URL_BYTE_2}|{URL_BYTE_3}

    /* allowing unncoded / in a query is not by the rules but real world examples use it */
QUERY_TOKEN ({URL_BYTE}|{QUERYCHARS})+

%x EXTENSION QUERYSTATE QUERY_EXTENSION FRAGSTATE

%%

[ \t]+          { /*TODO: has to be URL-encodeed, so an error? */ }

{ACCESSION}     { RETURN(  ACCESSION ); }
{SLASH}         { RETURN(  SLASH ); }
{PERCENT}       { RETURN(  PERCENT ); }
{DOT}           { PUSH( EXTENSION ); RETURN( DOT ); }
{PATHSTR}       { RETURN( PATHSTR ); }
{QMARK}         { PUSH( QUERYSTATE ); RETURN( QMARK ); }
{HASH}          { PUSH( FRAGSTATE ); RETURN( HASH ); }

<EXTENSION>{SLASH}         { POP(); RETURN( SLASH ); }
<EXTENSION>{QMARK}         { POP(); PUSH( QUERYSTATE ); RETURN( QMARK ); }
<EXTENSION>{HASH}          { POP(); PUSH( FRAGSTATE ); RETURN( HASH ); }
<EXTENSION>{PERCENT}       { RETURN( EXTPERCENT ); }
<EXTENSION>{DOT}           { RETURN( EXTSTR ); }
<EXTENSION>{PATHSTR}       { RETURN( EXTSTR ); }
<EXTENSION>.               { RETURN( UNRECOGNIZED ); }

<QUERYSTATE>{QUERY_SEP}         { RETURN( QUERY_SEP ); }
<QUERYSTATE>{EQUAL}             { RETURN( EQUAL ); }
<QUERYSTATE>{ACCESSION}         { RETURN( ACCESSION ); }
<QUERYSTATE>{SLASH}             { RETURN( SLASH ); }
<QUERYSTATE>{PERCENT}           { RETURN( PERCENT ); }
<QUERYSTATE>{DOT}               { PUSH( QUERY_EXTENSION ); RETURN( DOT ); }
<QUERYSTATE>{QUERY_TOKEN}       { RETURN( PATHSTR ); }
<QUERYSTATE>{HASH}              { PUSH( FRAGSTATE ); RETURN( HASH ); }
<QUERYSTATE>.                   { RETURN( UNRECOGNIZED ); }

<QUERY_EXTENSION>{QUERY_SEP}     { POP(); RETURN( QUERY_SEP ); }
<QUERY_EXTENSION>{EQUAL}         { POP(); RETURN( EQUAL ); }
<QUERY_EXTENSION>{SLASH}         { POP(); RETURN( SLASH ); }
<QUERY_EXTENSION>{HASH}          { POP(); POP(); PUSH( FRAGSTATE ); RETURN( HASH ); }
<QUERY_EXTENSION>{PERCENT}       { RETURN( EXTPERCENT ); }
<QUERY_EXTENSION>{DOT}           { RETURN( EXTSTR ); }
<QUERY_EXTENSION>{QUERY_TOKEN}   { RETURN( EXTSTR ); }
<QUERY_EXTENSION>.               { RETURN( UNRECOGNIZED ); }

<FRAGSTATE>.*               { RETURN( FRAGMENT_TOKEN ); }

.                           { RETURN( UNRECOGNIZED ); }

%%

YY_BUFFER_STATE url_scan_bytes( const char * input, size_t size, yyscan_t yyscanner )
{
    YY_BUFFER_STATE ret = url__scan_bytes( input, size, yyscanner );
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
    yyg -> yyextra_r = (void*)input;
    BEGIN( INITIAL );
    return ret;
}
