%define api.pure full
%lex-param { void * scanner }
%parse-param { void * scanner }{ NCBI::Logging::OP_LogLines * lib }

%define parse.trace
%define parse.error verbose

%name-prefix "op_"

%{
#define YYDEBUG 1

#include <stdint.h>
#include "log_lines.hpp"
#include "op_parser.hpp"
#include "op_scanner.hpp"

using namespace std;
using namespace NCBI::Logging;

void op_error( yyscan_t locp, NCBI::Logging::OP_LogLines * lib, const char* msg );

%}

%code requires
{
#include "types.h"
#include "log_lines.hpp"
#include "helper.hpp"

extern void op_get_scanner_input( void * yyscanner, t_str & str );
extern void op_start_URL( void * yyscanner );
extern void op_start_UserAgent( void * yyscanner );
extern void op_pop_state( void * yyscanner );

using namespace NCBI::Logging;
}

%union
{
    t_str s;
    int64_t i64;
    t_timepoint tp;
    t_request req;
    t_agent agent;
}

%token<s> STR MONTH IPV4 IPV6 FLOAT METHOD VERS QSTR QSTR_ESC SPACE SLASH QMARK
%token<i64> I64
%token DOT DASH COLON QUOTE OB CB PORT RL CR LF    
%token UNRECOGNIZED 
%token<s> PATHSTR PATHEXT ACCESSION 
%token<s> OS SRA_TOOLKIT LIBCVERSION AGENTSTR SRATOOLVERS
%token<s> PAREN_OPEN PAREN_CLOSE COMMA PHIDVALUE

%type<tp> time
%type<req> server_and_request request url url_token url_entry 
%type<req> request_tail_elem request_tail url_with_optional_params
%type<s> ip user req_time referer forwarded method server
%type<s> quoted_list quoted_list_body quoted_list_elem url_params 
%type<agent> agent vdb_agent_token vdb_agent
%type<i64> result_code result_len port req_len

%start line

%%

line
    : log_onprem        { YYACCEPT; }
    | log_onprem_err    { YYACCEPT; }
    ;

log_onprem
    : ip DASH user time server_and_request result_code result_len req_time referer 
    { op_start_UserAgent( scanner ); } 
    SPACE agent 
    { op_pop_state( scanner ); }
    forwarded port req_len
    {
        LogOPEvent ev;
        ev . ip = $1;
        ev . user = $3;
        ev . time = $4;
        ev . request = $5;
        ev . res_code = $6;
        ev . res_len = $7;
        ev . req_time = $8;
        ev . referer = $9;
        ev . agent = $12;
        ev . forwarded = $14;
        ev . port = $15;
        ev . req_len = $16;

        lib -> acceptLine( ev );
    }
    ;

log_onprem_err
    : ip error
    {
        LogOPEvent ev;
        ev . ip = $1;
        op_get_scanner_input( scanner, ev . unparsed );
        lib -> rejectLine( ev );
    }
    ;

ip
    : IPV4
    | IPV6
    ;

user
    : DASH                          { EMPTY_TSTR($$); }
    | STR                           { $$ = $1; }
    ;

method
    : METHOD        { $$ = $1; }
    ;

server 
    : QUOTE QSTR QUOTE  { $$ = $2; }
    | STR               { $$ = $1; }
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
    : server request
    {
        $$ = $2;
        $$.server = $1;
    }
    | server quoted_list
    { 
        InitRequest( $$ );
        $$.server = $1;
        $$.path = $2; 
    }    
    | request
    {
        $$ = $1;
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
    : I64
    ;

result_len
    : I64
    ;

req_time
    : FLOAT
    ;

referer
    : QUOTE QSTR QUOTE          { $$ = $2; }
    | QUOTE QUOTE               { EMPTY_TSTR($$); }
    ;

vdb_agent_token
    : SRA_TOOLKIT   { InitAgent( $$ ); $$.original = $1; } 
    | SRATOOLVERS
        { 
            InitAgent( $$ ); 
            $$.original = $1; 
            const char * dot = strchr( $1 . p, '.' );
            $$ . vdb_tool . p = $1 . p;
            $$ . vdb_tool . n = dot - $1 . p;
            /* skip the leading dot */
            $$ . vdb_release . p = dot + 1; 
            $$ . vdb_release . n = $1 . n - ( dot - $1 . p ) - 1; 
        } 
    | LIBCVERSION   { InitAgent( $$ ); $$.original = $1; $$.vdb_libc = $1; }
    | PHIDVALUE     { InitAgent( $$ ); $$.original = $1; $$.vdb_phid_compute_env = $1; }
    | SPACE         { InitAgent( $$ ); $$.original = $1; } 
    | AGENTSTR      { InitAgent( $$ ); $$.original = $1; } 
    ;

vdb_agent
    : vdb_agent_token           { $$ = $1; }
    | vdb_agent vdb_agent_token 
    { 
        $$ = $1;
        MERGE_TSTR( $$ . original, $2 . original );
        if ( $2 . vdb_phid_compute_env . n > 0 )
        {
            $$ . vdb_phid_compute_env . p = $2 . vdb_phid_compute_env . p + 5;
            $$ . vdb_phid_compute_env . n = 3;

            /*
                the whole token is either 14 or 15 chars long!
                guid is either 3 or 4 chars long, the other parts have fixed length.
            */
            int guid_len = $2 . vdb_phid_compute_env . n - 11;
            $$ . vdb_phid_guid . p = $2 . vdb_phid_compute_env . p + 8;
            $$ . vdb_phid_guid . n = guid_len;

            $$ . vdb_phid_session_id . p = $$ . vdb_phid_guid . p + guid_len;
            $$ . vdb_phid_session_id . n = 3;
        }
        else if ( $2 . vdb_libc . n > 0 )
        {
            $$ . vdb_libc . p = $2 . vdb_libc . p + 5;
            $$ . vdb_libc . n = $2 . vdb_libc . n - 5;
        }
        else if ( $2 . vdb_tool . n > 0 )
        {
            $$ . vdb_tool = $2 . vdb_tool;
            $$ . vdb_release = $2 . vdb_release;
        }
    }
    | vdb_agent OS
    {
        $$ = $1;
        MERGE_TSTR( $$ . original, $2 );
    } 
    ;

agent
    : QUOTE OS vdb_agent QUOTE       
        { 
            t_agent temp;
            InitAgent( temp ); 
            temp . original = $2;
            MERGE_TSTR( temp . original, $3 . original );
            $$ = $3;
            $$ . original = temp . original;

            $$.vdb_os = $2;
        }
    | QUOTE vdb_agent QUOTE
        { 
            $$ = $2;
        }
    | QUOTE QUOTE                                           
        { 
            InitAgent( $$ ); 
        }
    ;

forwarded
    : QUOTE QSTR QUOTE              { $$ = $2; }
    ;

port
    : PORT I64                      { $$ = $2; }
    ;

req_len
    : RL I64                        { $$ = $2; }
    ;

time
    : OB I64 SLASH MONTH SLASH I64 COLON I64 COLON I64 COLON I64 I64 CB
    {
        $$.day = $2;
        $$.month = month_int( &($4) );
        $$.year = $6;
        $$.hour = $8;
        $$.minute = $10;
        $$.second = $12;
        $$.offset = $13;
    }
    ;

%%

void op_error( yyscan_t locp, NCBI::Logging::OP_LogLines * lib, const char * msg )
{
    // intentionally left empty, we communicate errors rejected lines
}
