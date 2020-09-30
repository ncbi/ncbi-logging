#include "GCP_Interface.hpp"

#include "gcp_parser.hpp"
#include "gcp_scanner.hpp"
#include <ncbi/json.hpp>

#include "Formatters.hpp"
#include "AGENT_Interface.hpp"

extern YY_BUFFER_STATE gcp_scan_bytes( const char * input, size_t size, yyscan_t yyscanner );

using namespace NCBI::Logging;
using namespace std;
using namespace ncbi;

GCPReceiver::GCPReceiver( ReceiverInterface::FormatterRef fmt )
: ReceiverInterface ( fmt )
{
}

void GCPReceiver::set( GCP_Members m, const t_str & v )
{
#define CASE(mem) case mem: setMember( #mem, v ); break;
    switch( m )
    {
    CASE( time )
    CASE( ip_type )
    CASE( ip_region )
    CASE( uri )
    CASE( status )
    CASE( request_bytes )
    CASE( result_bytes )
    CASE( time_taken )
    CASE( host )
    CASE( request_id )
    CASE( operation )
    CASE( bucket )
    default: ReceiverInterface::set((ReceiverInterface::Members)m, v);
    }
#undef CASE
    if ( m_cat == cat_unknown )
        m_cat = cat_good;
}

void GCPReceiver::post_process( void )
{
    AGENTReceiver agt( m_fmt );
    AGENTParseBlock pb ( agt );
    pb.format_specific_parse( agent_for_postprocess.c_str(), agent_for_postprocess.size() );
}

namespace NCBI
{
    namespace Logging
    {
        class GCPParseBlock : public ParseBlockInterface
        {
        public:
            GCPParseBlock( ReceiverInterface::FormatterRef fmt );
            virtual ~GCPParseBlock();
            virtual ReceiverInterface & GetReceiver() { return m_receiver; }
            virtual bool format_specific_parse( const char * line, size_t line_size );
            virtual void SetDebug( bool onOff );

            yyscan_t m_sc;
            GCPReceiver m_receiver;
        };

        class GCPReverseBlock : public ParseBlockInterface
        {
        public:
            GCPReverseBlock( ReceiverInterface::FormatterRef fmt );
            virtual ~GCPReverseBlock();
            virtual ReceiverInterface & GetReceiver() { return m_receiver; }
            virtual bool format_specific_parse( const char * line, size_t line_size );
            virtual void SetDebug( bool onOff );

            GCPReceiver m_receiver;
        };

    }
}

/* ----------- GCPParseBlockFactory----------- */

GCPParseBlockFactory::~GCPParseBlockFactory() {}

std::unique_ptr<ParseBlockInterface>
GCPParseBlockFactory::MakeParseBlock() const
{
    ReceiverInterface::FormatterRef fmt;
    if ( m_fast )
        fmt = std::make_unique<JsonFastFormatter>();
    else
        fmt = std::make_unique<JsonLibFormatter>();
    return std::make_unique<GCPParseBlock>( fmt );
}

/* ----------- GCPParseBlock----------- */

GCPParseBlock::GCPParseBlock( ReceiverInterface::FormatterRef fmt )
: m_receiver ( fmt )
{
    gcp_lex_init( &m_sc );
}

GCPParseBlock::~GCPParseBlock()
{
    gcp_lex_destroy( m_sc );
}

void
GCPParseBlock::SetDebug( bool onOff )
{
    gcp_debug = onOff ? 1 : 0;            // bison (gcp_debug is global)
    gcp_set_debug( onOff ? 1 : 0, m_sc );   // flex
}

static string DefaultHeader = "\"time_micros\",\"c_ip\",\"c_ip_type\",\"c_ip_region\",\"cs_method\",\"cs_uri\",\"sc_status\",\"cs_bytes\",\"sc_bytes\",\"time_taken_micros\",\"cs_host\",\"cs_referer\",\"cs_user_agent\",\"s_request_id\",\"cs_operation\",\"cs_bucket\",\"cs_object\"";

bool
GCPParseBlock::format_specific_parse( const char * line, size_t line_size )
{
    YY_BUFFER_STATE bs = gcp_scan_bytes( line, line_size, m_sc );
    int ret = gcp_parse( m_sc, & m_receiver );
    if ( m_receiver.GetCategory() == ReceiverInterface::cat_ignored )
    {
        if ( 0 != strncmp( DefaultHeader.c_str(), line, DefaultHeader.size() ) )
        {
            m_receiver.SetCategory( ReceiverInterface::cat_review );
        }
    }
    gcp__delete_buffer( bs, m_sc );
    return ret == 0;
}

/* ----------- GCPReverseBlockFactory ----------- */
GCPReverseBlockFactory::~GCPReverseBlockFactory() {}

std::unique_ptr<ParseBlockInterface>
GCPReverseBlockFactory::MakeParseBlock() const
{
    ReceiverInterface::FormatterRef fmt = std::make_unique<ReverseFormatter>( ',' );
    // return a revers-parseblock....
    return std::make_unique<GCPReverseBlock>( fmt );
}

/* ----------- GCPReverseBlock ----------- */
GCPReverseBlock::GCPReverseBlock( ReceiverInterface::FormatterRef fmt )
: m_receiver ( fmt )
{ // no need to do anything here
}

GCPReverseBlock::~GCPReverseBlock()
{ // no need to do anything here
}

void
GCPReverseBlock::SetDebug( bool onOff )
{ // no need to do anything here
}

static void
extract_and_set( const JSONObject &obj, FormatterInterface &formatter, const char * fieldname )
{
    const JSONValue &entry = obj . getValue ( fieldname );
    const String &S = entry . toString();
    if ( S . isEmpty() )
    {
        formatter . addNameValue( fieldname, "\"\"" );
    }
    else
    {
        std::stringstream ss;
        ss . put ( '"' );
        ss . write( S . data(), S . size() );
        ss . put ( '"' );
        formatter . addNameValue( fieldname, ss . str() );
    }
}

bool
GCPReverseBlock::format_specific_parse( const char * line, size_t line_size )
{
    /* here we will take the line, and ask the vdb-3 lib to parse it into a JSONValueRef
       we will inspect it and call setters on the formatter to produce output */
    String src( line, line_size );
    ReceiverInterface &receiver = GetReceiver();
    FormatterInterface &formatter = receiver . GetFormatter();
    try
    {
        const JSONValueRef values = JSON::parse( src );
        const JSONObject &obj = values -> toObject();

        extract_and_set( obj, formatter, "time" );
        extract_and_set( obj, formatter, "ip" );
        extract_and_set( obj, formatter, "ip_type" );
        extract_and_set( obj, formatter, "ip_region" );
        extract_and_set( obj, formatter, "method" );
        extract_and_set( obj, formatter, "uri" );
        extract_and_set( obj, formatter, "status" );
        extract_and_set( obj, formatter, "request_bytes" );
        extract_and_set( obj, formatter, "result_bytes" );
        extract_and_set( obj, formatter, "time_taken" );
        extract_and_set( obj, formatter, "host" );
        extract_and_set( obj, formatter, "referer" );
        extract_and_set( obj, formatter, "agent" );
        extract_and_set( obj, formatter, "request_id" );
        extract_and_set( obj, formatter, "operation" );
        extract_and_set( obj, formatter, "bucket" );
        extract_and_set( obj, formatter, "path" );

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
