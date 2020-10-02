%define api.pure full
%lex-param { void * scanner }
%parse-param { void * scanner }{ NCBI::Logging::URLReceiver * lib }

%define parse.trace
%define parse.error verbose

%name-prefix "url_"

%{
    #define YYDEBUG 1

    #include <stdint.h>
    #include "url_parser.hpp"
    #include "url_scanner.hpp"

    using namespace std;
    using namespace NCBI::Logging;

    void url_error( yyscan_t locp, NCBI::Logging::URLReceiver * lib, const char* msg );

    const t_str EmptyTSTR = { "", 0, false };

    void Merge( Node & to, const Node & from)
    {
        switch ( from . type )
        {
            case ACCESSION:
            {
                to . accession = from . str;
                break;
            }
            case PATHSTR:
            {
                to . filename = from . str;
                break;
            }
            case PATHEXT:
            {
                if ( to . type == ACCESSION )
                {   // SRR000123.ext : use SRR000123 as both accession and filename
                    to . filename = to . accession;
                }
                to . extension = from . str;
                break;
            }
            default:
                break;
        }
        to . type = from . type;
    }

    void QueryMerge( Node & to, const Node & from)
    {
        switch ( from . type )
        {
            case ACCESSION:
            {
                to . accession = from . str;
                break;
            }
            default:
                break;
        }
        to . type = from . type;
    }

    void Init_Node( Node& n) { memset( & n, 0, sizeof n ); }

%}

%code requires
{
    #include "types.h"
    #include "URL_Interface.hpp"

    extern void url_get_scanner_input( void * yyscanner, t_str & str );

    using namespace NCBI::Logging;

    typedef struct {
        t_str str;
        int   type;

        t_str accession;
        t_str filename;
        t_str extension;
    } Node;

}

%union
{
    Node s;
}

%token<s> ACCESSION PATHSTR PATHEXT SLASH
%token<s> QMARK QUERY_SEP EQUAL QUERY_TOKEN
%token<s> HASH FRAGMENT_TOKEN UNRECOGNIZED

%type<s> path path_list path_token
%type<s> query query_list query_key query_value query_entry
%type<s> fragment

%start url_line

%%

url_line
    : error             { YYABORT; }
    | path query fragment
        {
            // look at accessions detected in path and/or query, decide whit to use
            if ( $1 . accession . n > 0 )
            {
                lib->set( URLReceiver::accession, $1 . accession );

                if ( $1 . filename . n == 0 )
                {
                    if ( $1 . extension . n == 0 )
                    {   // use the accession
                        lib->set( URLReceiver::filename, $1 . accession );
                    }
                }
                else
                {
                    lib->set( URLReceiver::filename, $1 . filename );
                }

                lib->set( URLReceiver::extension, $1 . extension );
            }
            else // use the query
            {
                lib->set( URLReceiver::accession, $2 . accession );
            }
        }
    | path query error  { lib->reportField( "Invalid URL query" ); YYACCEPT; }
    ;

path
    : path_list { $$ = $1; }
    | %empty    { Init_Node( $$ ); }
    ;

path_token
    : ACCESSION         { $$ = $1; $$ . type = ACCESSION; }
    | PATHSTR           { $$ = $1; $$ . type = PATHSTR; }
    | PATHEXT           { $$ = $1; $$ . type = PATHEXT; }
    | SLASH             { $$ = $1; $$ . type = SLASH; }
    ;

path_list
    : path_token
        {
            $$ = $1;
            Merge( $$, $1 );
        }
    | path_list path_token
        {
            $$ = $1;
            Merge( $$, $2 );
        }
    ;

query
    : QMARK query_list { $$ = $2; }
    | %empty { Init_Node( $$ ); }
    ;

query_list
    : query_entry
        {
            $$  = $1;
        }
    | query_list QUERY_SEP query_entry
        {
            $$ = $1;
            QueryMerge( $$, $3 );
        }
    ;

query_key
    : QUERY_TOKEN { Init_Node( $$ ); }
    ;

query_value
    : ACCESSION     { $$ = $1; $$ . type = ACCESSION; }
    | QUERY_TOKEN   { $$ = $1; $$ . type = QUERY_TOKEN; }
    | %empty        { Init_Node( $$ ); }
    ;

query_entry
    : query_key EQUAL query_value { $$ = $3; }
    | QUERY_TOKEN   { $$ = $1; }
    | %empty        { Init_Node( $$ ); }
    ;

fragment
    : HASH
    | HASH FRAGMENT_TOKEN
    | %empty    {}
    ;

%%

void url_error( yyscan_t locp, NCBI::Logging::URLReceiver * lib, const char * msg )
{
    // intentionally left empty, we communicate errors rejected lines
}
