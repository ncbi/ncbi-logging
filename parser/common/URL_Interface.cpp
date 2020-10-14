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

void URLReceiver::finalize( void )
{
    // todo: drop the boolean in t_str
    setMember( "accession", t_str{ m_accession.c_str(), m_accession.size(), false } );
    setMember( "filename",  t_str{ m_filename.c_str(), m_filename.size(), false } );
    setMember( "extension", t_str{ m_extension.c_str(), m_extension.size(), false } );
    m_accession . clear();
    m_filename . clear();
    m_extension . clear();
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

    return ret == 0; //&& ! m_receiver.m_accession.empty();
}
