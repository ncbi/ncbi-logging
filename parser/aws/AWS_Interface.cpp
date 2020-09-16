#include "AWS_Interface.hpp"

#include "aws_parser.hpp"
#include "aws_scanner.hpp"

extern YY_BUFFER_STATE aws_scan_bytes( const char * input, size_t size, yyscan_t yyscanner );

using namespace NCBI::Logging;
using namespace std;

AWSReceiver::AWSReceiver( unique_ptr<FormatterInterface> & fmt )
: ReceiverInterface ( fmt )
{
}

void AWSReceiver::set( AWS_Members m, const t_str & v )
{
#define CASE(mem) case mem: setMember( #mem, v ); break;
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
    CASE( _extra )
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
        class AWSParseBlock : public ParseBlockInterface
        {
        public:
            AWSParseBlock( std::unique_ptr<FormatterInterface> & fmt );
            virtual ~AWSParseBlock();
            virtual ReceiverInterface & GetReceiver() { return m_receiver; }
            virtual bool format_specific_parse( const char * line, size_t line_size );
            virtual void SetDebug( bool onOff );

            yyscan_t m_sc;
            AWSReceiver m_receiver;
        };

        class AWSReverseBlock : public ParseBlockInterface
        {
        public:
            AWSReverseBlock( std::unique_ptr<FormatterInterface> & fmt );
            virtual ~AWSReverseBlock();
            virtual ReceiverInterface & GetReceiver() { return m_receiver; }
            virtual bool format_specific_parse( const char * line, size_t line_size );
            virtual void SetDebug( bool onOff );

            AWSReceiver m_receiver;
        };

        class AWSFormatter : public FormatterInterface
        {
        public:
            virtual ~AWSFormatter();

            virtual std::string format();

            virtual void addNameValue( const std::string & name, const t_str & value );
            virtual void addNameValue( const std::string & name, int64_t value );
            virtual void addNameValue( const std::string & name, const std::string & value );

        private:
            std::map< std::string, std::string > kv;
            std::stringstream ss;
        };

    }
}

/* ----------- AWSParseBlockFactory ----------- */
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

/* ----------- AWSReverseBlockFactory ----------- */
AWSReverseBlockFactory::~AWSReverseBlockFactory() {}

std::unique_ptr<ParseBlockInterface>
AWSReverseBlockFactory::MakeParseBlock() const
{
     std::unique_ptr<FormatterInterface> fmt = std::make_unique<AWSFormatter>();
    // return a revers-parseblock....
    return std::make_unique<AWSReverseBlock>( fmt );
}

/* ----------- AWSParseBlock ----------- */
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
AWSParseBlock::format_specific_parse( const char * line, size_t line_size )
{
    YY_BUFFER_STATE bs = aws_scan_bytes( line, line_size, m_sc );
    int ret = aws_parse( m_sc, & m_receiver );
    aws__delete_buffer( bs, m_sc );
    return ret == 0;
}

/* ----------- AWSReverseBlock ----------- */
AWSReverseBlock::AWSReverseBlock( std::unique_ptr<FormatterInterface> & fmt )
: m_receiver ( fmt )
{
}

AWSReverseBlock::~AWSReverseBlock()
{
}

void
AWSReverseBlock::SetDebug( bool onOff )
{
}

bool
AWSReverseBlock::format_specific_parse( const char * line, size_t line_size )
{
    /* here we will take the line, and ask the vdb-3 lib to parse it into a JSONValueRef
       we will inspect it and call setters on the formatter to produce output */
    return false;
}

/* ----------- AWSFormatter ----------- */
AWSFormatter::~AWSFormatter()
{
}

string AWSFormatter::format()
{
    return std::string();
}

void AWSFormatter::addNameValue( const std::string & name, const t_str & value )
{
}

void AWSFormatter::addNameValue( const std::string & name, int64_t value )
{
}

void AWSFormatter::addNameValue( const std::string & name, const std::string & value )
{
}
