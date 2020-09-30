%{
#include "gcp_parser.hpp"
#include "stdint.h"

#define MAKE_STR()          do { yylval -> s . p = yytext; yylval -> s . n = yyleng; yylval -> s . escaped = false; } while(false)
#define MAKE_STR_ESCAPED()  do { yylval -> s . p = yytext; yylval -> s . n = yyleng; yylval -> s . escaped = true; }  while(false)

%}

%option noyywrap yylineno
%option reentrant bison-bridge
%option prefix="gcp_"
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

I64     [-+]?[0-9]+

CHARS       [A-Za-z0-9\-_~!*'();:@+$,#\[\] ^{}]
QCHARS      [.?&=%/\x00-\x1f]|{CHARS}

QSTR        {QCHARS}+
QSTR_ESC    (\\\"|{QCHARS})+

ACCESSION   [DES]R[RZ][0-9]{6,9}
PATHSTR     {CHARS}+
PATHEXT     \.(\.|{PATHSTR})*

SPACE   " "

%x QUOTED
%x PATH

AGENTCHARS      [A-Za-z0-9\-_~!*';:@&+$%#\[\]<>|`{}^?/(),=\\. ]
AGENTSTR        (\\\"|{AGENTCHARS})+

%x AGENT

%%

[ \t\n]             { }
\,                  { return COMMA; }

\"                  { yy_push_state( QUOTED, yyscanner ); return QUOTE; }
<QUOTED>\"          { yy_pop_state( yyscanner ); return QUOTE; }
<QUOTED>{I64}       { MAKE_STR(); return I64; }
<QUOTED>{IPV4ADDR}  { MAKE_STR(); return IPV4; }
<QUOTED>{IPV6ADDR}  { MAKE_STR(); return IPV6; }
<QUOTED>{QSTR}      { MAKE_STR(); return QSTR; }
<QUOTED>{QSTR_ESC}  { MAKE_STR_ESCAPED(); return QSTR; }
<QUOTED>.           { }

<PATH>\"            { return QUOTE; }
<PATH>{ACCESSION}   { MAKE_STR(); return ACCESSION; }
<PATH>\/            { MAKE_STR(); return SLASH; };
<PATH>=             { MAKE_STR(); return EQUAL; };
<PATH>&             { MAKE_STR(); return AMPERSAND; };
<PATH>\?            { MAKE_STR(); return QMARK; };
<PATH>%2F           { MAKE_STR(); return SLASH; };
<PATH>%             { MAKE_STR(); return PERCENT; };
<PATH>{PATHEXT}     { MAKE_STR(); return PATHEXT; }
<PATH>{PATHSTR}     { MAKE_STR(); return PATHSTR; }
<PATH>.             { return yytext[0]; };

<AGENT>\"           { MAKE_STR(); return QUOTE; }
<AGENT>{AGENTSTR}   { MAKE_STR(); return QSTR; }
<AGENT>.            { MAKE_STR(); return QSTR; }

.                   { MAKE_STR(); return UNRECOGNIZED; }

%%

YY_BUFFER_STATE gcp_scan_bytes( const char * input, size_t size, yyscan_t yyscanner )
{
    YY_BUFFER_STATE ret = gcp__scan_bytes( input, size, yyscanner );
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
    yyg -> yyextra_r = (void*)input;
    BEGIN( INITIAL );
    return ret;
}

void gcp_start_URL( yyscan_t yyscanner )
{
    yy_push_state( PATH, yyscanner );
}

void gcp_start_UserAgent( yyscan_t yyscanner )
{
    yy_push_state( AGENT, yyscanner );
}

void gcp_pop_state( yyscan_t yyscanner )
{
    yy_pop_state( yyscanner );
}
