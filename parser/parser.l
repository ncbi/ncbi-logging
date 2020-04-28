%option noyywrap
%option yylineno

%{
#include <stdio.h>

#define YY_DECL int yylex()

#include "parser.tab.h"

%}

U8      [0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5]
IPV4    {U8}\.{U8}\.{U8}\.{U8}
MONTH   "Jan"|"Feb"|"Mar"|"Apr"|"May"|"Jun"|"Jul"|"Aug"|"Sep"|"Oct"|"Nov"|"Dec"
I64     [-+]?[0-9]+
FLOAT   [-+]?[0-9]+\.[0-9]+
QSTR    [ \.\-\?/a-zA-Z0-9]+
STR     [.\-a-zA-Z0-9_]+

%x QUOTED

%%

[ \t]           ;
\n              { return LF; }
\r              { return CR; }
\.              { return DOT; }
-               { return DASH; }

"port="         { return PORT; }
"rl="           { return RL; }

\"              { BEGIN( QUOTED ); return QUOTE; }
<QUOTED>\"      { BEGIN( INITIAL ); return QUOTE; }
<QUOTED>{QSTR}  { yylval . s . p = yytext; yylval . s . n = yyleng; return QSTR; }

\[              { return OB; }
\]              { return CB; }
\/              { return SLASH; }
\:              { return COLON; }

{IPV4}          { yylval . s . p = yytext; yylval . s . n = yyleng; return IPV4; }
{FLOAT}         { yylval . s . p = yytext; yylval . s . n = yyleng; return FLOAT; }
{I64}           { yylval . i64 = atol( yytext ); return I64; }
{MONTH}         { yylval . s . p = yytext; yylval . s . n = yyleng; return MONTH; }

{STR}           { yylval . s . p = yytext; yylval . s . n = yyleng; return STR; }

%%
