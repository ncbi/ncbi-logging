#include "JWT_Interface.hpp"

#include "jwt_parser.hpp"
#include "jwt_scanner.hpp"

#include <ncbi/json.hpp>
#include "Formatters.hpp"

extern YY_BUFFER_STATE jwt_scan_bytes( const char * input, size_t size, yyscan_t yyscanner );

using namespace NCBI::Logging;
using namespace std;
using namespace ncbi;


JWTReceiver::JWTReceiver( unique_ptr<FormatterInterface> & fmt )
: ReceiverInterface ( fmt )
{
}

void JWTReceiver::set( JWT_Members m, const t_str & v )
{
#define CASE(mem) case mem: setMember( #mem, v ); break;
    switch( m )
    {
    CASE( jwt1 )
    CASE( jwt2 )
    default: ReceiverInterface::set((ReceiverInterface::Members)m, v);
    }
#undef CASE
    if ( m_cat == cat_unknown )
        m_cat = cat_good;
}

JWTParseBlock::JWTParseBlock( JWTReceiver & receiver )
: m_receiver ( receiver )
{
    jwt_lex_init( &m_sc );
}

JWTParseBlock::~JWTParseBlock()
{
    jwt_lex_destroy( m_sc );
}

void
JWTParseBlock::SetDebug( bool onOff )
{
    jwt_debug = onOff ? 1 : 0;            // bison (op_debug is global)
    jwt_set_debug( onOff ? 1 : 0, m_sc );   // flex
}

bool
JWTParseBlock::format_specific_parse( const char * line, size_t line_size )
{
    YY_BUFFER_STATE bs = jwt_scan_bytes( line, line_size, m_sc );
    int ret = jwt_parse( m_sc, & m_receiver );
    jwt__delete_buffer( bs, m_sc );
    return ret == 0;
}
