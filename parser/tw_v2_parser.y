%define api.pure full
%lex-param { void * scanner }
%parse-param { void * scanner }{ NCBI::Logging::TWReceiver * lib }

%define parse.trace
%define parse.error verbose

%name-prefix "tw_"

%{
#define YYDEBUG 1

#include <stdint.h>
#include "tw_v2_parser.hpp"
#include "tw_v2_scanner.hpp"

using namespace std;
using namespace NCBI::Logging;

void tw_error( yyscan_t locp, NCBI::Logging::TWReceiver * lib, const char* msg );

const t_str EmptyTSTR = { "", 0, false };

#define SET_VALUE( selector, source )   ( lib -> set( (selector), (source) ) )

%}

%code requires
{
#include "types.h"
#include "TW_Interface.hpp"

extern void tw_get_scanner_input( void * yyscanner, t_str & str );
extern void tw_start_ID1( void * yyscanner );
extern void tw_start_ID2( void * yyscanner );
extern void tw_start_ID3( void * yyscanner );
extern void tw_start_TIME( void * yyscanner );
extern void tw_start_SERVER( void * yyscanner );
extern void tw_start_IPADDR( void * yyscanner );
extern void tw_start_SID( void * yyscanner );
extern void tw_start_SERVICE( void * yyscanner );
extern void tw_start_EVENT( void * yyscanner );
extern void tw_start_MSG( void * yyscanner );

extern void tw_pop_state( void * yyscanner );

using namespace NCBI::Logging;
}

%union
{
    t_str s;
}

%token<s> ID1 ID2 ID3 TIME SERVER IPADDR SID SERVICE EVENT MSG
%token SEP UNRECOGNIZED

%type<s> id1 id2 id3 time server ipaddr sid service event msg

%start tw_line

%%

tw_line
    : log_tw        { YYACCEPT; }
    | log_tw_err    { YYABORT; }
    ;

log_tw
    : id1 SEP id2 SEP id3 SEP time SEP server SEP ipaddr SEP sid SEP service SEP event SEP msg
    {
    }
    ;

log_tw_err
    : id1 error
    | id1 SEP error
    | id1 SEP id2 error
    | id1 SEP id2 SEP error
    | id1 SEP id2 SEP id3 error
    | id1 SEP id2 SEP id3 SEP error
    | id1 SEP id2 SEP id3 SEP time error
    | id1 SEP id2 SEP id3 SEP time SEP error
    | id1 SEP id2 SEP id3 SEP time SEP server error
    | id1 SEP id2 SEP id3 SEP time SEP server SEP error
    | id1 SEP id2 SEP id3 SEP time SEP server SEP ipaddr error
    | id1 SEP id2 SEP id3 SEP time SEP server SEP ipaddr SEP error
    | id1 SEP id2 SEP id3 SEP time SEP server SEP ipaddr SEP sid error
    | id1 SEP id2 SEP id3 SEP time SEP server SEP ipaddr SEP sid SEP error
    | id1 SEP id2 SEP id3 SEP time SEP server SEP ipaddr SEP sid SEP service error
    | id1 SEP id2 SEP id3 SEP time SEP server SEP ipaddr SEP sid SEP service SEP error
    | id1 SEP id2 SEP id3 SEP time SEP server SEP ipaddr SEP sid SEP service SEP event error
    | id1 SEP id2 SEP id3 SEP time SEP server SEP ipaddr SEP sid SEP service SEP event SEP error
    ;

id1
    : { tw_start_ID1( scanner ); } ID1      { SET_VALUE( TWReceiver::id1, $2 ); }
    ;

id2
    : { tw_start_ID2( scanner ); } ID2      { SET_VALUE( TWReceiver::id2, $2 ); }
    ;

id3
    : { tw_start_ID3( scanner ); } ID3      { SET_VALUE( TWReceiver::id3, $2 ); }
    ;

time
    : { tw_start_TIME( scanner ); } TIME    { SET_VALUE( TWReceiver::time, $2 ); }
    ;

server
    : { tw_start_SERVER( scanner ); } SERVER  { SET_VALUE( TWReceiver::server, $2 ); }
    ;

ipaddr
    : { tw_start_IPADDR( scanner ); } IPADDR  { SET_VALUE( ReceiverInterface::ip, $2 ); }
    ;

sid
    : { tw_start_SID( scanner ); } IPADDR  { SET_VALUE( TWReceiver::sid, $2 ); }
    ;

service
    : { tw_start_SERVICE( scanner ); } IPADDR  { SET_VALUE( TWReceiver::service, $2 ); }
    ;

event
    : { tw_start_EVENT( scanner ); } IPADDR  { SET_VALUE( TWReceiver::event, $2 ); }
    ;

msg
    : { tw_start_MSG( scanner ); } IPADDR  { SET_VALUE( TWReceiver::msg, $2 ); }
    ;

%%

void tw_error( yyscan_t locp, NCBI::Logging::TWReceiver * lib, const char * msg )
{
    // intentionally left empty, we communicate errors rejected lines
}
