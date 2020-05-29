%{
#include "aws_parser.hpp"
#include "stdint.h"

#define MAKE_STR()          do { yylval -> s . p = yytext; yylval -> s . n = yyleng; yylval -> s . escaped = false; /*printf("\nSTR %s\n", yytext );*/ } while(false)
#define MAKE_STR1()         do { yylval -> s . p = yytext; yylval -> s . n = yyleng; yylval -> s . escaped = false; /*printf("\nSTR1 %s\n", yytext );*/ } while(false)
#define MAKE_STR_ESCAPED()  do { yylval -> s . p = yytext; yylval -> s . n = yyleng; yylval -> s . escaped = true; }  while(false)

%}

%option noyywrap yylineno
%option reentrant bison-bridge
%option prefix="aws_"
%option stack

%option debug

IPV4SEG     (25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])
IPV4ADDR    ({IPV4SEG}\.){3,3}{IPV4SEG}

IPV6SEG     [0-9a-fA-F]{1,4}
IPV6_1      ({IPV6SEG}:){7,7}{IPV6SEG}
IPV6_2      ({IPV6SEG}:){1,7}:
IPV6_3      ({IPV6SEG}:){1,6}:{IPV6SEG}
IPV6_4      ({IPV6SEG}:){1,5}(:{IPV6SEG}){1,2}
IPV6_5      ({IPV6SEG}:){1,4}(:{IPV6SEG}){1,3}
IPV6_6      ({IPV6SEG}:){1,3}(:{IPV6SEG}){1,4}
IPV6_7      ({IPV6SEG}:){1,2}(:{IPV6SEG}){1,5}
IPV6_8      {IPV6SEG}:((:{IPV6SEG}){1,6})
IPV6_9      :((:{IPV6SEG}){1,7}|:)
IPV6_10     fe80:(:{IPV6SEG}){0,4}%[0-9a-zA-Z]{1,}
IPV6_11     ::(ffff(:0{1,4}){0,1}:){0,1}{IPV4ADDR}
IPV6_12     ({IPV6SEG}:){1,4}:{IPV4ADDR}
IPV6ADDR   {IPV6_1}|{IPV6_2}|{IPV6_3}|{IPV6_4}|{IPV6_5}|{IPV6_6}|{IPV6_7}|{IPV6_8}|{IPV6_9}|{IPV6_10}|{IPV6_11}|{IPV6_12}

MONTH   "Jan"|"Feb"|"Mar"|"Apr"|"May"|"Jun"|"Jul"|"Aug"|"Sep"|"Oct"|"Nov"|"Dec"
I64     [-+]?[0-9]+
QSTR              [.A-Za-z0-9\-_~!*'();:@&=+$,/%#\[\] ?]+
QSTR_ESC    (\\\"|[.A-Za-z0-9\-_~!*'();:@&=+$,/%#\[\] ?])+

SPACE   " "
STR     [.\-a-zA-Z0-9_]+
STR1    [/:+=%@&*!~;$,#.\-a-zA-Z0-9_]+

%x QUOTED TIMEFMT

%%

[ \t]+          { return SPACE; }
\r\n            { return LF; }
\n              { return LF; }
\r              { return CR; }
\.              { return DOT; }
-               { MAKE_STR(); return DASH; }
\]              { return CB; }

\"                  { yy_push_state( QUOTED, yyscanner ); return QUOTE; }
<QUOTED>\"          { yy_pop_state( yyscanner ); return QUOTE; }
<QUOTED>{QSTR}      { MAKE_STR();           return QSTR; }
<QUOTED>{QSTR_ESC}  { MAKE_STR_ESCAPED();   return QSTR; }
<QUOTED>.           ;

\[                  { yy_push_state( TIMEFMT, yyscanner ); return OB; }
<TIMEFMT>\]         { yy_pop_state( yyscanner ); return CB; }
<TIMEFMT>{I64}      { MAKE_STR(); return I64; }
<TIMEFMT>\:         { return COLON; }
<TIMEFMT>\/         { return SLASH; }
<TIMEFMT>[ \t]      { ; }
<TIMEFMT>{MONTH}    { MAKE_STR(); return MONTH; }

{IPV4ADDR}      { MAKE_STR(); return IPV4; }
{IPV6ADDR}      { MAKE_STR(); return IPV6; }
{MONTH}         { MAKE_STR(); return MONTH; }
{STR}           { MAKE_STR(); return STR; }
{STR1}          { MAKE_STR1(); return STR1; }

.               { fprintf(stderr, "Unexpected character '%c' (%x)\n", yytext[0], yytext[0]); return UNRECOGNIZED; }

%%
