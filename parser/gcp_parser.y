%define api.pure full
%lex-param { void * scanner }
%parse-param { void * scanner }{ NCBI::Logging::GCP_LogLines * lib }

%define parse.trace
%define parse.error verbose

%name-prefix "gcp_"

%{
#define YYDEBUG 1

#include <stdint.h>
#include "parser-functions.h"
#include "log_lines.hpp"
#include "gcp_parser.hpp"
#include "gcp_scanner.hpp"
#include "helper.hpp"

using namespace std;
using namespace NCBI::Logging;

void gcp_error( yyscan_t locp, NCBI::Logging::GCP_LogLines * lib, const char* msg );

%}

%code requires
{
#include "types.h"
#include "log_lines.hpp"

extern void gcp_input( void * yyscanner, t_str & str );

using namespace NCBI::Logging;

}

%union
{
    t_str s;
}

%token<s> IPV4 IPV6 QSTR I64
%token QUOTE COMMA UNRECOGNIZED

%type<s> ip ip_region method uri host referrer agent
%type<s> req_id operation bucket object hdr_item hdr_item_text
%type<s> q_i64 time ip_type status req_bytes res_bytes time_taken

%start line

%%

line
    :
    | log_gcp       { YYACCEPT; }
    | log_hdr       { lib->headerLine( lib->header ); lib->header.clear(); YYACCEPT; }
    | log_err       { YYACCEPT; }
    ;

log_err
    : time error
    {
        LogGCPEvent ev;
        ev . time = ( $1 . p == nullptr ) ? 0 : atol( $1 . p );
        gcp_input( scanner, ev . unparsed );
        lib -> rejectLine( ev );
    }
    ;

log_hdr
    : hdr_item_text             { lib->header.append_fieldname( $1 ); }
    | log_hdr COMMA hdr_item    { lib->header.append_fieldname( $3 ); }
    ;

hdr_item_text
    : QUOTE QSTR QUOTE      { $$ = $2; }
    | QUOTE QUOTE           { $$ . p = nullptr; $$ . n = 0; }
    ;

hdr_item
    : QUOTE QSTR QUOTE      { $$ = $2; }
    | QUOTE I64 QUOTE       { $$ = $2; }
    | QUOTE QUOTE           { $$ . p = nullptr; $$ . n = 0; }
    ;

log_gcp
    : time COMMA ip COMMA ip_type COMMA ip_region COMMA method COMMA uri COMMA
      status COMMA req_bytes COMMA res_bytes COMMA time_taken COMMA host COMMA
      referrer COMMA agent COMMA req_id COMMA operation COMMA bucket COMMA object
    {
        LogGCPEvent ev;
        ev . time = ( $1 . p == nullptr ) ? 0 : atol( $1 . p );
        ev . ip = $3;
        ev . ip_type = ( $5 . p == nullptr ) ? 0 : atoi( $5 . p );
        ev . ip_region = $7;
        ev . method = $9;
        ev . uri = $11;
        ev . status = ( $13 . p == nullptr ) ? 0 : atol( $13 . p );
        ev . request_bytes = ( $15 . p == nullptr ) ? 0 : atol( $15 . p );
        ev . result_bytes = ( $17 . p == nullptr ) ? 0 : atol( $17 . p );
        ev . time_taken = ( $19 . p == nullptr ) ? 0 : atol( $19 . p );
        ev . host = $21;
        ev . referrer = $23;
        ev . agent = $25;
        ev . request_id = $27;
        ev . operation = $29;
        ev . bucket = $31;
        ev . object = $33;
        lib -> acceptLine( ev );
    }
    ;

time
    : QUOTE I64 QUOTE       { $$ = $2; } /* if empty, would conflict with the log_hdr rule */
    ;

ip
    : QUOTE IPV4 QUOTE      { $$ = $2; }
    | QUOTE IPV6 QUOTE      { $$ = $2; }
    | QUOTE QUOTE           { $$ . p = nullptr; $$ . n = 0; }
    ;

ip_type
    : q_i64
    ;

ip_region
    : QUOTE QSTR QUOTE      { $$ = $2; }
    | QUOTE QUOTE           { $$ . p = nullptr; $$ . n = 0; }
    ;

method
    : QUOTE QSTR QUOTE      { $$ = $2; }
    ;

uri
    : QUOTE QSTR QUOTE      { $$ = $2; }
    | QUOTE QUOTE           { $$ . p = nullptr; $$ . n = 0; }
    ;

status
    : q_i64
    ;

req_bytes
    : q_i64
    ;

res_bytes
    : q_i64
    ;

time_taken
    : q_i64
    ;

host
    : QUOTE QSTR QUOTE      { $$ = $2; }
    | QUOTE QUOTE           { $$ . p = nullptr; $$ . n = 0; }
    ;

referrer
    : QUOTE QSTR QUOTE      { $$ = $2; }
    | QUOTE QUOTE           { $$ . p = nullptr; $$ . n = 0; }
    ;

agent
    : QUOTE QSTR QUOTE      { $$ = $2; }
    | QUOTE QUOTE           { $$ . p = nullptr; $$ . n = 0; }
    ;

req_id
    : QUOTE QSTR QUOTE      { $$ = $2; }
    | QUOTE I64 QUOTE       { $$ = $2; }
    | QUOTE QUOTE           { $$ . p = nullptr; $$ . n = 0; }
    ;

operation
    : QUOTE QSTR QUOTE      { $$ = $2; }
    | QUOTE QUOTE           { $$ . p = nullptr; $$ . n = 0; }
    ;

bucket
    : QUOTE QSTR QUOTE      { $$ = $2; }
    | QUOTE QUOTE           { $$ . p = nullptr; $$ . n = 0; }
    ;

object
    : QUOTE QSTR QUOTE      { $$ = $2; }
    | QUOTE QUOTE           { $$ . p = nullptr; $$ . n = 0; }
    ;

q_i64
    : QUOTE I64 QUOTE       { $$ = $2; }
    | QUOTE QUOTE           { $$ . p = nullptr; $$ . n = 0; }
    ;

%%

void gcp_error( yyscan_t locp, NCBI::Logging::GCP_LogLines * lib, const char * msg )
{
    // intentionally left empty, we communicate errors rejected lines
}
