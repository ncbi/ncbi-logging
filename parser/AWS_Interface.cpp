#include "AWS_Interface.hpp"

#include "aws_v2_parser.hpp"
#include "aws_v2_scanner.hpp"

extern YY_BUFFER_STATE aws_scan_reset( const char * input, yyscan_t yyscanner );

using namespace NCBI::Logging;
using namespace std;

AWSReceiver::AWSReceiver( unique_ptr<FormatterInterface> & fmt )
: ReceiverInterface ( fmt )
{
}

void AWSReceiver::set( AWS_Members m, const t_str & v ) 
{
#define CASE(mem) case mem: m_fmt -> addNameValue(#mem, v); break;
    switch( m )
    {
    CASE( owner )
    CASE( bucket )
    CASE( time )
    CASE( requester )
    CASE( request_id )
    CASE( operation )
    CASE( key )
    CASE( res_code )
    CASE( error )
    CASE( res_len )
    CASE( obj_size )
    CASE( total_time )
    CASE( turnaround_time )
    CASE( version_id )
    CASE( host_id )
    CASE( sig_ver )
    CASE( cipher_suite )
    CASE( auth_type )
    CASE( host_header )
    CASE( tls_version )
    default: ReceiverInterface::set((ReceiverInterface::Members)m, v); 
    }
#undef CASE
    if ( m_cat == cat_unknown )
        m_cat = cat_good;
}

/* should be lifted into ReceiverInterface.cpp, because the same for every format */
void AWSReceiver::reportField( const char * message ) 
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
        class AWSParseBlock : public ParseBlockInterface
        {
        public:
            AWSParseBlock( std::unique_ptr<FormatterInterface> & fmt ); 
            virtual ~AWSParseBlock();
            virtual ReceiverInterface & GetReceiver() { return m_receiver; }
            virtual bool Parse( const std::string & line );
            virtual void SetDebug( bool onOff );

            yyscan_t m_sc;
            AWSReceiver m_receiver;
        };
    }
}

AWSParseBlockFactory::~AWSParseBlockFactory() {}

std::unique_ptr<ParseBlockInterface> 
AWSParseBlockFactory::MakeParseBlock() const
{
    std::unique_ptr<FormatterInterface> fmt;
    if ( m_fast )
        fmt = std::make_unique<JsonFastFormatter>();
    else
        fmt = std::make_unique<JsonLibFormatter>();
    return std::make_unique<AWSParseBlock>( fmt );     
}

AWSParseBlock::AWSParseBlock( std::unique_ptr<FormatterInterface> & fmt ) 
: m_receiver ( fmt )
{
    aws_lex_init( &m_sc );
}

AWSParseBlock::~AWSParseBlock() 
{
    aws_lex_destroy( m_sc );
}

void 
AWSParseBlock::SetDebug( bool onOff )
{
    aws_debug = onOff ? 1 : 0;            // bison (aws_debug is global)
    aws_set_debug( onOff ? 1 : 0, m_sc );   // flex
}

bool 
AWSParseBlock::Parse( const string & line )
{
    YY_BUFFER_STATE bs = aws_scan_reset( line.c_str(), m_sc );
    int ret = aws_parse( m_sc, & m_receiver );
    aws__delete_buffer( bs, m_sc );
    return ret == 0;
}

