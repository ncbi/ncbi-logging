%{
#include "aws_parser.hpp"
#include "stdint.h"

#define MAKE_STR()          do { yylval -> p = yytext; yylval -> n = yyleng; /*printf("\nSTR %s\n", yytext );*/ } while(false)
#define MAKE_STR1()         do { yylval -> p = yytext; yylval -> n = yyleng; /*printf("\nSTR1 %s\n", yytext );*/ } while(false)

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
CTRLCHARS   [\x00-\x08\x0a-\x1f]
QCHARS      [.?&=%/\\]|{CHARS}|{CTRLCHARS}

QSTR        {QCHARS}+
QSTR_ESC    (\\\"|{QCHARS})+

PATHSTR     (\\\"|[^ "])+

STR         [.\-a-zA-Z0-9_]+
STR1        [/:+=%@&*!~;$\],#.\-a-zA-Z0-9_\[]+

AGENTCHARS      [A-Za-z0-9\-_~!*';:@&+$%#\[\]<>|`{}^?/(),=\\. ]
AGENTSTR        (\\\"|{AGENTCHARS})+

INT4            [0-9]{4}
INT3            [0-9]{3}
INT2            [0-9]{2}
TIMEZONE        [-+]?[0-9]{4}
TIMEFMT         "["{INT2}"/"{MONTH}"/"{INT4}":"{INT2}":"{INT2}":"{INT2}" "{TIMEZONE}"]"

TLSV            [0-9]+(\.[0-9]+){0,2}
TLSVERSION      "TLSv"{TLSV}

X_AMZ_ID_2      [A-Z0-9]{21}
BASE64          [A-Za-z0-9+/]+={0,2}
S3_EXT_REQ_ID   {BASE64}

RESULTCODE      ([0-9]{3})|"-"

%x QUOTED TIMEFMT KEY PATH AGENT_OPEN AGENT TLSVERSION HOSTID IPADDR RESCODE REFERER REST_OF_LINE

%%

"\n"            { }
[ \t]+          { return SPACE; }
-               { MAKE_STR(); return DASH; }
\"-\"           { MAKE_STR(); return DASH; }

\"                  { yy_push_state( QUOTED, yyscanner ); return QUOTE; }
<QUOTED>\"          { yy_pop_state( yyscanner ); return QUOTE; }
<QUOTED>{SPACE}     { MAKE_STR(); return SPACE; }
<QUOTED>\t          { MAKE_STR(); return SPACE; }
<QUOTED>{METHOD}    { MAKE_STR(); return METHOD; }
<QUOTED>{VERS}      { MAKE_STR(); return VERS; }
<QUOTED>{QSTR}      { MAKE_STR(); return QSTR; }
<QUOTED>{QSTR_ESC}  { MAKE_STR(); return QSTR; }
<QUOTED>.           { MAKE_STR(); return UNRECOGNIZED; }

{STR}           { MAKE_STR(); return STR; }
{STR1}          { MAKE_STR1(); return STR1; }
.               { MAKE_STR(); return UNRECOGNIZED; }

<TIMEFMT>{TIMEFMT}  { yy_pop_state( yyscanner ); MAKE_STR(); return TIME_FMT; }
<TIMEFMT>-          { yy_pop_state( yyscanner ); MAKE_STR(); return DASH; }
<TIMEFMT>\"-\"      { yy_pop_state( yyscanner ); MAKE_STR(); return DASH; }
<TIMEFMT>.          { yy_pop_state( yyscanner ); MAKE_STR(); return UNRECOGNIZED; }

<PATH>{SPACE}       { yy_pop_state( yyscanner ); return SPACE; }
<PATH>\"            { yy_pop_state( yyscanner ); return QUOTE; }
<PATH>{PATHSTR}     { MAKE_STR(); return PATHSTR; }
<PATH>.             { yy_pop_state( yyscanner ); MAKE_STR(); return UNRECOGNIZED; }

<KEY>{SPACE}        { yy_pop_state( yyscanner ); return SPACE; }
<KEY>\"             { yy_pop_state( yyscanner ); return QUOTE; }
<KEY>{PATHSTR}      { MAKE_STR(); return PATHSTR; }
<KEY>.              { yy_pop_state( yyscanner ); MAKE_STR(); return UNRECOGNIZED; }

    /* Agent can be a naked dash or a quoted string.
       In case of quoted string (the AGENT state) we consume everything between the quotes and
       return it as a sequence of STR tokens */
<AGENT_OPEN>\"      { yy_pop_state( yyscanner ); yy_push_state( AGENT, yyscanner ); MAKE_STR(); return QUOTE; }
<AGENT_OPEN>-       { yy_pop_state( yyscanner ); MAKE_STR(); return DASH; }

<AGENT>\"           { yy_pop_state( yyscanner ); MAKE_STR(); return QUOTE; }
<AGENT>{AGENTSTR}   { MAKE_STR(); return STR; }
<AGENT>.            { MAKE_STR(); return STR; }

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
<IPADDR>.               { yy_pop_state( yyscanner ); MAKE_STR(); return UNRECOGNIZED; }

<RESCODE>{INT3}         { yy_pop_state( yyscanner ); MAKE_STR(); return RESULTCODE; }
<RESCODE>-              { yy_pop_state( yyscanner ); MAKE_STR(); return DASH; }
<RESCODE>\"-\"          { yy_pop_state( yyscanner ); MAKE_STR(); return DASH; }
<RESCODE>.              { yy_pop_state( yyscanner ); MAKE_STR(); return UNRECOGNIZED; }

<REFERER>\"             { yy_pop_state( yyscanner ); MAKE_STR(); return QUOTE; }
<REFERER>\\\"           { MAKE_STR(); return QSTR; }
<REFERER>{QSTR}         { MAKE_STR(); return QSTR; }
<REFERER>{QSTR_ESC}     { MAKE_STR(); return QSTR; }
<REFERER>.              { MAKE_STR(); return QSTR; }

<REST_OF_LINE>-         { MAKE_STR(); return DASH; }
<REST_OF_LINE>.*        { MAKE_STR(); return STR; }

%%

YY_BUFFER_STATE aws_scan_bytes( const char * input, size_t size, yyscan_t yyscanner )
{
    YY_BUFFER_STATE ret = aws__scan_bytes( input, size, yyscanner );
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
    yyg -> yyextra_r = (void*)input;
    BEGIN( INITIAL );
    return ret;
}

void aws_start_URL( yyscan_t yyscanner )
{
    yy_push_state( PATH, yyscanner );
}

void aws_start_key( yyscan_t yyscanner )
{
    yy_push_state( KEY, yyscanner );
}

void aws_start_UserAgent( yyscan_t yyscanner )
{
    yy_push_state( AGENT_OPEN, yyscanner );
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

void aws_start_rest_of( yyscan_t yyscanner )
{
    yy_push_state( REST_OF_LINE, yyscanner );
}

void aws_pop_state( yyscan_t yyscanner )
{
    yy_pop_state( yyscanner );
}