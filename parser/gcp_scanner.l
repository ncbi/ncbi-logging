%{
#include "gcp_parser.hpp"
#include "stdint.h"
#include "assert.h"
%}

%option noyywrap yylineno
%option reentrant bison-bridge
%option prefix="gcp_"
%option stack

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

I64     [-+]?[0-9]+
QSTR    [.A-Za-z0-9\-_~!*'();:@&=+$,/%#\[\]?]+

SPACE   " "
STR     [.\-a-zA-Z0-9_]+
STR1    {STR}([/:/+=][.\-a-zA-Z0-9_]*)+

%x QUOTED

%%

[ \t]           ;
\r\n            { return LF; }
\n              { return LF; }
\r              { return CR; }
\.              { return DOT; }
\,              { return COMMA; }
-               { yylval -> s . p = yytext; yylval -> s . n = yyleng; return DASH; }
\/              { return SLASH; }
\:              { return COLON; }

\"                  { assert( yyscanner ); yy_push_state( QUOTED, yyscanner ); return QUOTE; }
<QUOTED>\"          { assert( yyscanner ); yy_pop_state( yyscanner ); return QUOTE; }
<QUOTED>{SPACE}     { return SPACE; }
<QUOTED>{I64}       { yylval -> i64 = atol( yytext ); return I64; }
<QUOTED>{IPV4ADDR}  { yylval -> s . p = yytext; yylval -> s . n = yyleng; return IPV4; }
<QUOTED>{IPV6ADDR}  { yylval -> s . p = yytext; yylval -> s . n = yyleng; return IPV6; }
<QUOTED>{QSTR}      { yylval -> s . p = yytext; yylval -> s . n = yyleng; return QSTR; }
<QUOTED>.           ;

{IPV4ADDR}      { yylval -> s . p = yytext; yylval -> s . n = yyleng; return IPV4; }
{IPV6ADDR}      { yylval -> s . p = yytext; yylval -> s . n = yyleng; return IPV6; }
{I64}           { yylval -> i64 = atol( yytext ); return I64; }
{STR1}          { yylval -> s . p = yytext; yylval -> s . n = yyleng; return STR1; }
{STR}           { yylval -> s . p = yytext; yylval -> s . n = yyleng; return STR; }

%%
