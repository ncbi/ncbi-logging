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

extern void gcp_get_scanner_input( void * yyscanner, t_str & str );
extern void gcp_start_URL( void * yyscanner );
extern void gcp_start_UserAgent( void * yyscanner );
extern void gcp_pop_state( void * yyscanner );

using namespace NCBI::Logging;

}

%union
{
    t_str s;
    t_request req;
    t_agent agent;
}

%token<s> IPV4 IPV6 QSTR I64 PATHSTR PATHEXT ACCESSION SLASH
%token<s> EQUAL AMPERSAND QMARK PERCENT
%token QUOTE COMMA UNRECOGNIZED
%token<s> OS SRA_TOOLKIT LIBCVERSION AGENTSTR SRATOOLVERS PHIDVALUE

%type<s> ip ip_region method host referrer
%type<s> ext_opt ext_list ext_elem file_opt file_list file_elem object_token object_list
%type<s> req_id operation bucket hdr_item hdr_item_text
%type<s> q_i64 time ip_type status req_bytes res_bytes time_taken
%type<req> object url url_list url_token
%type<agent> agent vdb_agent vdb_agent_token

%start line

%%

line
    : log_gcp       { YYACCEPT; }
    | log_hdr       { lib -> headerLine(); YYACCEPT; }
    | log_err       { YYACCEPT; }
    ;

log_err
    : time error
    {
        LogGCPEvent ev;
        ev . time = ( $1 . p == nullptr ) ? 0 : atol( $1 . p );
        gcp_get_scanner_input( scanner, ev . unparsed );
        lib -> rejectLine( ev );
    }
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
    {
        LogGCPEvent ev;
        ev . time = ( $1 . p == nullptr ) ? 0 : atol( $1 . p );
        ev . ip = $3;
        ev . ip_type = ( $5 . p == nullptr ) ? 0 : atoi( $5 . p );
        ev . ip_region = $7;
        // $9 (method) goes into ev.request below
        ev . uri = $12 . path;
        ev . status = ( $15 . p == nullptr ) ? 0 : atol( $15 . p );
        ev . request_bytes = ( $17 . p == nullptr ) ? 0 : atol( $17 . p );
        ev . result_bytes = ( $19 . p == nullptr ) ? 0 : atol( $19 . p );
        ev . time_taken = ( $21 . p == nullptr ) ? 0 : atol( $21 . p );
        ev . host = $23;
        ev . referer = $25;
        ev . agent = $28;
        ev . request_id = $31;
        ev . operation = $33;
        ev . bucket = $35;

        if ( $38 . accession . n > 0 )
        {   // the cloud did populate the object-field
            ev . request = $38;
        }
        else
        {   // no accession found in the object field, use the results of parsing the request field
            ev . request = $12;
        }

        if ( ev . request . filename . n == 0 && ev . request . extension . n == 0 )
        {   // reuse the accession as the filename
            ev . request . filename = ev . request . accession;
        }

        ev . request . method = $9;

        lib -> acceptLine( ev );
    }
    ;

time
    : QUOTE I64 QUOTE       { $$ = $2; } /* if empty, would conflict with the log_hdr rule */
    ;

ip
    : QUOTE IPV4 QUOTE      { $$ = $2; }
    | QUOTE IPV6 QUOTE      { $$ = $2; }
    | QUOTE QUOTE           { EMPTY_TSTR($$); }
    ;

ip_type
    : q_i64
    ;

ip_region
    : QUOTE QSTR QUOTE      { $$ = $2; }
    | QUOTE QUOTE           { EMPTY_TSTR($$); }
    ;

method
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
    | QUOTE QUOTE           { EMPTY_TSTR($$); }
    ;

referrer
    : QUOTE QSTR QUOTE      { $$ = $2; }
    | QUOTE QUOTE           { EMPTY_TSTR($$); }
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
    | AGENTSTR      { InitAgent( $$ ); $$.original = $1; }
    /*TODO: add an UNKNOWTOOLVERS token, categorize */
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

req_id
    : QUOTE QSTR QUOTE      { $$ = $2; }
    | QUOTE I64 QUOTE       { $$ = $2; }
    | QUOTE QUOTE           { EMPTY_TSTR($$); }
    ;

operation
    : QUOTE QSTR QUOTE      { $$ = $2; }
    | QUOTE QUOTE           { EMPTY_TSTR($$); }
    ;

bucket
    : QUOTE QSTR QUOTE      { $$ = $2; }
    | QUOTE QUOTE           { EMPTY_TSTR($$); }
    ;

/*
    typedef enum { acc_before = 0, acc_inside, acc_after } eAccessionMode; (defined in log_lines.hpp)

        for a url_token node, set to
            'acc_inside' if it is an ACCESSION
            'acc_after'  if it is a delimiter

        for a url_list node, describes the state of URL parsing:
            'acc_before' - no accession has been seen yet
            'acc_inside' - we are between the first accession and the following delimiter,
                            capture the filename and extension tokens
            'acc_after'  - we are past delimiter following an accession,
                            no further action necessary
 */

