%{
#include "cl_parser.hpp"
#include "stdint.h"

#define MAKE_STR()          do { yylval -> s . p = yytext; yylval -> s . n = yyleng; } while(false)

%}

%option noyywrap yylineno
%option reentrant bison-bridge
%option prefix="cl_"
%option stack

%option debug

 /*%x */

%%

.               { MAKE_STR(); return UNRECOGNIZED; }

%%

YY_BUFFER_STATE cl_scan_bytes( const char * input, size_t size, yyscan_t yyscanner )
{
    YY_BUFFER_STATE ret = cl__scan_bytes( input, size, yyscanner );
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
    yyg -> yyextra_r = (void*)input;
    BEGIN( INITIAL );
    return ret;
}

