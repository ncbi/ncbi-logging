%{
#include "cl_parser.hpp"
#include "stdint.h"

#define MAKE_STR()          do { yylval -> p = yytext; yylval -> n = yyleng; } while(false)

%}

%option noyywrap yylineno
%option reentrant bison-bridge
%option prefix="cl_"
%option stack

%option debug

 /*%x */

MONTH       "Jan"|"Feb"|"Mar"|"Apr"|"May"|"Jun"|"Jul"|"Aug"|"Sep"|"Oct"|"Nov"|"Dec"
DIGITS1_2   [0-9]{1,2}
SPACE       " "
DATETIME1   {MONTH}{SPACE}{DIGITS1_2}{SPACE}{DIGITS1_2}:{DIGITS1_2}:{DIGITS1_2}

INT4            [0-9]{4}
INT3            [0-9]{3}
INT2            [0-9]{2}
TIMEZONE        [-+]{INT2}":"{INT2}
DATETIME2       {INT4}"-"{INT2}"-"{INT2}"T"{INT2}":"{INT2}":"{INT2}"\."{INT3}{TIMEZONE}
DATETIME3       {INT4}"-"{INT2}"-"{INT2}" "{INT2}":"{INT2}":"{INT2}","{INT3}

ASCII_NO_PIPE     [\x21-\x7B\x7D-\xFF]
STR             {ASCII_NO_PIPE}+

%%

[ \t]+          { MAKE_STR(); return SPACE; }
"|"             { MAKE_STR(); return PIPE; }
{DATETIME1}     { MAKE_STR(); return DATETIME1; }
{DATETIME2}     { MAKE_STR(); return DATETIME2; }
{DATETIME3}     { MAKE_STR(); return DATETIME3; }
"[".*"]"        { MAKE_STR(); return REQID; }

{STR}           { MAKE_STR(); return STR; }

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

