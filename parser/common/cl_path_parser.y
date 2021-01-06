%define api.pure full
%lex-param { void * scanner }
%parse-param { void * scanner }{ NCBI::Logging::CLPATHReceiver * lib }

%define parse.trace
%define parse.error verbose

%name-prefix "cl_path_"

%{
    #define YYDEBUG 1

    #include <stdint.h>
    #include "cl_path_parser.hpp"
    #include "cl_path_scanner.hpp"

    using namespace std;
    using namespace NCBI::Logging;

    void cl_path_error( yyscan_t locp, NCBI::Logging::CLPATHReceiver * lib, const char* msg );

    const t_str EmptyTSTR = { "", 0 };

    static Node Merge( const Node & to, const Node & from)
    {
//   printf("accession=%.*s<-%.*s\n", to.accession.n, to.accession.p, from.accession.n, from.accession.p );
//   printf("filename=%.*s<-%.*s\n", to.filename.n, to.filename.p, from.filename.n, from.filename.p );
//  printf("extension=%.*s<-%.*s\n", to.extension.n, to.extension.p, from.extension.n, from.extension.p );
//  printf("from.str=%.*s\n", from.str.n, from.str.p );
//  printf("from.type=%i\n", from.type );

        Node ret = to;

        switch ( from . type )
        {
            case ACCESSION:
            {
                ret . accession = from . str;
                ret . filename = EmptyTSTR;
                ret . extension = EmptyTSTR;
                break;
            }
            case PATHSTR:
            {
                ret . filename = from . str;
                ret . extension = EmptyTSTR;
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
            case SLASH:
            {
                ret . filename = EmptyTSTR;
                ret . extension = EmptyTSTR;
                break;
            }
            default:
                break;
        }
        ret . type = from . type;

        return ret;
    }

    static void Init_Node( Node& n) { memset( & n, 0, sizeof n ); }

%}

%code requires
{
    #include "types.h"
    #include "CL_PATH_Interface.hpp"

    extern void cl_path_get_scanner_input( void * yyscanner, t_str & str );

    using namespace NCBI::Logging;

    typedef struct {
        t_str str;
        int   type;

        t_str accession;
        t_str filename;
        t_str extension;
    } Node;

}

%define api.value.type {Node}

%token ACCESSION PATHSTR DOT SLASH PERCENT PATHEXT EXTSTR EXTPERCENT
%token QMARK QUERY_SEP EQUAL QUERY_TOKEN
%token HASH FRAGMENT_TOKEN UNRECOGNIZED

%start url_line

%%

url_line
    : path query fragment
        {
            // look at accessions detected in path and/or query, decide which to use
            if ( $1 . accession . n > 0 )
            {
                if ( lib -> m_accession . empty() )
                {
                    lib -> m_accession = std::string( $1 . accession . p, $1 . accession . n );
                    lib -> m_filename . clear();
                    lib -> m_extension . clear();

                    // file and extension from Path if exist
                    if ( $1 . filename . n > 0 )
                    {
                        lib -> m_filename  = std::string( $1 . filename . p, $1 . filename . n );
                        lib -> m_extension = std::string( $1 . extension . p, $1 . extension . n );
                    }
                    else if ( $1 . extension . n > 0 )
                    {   // extension, empty filename on the path; use the accession as filename
                        lib -> m_filename  . clear();
                        lib -> m_extension = std::string( $1 . extension . p, $1 . extension . n );
                    }
                    else // $1 . extension . n == 0
                    {   // no filename or extension on the path; use the accession as filename
                        lib -> m_filename  = std::string( $1 . accession . p, $1 . accession . n );
                        lib -> m_extension . clear();
                    }
                }
            }
            else if ( $2 . accession . n > 0 )
            { // use the accession, filename and extension from the query

                if ( lib -> m_accession . empty() )
                {
                    lib -> m_accession = std::string( $2 . accession . p, $2 . accession . n );
                    lib -> m_filename . clear();
                    lib -> m_extension . clear();

                    if ( $2 . filename . n > 0 )
                    {
                        lib -> m_filename  = std::string( $2 . filename . p, $2 . filename . n );
                        lib -> m_extension = std::string( $2 . extension . p, $2 . extension . n );
                    }
                    else if ( $2 . extension . n > 0 )
                    {   // extension, empty filename in the query; use the accession as filename
                        lib -> m_filename . clear();
                        lib -> m_extension = std::string( $2 . extension . p, $2 . extension . n );
                    }
                    else // $2 . extension . n == 0
                    {   // no filename or extension in the query; use the accession as filename
                        lib -> m_filename  = std::string( $2 . accession . p, $2 . accession . n );
                        lib -> m_extension . clear();
                    }
                }
            }
            else
            { // no accession found what-so-ever
                if ( $1 . filename . n > 0 )
                {  // if we found a filename in the path
                    lib -> m_filename  = std::string( $1 . filename . p, $1 . filename . n );
                    lib -> m_extension = std::string( $1 . extension . p, $1 . extension . n );
                }
                else if ( $2 . filename . n > 0 )
                {  // if not take the filename from the query, which may be empty
                    lib -> m_filename  = std::string( $2 . filename . p, $2 . filename . n );
                    lib -> m_extension = std::string( $2 . extension . p, $2 . extension . n );
                }
                else if ( $1 . extension . n > 0 )
                {
                    lib -> m_filename  = std::string( $1 . filename . p, $1 . filename . n );
                    lib -> m_extension = std::string( $1 . extension . p, $1 . extension . n );
                }
                else if ( $2 . extension . n > 0 )
                {
                    lib -> m_filename  = std::string( $2 . filename . p, $2 . filename . n );
                    lib -> m_extension = std::string( $2 . extension . p, $2 . extension . n );
                }
            }
            YYACCEPT;
        }
    | path query error  { lib->reportField( "Invalid URL query" ); YYACCEPT; }
    /*| path error        { lib->reportField( "Invalid URL path" ); YYACCEPT; }*/
    /*| error             { lib->reportField( "Invalid URL path" ); YYACCEPT; }*/
    ;

/********************************************* Path ***************************************/

ext_element
    : EXTSTR        { $$ = $1; }
    | EXTPERCENT    { $$ = $1; }
    ;

ext_list
    : ext_element           { $$ = $1; }
    | ext_list ext_element  { $$ = $1; MERGE_TSTR( $$ . str, $2 . str ); }
    ;

extension
    : DOT ext_list
        {
            $$ = $1;
            MERGE_TSTR( $$ . str, $2 . str );
            $$ . type = PATHEXT;
        }
    | DOT
        {
            $$ = $1;
            $$ . type = PATHEXT;
        }
    ;

path_element
    : ACCESSION     { $$ = $1; $$ . type = ACCESSION; $$ . accession = $$ . str; }
    | PATHSTR       { $$ = $1; $$ . type = PATHSTR; $$ . filename = $$ . str; }
    | PERCENT       { $$ = $1; $$ . type = PATHSTR; $$ . filename = $$ . str; }
    | extension     { $$ = $1; $$ . type = PATHEXT; $$ . extension = $$ . str; }
    | SLASH         { $$ = $1; $$ . type = SLASH; }
    ;

path_list
    : path_element
        {
            $$ = $1;
        }
    | path_list path_element
        {
            if ( $1 . type == PATHSTR && $2 . type == PATHSTR )
            {   // filenames in a path can come as a sequence of tokens (e.g. "dir/a%b");
                // reassemble consequtive PATHSTRs into a single filename if needed
                $$ . filename = $1 . filename;
                MERGE_TSTR( $$ . filename, $2 . filename );
            }
            else
            {
                $$ = Merge( $1, $2 );
            }
        }
    ;

path
    : path_list { $$ = $1; }
    | %empty    { Init_Node( $$ ); }
    ;

/********************************************* URL ***************************************/

value_element
    : ACCESSION     { $$ = $1; $$ . type = ACCESSION; $$ . accession = $$ . str; }
    | PATHSTR       { $$ = $1; $$ . type = PATHSTR; $$ . filename = $$ . str; }
    | PERCENT       { $$ = $1; $$ . type = PATHSTR; $$ . filename = $$ . str; }
    | SLASH         { $$ = $1; $$ . type = SLASH; }
    | extension     { $$ = $1; }
    ;

value_list
    : value_element                     { $$ = $1; }
    | value_list value_element          { $$ = Merge( $1, $2 ); }
    | value_list value_element EQUAL
        {
            MERGE_TSTR( $2 . str, $3 . str );
            $$ . type = PATHSTR;
            $$ = Merge( $1, $2 );
        }
    | EQUAL                             { $$ = $1; $$ . type = PATHSTR; }
    ;

query_key
    : QUERY_TOKEN  {}
    | PATHSTR      {}
    ;

query_entry
    : query_key EQUAL value_list  { /* ignore the key, report the value */ $$ = $3; }
    | query_key EQUAL             {  }
    | value_list                  { $$ = $1; }
    ;

query_list
    : query_entry
        {
            $$ = $1;
        }
    | query_list QUERY_SEP query_entry
        {
            // if $3 has more elements than $1, use $3
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

void cl_path_error( yyscan_t locp, NCBI::Logging::CLPATHReceiver * lib, const char * msg )
{
    // intentionally left empty, we communicate errors rejected lines
}
