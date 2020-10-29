%define api.pure full
%lex-param { void * scanner }
%parse-param { void * scanner }{ NCBI::Logging::GCPReceiver * lib }

%define parse.trace
%define parse.error verbose

%name-prefix "gcp_"

%{
#define YYDEBUG 1

#include <stdint.h>
#include "gcp_parser.hpp"
#include "gcp_scanner.hpp"

using namespace std;
using namespace NCBI::Logging;

void gcp_error( yyscan_t locp, NCBI::Logging::GCPReceiver * lib, const char* msg );

const t_str EmptyTSTR = { "", 0 };

%}

%code requires
{
#include "types.h"
#include "GCP_Interface.hpp"

extern void gcp_get_scanner_input( void * yyscanner, t_str & str );
extern void gcp_start_URL( void * yyscanner );
extern void gcp_start_UserAgent( void * yyscanner );
extern void gcp_pop_state( void * yyscanner );

using namespace NCBI::Logging;

}

%define api.value.type {t_str}

%token IPV4 IPV6 QSTR I64 PATHSTR PATHEXT SLASH
%token EQUAL AMPERSAND QMARK PERCENT
%token QUOTE COMMA UNRECOGNIZED
%token OS SRA_TOOLKIT LIBCVERSION AGENTSTR SRATOOLVERS PHIDVALUE

%start line

%%

line
    : log_gcp       { YYACCEPT; }
    | log_hdr       { lib -> SetCategory( ReceiverInterface::cat_ignored ); YYACCEPT; }
    | log_err       { YYABORT; }
    ;

log_err
    : time error                {}
    ;

log_hdr
    : hdr_item_text             {}
    | log_hdr COMMA hdr_item    {}
    ;

hdr_item_text
    : QUOTE QSTR QUOTE   {}
    | QUOTE QUOTE        {}
    ;

hdr_item
    : QUOTE QSTR QUOTE   {}
    | QUOTE I64 QUOTE    {}
    | QUOTE QUOTE        {}
    ;

log_gcp
    : time COMMA ip COMMA ip_type COMMA ip_region COMMA method COMMA
       { gcp_start_URL( scanner ); }
      url
       { gcp_pop_state( scanner ); }
      COMMA status COMMA req_bytes COMMA res_bytes COMMA time_taken COMMA host COMMA
      referrer COMMA
      { gcp_start_UserAgent( scanner ); }
      agent
      { gcp_pop_state( scanner ); }
      COMMA req_id COMMA operation COMMA bucket COMMA
      { gcp_start_URL( scanner ); }
      object
      { gcp_pop_state( scanner ); }
    ;

time
    : QUOTE I64 QUOTE       { lib->set( GCPReceiver::time, $2 ); } /* if empty, would conflict with the log_hdr rule */
    ;

ip
    : QUOTE IPV4 QUOTE      { lib->set( GCPReceiver::ip, $2 ); }
    | QUOTE IPV6 QUOTE      { lib->set( GCPReceiver::ip, $2 ); }
    | QUOTE QUOTE           { lib->set( GCPReceiver::ip, EmptyTSTR ); }
    ;

ip_type
    : q_i64 { lib->set( GCPReceiver::ip_type, $1 ); }
    ;

ip_region
    : QUOTE QSTR QUOTE      { lib->set( GCPReceiver::ip_region, $2 ); }
    | QUOTE QUOTE           { lib->set( GCPReceiver::ip_region, EmptyTSTR ); }
    ;

method
    : QUOTE QSTR QUOTE      { $$ = $2; lib->set( GCPReceiver::method, $2 ); }
    ;

status
    : q_i64 { lib->set( GCPReceiver::status, $1 ); }
    ;

req_bytes
    : q_i64 { lib->set( GCPReceiver::request_bytes, $1 ); }
    ;

res_bytes
    : q_i64 { lib->set( GCPReceiver::result_bytes, $1 ); }
    ;

time_taken
    : q_i64 { lib->set( GCPReceiver::time_taken, $1 ); }
    ;

host
    : QUOTE QSTR QUOTE  { lib->set( GCPReceiver::host, $2 ); }
    | QUOTE QUOTE       { lib->set( GCPReceiver::host, EmptyTSTR ); }
    ;

referrer
    : QUOTE QSTR QUOTE  { lib->set( GCPReceiver::referer, $2 ); }
    | QUOTE QUOTE       { lib->set( GCPReceiver::referer, EmptyTSTR ); }
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

req_id
    : QUOTE QSTR QUOTE      { lib->set( GCPReceiver::request_id, $2 ); }
    | QUOTE I64 QUOTE       { lib->set( GCPReceiver::request_id, $2 ); }
    | QUOTE QUOTE           { lib->set( GCPReceiver::request_id, EmptyTSTR ); }
    ;

operation
    : QUOTE QSTR QUOTE      { lib->set( GCPReceiver::operation, $2 ); }
    | QUOTE QUOTE           { lib->set( GCPReceiver::operation, EmptyTSTR ); }
    ;

bucket
    : QUOTE QSTR QUOTE      { lib->set( GCPReceiver::bucket, $2 ); }
    | QUOTE QUOTE           { lib->set( GCPReceiver::bucket, EmptyTSTR ); }
    ;

 /*
    'object' is saved in ReceiverInterface::path which is not the obvious choice.
    It be more logical to save 'object' in GCPReceiver::uri and 'url' in ReceiverInterface::path (the way AWS does it).
    However, changing it would invalidate already saved data in the database in case we want to reprocess them.
 */
url
    :  QUOTE PATHSTR QUOTE
    {
        lib->set( GCPReceiver::uri, $2 );
        lib -> url_for_postprocess = string( $2.p, $2.n );
    }
    |  QUOTE QUOTE
        { lib->set( GCPReceiver::uri, EmptyTSTR ); }
    ;

object
    : QUOTE PATHSTR QUOTE
        {
            lib->set( ReceiverInterface::path, $2 );
            lib -> object_for_postprocess = string( $2.p, $2.n );
        }
    |  QUOTE QUOTE
        {
            lib->set( GCPReceiver::path, EmptyTSTR );
        }
    ;

q_i64
    : QUOTE I64 QUOTE       { $$ = $2; }
    | QUOTE QUOTE           { EMPTY_TSTR($$); }
    ;

%%

void gcp_error( yyscan_t locp, NCBI::Logging::GCPReceiver * lib, const char * msg )
{
    // intentionally left empty, we communicate errors by rejecting lines
}