url_token
    : SLASH         { InitRequest( $$ ); $$ . path = $1; }
    | EQUAL         { InitRequest( $$ ); $$ . path = $1; $$.accession_mode = acc_after; }
    | AMPERSAND     { InitRequest( $$ ); $$ . path = $1; $$.accession_mode = acc_after; }
    | QMARK         { InitRequest( $$ ); $$ . path = $1; $$.accession_mode = acc_after; }
    | PERCENT       { InitRequest( $$ ); $$ . path = $1; }
    | ACCESSION
        {
            InitRequest( $$ );
            $$ . path = $1;
            $$ . accession = $1;
            $$ . accession_mode = acc_inside;
        }
    | PATHSTR       { InitRequest( $$ ); $$ . path = $1; $$ . filename = $1; }
    | PATHEXT       { InitRequest( $$ ); $$ . path = $1; $$ . extension = $1; }
    ;

 /* This is a collection of url tokens and accessions.
    We are looking for the first accession and filename/extension that follow it.*/
url_list
    :  url_token
        {
            $$ = $1;
            if ( $1 . accession_mode == acc_after )
            {   /* a delimiter seen before an accession */
                $$ . accession_mode = acc_before;
            }
        }
    |  url_list url_token
        {
            $$ = $1;
            MERGE_TSTR( $$ . path, $2 . path );
            switch ( $$.accession_mode )
            {
            case acc_before:
                if ( $2.accession_mode == acc_inside )
                {
                    $$ . accession = $2 . accession;
                    $$ . accession_mode = acc_inside;
                }
                break;
            case acc_inside:
                switch ( $2 . accession_mode )
                {
                case acc_inside:
                    $$ . filename  = $2 . accession;
                    break;
                case acc_after:
                    $$ . accession_mode = acc_after;
                    break;
                default:
                    if ( $2 . filename . n > 0 )    $$ . filename  = $2 . filename;
                    if ( $2 . extension . n > 0 )   $$ . extension = $2 . extension;
                }
                break;
            }
        }
    ;

url
    : QUOTE url_list QUOTE
        {
            $$ = $2;
        }
    | QUOTE QUOTE
    {
        InitRequest( $$ );
    }
    ;

object_token
    : SLASH         { $$ = $1; }
    | PATHSTR       { $$ = $1; }
    | PATHEXT       { $$ = $1; }
    | EQUAL         { $$ = $1; }
    | AMPERSAND     { $$ = $1; }
    | QMARK         { $$ = $1; }
    | PERCENT       { $$ = $1; }
    ;

object_list
    : object_token
    {
        $$ = $1;
    }
    | object_list object_token
    {
        $$ = $1;
        MERGE_TSTR( $$, $2 );
    }
    | object_list ACCESSION
    {
        $$ = $1;
        MERGE_TSTR( $$, $2 );
    }
    ;

file_opt
    : file_list     { $$ = $1; }
    |               { EMPTY_TSTR($$); }
    ;

file_list
    : file_elem             { $$ = $1; }
    | file_list file_elem   { $$ = $1; MERGE_TSTR( $$, $2 ); }
    ;

file_elem
    : ACCESSION     { $$ = $1; }
    | PATHSTR       { $$ = $1; }
    | EQUAL         { $$ = $1; }
    | AMPERSAND     { $$ = $1; }
    ;

ext_opt
    : ext_list      { $$ = $1; }
    |               { EMPTY_TSTR($$); }
    ;

ext_list
    : PATHEXT           { $$ = $1; }
    | ext_list ext_elem { $$ = $1; MERGE_TSTR( $$, $2 ); }
    ;

ext_elem
    : ACCESSION     { $$ = $1; }
    | PATHSTR       { $$ = $1; }
    | PATHEXT       { $$ = $1; }
    | EQUAL         { $$ = $1; }
    | AMPERSAND     { $$ = $1; }
    ;

object
    : QUOTE ACCESSION SLASH file_opt ext_opt QUOTE
        {
            InitRequest( $$ );

            $$.path . p = $2 . p;
            $$.path . n = $2 . n + 1 + $4 . n + $5 . n;

            $$.accession = $2;
            $$.filename  = $4;
            $$.extension = $5;
        }
    | QUOTE object_list QUOTE
        {   // TODO: somehow signal to the back end that we did not understand the pre-parsed object
            InitRequest( $$ );
            $$ . path = $2;
        }
    | QUOTE QUOTE
        {
             InitRequest( $$ );
        }
    | error object_list QUOTE
        {   // TODO: somehow signal to the back end that we did not understand the pre-parsed object
            InitRequest( $$ );
        }
    | error QUOTE
        {   // TODO: somehow signal to the back end that we did not understand the pre-parsed object
            InitRequest( $$ );
        }
    ;

q_i64
    : QUOTE I64 QUOTE       { $$ = $2; }
    | QUOTE QUOTE           { EMPTY_TSTR($$); }
    ;

%%

void gcp_error( yyscan_t locp, NCBI::Logging::GCP_LogLines * lib, const char * msg )
{
    // intentionally left empty, we communicate errors by rejecting lines
}
