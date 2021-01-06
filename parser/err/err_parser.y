%define api.pure full
%lex-param { void * scanner }
%parse-param { void * scanner }{ NCBI::Logging::ERRReceiver * lib }

%define parse.trace
%define parse.error verbose

%name-prefix "err_"

%{
#define YYDEBUG 1

#include <stdint.h>
#include "err_parser.hpp"
#include "err_scanner.hpp"

using namespace std;
using namespace NCBI::Logging;

void err_error( yyscan_t locp, NCBI::Logging::ERRReceiver * lib, const char* msg );
void err_start_MSG( yyscan_t yyscanner );

const t_str EmptyTSTR = { "", 0 };

%}

%code requires
{
#include "types.h"
#include "ERR_Interface.hpp"

using namespace NCBI::Logging;
}

%define api.value.type {t_str}

%token UNRECOGNIZED SPACE STR DATETIME SEVERITY MSG

%start log_line

%%

log_line
    : error { YYABORT; }
    | datetime SPACE severity SPACE pid SPACE { err_start_MSG( scanner ); } msg
        {
            YYACCEPT;
        }
    | STR SPACE { err_start_MSG( scanner ); } MSG
        {
            if ( 0 == strncmp( $1.p, "file-meta", $1.n ) )
            {
                lib -> SetCategory( ReceiverInterface::cat_ignored );
            }
            else
            {
                lib -> SetCategory( ReceiverInterface::cat_review );
            }
            YYACCEPT;
        }
    ;

datetime
    : DATETIME { lib -> set( ERRReceiver::datetime, $1 ); }
    ;

severity
    : SEVERITY
    {
        lib -> set( ERRReceiver::severity, $1 );
        if ( 0 == strncmp( $1.p, "[notice]", $1.n ) )
        {
            lib -> SetCategory( ReceiverInterface::cat_ignored );
        }
    }
    ;

pid
    : STR { lib -> set( ERRReceiver::pid, $1 ); }
    ;

msg
    : MSG
    {
        lib -> set( ERRReceiver::msg, $1 );
        lib -> msg_for_postprocess = string( $1.p, $1.n );
    }
    ;

%%

void err_error( yyscan_t locp, NCBI::Logging::ERRReceiver * lib, const char * msg )
{
    // intentionally left empty, we communicate errors rejected lines
}
