%{
#include "log1_parser.h"
#include "stdint.h"
%}

%option noyywrap yylineno
%option reentrant bison-bridge
%option prefix="log1_"

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
METHOD  "GET"|"HEAD"|"POST"|"OPTIONS"|"PUT"|"DELETE"|"TRACE"|"CONNECT"
VERS    "HTTP/"("1.0"|"1.1"|"2"|"3")
I64     [-+]?[0-9]+
FLOAT   [-+]?[0-9]+\.[0-9]+
QSTR    [.A-Za-z0-9\-_~!*'();:@&=+$,/%#\[\]]+

SPACE   " "
QMARK   \?
STR     [.\-a-zA-Z0-9_]+

%x QUOTED

%%

[ \t]           ;
\n              { return LF; }
\r              { return CR; }
\.              { return DOT; }
-               { return DASH; }
\[              { return OB; }
\]              { return CB; }
\/              { return SLASH; }
\:              { return COLON; }

"port="         { return PORT; }
"rl="           { return RL; }

\"                  { BEGIN( QUOTED ); return QUOTE; }
<QUOTED>\"          { BEGIN( INITIAL ); return QUOTE; }
<QUOTED>{SPACE}     { return SPACE; }
<QUOTED>{QMARK}     { return QMARK; }
<QUOTED>{METHOD}    { yylval -> s . p = yytext; yylval -> s . n = yyleng; return METHOD; }
<QUOTED>{VERS}      { yylval -> s . p = yytext; yylval -> s . n = yyleng; return VERS; }
<QUOTED>{QSTR}      { yylval -> s . p = yytext; yylval -> s . n = yyleng; return QSTR; }

{IPV4ADDR}      { yylval -> s . p = yytext; yylval -> s . n = yyleng; return IPV4; }
{IPV6ADDR}      { yylval -> s . p = yytext; yylval -> s . n = yyleng; return IPV6; }
{FLOAT}         { yylval -> s . p = yytext; yylval -> s . n = yyleng; return FLOAT; }
{I64}           { yylval -> i64 = atol( yytext ); return I64; }
{MONTH}         { yylval -> s . p = yytext; yylval -> s . n = yyleng; return MONTH; }
{STR}           { yylval -> s . p = yytext; yylval -> s . n = yyleng; return STR; }

%%
