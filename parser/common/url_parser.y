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

    Node Merge( const Node & to, const Node & from)
    {
// printf("accession=%.*s<-%.*s\n", to.accession.n, to.accession.p, from.accession.n, from.accession.p );
// printf("filename=%.*s<-%.*s\n", to.filename.n, to.filename.p, from.filename.n, from.filename.p );
// printf("extension=%.*s<-%.*s\n", to.extension.n, to.extension.p, from.extension.n, from.extension.p );
// printf("from.str=%.*s\n", from.str.n, from.str.p );
// printf("from.type=%i\n", from.type );

        Node ret = to;

        switch ( from . type )
        {
            case ACCESSION:
            {
                ret . accession = from . str;
                break;
            }
            case PATHSTR:
            {
                ret . filename = from . str;
                break;
            }
            case PATHEXT:
            {
                // thos code needs to be here because we need the information
                // that the preceding token was an accession
                if ( to . type == ACCESSION )
                {   // SRR000123.ext : use SRR000123 as both accession and filename
                    ret . filename = to . accession;
                }
                ret . extension = from . str;
                break;
            }
            default:
                break;
        }
        ret . type = from . type;

        return ret;
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
%type<s> query query_list query_key query_entry query_token_list query_token
%type<s> fragment

%start url_line

%%

url_line
    : path query fragment
        {
            // look at accessions detected in path and/or query, decide which to use
            if ( $1 . accession . n > 0 )
            {
                lib->set( URLReceiver::accession, $1 . accession );

                // file and extension from Path if exist
                if ( $1 . filename . n > 0 )
                {
                    lib->set( URLReceiver::filename,    $1 . filename );
                    lib->set( URLReceiver::extension,   $1 . extension );
                }
                else if ( $1 . extension . n > 0 )
                {   // extension, empty filename on the path; use the accession as filename
                    lib->set( URLReceiver::filename, EmptyTSTR );
                    lib->set( URLReceiver::extension, $1 . extension );
                }
                else // $1 . extension . n == 0
                {   // no filename or extension on the path; use the accession as filename
                    //TODO: use filename/extension from URL
                    lib->set( URLReceiver::filename, $1.accession );
                    lib->set( URLReceiver::extension, EmptyTSTR );
                }
            }
            else if ( $2 . accession . n > 0 )
            { // use the accession, filename and extension from the query
                lib->set( URLReceiver::accession, $2 . accession );

                if ( $2 . filename . n > 0 )
                {
                    lib->set( URLReceiver::filename,    $2 . filename );
                    lib->set( URLReceiver::extension,   $2 . extension );
                }
                else if ( $2 . extension . n > 0 )
                {   // extension, empty filename in the query; use the accession as filename
                    // lib->set( URLReceiver::filename, EmptyTSTR );
                    // lib->set( URLReceiver::extension, $2 . extension );
                }
                else // $2 . extension . n == 0
                {   // no filename or extension in the query; use the accession as filename
                    lib->set( URLReceiver::filename, $2 . accession );
                    lib->set( URLReceiver::extension, EmptyTSTR );
                }
            }
            // no accessions; use the filename/extension from the Path if present
            // from the URL otherwise

            YYACCEPT;
        }
    | path query error  { lib->reportField( "Invalid URL query" ); YYACCEPT; }
    ;

/********************************************* Path ***************************************/

path
    : path_list { $$ = $1; }
    | %empty    { Init_Node( $$ ); }
    ;

path_token
    : ACCESSION         { $$ = $1; $$ . type = ACCESSION; $$ . accession = $$ . str; }
    | PATHSTR           { $$ = $1; $$ . type = PATHSTR; $$ . filename = $$ . str; }
    | PATHEXT           { $$ = $1; $$ . type = PATHEXT; $$ . extension = $$ . str; }
    | SLASH             { $$ = $1; $$ . type = SLASH; }
    ;

path_list
    : path_token
        {
            $$ = $1;
        }
    | path_list path_token
        {
            $$ = Merge( $1, $2 );
        }
    ;

/********************************************* URL ***************************************/

query_token
    : ACCESSION         { $$ = $1; $$ . type = ACCESSION; $$ . accession = $$ . str; }
    | PATHSTR           { $$ = $1; $$ . type = PATHSTR; $$ . filename = $$ . str; }
    | PATHEXT           { $$ = $1; $$ . type = PATHEXT; $$ . extension = $$ . str; }
    | QUERY_TOKEN       { $$ = $1; $$ . type = QUERY_TOKEN; }
    ;

query_token_list
    : query_token
        {
            $$ = $1;
        }
    | query_token_list query_token
        {
            $$ = Merge( $1, $2 );
        }
    ;

query_key
    : QUERY_TOKEN   { Init_Node( $$ ); }
    | PATHSTR       { Init_Node( $$ ); }
    ;

query_entry
    : query_key EQUAL query_token_list  { /* ignore the key, report the value */ $$ = $3; }
    | query_token_list                  { $$ = $1; }
    ;

query_list
    : query_entry
        {
            $$ = $1;
        }
    | query_list QUERY_SEP query_entry
        {
            // if $3 has more elements than $1, use $3
//printf("$1=%.*s $3=%.*s\n", $1.accession.n, $1.accession.p, $3.accession.n, $3.accession.p );

            if ( $1.accession.n == 0 )
            {
                if ( $3.accession.n > 0 )
                {
                    $$ = $3;
                }
                // else no accessions
            }
            else // $1 accession exists
            {
                if ( $3 . accession . n == 0 )
                {
                    $$ = $1;
                }
                else
                {   // accession present in both $1 and $3. use filename as tiebreaker
                    // if on both sides, use the $1
                    if ( $1 . filename . n == 0 && $3 . filename . n > 0 )
                    {
                        $$ = $3;
                    }
                    else
                    {
                        $$ = $1;
                    }
                }
            }
        }
    ;

query
    : QMARK query_list { $$ = $2; }
    | %empty { Init_Node( $$ ); }
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
