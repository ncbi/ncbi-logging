#include "MSG_Interface.hpp"

#include "msg_parser.hpp"
#include "msg_scanner.hpp"

#include <ncbi/json.hpp>
#include "Formatters.hpp"

extern YY_BUFFER_STATE msg_scan_bytes( const char * input, size_t size, yyscan_t yyscanner );

using namespace NCBI::Logging;
using namespace std;
using namespace ncbi;


MSGReceiver::MSGReceiver( FormatterRef fmt )
: ReceiverInterface ( fmt )
{
}

void MSGReceiver::set( MSG_Members m, const t_str & v )
{
#define CASE(mem) case mem: setMember( #mem, v ); break;
    switch( m )
    {
        CASE( client )
        CASE( server )
        CASE( host )
        default: ReceiverInterface::set((ReceiverInterface::Members)m, v);
    }
#undef CASE
    if ( m_cat == cat_unknown )
        m_cat = cat_good;
}

MSGParseBlock::MSGParseBlock( MSGReceiver & receiver )
: m_receiver ( receiver )
{
    msg_lex_init( &m_sc );
}

MSGParseBlock::~MSGParseBlock()
{
    msg_lex_destroy( m_sc );
}

void
MSGParseBlock::SetDebug( bool onOff )
{
    msg_debug = onOff ? 1 : 0;            // bison (op_debug is global)
    msg_set_debug( onOff ? 1 : 0, m_sc );   // flex
}

bool
MSGParseBlock::format_specific_parse( const char * line, size_t line_size )
{
    YY_BUFFER_STATE bs = msg_scan_bytes( line, line_size, m_sc );
    int ret = msg_parse( m_sc, & m_receiver );
    msg__delete_buffer( bs, m_sc );
    if ( ret != 0 )
        m_receiver . SetCategory( ReceiverInterface::cat_ugly );
    else if ( m_receiver .GetCategory() == ReceiverInterface::cat_unknown )
        m_receiver . SetCategory( ReceiverInterface::cat_good );
    return ret == 0;
}
