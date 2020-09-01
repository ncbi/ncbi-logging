#include "TW_Interface.hpp"

#include "tw_v2_parser.hpp"
#include "tw_v2_scanner.hpp"

extern YY_BUFFER_STATE tw_scan_bytes( const char * input, size_t size, yyscan_t yyscanner );

using namespace NCBI::Logging;
using namespace std;

TWReceiver::TWReceiver( unique_ptr<FormatterInterface> & fmt )
: ReceiverInterface ( fmt )
{
}

void TWReceiver::set( TW_Members m, const t_str & v )
{
#define CASE(mem) case mem: setMember( #mem, v ); break;
    switch( m )
    {
    CASE( id1 )
    CASE( id2 )
    CASE( id3 )
    CASE( time )
    CASE( server )
    CASE( ip )
    CASE( sid )
    CASE( service )
    CASE( event )
    CASE( msg )
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
        class TWParseBlock : public ParseBlockInterface
        {
        public:
            TWParseBlock( std::unique_ptr<FormatterInterface> & fmt );
            virtual ~TWParseBlock();
            virtual ReceiverInterface & GetReceiver() { return m_receiver; }
            virtual bool Parse( const std::string & line );
            virtual bool Parse( const char * line, size_t line_size );
            virtual void SetDebug( bool onOff );

            yyscan_t m_sc;
            TWReceiver m_receiver;
        };
    }
}

TWParseBlockFactory::~TWParseBlockFactory() {}

std::unique_ptr<ParseBlockInterface>
TWParseBlockFactory::MakeParseBlock() const
{
    std::unique_ptr<FormatterInterface> fmt;
    if ( m_fast )
        fmt = std::make_unique<JsonFastFormatter>();
    else
        fmt = std::make_unique<JsonLibFormatter>();
    return std::make_unique<TWParseBlock>( fmt );
}

TWParseBlock::TWParseBlock( std::unique_ptr<FormatterInterface> & fmt )
: m_receiver ( fmt )
{
    tw_lex_init( &m_sc );
}

TWParseBlock::~TWParseBlock()
{
    tw_lex_destroy( m_sc );
}

void
TWParseBlock::SetDebug( bool onOff )
{
    tw_debug = onOff ? 1 : 0;            // bison (op_debug is global)
    tw_set_debug( onOff ? 1 : 0, m_sc );   // flex
}

bool
TWParseBlock::Parse( const string & line )
{
    return Parse( line.c_str(), line.size() );
}

bool
TWParseBlock::Parse( const char * line, size_t line_size )
{
    YY_BUFFER_STATE bs = tw_scan_bytes( line, line_size, m_sc );
    int ret = tw_parse( m_sc, & m_receiver );
    tw__delete_buffer( bs, m_sc );
    return ret == 0;
}
