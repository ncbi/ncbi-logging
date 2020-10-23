%define api.pure full
%lex-param { void * scanner }
%parse-param { void * scanner }{ NCBI::Logging::MSGReceiver * lib }

%define parse.trace
%define parse.error verbose

%name-prefix "msg_"

%{
#define YYDEBUG 1

#include <stdint.h>
#include "msg_parser.hpp"
#include "msg_scanner.hpp"

using namespace std;
using namespace NCBI::Logging;

void msg_error( yyscan_t locp, NCBI::Logging::MSGReceiver * lib, const char* msg );
void msg_start_VALUE( yyscan_t yyscanner );
void msg_start_REQUEST( yyscan_t yyscanner );
void msg_pop_state( yyscan_t yyscanner );

const t_str EmptyTSTR = { "", 0 };

%}

%code requires
{
#include "types.h"
#include "MSG_Interface.hpp"

using namespace NCBI::Logging;
}

%union
{
    t_str s;
}

%token<s> UNRECOGNIZED SPACE STR CLIENT SERVER QUOTE REQUEST HOST
%type<s> value_str

%start msg_line

%%

msg_line
    : error     { YYABORT; }
    | str_list  { YYACCEPT; }
    ;

method: STR { lib -> ReceiverInterface::set( ReceiverInterface::method, $1 ); } ;
url:    STR
            {
                lib -> ReceiverInterface::set( ReceiverInterface::path, $1 );
                lib -> path_for_url_parser = string( $1.p, $1.n );
            } ;
vers:   STR { lib -> ReceiverInterface::set( ReceiverInterface::vers, $1 ); } ;

value_str
    : QUOTE STR QUOTE { $$ = $2; }
    | STR  { $$ = $1; }
    ;

str_elem
    : STR
    | CLIENT SPACE { msg_start_VALUE( scanner ); } value_str
        { lib -> set( MSGReceiver::client, $4 ); msg_pop_state( scanner ); }
    | SERVER SPACE { msg_start_VALUE( scanner ); } value_str
        { lib -> set( MSGReceiver::server, $4 ); msg_pop_state( scanner ); }
    | HOST SPACE { msg_start_VALUE( scanner ); } value_str
        { lib -> set( MSGReceiver::host, $4 ); msg_pop_state( scanner ); }
    | REQUEST SPACE { msg_start_REQUEST( scanner ); } QUOTE method SPACE url SPACE vers QUOTE
        { msg_pop_state( scanner ); }
    ;

str_list
    : str_elem
    | str_list str_elem
    | str_list SPACE str_elem
    ;

%%

void msg_error( yyscan_t locp, NCBI::Logging::MSGReceiver * lib, const char * msg )
{
    // intentionally left empty, we communicate errors rejected lines
}
