#include "CL_PATH_Interface.hpp"

#include "cl_path_parser.hpp"
#include "cl_path_scanner.hpp"

#include <ncbi/json.hpp>
#include "Formatters.hpp"

extern YY_BUFFER_STATE cl_path_scan_bytes( const char * input, size_t size, yyscan_t yyscanner );

using namespace NCBI::Logging;
using namespace std;
using namespace ncbi;


CLPATHReceiver::CLPATHReceiver( FormatterRef fmt )
: ReceiverInterface ( fmt )
{
}

void CLPATHReceiver::finalize( void )
{
    setMember( "accession", t_str{ m_accession.c_str(), m_accession.size() } );
    setMember( "filename",  t_str{ m_filename.c_str(), m_filename.size() } );
    setMember( "extension", t_str{ m_extension.c_str(), m_extension.size() } );
    m_accession . clear();
    m_filename . clear();
    m_extension . clear();
}

/* -------------------------------------------------------------------------- */

CLPATHParseBlock::CLPATHParseBlock( CLPATHReceiver & receiver )
: m_receiver ( receiver )
{
    cl_path_lex_init( &m_sc );
}

CLPATHParseBlock::~CLPATHParseBlock()
{
    cl_path_lex_destroy( m_sc );
}

bool
CLPATHParseBlock::format_specific_parse( const char * line, size_t line_size )
{
    cl_path_debug = m_debug ? 1 : 0;                // bison (is global)
    cl_path_set_debug( m_debug ? 1 : 0, m_sc );   // flex

    YY_BUFFER_STATE bs = cl_path_scan_bytes( line, line_size, m_sc );
    int ret = cl_path_parse( m_sc, & m_receiver );
    cl_path__delete_buffer( bs, m_sc );

    if ( ret != 0 )
        m_receiver . SetCategory( ReceiverInterface::cat_ugly );
    else if ( m_receiver .GetCategory() == ReceiverInterface::cat_unknown )
        m_receiver . SetCategory( ReceiverInterface::cat_good );
    return ret == 0;
}
