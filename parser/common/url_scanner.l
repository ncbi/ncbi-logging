%{
#include "url_parser.hpp"

#define MAKE_STR() \
    do { \
        yylval -> str . p = yytext; \
        yylval -> str . n = yyleng; \
        EMPTY_TSTR( yylval -> accession ); \
        EMPTY_TSTR( yylval -> filename ); \
        EMPTY_TSTR( yylval -> extension ); \
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

 /* accession names */
ALPHA       [A-Za-z]
DIGIT       [0-9]
SRA         [DES]R[RZ]{DIGIT}{6,9}
REFSEQ1     ({ALPHA}+_){0,1}{ALPHA}{1,2}{DIGIT}{5,9}
REFSEQ2     {ALPHA}+_{DIGIT}{5,9}
WGS         ({ALPHA}+_){0,1}{ALPHA}{4,6}{DIGIT}{2,10}
ACCESSION   {SRA}|{REFSEQ1}|{REFSEQ2}|{WGS}|"hs37d5"

SLASH       (\/|%2F|%2f)
QMARK       "?"
QUERY_SEP   [&;]
EQUAL       "="
HASH        "#"
PERCENT     "%"
DOT         \.

    /* %2f and %2F (/) are excluded because they are a special token (SLASH) */
HEXCHAR     [0-9A-Fa-f]
HEXCHAR_1   [013-9A-Fa-f]
HEXCHAR_2   [0-9A-Ea-e]
URL_BYTE_1  %{HEXCHAR_1}{HEXCHAR_2}
URL_BYTE_2  %{HEXCHAR_1}{HEXCHAR}
URL_BYTE_3  %{HEXCHAR}{HEXCHAR_2}
URL_BYTE    {URL_BYTE_1}|{URL_BYTE_2}|{URL_BYTE_3}

PATHCHARS    [^/?#%.]
PATHSTR      ({URL_BYTE}|{PATHCHARS})+

 /* allowing unncoded / in a query is not by the rules but real world examples use it */
QUERYCHARS   [^/&;=#%.]
QUERY_TOKEN  ({URL_BYTE}|{QUERYCHARS})+

%x EXTENSION QUERYSTATE QUERY_EXTENSION FRAGSTATE

%%

[ \t]+          { }

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

<QUERYSTATE>{QUERY_SEP}         { RETURN( QUERY_SEP ); }
<QUERYSTATE>{EQUAL}             { RETURN( EQUAL ); }
<QUERYSTATE>{ACCESSION}         { RETURN( ACCESSION ); }
<QUERYSTATE>{SLASH}             { RETURN( SLASH ); }
<QUERYSTATE>{PERCENT}           { RETURN( PERCENT ); }
<QUERYSTATE>{DOT}               { PUSH( QUERY_EXTENSION ); RETURN( DOT ); }
<QUERYSTATE>{QUERY_TOKEN}       { RETURN( PATHSTR ); }
<QUERYSTATE>{HASH}              { PUSH( FRAGSTATE ); RETURN( HASH ); }

<QUERY_EXTENSION>{QUERY_SEP}     { POP(); RETURN( QUERY_SEP ); }
<QUERY_EXTENSION>{EQUAL}         { POP(); RETURN( EQUAL ); }
<QUERY_EXTENSION>{SLASH}         { POP(); RETURN( SLASH ); }
<QUERY_EXTENSION>{HASH}          { POP(); POP(); PUSH( FRAGSTATE ); RETURN( HASH ); }
<QUERY_EXTENSION>{PERCENT}       { RETURN( EXTPERCENT ); }
<QUERY_EXTENSION>{DOT}           { RETURN( EXTSTR ); }
<QUERY_EXTENSION>{QUERY_TOKEN}   { RETURN( EXTSTR ); }

<FRAGSTATE>.*               { RETURN( FRAGMENT_TOKEN ); }

%%

YY_BUFFER_STATE url_scan_bytes( const char * input, size_t size, yyscan_t yyscanner )
{
    YY_BUFFER_STATE ret = url__scan_bytes( input, size, yyscanner );
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
    yyg -> yyextra_r = (void*)input;
    BEGIN( INITIAL );
    return ret;
}
