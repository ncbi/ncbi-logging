%{
#include "msg_parser.hpp"
#include "stdint.h"

#define MAKE_STR() do { yylval -> p = yytext; yylval -> n = yyleng; } while(false)

%}

%option noyywrap yylineno
%option reentrant bison-bridge
%option prefix="msg_"
%option stack
%option debug

STR         [\x21-\xFF]+

    /*no ',' or '"' */
STR_VALUE    [\x21\x23-\x2B\x2D-\xFF]+

STR_NOQUOTE    [\x21\x23-\xFF]+

%x VALUESTATE REQUESTSTATE

%%

" "+            { MAKE_STR(); return SPACE; }
"client:"       { MAKE_STR(); return CLIENT; }
"server:"       { MAKE_STR(); return SERVER; }
"request:"      { MAKE_STR(); return REQUEST; }
"host:"         { MAKE_STR(); return HOST; }
{STR}           { MAKE_STR(); return STR; }
.               { MAKE_STR(); return UNRECOGNIZED; }

<VALUESTATE>\"              { MAKE_STR(); return QUOTE; }
<VALUESTATE>{STR_VALUE}     { MAKE_STR(); return STR; }
<VALUESTATE>.               { MAKE_STR(); return UNRECOGNIZED; }

<REQUESTSTATE>\"            { MAKE_STR(); return QUOTE; }
<REQUESTSTATE>" "+          { MAKE_STR(); return SPACE; }
<REQUESTSTATE>{STR_NOQUOTE} { MAKE_STR(); return STR; }
<REQUESTSTATE>.             { MAKE_STR(); return UNRECOGNIZED; }

%%

YY_BUFFER_STATE msg_scan_bytes( const char * input, size_t size, yyscan_t yyscanner )
{
    YY_BUFFER_STATE ret = msg__scan_bytes( input, size, yyscanner );
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
    yyg -> yyextra_r = (void*)input;
    BEGIN( INITIAL );
    return ret;
}

void msg_start_VALUE( yyscan_t yyscanner )
{
    yy_push_state( VALUESTATE, yyscanner );
}

void msg_start_REQUEST( yyscan_t yyscanner )
{
    yy_push_state( REQUESTSTATE, yyscanner );
}

void msg_pop_state( yyscan_t yyscanner )
{
    yy_pop_state( yyscanner );
}