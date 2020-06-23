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

QSTR              [.A-Za-z0-9\-_~!*'();:@&=+$,/%#\[\]? ^{}]+
QSTR_ESC    (\\\"|[.A-Za-z0-9\-_~!*'();:@&=+$,/%#\[\]? ^{}])+

ACCESSION   [DES]R[RZ][0-9]{6,9}
PATHSTR     [A-Za-z0-9\-_~!*'();:@&=+$,%#\[\]? ^{}]+
PATHEXT     \.{PATHSTR}*

SPACE   " "

%x QUOTED
%x PATH

%%

[ \t]           ;
\,              { return COMMA; }

\"                  { yy_push_state( QUOTED, yyscanner ); return QUOTE; }
<QUOTED>\"          { yy_pop_state( yyscanner ); return QUOTE; }
<QUOTED>{I64}       { MAKE_STR(); return I64; }
<QUOTED>{IPV4ADDR}  { MAKE_STR(); return IPV4; }
<QUOTED>{IPV6ADDR}  { MAKE_STR(); return IPV6; }
<QUOTED>{QSTR}      { MAKE_STR(); return QSTR; }
<QUOTED>{QSTR_ESC}  { MAKE_STR_ESCAPED(); return QSTR; }
<QUOTED>.           ;

<PATH>\"            { return QUOTE; }
<PATH>{ACCESSION}   { MAKE_STR(); return ACCESSION; }
<PATH>\/            { return SLASH; };
<PATH>{PATHEXT}     { MAKE_STR(); return PATHEXT; }
<PATH>{PATHSTR}     { MAKE_STR(); return PATHSTR; }
<PATH>.             { return yytext[0]; };

.               { fprintf(stderr, "Unexpected character '%c' (%x)\n", yytext[0], yytext[0]); return UNRECOGNIZED; }

%%

YY_BUFFER_STATE gcp_scan_reset( const char * input, yyscan_t yyscanner )
{
    YY_BUFFER_STATE ret = gcp__scan_string( input, yyscanner );
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;    
    yyg -> yyextra_r = (void*)input;
    BEGIN( INITIAL );
    return ret;
}

void gcp_get_scanner_input( yyscan_t yyscanner, t_str & str )
{
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;    
    const char * input = (const char*) ( yyg -> yyextra_r );
    str . p = input;
    str . n = strlen( input ); // looks wasteful but only happens when we report errors 
}

void gcp_start_URL( yyscan_t yyscanner )
{
    yy_push_state( PATH, yyscanner ); 
}

void gcp_stop_URL( yyscan_t yyscanner )
{
    yy_pop_state( yyscanner ); 
}
