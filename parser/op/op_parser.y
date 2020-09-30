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

const t_str EmptyTSTR = { "", 0, false };

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

%union
{
    t_str s;
    t_request req;
}

%token<s> STR MONTH IPV4 IPV6 FLOAT METHOD VERS QSTR QSTR_ESC SPACE SLASH QMARK
%token<s> I64
%token DOT DASH COLON QUOTE OB CB PORT RL CR LF
%token UNRECOGNIZED
%token<s> PATHSTR PATHEXT ACCESSION
%token<s> OS SRA_TOOLKIT LIBCVERSION AGENTSTR SRATOOLVERS
%token<s> PAREN_OPEN PAREN_CLOSE COMMA PHIDVALUE TIME_FMT

%type<s> time
%type<req> server_and_request request url url_token url_entry
%type<req> request_tail_elem request_tail url_with_optional_params
%type<s> ip user req_time referer forwarded method server
%type<s> quoted_list quoted_list_body quoted_list_elem url_params
%type<s> agent vdb_agent
%type<s> result_code result_len port req_len referer_token referer_list

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
    : METHOD        { $$ = $1; }
    ;

server
    : QUOTE QSTR QUOTE  { SET_VALUE( OPReceiver::server, $2 ); }
    | STR               { SET_VALUE( OPReceiver::server, $1 ); }
    ;

url_token
    : SLASH
        {
            InitRequest( $$ );
            $$ . path = $1;
        }
    | ACCESSION
        {
            InitRequest( $$ );
            $$ . path = $1;
            $$ . accession = $1;
        }
    | PATHSTR
        {
            InitRequest( $$ );
            $$ . path = $1;
            $$ . filename = $1;
        }
    | PATHEXT
        {
            InitRequest( $$ );
            $$ . path = $1;
            $$ . extension = $1;
        }
    ;

url_params
    : QMARK                 { $$ = $1; }
    | url_params SLASH      { $$ = $1; $$ . n += $2 . n; }
    | url_params ACCESSION  { $$ = $1; $$ . n += $2 . n; }
    | url_params PATHSTR    { $$ = $1; $$ . n += $2 . n; }
    | url_params PATHEXT    { $$ = $1; $$ . n += $2 . n; }
    | url_params QMARK      { $$ = $1; $$ . n += $2 . n; }
    ;

url
    : url_token
        {
            $$ = $1;
            if ( $1 . accession . n > 0 )
            {
                $$ . filename  = $1 . accession;
            }
        }
    | url url_token
        {
            $$ = $1;
            $$ . path . n += $2 . path . n;

            // clear the filename and extension after every slash - to make sure we catch only the last
            // filename and extension in case of middle segments existing
            if ( $2 . path . n == 1 && $2 . path . p[ 0 ] == '/' )
            {
                $$ . filename . n = 0;
                $$ . extension . n = 0;
            }
            else if ( $2 . accession . n > 0 )
            {
                // we will use the last non-empty accession-looking token
                $$ . accession = $2 . accession;
                $$ . filename  = $2 . accession;
            }
            else if ( $2 . filename . n > 0 )
            {
                $$ . filename  = $2 . filename;
                $$ . extension . n = 0;
            }
            else if ( $2 . extension . n > 0 )
            {
                $$ . extension = $2 . extension;
            }
        }
    ;

url_with_optional_params
    : url
        {
            $$ = $1;
        }
    | url url_params
        {
            $$ = $1;
            $$ . path . n += $2 . n;
        }
    ;

url_entry
    : SPACE { op_start_URL ( scanner ); } url_with_optional_params
        {
            $$ = $3;
            op_pop_state ( scanner );
        }
    ;

request_tail_elem
    : VERS
        {
            InitRequest( $$ );
            $$ . path = $1;
            $$ . vers = $1;
        }
    | QSTR
        {
            InitRequest( $$ );
            $$ . path = $1;
        }
    | QSTR_ESC
        {
            InitRequest( $$ );
            $$ . path = $1;
        }
    | METHOD
        {
            InitRequest( $$ );
            $$ . path = $1;
        }
    | SPACE
        {
            InitRequest( $$ );
            $$ . path = $1;
        }
    ;

request_tail
    : request_tail_elem
        { $$ = $1; }
    | request_tail request_tail_elem
        {
            $$ = $1;
            $$ . path . n += $2 . path .n;
            $$ . path . escaped = $1 . path . escaped || $2 . path . escaped;
        }
    ;

request
    : QUOTE method url_entry SPACE request_tail QUOTE
    {
        $$ = $3;
        $$.method = $2;
        $$.vers   = $5 . vers;
    }
    | QUOTE method url_entry SPACE QUOTE
    {
        $$ = $3;
        $$.method = $2;
    }
    | QUOTE method url_entry QUOTE
    {
        $$ = $3;
        $$.method = $2;

        // the scanner was in the PATH state, consumed the closing QUOTE and popped the state,
        // returning to the QUOTED state which we should get out of as well
        op_pop_state ( scanner );
    }
    | QUOTE method QUOTE
    {
        InitRequest( $$ );
        $$.method = $2;
    }
    ;

server_and_request
    : server SPACE request
    {
        lib->setRequest( $3 );
    }
    | server SPACE quoted_list
    {
        lib->reportField( "Invalid request" );
    }
    | request
    {
        SET_VALUE( OPReceiver::server, EmptyTSTR );
        lib->setRequest( $1 );
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
    | quoted_list_body SPACE quoted_list_elem   { $$ = $1; $$.n += 1 + $3.n; $$.escaped = $1.escaped || $3.escaped; }
    | quoted_list_body SPACE                    { $$ = $1; $$.n += 1; }
    | quoted_list_body METHOD                   { $$ = $1; $$.n += $2.n; $$.escaped = $1.escaped || $2.escaped; }
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

vdb_agent
    : QSTR              { $$ = $1; }
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
