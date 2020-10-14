%{
#include "agent_parser.hpp"
#include "stdint.h"

#define MAKE_STR()          do { yylval -> s . p = yytext; yylval -> s . n = yyleng; } while(false)
#define MAKE_STR_1()        do { yylval -> s . p = yytext + 1; yylval -> s . n = yyleng - 1; } while(false)

%}

%option noyywrap yylineno
%option reentrant bison-bridge
%option prefix="agent_"
%option stack
%option debug

OS              "linux64"|"mac64"|"win64"
VERSION23       [0-9]+(\.[0-9]+){1,2}
SRATOOL         "abi-dump"|"align-cache"|"align-info"|"dbgap-mount-tool"|"fastdump"|"fasterq-dump"|"fastq-dump"|"illumina-dump"|"kdb-meta"|"kget"|"pileup-stats"|"prefetch"|"sam-dump"|"sff-dump"|"sra-pileup"|"sra-search"|"sra-stat"|"srapath"|"test-dependencies"|"test-sra"|"vdb-copy"|"vdb-dump"|"vdb-validate"
    /* TODO */
    /* UNKNOWNTOOLVERS      [a-z]([a-z\-])+{VERSION23} */
    /* TESTVERS             test-([a-z\-])+{VERSION23} */

PHIDCHAR        [0-9a-f]
PHIDCE          "noc"|({PHIDCHAR}{3})
PHIDGUID        "nog"|({PHIDCHAR}{4})
PHIDSESS        "nos"|({PHIDCHAR}{3})

%x S_PHIDCE S_PHIDGUID S_PHIDSESS S_TOOL S_LIBC

%%

[ \t]+           { }

{OS}                    { MAKE_STR(); return OS; }

"phid="                 { yy_push_state( S_PHIDCE, yyscanner ); }
<S_PHIDCE>{PHIDCE}      { yy_pop_state( yyscanner ); yy_push_state( S_PHIDGUID, yyscanner ); MAKE_STR(); return T_PHIDCE; }
<S_PHIDCE>.             { yy_pop_state( yyscanner ); unput( yytext[0] ); }

<S_PHIDGUID>{PHIDGUID}  { yy_pop_state( yyscanner ); yy_push_state( S_PHIDSESS, yyscanner ); MAKE_STR(); return T_PHIDGUID; }
<S_PHIDGUID>.           { yy_pop_state( yyscanner ); unput( yytext[0] ); }

<S_PHIDSESS>{PHIDSESS}  { yy_pop_state( yyscanner ); MAKE_STR(); return T_PHIDSESS; }
<S_PHIDSESS>.           { yy_pop_state( yyscanner ); unput( yytext[0] ); }

{SRATOOL}               { yy_push_state( S_TOOL, yyscanner ); MAKE_STR(); return T_TOOL; }
<S_TOOL>\.{VERSION23}   { yy_pop_state( yyscanner ); MAKE_STR_1(); return T_TOOLVERS; }
<S_TOOL>.               { yy_pop_state( yyscanner ); unput( yytext[0] ); }

"libc="                 { yy_push_state( S_LIBC, yyscanner ); }
<S_LIBC>{VERSION23}     { yy_pop_state( yyscanner ); MAKE_STR(); return T_LIBC; }
<S_LIBC>.               { yy_pop_state( yyscanner ); unput( yytext[0] ); }

.                       { }


%%

YY_BUFFER_STATE agent_scan_bytes( const char * input, size_t size, yyscan_t yyscanner )
{
    YY_BUFFER_STATE ret = agent__scan_bytes( input, size, yyscanner );
    struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
    yyg -> yyextra_r = (void*)input;
    BEGIN( INITIAL );
    return ret;
}
