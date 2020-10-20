%define api.pure full
%lex-param { void * scanner }
%parse-param { void * scanner }{ NCBI::Logging::CLReceiver * lib }

%define parse.trace
%define parse.error verbose

%name-prefix "cl_"

%{
#define YYDEBUG 1

#include <stdint.h>
#include "cl_parser.hpp"
#include "cl_scanner.hpp"

using namespace std;
using namespace NCBI::Logging;

void cl_error( yyscan_t locp, NCBI::Logging::CLReceiver * lib, const char* msg );

const t_str EmptyTSTR = { "", 0 };

#define SET_VALUE( selector, source )   ( lib -> set( (selector), (source) ) )

%}

%code requires
{
#include "types.h"
#include "CL_Interface.hpp"

using namespace NCBI::Logging;
}

%token<s> UNRECOGNIZED

%union
{
    t_str s;
}

%start line

%%

line
    : { YYACCEPT; }
    ;

%%

void cl_error( yyscan_t locp, NCBI::Logging::CLReceiver * lib, const char * msg )
{
    // intentionally left empty, we communicate errors rejected lines
}
