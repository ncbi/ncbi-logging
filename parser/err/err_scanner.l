%{
#include "err_parser.hpp"
#include "stdint.h"

#define MAKE_STR() do { yylval -> p = yytext; yylval -> n = yyleng; } while(false)

%}

%option noyywrap yylineno
%option reentrant bison-bridge
%option prefix="err_"
%option stack
%option debug

INT4        [0-9]{4}
INT2        [0-9]{2}
DATETIME    {INT4}"/"{INT2}"/"{INT2}" "{INT2}":"{INT2}":"{INT2}

LOWERASCII  [a-z]+
SEVERITY    "["{LOWERASCII}"]"

STR         [\x21-\xFF]+

%x MSGSTATE

%%

" "+            { MAKE_STR(); return SPACE; }
{DATETIME}      { MAKE_STR(); return DATETIME; }
{SEVERITY}      { MAKE_STR(); return SEVERITY; }
{STR}           { MAKE_STR(); return STR; }
.               { MAKE_STR(); return UNRECOGNIZED; }

<MSGSTATE>.*    { MAKE_STR(); yy_pop_state( yyscanner ); return MSG; }

%%

YY_BUFFER_STATE err_scan_bytes( const char * input, size_t size, yyscan_t yyscanner )
{
    YY_BUFFER_STATE ret = err__scan_bytes( input, size, yyscanner );
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
    yyg -> yyextra_r = (void*)input;
    BEGIN( INITIAL );
    return ret;
}

void err_start_MSG( yyscan_t yyscanner )
{
    yy_push_state( MSGSTATE, yyscanner );
}
