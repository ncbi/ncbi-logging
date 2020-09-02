#include "OP_Interface.hpp"

#include "op_v2_parser.hpp"
#include "op_v2_scanner.hpp"

extern YY_BUFFER_STATE op_scan_bytes( const char * input, size_t size, yyscan_t yyscanner );

using namespace NCBI::Logging;
using namespace std;

OPReceiver::OPReceiver( unique_ptr<FormatterInterface> & fmt )
: ReceiverInterface ( fmt )
{
}

void OPReceiver::set( OP_Members m, const t_str & v )
{
#define CASE(mem) case mem: setMember( #mem, v ); break;
    switch( m )
    {
    CASE( owner )
    CASE( time )
    CASE( server )
    CASE( user )
    CASE( req_time )
    CASE( forwarded )
    CASE( port )
    CASE( req_len )
    CASE( res_code )
    CASE( res_len )
    default: ReceiverInterface::set((ReceiverInterface::Members)m, v);
    }
#undef CASE
    if ( m_cat == cat_unknown )
        m_cat = cat_good;
}

namespace NCBI
{
    namespace Logging
    {
        class OPParseBlock : public ParseBlockInterface
        {
        public:
            OPParseBlock( std::unique_ptr<FormatterInterface> & fmt );
            virtual ~OPParseBlock();
            virtual ReceiverInterface & GetReceiver() { return m_receiver; }
            virtual bool Parse( const char * line, size_t line_size );
            virtual void SetDebug( bool onOff );

            yyscan_t m_sc;
            OPReceiver m_receiver;
        };
    }
}

OPParseBlockFactory::~OPParseBlockFactory() {}

std::unique_ptr<ParseBlockInterface>
OPParseBlockFactory::MakeParseBlock() const
{
    std::unique_ptr<FormatterInterface> fmt;
    if ( m_fast )
        fmt = std::make_unique<JsonFastFormatter>();
    else
        fmt = std::make_unique<JsonLibFormatter>();
    return std::make_unique<OPParseBlock>( fmt );
}

OPParseBlock::OPParseBlock( std::unique_ptr<FormatterInterface> & fmt )
: m_receiver ( fmt )
{
    op_lex_init( &m_sc );
}

OPParseBlock::~OPParseBlock()
{
    op_lex_destroy( m_sc );
}

void
OPParseBlock::SetDebug( bool onOff )
{
    op_debug = onOff ? 1 : 0;            // bison (op_debug is global)
    op_set_debug( onOff ? 1 : 0, m_sc );   // flex
}

bool
OPParseBlock::Parse( const char * line, size_t line_size )
{
    YY_BUFFER_STATE bs = op_scan_bytes( line, line_size, m_sc );
    int ret = op_parse( m_sc, & m_receiver );
    op__delete_buffer( bs, m_sc );
    return ret == 0;
}
