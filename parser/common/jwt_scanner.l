%{
#include "jwt_parser.hpp"
//#include "jwt_scanner.hpp"
#include "stdint.h"

#define MAKE_STR()          do { yylval -> s . p = yytext; yylval -> s . n = yyleng; yylval -> s . escaped = false; } while(false)

%}

%option noyywrap yylineno
%option reentrant bison-bridge
%option prefix="jwt_"
%option stack
%option debug

%%

.*               { MAKE_STR(); return STR; }

%%

YY_BUFFER_STATE jwt_scan_bytes( const char * input, size_t size, yyscan_t yyscanner )
{
    YY_BUFFER_STATE ret = jwt__scan_bytes( input, size, yyscanner );
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
    yyg -> yyextra_r = (void*)input;
    BEGIN( INITIAL );
    return ret;
}
