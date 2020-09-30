#include "AGENT_Interface.hpp"

#include "agent_parser.hpp"
#include "agent_scanner.hpp"

#include <ncbi/json.hpp>
#include "Formatters.hpp"

extern YY_BUFFER_STATE agent_scan_bytes( const char * input, size_t size, yyscan_t yyscanner );

using namespace NCBI::Logging;
using namespace std;
using namespace ncbi;


AGENTReceiver::AGENTReceiver( FormatterRef fmt )
: ReceiverInterface ( fmt )
{
}

void AGENTReceiver::set( AGENT_Members m, const t_str & v )
{
#define CASE(mem) case mem: setMember( #mem, v ); break;
    switch( m )
    {
        CASE( os )
        CASE( tool )
        CASE( release )
        CASE( phid_compute_env )
        CASE( phid_guid )
        CASE( phid_session_id )
        CASE( libc )
        default: ReceiverInterface::set((ReceiverInterface::Members)m, v);
    }
#undef CASE
    if ( m_cat == cat_unknown )
        m_cat = cat_good;
}

/* -------------------------------------------------------------------------- */

AGENTParseBlock::AGENTParseBlock( AGENTReceiver & receiver )
: m_receiver ( receiver )
{
    agent_lex_init( &m_sc );
}

AGENTParseBlock::~AGENTParseBlock()
{
    agent_lex_destroy( m_sc );
}

void
AGENTParseBlock::SetDebug( bool onOff )
{
    agent_debug = onOff ? 1 : 0;            // bison (op_debug is global)
    agent_set_debug( onOff ? 1 : 0, m_sc );   // flex
}

bool
AGENTParseBlock::format_specific_parse( const char * line, size_t line_size )
{
    YY_BUFFER_STATE bs = agent_scan_bytes( line, line_size, m_sc );
    int ret = agent_parse( m_sc, & m_receiver );
    agent__delete_buffer( bs, m_sc );
    return ret == 0;
}
