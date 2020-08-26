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

%start log_tw

%%

log_tw
    : id1 id2 id3 time server ipaddr sid service event SEP { tw_start_MSG( scanner ); } msg
    {
        YYACCEPT;
    }
    | id1 id2 id3 time server ipaddr sid service event
    {
        SET_VALUE( TWReceiver::msg, EmptyTSTR );
        YYACCEPT;
    }
    | error     { YYABORT; }    
    ;

id1
    : ID1       { SET_VALUE( TWReceiver::id1, $1 ); }
    | UNRECOGNIZED  { lib -> reportField( "invalid ID1" ); YYACCEPT; }
    ;

id2
    : SEP { tw_start_ID2( scanner ); } ID2 { SET_VALUE( TWReceiver::id2, $3 ); }
    | UNRECOGNIZED  { lib -> reportField( "invalid ID2" ); YYACCEPT; }
    ;

id3
    : SEP { tw_start_ID3( scanner ); } ID3 { SET_VALUE( TWReceiver::id3, $3 ); }
    | UNRECOGNIZED  { lib -> reportField( "invalid ID3" ); YYACCEPT; }
    ;

time
    : SEP { tw_start_TIME( scanner ); } TIME { SET_VALUE( TWReceiver::time, $3 ); }
    | UNRECOGNIZED  { lib -> reportField( "invalid TIME" ); YYACCEPT; }
    ;

server
    : SEP { tw_start_SERVER( scanner ); } SERVER { SET_VALUE( TWReceiver::server, $3 ); }
    | UNRECOGNIZED  { lib -> reportField( "invalid SERVER" ); YYACCEPT; }
    ;

ipaddr
    : SEP { tw_start_IPADDR( scanner ); } IPADDR { SET_VALUE( ReceiverInterface::ip, $3 ); }
    | UNRECOGNIZED  { lib -> reportField( "invalid IPADDR" ); YYACCEPT; }
    ;

sid
    : SEP { tw_start_SID( scanner ); } SID { SET_VALUE( TWReceiver::sid, $3 ); }
    | UNRECOGNIZED  { lib -> reportField( "invalid SID" ); YYACCEPT; }
    ;

service
    : SEP { tw_start_SERVICE( scanner ); } SERVICE { SET_VALUE( TWReceiver::service, $3 ); }
    | UNRECOGNIZED  { lib -> reportField( "invalid SERVICE" ); YYACCEPT; }
    ;

event
    : SEP { tw_start_EVENT( scanner ); } EVENT { SET_VALUE( TWReceiver::event, $3 ); }
    | UNRECOGNIZED  { lib -> reportField( "invalid EVENT" ); YYACCEPT; }    
    ;

msg
    : MSG        { SET_VALUE( TWReceiver::msg, $1 ); }
    | %empty     { SET_VALUE( TWReceiver::msg, EmptyTSTR ); }
    ;

%%

void tw_error( yyscan_t locp, NCBI::Logging::TWReceiver * lib, const char * msg )
{
    // intentionally left empty, we communicate errors rejected lines
}
