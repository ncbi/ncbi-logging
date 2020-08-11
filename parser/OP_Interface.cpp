#include "OP_Interface.hpp"

#include "op_v2_parser.hpp"
#include "op_v2_scanner.hpp"

extern YY_BUFFER_STATE op_scan_reset( const char * input, yyscan_t yyscanner );

using namespace NCBI::Logging;
using namespace std;

LogOPEvent::LogOPEvent( unique_ptr<FormatterInterface> & fmt )
: LogLinesInterface ( fmt )
{
}

void LogOPEvent::set( OP_Members m, const t_str & v ) 
{
#define CASE(mem) case mem: m_fmt -> addNameValue(#mem, v); break;
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
    default: LogLinesInterface::set((LogLinesInterface::Members)m, v); 
    }
#undef CASE
    if ( m_cat == cat_unknown )
        m_cat = cat_good;
}

void LogOPEvent::reportField( const char * message ) 
{
    if ( m_cat == cat_unknown || m_cat == cat_good )
        m_cat = cat_review;
    try
    {
        t_str msg { message, (int)strlen( message ), false };
        m_fmt -> addNameValue( "_error", msg );
    }
    catch( const ncbi::JSONUniqueConstraintViolation & e )
    {
        // in case we already inserted a parse-error value, we do nothing...
    }
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
            virtual LogLinesInterface & GetReceiver() { return m_receiver; }
            virtual bool Parse( const std::string & line );
            virtual void SetDebug( bool onOff );

            yyscan_t m_sc;
            LogOPEvent m_receiver;
        };
    }
}

OPParseBlockFactory::~OPParseBlockFactory() {}

std::unique_ptr<ParseBlockInterface> 
OPParseBlockFactory::MakeParseBlock() const
{
    std::unique_ptr<FormatterInterface> fmt = std::make_unique<JsonLibFormatter>();
    //std::unique_ptr<FormatterInterface> fmt = std::make_unique<JsonFastFormatter>();
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
OPParseBlock::Parse( const string & line )
{
    YY_BUFFER_STATE bs = op_scan_reset( line.c_str(), m_sc );
    int ret = op_parse( m_sc, & m_receiver );
    op__delete_buffer( bs, m_sc );
    return ret == 0;
}

