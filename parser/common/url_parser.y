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
                to . apply_accession = true;
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
        t_str accession;
        t_str filename;
        t_str extension;
    } Node;

}

%union
{
    Node s;
}

%token<s> ACCESSION DELIMITER PATHSTR PATHEXT SLASH
%type<s> url_token token_list

%start url_line

%%

url_line
    : error         { YYABORT; }
    | %empty        {}
    | token_list
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
    ;

url_token
    : DELIMITER
    | ACCESSION { $$ = $1; $$ . type = ACCESSION; }
    | PATHSTR   { $$ = $1; $$ . type = PATHSTR; }
    | PATHEXT   { $$ = $1; $$ . type = PATHEXT; }
    | SLASH     { $$ = $1; $$ . type = SLASH; }
    ;

token_list
    : url_token
        {
            $$ = $1;
            Merge( $$, $1 );
        }
    | token_list url_token
        {
//printf( "$1=%i $2=%i\n", $1.mode, $2.mode );
            $$ = $1;
            Merge( $$, $2 );

/*            switch ( $1 . mode )
            {
            case acc_before:
                if ( $2 . type == ACCESSION )
                {
                    lib->set( URLReceiver::accession, $2 . str );
                    $$ . accession = $2 . str;
                    $$ . mode = acc_inside;
                }
                break;
            case acc_inside:
                switch ( $2 . type )
                {
                case ACCESSION:
                case PATHSTR:
                    {
                        $$ . filename = $2 . str;
                        break;
                    }
                case PATHEXT:
                    {
                        $$ . extension = $2 . str;
                        break;
                    }
                default:
                    break;
                }
                break;
            default:
                break;
            }
*/
        }
    ;

%%

void url_error( yyscan_t locp, NCBI::Logging::URLReceiver * lib, const char * msg )
{
    // intentionally left empty, we communicate errors rejected lines
}
