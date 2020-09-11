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
VERS    [Hh][Tt][Tt][Pp]"/"([0-9/.]*)
I64     [-+]?[0-9]+
FLOAT   [-+]?[0-9]+\.[0-9]+

CHARS       [A-Za-z0-9\-_~!*'();:@&=+$,%#\[\]<>|`{}^]
STR_CHARS   "?"|"\."|"\/"|"\\"|{CHARS}|[\x00-\x1f]

QSTR        {STR_CHARS}+
QSTR_ESC    (\\\"|{STR_CHARS})+

ACCESSION   [DES]R[RZXP][0-9]{6,9}
PATHSTR     {CHARS}+
PATHEXT     \.(\.|{PATHSTR})*

SPACE   " "
STR     [.\-a-zA-Z0-9_]+

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

%x QUOTED PATH AGENT TIMEFMT

%%

"\n"            { }
[ \t]+          { return SPACE; }
-               { MAKE_STR(); return DASH; }

"port="         { return PORT; }
"rl="           { return RL; }

\"                  { yy_push_state( QUOTED, yyscanner ); return QUOTE; }
<QUOTED>\"          { yy_pop_state( yyscanner ); return QUOTE; }
<QUOTED>{SPACE}     { MAKE_STR(); return SPACE; }
<QUOTED>{METHOD}    { MAKE_STR(); return METHOD; }
<QUOTED>{VERS}      { MAKE_STR(); return VERS; }
<QUOTED>{QSTR}      { MAKE_STR();           return QSTR; }
<QUOTED>{QSTR_ESC}  { MAKE_STR_ESCAPED();   return QSTR; }
<QUOTED>.           ;                                               /* TODO: include in the output token */

<TIMEFMT>{TIMEFMT}  { yy_pop_state( yyscanner ); MAKE_STR(); return TIME_FMT; }
<TIMEFMT>.          { yy_pop_state( yyscanner ); MAKE_STR(); return UNRECOGNIZED; }

<PATH>{ACCESSION}   { MAKE_STR(); return ACCESSION; }
<PATH>{SPACE}       { MAKE_STR(); return SPACE; }
<PATH>\\\"          { MAKE_STR(); return PATHSTR; }
<PATH>\"            { MAKE_STR(); return QUOTE; }
<PATH>\?            { MAKE_STR(); return QMARK; }
<PATH>\/            { MAKE_STR(); return SLASH; };
<PATH>\\            { MAKE_STR(); return PATHSTR; };
<PATH>{PATHEXT}     { MAKE_STR(); return PATHEXT; }
<PATH>{PATHSTR}     { MAKE_STR(); return PATHSTR; }
<PATH>.             { return yytext[0]; };

<AGENT>{OS}             { MAKE_STR(); return OS; }
<AGENT>"sra-toolkit"    { MAKE_STR(); return SRA_TOOLKIT; }
<AGENT>{PHIDVALUE}      { MAKE_STR(); return PHIDVALUE; }
<AGENT>{SRATOOLVERS}    { MAKE_STR(); return SRATOOLVERS; }
<AGENT>{LIBCVERSION}    { MAKE_STR(); return LIBCVERSION; }
<AGENT>{SPACE}          { MAKE_STR(); return SPACE; }
<AGENT>\\\"             { MAKE_STR(); return AGENTSTR; }
<AGENT>\"               { MAKE_STR(); return QUOTE; }
<AGENT>{AGENTSTR}       { MAKE_STR(); return AGENTSTR; }
<AGENT>{AGENTDELIMS}    { MAKE_STR(); return AGENTSTR; }
<AGENT>.                { MAKE_STR(); return AGENTSTR; };

{IPV4ADDR}      { MAKE_STR(); return IPV4; }
{IPV6ADDR}      { MAKE_STR(); return IPV6; }
{FLOAT}         { MAKE_STR(); return FLOAT; }
{I64}           { MAKE_STR(); return I64; }
{MONTH}         { MAKE_STR(); return MONTH; }
{STR}           { MAKE_STR(); return STR; }

.               { MAKE_STR(); return UNRECOGNIZED; }

%%

YY_BUFFER_STATE op_scan_bytes( const char * input, size_t size, yyscan_t yyscanner )
{
    YY_BUFFER_STATE ret = op__scan_bytes( input, size, yyscanner );
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
    yyg -> yyextra_r = (void*)input;
    BEGIN( INITIAL );
    return ret;
}

void op_start_URL( yyscan_t yyscanner )
{
    yy_push_state( PATH, yyscanner );
}

void op_start_UserAgent( yyscan_t yyscanner )
{
    yy_push_state( AGENT, yyscanner );
}

void op_start_time( yyscan_t yyscanner )
{
    yy_push_state( TIMEFMT, yyscanner );
}

void op_pop_state( yyscan_t yyscanner )
{
    yy_pop_state( yyscanner );
}
