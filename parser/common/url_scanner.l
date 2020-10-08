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

//CMNCHARS    [A-Za-z0-9\-_~!*'();:@+$,\[\] ^{}]
CMNCHARS    [A-Za-z013-9\-_~!*'();:@+$,\[\] ^{}]
PATHCHARS   {CMNCHARS}|{QUERY_SEP}|{EQUAL}|{PERCENT}
PATHSTR     {PATHCHARS}+
PATHEXT     \.(\.|{PATHSTR})*

QUERYCHARS  [A-Za-z0-9\-_~@:?]
    /* %2f and %2F (/) are excluded because they are a special token (SLASH) */
HEXCHAR     [0-9A-Fa-f]
HEXCHAR_1   [013-9A-Fa-f]
HEXCHAR_2   [0-9A-Ea-e]
URL_BYTE_1  %{HEXCHAR_1}{HEXCHAR_2}
URL_BYTE_2  %{HEXCHAR_1}{HEXCHAR}
URL_BYTE_3  %{HEXCHAR}{HEXCHAR_2}
URL_BYTE    {URL_BYTE_1}|{URL_BYTE_2}|{URL_BYTE_3}

    /* allowing unncoded / in a query is not by the rules but real world examples use it */
QUERY_SLASH (\/|%2F|%2f)
QUERY_TOKEN ({URL_BYTE}|{QUERYCHARS})+
QUERY_EXT   \.(\.|{QUERY_TOKEN})*

%x QUERYSTATE FRAGSTATE

%%

[ \t]+          { /*TODO: has to be URL-encodeed, so an error? */ }

{ACCESSION}     { MAKE_STR(); return ACCESSION; }
{SLASH}         { MAKE_STR(); return SLASH; }
{PATHSTR}       { MAKE_STR(); return PATHSTR; }
{PATHEXT}       { MAKE_STR(); return PATHEXT; }
{QMARK}         { yy_push_state( QUERYSTATE, yyscanner ); MAKE_STR(); return QMARK; }
{HASH}          { yy_push_state( FRAGSTATE, yyscanner ); MAKE_STR(); return HASH; }

<QUERYSTATE>{QUERY_SEP}     { MAKE_STR(); return QUERY_SEP; }
<QUERYSTATE>{EQUAL}         { MAKE_STR(); return EQUAL; }
<QUERYSTATE>{ACCESSION}     { MAKE_STR(); return ACCESSION; }
<QUERYSTATE>{QUERY_SLASH}   { MAKE_STR(); return SLASH; }
<QUERYSTATE>{QUERY_EXT}     { MAKE_STR(); return PATHEXT; }
<QUERYSTATE>{QUERY_TOKEN}   { MAKE_STR(); return PATHSTR; }
<QUERYSTATE>{HASH}          { yy_push_state( FRAGSTATE, yyscanner ); MAKE_STR(); return HASH; }
<QUERYSTATE>.               { MAKE_STR(); return UNRECOGNIZED; }

<FRAGSTATE>.*               { MAKE_STR(); return FRAGMENT_TOKEN; }

.                           { MAKE_STR(); return UNRECOGNIZED; }

%%

YY_BUFFER_STATE url_scan_bytes( const char * input, size_t size, yyscan_t yyscanner )
{
    YY_BUFFER_STATE ret = url__scan_bytes( input, size, yyscanner );
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
    yyg -> yyextra_r = (void*)input;
    BEGIN( INITIAL );
    return ret;
}
