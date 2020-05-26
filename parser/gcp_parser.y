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
using namespace NCBI::Logging;
}

%union
{
    t_str s;
    int64_t i64;
}

%token<s> STR STR1 IPV4 IPV6 QSTR DASH
%token<i64> I64
%token DOT SLASH COLON QUOTE RL CR LF SPACE QMARK COMMA

%type<s> ip ip_region method uri host referrer agent agent_list
%type<s> req_id operation bucket object
%type<i64> q_i64 time ip_type status req_bytes res_bytes time_taken

%start line

%%

line
    :
    | log_gcp
    ;

log_gcp
    : time COMMA ip COMMA ip_type COMMA ip_region COMMA method COMMA uri COMMA
      status COMMA req_bytes COMMA res_bytes COMMA time_taken COMMA host COMMA
      referrer COMMA agent COMMA req_id COMMA operation COMMA bucket COMMA object
    {
        LogGCPEvent ev;
        ev . time = $1;
        ev . ip = $3;
        ev . ip_type = $5;
        ev . ip_region = $7;
        ev . method = $9;
        ev . uri = $11;
        ev . status = $13;
        ev . request_bytes = $15;
        ev . result_bytes = $17;
        ev . time_taken = $19;
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
    : q_i64
    ;

ip
    : QUOTE IPV4 QUOTE      { $$ = $2; }
    | QUOTE IPV6 QUOTE      { $$ = $2; }
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
    ;

referrer
    : QUOTE QSTR QUOTE      { $$ = $2; }
    | QUOTE QUOTE           { $$ . p = nullptr; $$ . n = 0; }
    ;

agent
    : QUOTE agent_list QUOTE        { $$ = $2; }
    ;

agent_list
    : QSTR                          { $$ = $1; }
    | agent_list QSTR               { $$.n += $2.n; }
    | agent_list SPACE              { $$.n += 1; }
    ;

req_id
    : QUOTE QSTR QUOTE      { $$ = $2; }
    ;

operation
    : QUOTE QSTR QUOTE      { $$ = $2; }
    ;

bucket
    : QUOTE QSTR QUOTE      { $$ = $2; }
    ;

object
    : QUOTE QSTR QUOTE      { $$ = $2; }
    ;

q_i64
    : QUOTE I64 QUOTE       { $$ = $2; }
    ;

%%

void gcp_error( yyscan_t locp, NCBI::Logging::GCP_LogLines * lib, const char * msg )
{
    // TODO: find a way to comunicate the syntax error to the consumer...
    // t_str msg_p = { msg, (int)strlen( msg ) };
    // lib -> unrecognized( msg_p );
}
