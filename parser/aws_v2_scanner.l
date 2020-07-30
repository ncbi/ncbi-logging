%{
#include "aws_v2_parser.hpp"
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

MONTH       "Jan"|"Feb"|"Mar"|"Apr"|"May"|"Jun"|"Jul"|"Aug"|"Sep"|"Oct"|"Nov"|"Dec"
METHOD      "GET"|"HEAD"|"POST"|"OPTIONS"|"PUT"|"DELETE"|"TRACE"|"CONNECT"|"PROPFIND"
VERS        [Hh][Tt][Tt][Pp]"/"([0-9/.]*)
I64         [-+]?[0-9]+
SPACE       " "

CHARS       [A-Za-z0-9\-_~!*'();:@+$,#\[\]^{}]
QCHARS      ${SPACE}|[.?&=%/\\]|{CHARS}

QSTR        {QCHARS}+
QSTR_ESC    (\\\"|{QCHARS})+

ACCESSION   [DES]R[RZXP][0-9]{6,9}
PATHSTR     {CHARS}+
PATHEXT     \.(\.|{PATHSTR})*

STR     [.\-a-zA-Z0-9_]+
STR1    [/:+=%@&*!~;$,#.\-a-zA-Z0-9_]+

AGENTCHARS      [A-Za-z0-9\-_~!*';:@&+$%#\[\]<>|`{}^?/]
AGENTDELIMS     [(),=\\]
AGENTSTR        {AGENTCHARS}+
OS              "linux64"|"mac64"|"win64"
VERSION23       [0-9]+(\.[0-9]+){1,2}
LIBCVERSION     "libc="{VERSION23}
SRATOOL         "abi-dump"|"align-cache"|"align-info"|"dbgap-mount-tool"|"fastdump"|"fasterq-dump"|"fastq-dump"|"illumina-dump"|"kdb-meta"|"kget"|"pileup-stats"|"prefetch"|"sam-dump"|"sff-dump"|"sra-pileup"|"sra-search"|"sra-stat"|"srapath"|"test-dependencies"|"test-sra"|"vdb-copy"|"vdb-dump"|"vdb-validate"
SRATOOLVERS     {SRATOOL}\.{VERSION23}
    /* TODO */
    /* UNKNOWNTOOLVERS      [a-z]([a-z\-])+{VERSION23} */
    /* TESTVERS             test-([a-z\-])+{VERSION23} */

PHIDCHAR        [0-9a-f]
PHIDCE          "noc"|({PHIDCHAR}{3})
PHIDGUID        "nog"|({PHIDCHAR}{4})
PHIDSESS        "nos"|({PHIDCHAR}{3})
PHIDVALUE       "phid="{PHIDCE}{PHIDGUID}{PHIDSESS}

INT4            [0-9]{4}
INT3            [0-9]{3}
INT2            [0-9]{2}
TIMEZONE        [-+]?[0-9]{4}
TIMEFMT         "["{INT2}"/"{MONTH}"/"{INT4}":"{INT2}":"{INT2}":"{INT2}" "{TIMEZONE}"]"

TLSV           [0-9]+(\.[0-9]+){0,2}
TLSVERSION     "TLSv"{TLSV}

X_AMZ_ID_2       [A-Z0-9]{21}
BASE64           [A-Za-z0-9+/]+={0,2}   
S3_EXT_REQ_ID    {BASE64}

RESULTCODE      ([0-9]{3})|"-"

%x QUOTED TIMEFMT PATH AGENT TLSVERSION HOSTID IPADDR RESCODE REFERER

%%

[ \t]+          { return SPACE; }
-               { MAKE_STR(); return DASH; }
\"-\"           { MAKE_STR(); return DASH; }

\"                  { yy_push_state( QUOTED, yyscanner ); return QUOTE; }
<QUOTED>\"          { yy_pop_state( yyscanner ); return QUOTE; }
<QUOTED>{SPACE}     { MAKE_STR(); return SPACE; }
<QUOTED>\t          { MAKE_STR(); return SPACE; }
<QUOTED>{METHOD}    { MAKE_STR(); return METHOD; }
<QUOTED>{VERS}      { MAKE_STR(); return VERS; }
<QUOTED>{QSTR}      { MAKE_STR();           return QSTR; }
<QUOTED>{QSTR_ESC}  { MAKE_STR_ESCAPED();   return QSTR; }
<QUOTED>.           { fprintf(stderr, "Unexpected character '%c' (%x)\n", yytext[0], yytext[0]); return UNRECOGNIZED; }

{STR}           { MAKE_STR(); return STR; }
{STR1}          { MAKE_STR1(); return STR1; }
.               { /* fprintf(stderr, "Unexpected character '%c' (%x)\n", yytext[0], yytext[0]); */ return UNRECOGNIZED; }

<TIMEFMT>{TIMEFMT}  { yy_pop_state( yyscanner ); MAKE_STR(); return TIME_FMT; }
<TIMEFMT>-          { yy_pop_state( yyscanner ); MAKE_STR(); return DASH; }
<TIMEFMT>\"-\"      { yy_pop_state( yyscanner ); MAKE_STR(); return DASH; }
<TIMEFMT>.          { yy_pop_state( yyscanner ); fprintf(stderr, "Unexpected char in TIMEFMT '%c' (%x)\n", yytext[0], yytext[0]); return UNRECOGNIZED; }

<PATH>{SPACE}       { yy_pop_state( yyscanner ); return SPACE; }
<PATH>\"            { yy_pop_state( yyscanner ); return QUOTE; }
<PATH>{ACCESSION}   { MAKE_STR(); return ACCESSION; }
<PATH>\/            { MAKE_STR(); return SLASH; }
<PATH>=             { MAKE_STR(); return EQUAL; }
<PATH>&             { MAKE_STR(); return AMPERSAND; }
<PATH>\?            { MAKE_STR(); return QMARK; }
<PATH>%2F           { MAKE_STR(); return SLASH; }
<PATH>%             { MAKE_STR(); return PERCENT; }
<PATH>{PATHEXT}     { MAKE_STR(); return PATHEXT; }
<PATH>{PATHSTR}     { MAKE_STR(); return PATHSTR; }
<PATH>.             { return yytext[0]; }

<AGENT>{OS}             { MAKE_STR(); return OS; }
<AGENT>"sra-toolkit"    { MAKE_STR(); return SRA_TOOLKIT; }
<AGENT>{PHIDVALUE}      { MAKE_STR(); return PHIDVALUE; }
<AGENT>{SRATOOLVERS}    { MAKE_STR(); return SRATOOLVERS; }
<AGENT>{LIBCVERSION}    { MAKE_STR(); return LIBCVERSION; }
<AGENT>{SPACE}          { MAKE_STR(); return SPACE; }
<AGENT>-                { MAKE_STR(); return DASH; }
<AGENT>\\\"             { MAKE_STR(); return AGENTSTR; }
<AGENT>\"               { MAKE_STR(); return QUOTE; }
<AGENT>{AGENTSTR}       { MAKE_STR(); return AGENTSTR; }
<AGENT>{AGENTDELIMS}    { MAKE_STR(); return AGENTSTR; }
<AGENT>.                { MAKE_STR(); return AGENTSTR; }

<TLSVERSION>{TLSVERSION}    { MAKE_STR(); return TLS_VERSION; }
<TLSVERSION>\"              { MAKE_STR(); return QUOTE; }
<TLSVERSION>-               { MAKE_STR(); return DASH; }
<TLSVERSION>.               { return yytext[0]; }

<HOSTID>{SPACE}         { MAKE_STR(); return SPACE; }
<HOSTID>\"              { MAKE_STR(); return QUOTE; }
<HOSTID>-               { MAKE_STR(); return DASH; }
<HOSTID>{X_AMZ_ID_2}    { MAKE_STR(); return X_AMZ_ID_2; }
<HOSTID>{S3_EXT_REQ_ID} { MAKE_STR(); return S3_EXT_REQ_ID; }
<HOSTID>.               { return yytext[0]; }

<IPADDR>{IPV4ADDR}      { yy_pop_state( yyscanner ); MAKE_STR(); return IPV4; }
<IPADDR>{IPV6ADDR}      { yy_pop_state( yyscanner ); MAKE_STR(); return IPV6; }
<IPADDR>-               { yy_pop_state( yyscanner ); MAKE_STR(); return DASH; }
<IPADDR>\"-\"           { yy_pop_state( yyscanner ); MAKE_STR(); return DASH; }
<IPADDR>.               { yy_pop_state( yyscanner ); fprintf(stderr, "Unexpected char in IPADDR '%c' (%x)\n", yytext[0], yytext[0]); return UNRECOGNIZED; }

<RESCODE>{INT3}         { yy_pop_state( yyscanner ); MAKE_STR(); return RESULTCODE; }
<RESCODE>-              { yy_pop_state( yyscanner ); MAKE_STR(); return DASH; }
<RESCODE>\"-\"          { yy_pop_state( yyscanner ); MAKE_STR(); return DASH; }
<RESCODE>.              { yy_pop_state( yyscanner ); fprintf(stderr, "Unexpected char in RESCODE '%c' (%x)\n", yytext[0], yytext[0]); return UNRECOGNIZED; }

<REFERER>\"             { yy_pop_state( yyscanner ); MAKE_STR(); return QUOTE; }
<REFERER>\\\"           { MAKE_STR(); return QSTR; }
<REFERER>{QSTR}         { MAKE_STR(); return QSTR; }
<REFERER>{QSTR_ESC}     { MAKE_STR_ESCAPED(); return QSTR; }
<REFERER>.              { MAKE_STR(); return QSTR; }

%%

YY_BUFFER_STATE aws_scan_reset( const char * input, yyscan_t yyscanner )
{
    YY_BUFFER_STATE ret = aws__scan_string( input, yyscanner );
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;    
    yyg -> yyextra_r = (void*)input;
    BEGIN( INITIAL );
    return ret;
}

void aws_get_scanner_input( yyscan_t yyscanner, t_str & str )
{
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;    
    const char * input = (const char*) ( yyg -> yyextra_r );
    str . p = input;
    str . n = strlen( input ); // looks wasteful but only happens when we report errors 
}

void aws_start_URL( yyscan_t yyscanner )
{
    yy_push_state( PATH, yyscanner ); 
}

void aws_start_UserAgent( yyscan_t yyscanner )
{
    yy_push_state( AGENT, yyscanner ); 
}

void aws_start_TLS_vers( yyscan_t yyscanner )
{
    yy_push_state( TLSVERSION, yyscanner ); 
}

void aws_start_host_id( yyscan_t yyscanner )
{
    yy_push_state( HOSTID, yyscanner ); 
}

void aws_start_time( yyscan_t yyscanner )
{
    yy_push_state( TIMEFMT, yyscanner ); 
}

void aws_start_ipaddr( yyscan_t yyscanner )
{
    yy_push_state( IPADDR, yyscanner ); 
}

void aws_start_rescode( yyscan_t yyscanner )
{
    yy_push_state( RESCODE, yyscanner ); 
}

void aws_start_referer( yyscan_t yyscanner )
{
    yy_push_state( REFERER, yyscanner ); 
}

void aws_pop_state( yyscan_t yyscanner )
{
    yy_pop_state( yyscanner ); 
}