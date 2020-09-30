%define api.pure full
%lex-param { void * scanner }
%parse-param { void * scanner }{ NCBI::Logging::AGENTReceiver * lib }

%define parse.trace
%define parse.error verbose

%name-prefix "agent_"

%{
#define YYDEBUG 1

#include <stdint.h>
#include "agent_parser.hpp"
#include "agent_scanner.hpp"

using namespace std;
using namespace NCBI::Logging;

void agent_error( yyscan_t locp, NCBI::Logging::AGENTReceiver * lib, const char* msg );

const t_str EmptyTSTR = { "", 0, false };

%}

%code requires
{
#include "types.h"
#include "AGENT_Interface.hpp"

extern void agent_get_scanner_input( void * yyscanner, t_str & str );

using namespace NCBI::Logging;
}

%union
{
    t_str s;
}

%token<s> OS T_PHIDCE T_PHIDGUID T_PHIDSESS T_TOOL T_TOOLVERS T_LIBC
%type<s> agent_token token_list

%start agent_line

%%

agent_line
    : error     { YYABORT; }
    | %empty        {}
    | token_list    {}
    ;

/* duplicate calls to lib->set() with same member-id will be ignored */

agent_token
    : OS            { lib -> set( AGENTReceiver::vdb_os, $1 ); }
    | T_PHIDCE      { lib -> set( AGENTReceiver::vdb_phid_compute_env, $1 ); }
    | T_PHIDGUID    { lib -> set( AGENTReceiver::vdb_phid_guid, $1 ); }
    | T_PHIDSESS    { lib -> set( AGENTReceiver::vdb_phid_session_id, $1 ); }
    | T_TOOL        { lib -> set( AGENTReceiver::vdb_tool, $1 ); }
    | T_TOOLVERS    { lib -> set( AGENTReceiver::vdb_release, $1 ); }
    | T_LIBC        { lib -> set( AGENTReceiver::vdb_libc, $1 ); }
    ;

token_list
    : agent_token
    | token_list agent_token
    ;

%%

void agent_error( yyscan_t locp, NCBI::Logging::AGENTReceiver * lib, const char * msg )
{
    // intentionally left empty, we communicate errors rejected lines
}
