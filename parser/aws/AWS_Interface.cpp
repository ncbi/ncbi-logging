#include "AWS_Interface.hpp"

#include "aws_parser.hpp"
#include "aws_scanner.hpp"

#include <ncbi/json.hpp>

#include "Formatters.hpp"

extern YY_BUFFER_STATE aws_scan_bytes( const char * input, size_t size, yyscan_t yyscanner );

using namespace NCBI::Logging;
using namespace std;
using namespace ncbi;

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


bool
ReverseBlock::format_specific_parse( const char * line, size_t line_size )
{
    /* here we will take the line, and ask the vdb-3 lib to parse it into a JSONValueRef
       we will inspect it and call setters on the formatter to produce output */
    String src( line, line_size );
    ReceiverInterface &receiver = GetReceiver();
    FormatterInterface & formatter = receiver . GetFormatter();
    try
    {
        const JSONValueRef values = JSON::parse( src );
        extract_members( values -> toObject() );
        receiver . SetCategory( ReceiverInterface::cat_good );

        return true;
    }
    catch ( const ncbi::Exception &e )
    {
        formatter . addNameValue( "exception", e.what().zmsg );
        receiver . SetCategory( ReceiverInterface::cat_ugly );
    }
    return false;
}

        class AWSReverseBlock : public ReverseBlock
        {
        public:
            AWSReverseBlock( std::unique_ptr<FormatterInterface> & fmt );
            virtual ~AWSReverseBlock();

            virtual ReceiverInterface & GetReceiver() { return m_receiver; }

            virtual void extract_members( const JSONObject &obj );

            AWSReceiver m_receiver;
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
     std::unique_ptr<FormatterInterface> fmt = std::make_unique<ReverseFormatter>();
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
{ // no need to do anything here
}

AWSReverseBlock::~AWSReverseBlock()
{ // no need to do anything here
}

static void
extract_and_set( const JSONObject &obj, FormatterInterface &formatter, const char * fieldname, bool quote_spaces = true )
{
    const JSONValue &entry = obj . getValue ( fieldname );
    const String &S = entry . toString();
    if ( S . isEmpty() )
        formatter . addNameValue( fieldname, "-" );
    else
    {
        if ( quote_spaces )
        {
            if ( S . find( ' ' ) != String::npos )
            {
                std::stringstream ss;
                ss . put ( '"' );
                ss . write( S . data(), S . size() );
                ss . put ( '"' );
                formatter . addNameValue( fieldname, ss . str() );
            }
            else
                formatter . addNameValue( fieldname, S . toSTLString() );
        }
        else
            formatter . addNameValue( fieldname, S . toSTLString() );
        //todo: look for quotes, if found: escape them!
    }
}

static void
extract_and_set_url( const JSONObject &obj, FormatterInterface &formatter )
{
    const String & method = obj . getValue ( "method" ) . toString();
    const String & path = obj . getValue ( "path" ) . toString();
    const String & vers = obj . getValue ( "vers" ) . toString();
    if ( method.isEmpty() &&
         path.isEmpty() &&
         vers.isEmpty() )
    {
        formatter . addNameValue( "url", "-" );
    }
    else
    {
        std::stringstream ss;

        ss . put( '"' );
        ss . write( method . data(), method . size() );

        if ( ! method.isEmpty() && ! path.isEmpty() )
        {
            ss . put( ' ' );
        }

        ss . write( path . data(), path . size() );

        if ( ( ! method.isEmpty() || ! path.isEmpty() ) && ! vers.isEmpty() )
        {
            ss . put( ' ' );
        }

        ss . write( vers . data(), vers . size() );

        ss . put( '"' );

        formatter . addNameValue( "url", ss.str() );
    }
}

void
AWSReverseBlock::extract_members( const JSONObject &obj )
{
    FormatterInterface & formatter = GetReceiver() . GetFormatter();

    extract_and_set( obj, formatter, "owner" );
    extract_and_set( obj, formatter, "bucket" );
    extract_and_set( obj, formatter, "time", false );
    extract_and_set( obj, formatter, "ip" );
    extract_and_set( obj, formatter, "requester" );
    extract_and_set( obj, formatter, "request_id" );
    extract_and_set( obj, formatter, "operation" );
    extract_and_set( obj, formatter, "key" );
    extract_and_set_url( obj, formatter );
    extract_and_set( obj, formatter, "res_code" );
    extract_and_set( obj, formatter, "error" );
    extract_and_set( obj, formatter, "res_len" );
    extract_and_set( obj, formatter, "obj_size" );
    extract_and_set( obj, formatter, "total_time" );
    extract_and_set( obj, formatter, "turnaround_time" );
    extract_and_set( obj, formatter, "referer" );
    extract_and_set( obj, formatter, "agent" );
    extract_and_set( obj, formatter, "version_id" );
    extract_and_set( obj, formatter, "host_id", false );
    extract_and_set( obj, formatter, "sig_ver" );
    extract_and_set( obj, formatter, "cipher_suite" );
    extract_and_set( obj, formatter, "auth_type" );
    extract_and_set( obj, formatter, "host_header" );
    extract_and_set( obj, formatter, "tls_version" );
}
