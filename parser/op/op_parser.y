%define api.pure full
%lex-param { void * scanner }
%parse-param { void * scanner }{ NCBI::Logging::OPReceiver * lib }

%define parse.trace
%define parse.error verbose

%name-prefix "op_"

%{
#define YYDEBUG 1

#include <stdint.h>
#include "op_parser.hpp"
#include "op_scanner.hpp"

using namespace std;
using namespace NCBI::Logging;

void op_error( yyscan_t locp, NCBI::Logging::OPReceiver * lib, const char* msg );

const t_str EmptyTSTR = { "", 0 };

#define SET_VALUE( selector, source )   ( lib -> set( (selector), (source) ) )

%}

%code requires
{
#include "types.h"
#include "OP_Interface.hpp"

extern void op_get_scanner_input( void * yyscanner, t_str & str );
extern void op_start_URL( void * yyscanner );
extern void op_start_UserAgent( void * yyscanner );
extern void op_start_time( void * yyscanner );

extern void op_pop_state( void * yyscanner );

using namespace NCBI::Logging;
}

%define api.value.type {t_str}

%token STR IPV4 IPV6 FLOAT I64 METHOD VERS QSTR QSTR_ESC SPACE
%token DASH QUOTE PORT RL UNRECOGNIZED
%token PATHSTR AGENTSTR TIME_FMT

%start line

%%

line
    : log_onprem        { YYACCEPT; }
    | log_onprem_err    { YYABORT; }
    ;

log_onprem
    : ip SPACE
      DASH SPACE
      user SPACE
      { op_start_time( scanner ); } time SPACE
      server_and_request  SPACE
      result_code  SPACE
      result_len  SPACE
      req_time  SPACE
      referer SPACE
      { op_start_UserAgent( scanner ); } agent { op_pop_state( scanner ); } SPACE
      forwarded  SPACE
      port  SPACE
      req_len
    {
        // in case the productions did not find vers/path, set them here to make them at least empty in the ouput
        SET_VALUE( OPReceiver::vers, EmptyTSTR );
        SET_VALUE( OPReceiver::path, EmptyTSTR );
    }
    ;

log_onprem_err
    : ip error
    ;

ip
    : IPV4  { SET_VALUE( OPReceiver::ip, $1 ); }
    | IPV6  { SET_VALUE( OPReceiver::ip, $1 ); }
    ;

user
    : DASH  { SET_VALUE( OPReceiver::user, EmptyTSTR ); }
    | STR   { SET_VALUE( OPReceiver::user, $1 ); }
    ;

time
    : TIME_FMT { SET_VALUE( OPReceiver::time, $1 ); }
    ;

method
    : METHOD            { SET_VALUE( OPReceiver::method, $1 ); }
    ;

server
    : QUOTE QSTR QUOTE  { SET_VALUE( OPReceiver::server, $2 ); }
    | STR               { SET_VALUE( OPReceiver::server, $1 ); }
    ;

space_and_url
    : SPACE { op_start_URL ( scanner ); } PATHSTR
        {
            lib -> url_for_postprocess = string( $3.p, $3.n );
            SET_VALUE( OPReceiver::path, $3 );
            op_pop_state ( scanner );
        }
    ;

request_tail_elem
    : VERS      { SET_VALUE( OPReceiver::vers, $1 ); }
    | QSTR      { }
    | QSTR_ESC  { }
    | METHOD    { }
    | SPACE     { }
    ;

request_tail
    : request_tail_elem                 { }
    | request_tail request_tail_elem    { }
    ;

request
    : QUOTE method space_and_url SPACE request_tail QUOTE { }
    | QUOTE method space_and_url SPACE QUOTE { }
    | QUOTE method space_and_url QUOTE { }
    | QUOTE method QUOTE { }
    ;

server_and_request
    : server SPACE request              {  }
    | server SPACE quoted_list
    {
        lib->reportField( "Invalid request" );
    }
    | request
    {
        SET_VALUE( OPReceiver::server, EmptyTSTR );
    }
    ;

quoted_list
    : QUOTE quoted_list_body QUOTE  { $$ = $2; }
    | QUOTE QUOTE                   { EMPTY_TSTR($$); }
    ;

quoted_list_elem
    : QSTR      { $$ = $1; }
    | VERS      { $$ = $1; }
    | QSTR_ESC  { $$ = $1; }
    ;

quoted_list_body
    : quoted_list_elem                          { $$ = $1; }
    | quoted_list_body SPACE quoted_list_elem   { $$ = $1; $$.n += 1 + $3.n; }
    | quoted_list_body SPACE                    { $$ = $1; $$.n += 1; }
    | quoted_list_body METHOD                   { $$ = $1; $$.n += $2.n; }
    ;

result_code
    : I64 { SET_VALUE( OPReceiver::res_code, $1 ); }
    ;

result_len
    : I64   { SET_VALUE( OPReceiver::res_len, $1 ); }
    | DASH  { SET_VALUE( OPReceiver::res_len, EmptyTSTR ); }
    ;

req_time
    : FLOAT { SET_VALUE( OPReceiver::req_time, $1 ); }
    | I64   { SET_VALUE( OPReceiver::req_time, $1 ); }
    ;

referer_token
    : QSTR
    | SPACE
    | VERS
    | QSTR_ESC
    | METHOD
    ;

referer_list
    : referer_token
    | referer_list referer_token    { $$ = $1; MERGE_TSTR( $$, $2 ); }
    ;

referer
    : QUOTE referer_list QUOTE  { SET_VALUE( OPReceiver::referer, $2 ); }
    | QUOTE QUOTE               { SET_VALUE( OPReceiver::referer, EmptyTSTR ); }
    ;

// todo: convert vdb-agent to a single token
vdb_agent
    : QSTR               { $$ = $1; }
    | vdb_agent QSTR     { $$ = $1; MERGE_TSTR( $$, $2 ); }
    ;

agent
    : QUOTE vdb_agent QUOTE
        {
            lib -> set( ReceiverInterface::agent, $2 );
            lib -> agent_for_postprocess = string( $2.p, $2.n );
        }
    | QUOTE QUOTE       { lib -> set( ReceiverInterface::agent, EmptyTSTR ); }
    ;

forwarded
    : QUOTE QSTR QUOTE { SET_VALUE( OPReceiver::forwarded, $2 ); }
    ;

port
    : PORT I64 { SET_VALUE( OPReceiver::port, $2 ); }
    ;

req_len
    : RL I64 { SET_VALUE( OPReceiver::req_len, $2 ); }
    ;

%%

void op_error( yyscan_t locp, NCBI::Logging::OPReceiver * lib, const char * msg )
{
    // intentionally left empty, we communicate errors rejected lines
}
