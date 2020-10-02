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
                if ( to . after_qmark )
                {
                    if ( to . accession . n == 0 )
                    {
                        to . accession = from . str;
                        to . apply_accession = true;
                    }
                }
                else
                {
                    to . accession = from . str;
                    to . apply_accession = true;
                }
                break;
            }
            case PATHSTR:
            {
                to . filename = from . str;
                break;
            }
            case PATHEXT:
            {
                to . extension = from . str;
                break;
            }
            case SLASH:
            {
                to . apply_accession = false;
                break;
            }
            case QMARK:
            {
                to . after_qmark = true;
                break;
            }

            default:
                break;
        }
    }

%}

%code requires
{
    #include "types.h"
    #include "URL_Interface.hpp"

    extern void url_get_scanner_input( void * yyscanner, t_str & str );

    using namespace NCBI::Logging;

    typedef struct {
        t_str str;
        eAccessionMode mode;
        int   type;
        bool  apply_accession;
        bool  after_qmark;
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
    : error         { YYABORT; }
    | path query fragment
    ;

path
    : path_list
    {
        lib->set(URLReceiver::accession, $1 . accession );

        if ( $1 . filename . n == 0 && $1 . apply_accession )
        {   // no filename; use the accession
            lib->set( URLReceiver::filename, $1 . accession );
        }
        else
        {
            lib->set( URLReceiver::filename, $1 . filename );
        }
        lib->set( URLReceiver::extension, $1 . extension );
    }
    | %empty  {}
    ;

path_token
    : ACCESSION { $$ = $1; $$ . type = ACCESSION; }
    | PATHSTR   { $$ = $1; $$ . type = PATHSTR; }
    | PATHEXT   { $$ = $1; $$ . type = PATHEXT; }
    | SLASH     { $$ = $1; $$ . type = SLASH; }
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
    : QMARK query_list
    | %empty {}
    ;

query_list
    : query_entry
    | query_list QUERY_SEP query_entry
    ;

query_key
    : QUERY_TOKEN
    ;

query_value
    : QUERY_TOKEN
    | %empty   {} 
    ;

query_entry
    : query_key EQUAL query_value
    | QUERY_TOKEN
    | %empty    {}
    ;

fragment
    : HASH FRAGMENT_TOKEN
    | %empty    {}
    ;

%%

void url_error( yyscan_t locp, NCBI::Logging::URLReceiver * lib, const char * msg )
{
    // intentionally left empty, we communicate errors rejected lines
}
