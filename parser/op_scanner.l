%{
#include "op_parser.hpp"
#include "stdint.h"

#define MAKE_STR()          do { yylval -> s . p = yytext; yylval -> s . n = yyleng; yylval -> s . escaped = false; } while(false)
#define MAKE_STR_ESCAPED()  do { yylval -> s . p = yytext; yylval -> s . n = yyleng; yylval -> s . escaped = true; }  while(false)

%}

%option noyywrap yylineno
%option reentrant bison-bridge
%option prefix="op_"
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
METHOD  "GET"|"HEAD"|"POST"|"OPTIONS"|"PUT"|"DELETE"|"TRACE"|"CONNECT"|"PROPFIND"
XVERS   "HTTP/"("1.0"|"1.1"|"2"|"2.0"|"3")
VERS    [Hh][Tt][Tt][Pp]"/"([0-9/.]*)
I64     [-+]?[0-9]+
FLOAT   [-+]?[0-9]+\.[0-9]+

STR_CHARS   [.A-Za-z0-9\-_~!*'();:@&=+$,/%#\[\]?\\<>|`{}^]

QSTR        {STR_CHARS}+
QSTR_ESC    (\\\"|{STR_CHARS})+

SPACE   " "
STR     [.\-a-zA-Z0-9_]+

%x QUOTED

%%

[ \t]           ;
\r\n            { return LF; }
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

\"                  { yy_push_state( QUOTED, yyscanner ); return QUOTE; }
<QUOTED>\"          { yy_pop_state( yyscanner ); return QUOTE; }
<QUOTED>{SPACE}     { return SPACE; }
<QUOTED>{METHOD}    { MAKE_STR(); return METHOD; }
<QUOTED>{VERS}      { MAKE_STR(); return VERS; }
<QUOTED>{QSTR}      { MAKE_STR();           return QSTR; }
<QUOTED>{QSTR_ESC}  { MAKE_STR_ESCAPED();   return QSTR; }
<QUOTED>.           ;

{IPV4ADDR}      { MAKE_STR(); return IPV4; }
{IPV6ADDR}      { MAKE_STR(); return IPV6; }
{FLOAT}         { MAKE_STR(); return FLOAT; }
{I64}           { yylval -> i64 = atol( yytext ); return I64; }
{MONTH}         { MAKE_STR(); return MONTH; }
{STR}           { MAKE_STR(); return STR; }

.               { fprintf(stderr, "Unexpected character '%c' (%x)\n", yytext[0], yytext[0]); return UNRECOGNIZED; }

%%

void op_scan_reset( yyscan_t yyscanner )
{
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;    
    BEGIN( INITIAL );
}

