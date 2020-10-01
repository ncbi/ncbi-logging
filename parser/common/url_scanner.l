%{
#include "url_parser.hpp"

//TODO: convert into a function
#define MAKE_STR() \
    do { \
        yylval -> s . str . p = yytext; \
        yylval -> s . str . n = yyleng; \
        yylval -> s . str . escaped = false; \
        yylval -> s . mode = acc_before; \
        EMPTY_TSTR( yylval -> s . filename ); \
        EMPTY_TSTR( yylval -> s . extension ); \
        yylval -> s . apply_accession = false; \
    } while(false)

%}

%option noyywrap yylineno
%option reentrant bison-bridge
%option prefix="url_"
%option stack
%option debug

ACCESSION   [DES]R[RZ][0-9]{6,9}
DELIMITER   (%|%2C)+
SLASH       (\/|%2F)
CHARS       [A-Za-z0-9\-_~!*'();:@+$,#\[\] ^{}]
PATHSTR     {CHARS}+
PATHEXT     \.(\.|{PATHSTR})*

 /*%x*/

%%

[ \t]+          { /*TODO: has to be URL-encodeed, so an error? */ }

{ACCESSION}     { MAKE_STR(); return ACCESSION; }
{SLASH}         { MAKE_STR(); return SLASH; }
{DELIMITER}     { MAKE_STR(); return DELIMITER; }
{PATHSTR}       { MAKE_STR(); return PATHSTR; }
{PATHEXT}       { MAKE_STR(); return PATHEXT; }

.               { /*TODO: ignore or return? */ }

%%

YY_BUFFER_STATE url_scan_bytes( const char * input, size_t size, yyscan_t yyscanner )
{
    YY_BUFFER_STATE ret = url__scan_bytes( input, size, yyscanner );
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
    yyg -> yyextra_r = (void*)input;
    BEGIN( INITIAL );
    return ret;
}
