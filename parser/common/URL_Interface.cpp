#include "URL_Interface.hpp"

#include "url_parser.hpp"
#include "url_scanner.hpp"

#include <ncbi/json.hpp>
#include "Formatters.hpp"

extern YY_BUFFER_STATE url_scan_bytes( const char * input, size_t size, yyscan_t yyscanner );

using namespace NCBI::Logging;
using namespace std;
using namespace ncbi;


URLReceiver::URLReceiver( FormatterRef fmt )
: ReceiverInterface ( fmt )
{
}

void URLReceiver::set( URL_Members m, const t_str & v )
{
#define CASE(mem) case mem: setMember( #mem, v ); break;
    switch( m )
    {
        CASE( accession )
        CASE( filename )
        CASE( extension )
        default: ReceiverInterface::set((ReceiverInterface::Members)m, v);
    }
#undef CASE
    if ( m_cat == cat_unknown )
        m_cat = cat_good;
}

/* -------------------------------------------------------------------------- */

URLParseBlock::URLParseBlock( URLReceiver & receiver )
: m_receiver ( receiver )
{
    url_lex_init( &m_sc );
}

URLParseBlock::~URLParseBlock()
{
    url_lex_destroy( m_sc );
}

void
URLParseBlock::SetDebug( bool onOff )
{
    url_debug = onOff ? 1 : 0;            // bison (op_debug is global)
    url_set_debug( onOff ? 1 : 0, m_sc );   // flex
}

bool
URLParseBlock::format_specific_parse( const char * line, size_t line_size )
{
    YY_BUFFER_STATE bs = url_scan_bytes( line, line_size, m_sc );
    int ret = url_parse( m_sc, & m_receiver );
    url__delete_buffer( bs, m_sc );

    // any members that were not found should be set to empty string
    // if they are set already, the extra call to set() will be ignored
    t_str empty = {nullptr, 0, false };
    m_receiver.set( URLReceiver::accession, empty );
    m_receiver.set( URLReceiver::filename, empty );
    m_receiver.set( URLReceiver::extension, empty );

    return ret == 0;
}
