#include "CL_Interface.hpp"

#include "cl_parser.hpp"
#include "cl_scanner.hpp"
#include <ncbi/json.hpp>

#include "Formatters.hpp"
#include "URL_Interface.hpp"

extern YY_BUFFER_STATE cl_scan_bytes( const char * input, size_t size, yyscan_t yyscanner );

using namespace NCBI::Logging;
using namespace std;
using namespace ncbi;

CLReceiver::CLReceiver( ReceiverInterface::FormatterRef fmt )
: ReceiverInterface ( fmt )
{
}

void CLReceiver::set( CL_Members m, const t_str & v )
{
#define CASE(mem) case mem: setMember( #mem, v ); break;
    switch( m )
    {
    CASE(syslog_prefix)
    CASE(timestamp)
    CASE(owner)
    CASE(bucket)
    CASE(unknown1)
    CASE(requestHdrSize)
    CASE(requestBodySize)
    CASE(responseHdrSize)
    CASE(responseBodySize)
    CASE(totalSize)
    CASE(unknown2)
    CASE(httpStatus)
    CASE(reqId)
    CASE(unknown3)
    CASE(eTag)
    CASE(errorCode)
    default: ReceiverInterface::set((ReceiverInterface::Members)m, v);
    }
#undef CASE
    if ( m_cat == cat_unknown )
        m_cat = cat_good;
}

ReceiverInterface::Category CLReceiver::post_process( void )
{
    URLReceiver url( m_fmt );
    URLParseBlock pb ( url );
    pb.format_specific_parse( obj_for_postprocess.c_str(), obj_for_postprocess.size() );

    obj_for_postprocess.clear();
    auto cat_res = url.GetCategory();
    url . finalize(); // this is special to url-parsing

    return cat_res;
}

namespace NCBI
{
    namespace Logging
    {
        class CLParseBlock : public ParseBlockInterface
        {
        public:
            CLParseBlock( ReceiverInterface::FormatterRef fmt );
            virtual ~CLParseBlock();
            virtual ReceiverInterface & GetReceiver() { return m_receiver; }
            virtual bool format_specific_parse( const char * line, size_t line_size );
            virtual void SetDebug( bool onOff );

            yyscan_t m_sc;
            CLReceiver m_receiver;
        };

        class CLReverseBlock : public ParseBlockInterface
        {
        public:
            CLReverseBlock( ReceiverInterface::FormatterRef fmt );
            virtual ~CLReverseBlock();
            virtual ReceiverInterface & GetReceiver() { return m_receiver; }
            virtual bool format_specific_parse( const char * line, size_t line_size );
            virtual void SetDebug( bool onOff );

            CLReceiver m_receiver;
        };

    }
}

/* ----------- CLParseBlockFactory ----------- */
CLParseBlockFactory::~CLParseBlockFactory() {}

std::unique_ptr<ParseBlockInterface>
CLParseBlockFactory::MakeParseBlock() const
{
    ReceiverInterface::FormatterRef fmt;
    if ( m_fast )
        fmt = std::make_unique<JsonFastFormatter>();
    else
        fmt = std::make_unique<JsonLibFormatter>();
    return std::make_unique<CLParseBlock>( fmt );
}

/* ----------- CLReverseBlockFactory ----------- */
CLReverseBlockFactory::~CLReverseBlockFactory() {}

std::unique_ptr<ParseBlockInterface>
CLReverseBlockFactory::MakeParseBlock() const
{
    ReceiverInterface::FormatterRef fmt = std::make_unique<ReverseFormatter>();
    // return a revers-parseblock....
    return std::make_unique<CLReverseBlock>( fmt );
}

/* ----------- CLParseBlock----------- */
CLParseBlock::CLParseBlock( ReceiverInterface::FormatterRef fmt )
: m_receiver ( fmt )
{
    cl_lex_init( &m_sc );
}

CLParseBlock::~CLParseBlock()
{
    cl_lex_destroy( m_sc );
}

void
CLParseBlock::SetDebug( bool onOff )
{
    cl_debug = onOff ? 1 : 0;            // bison (cl_debug is global)
    cl_set_debug( onOff ? 1 : 0, m_sc );   // flex
}

bool
CLParseBlock::format_specific_parse( const char * line, size_t line_size )
{
    YY_BUFFER_STATE bs = cl_scan_bytes( line, line_size, m_sc );
    int ret = cl_parse( m_sc, & m_receiver );
    cl__delete_buffer( bs, m_sc );
    return ret == 0;
}

/* ----------- CLReverseBlock ----------- */
CLReverseBlock::CLReverseBlock( ReceiverInterface::FormatterRef fmt )
: m_receiver ( fmt )
{ // no need to do anything here
}

CLReverseBlock::~CLReverseBlock()
{ // no need to do anything here
}

void
CLReverseBlock::SetDebug( bool onOff )
{ // no need to do anything here
}

static void
extract_and_set( const JSONObject &obj, FormatterInterface &formatter, const char * fieldname )
{
    const JSONValue &entry = obj . getValue ( fieldname );
    const String &S = entry . toString();
    formatter . addNameValue( fieldname, S . toSTLString() );
}

bool
CLReverseBlock::format_specific_parse( const char * line, size_t line_size )
{
    /* here we will take the line, and ask the vdb-3 lib to parse it into a JSONValueRef
       we will inspect it and call setters on the formatter to produce output */
    String src( line, line_size );
    ReceiverInterface &receiver = GetReceiver();
    ReverseFormatter &formatter = static_cast< ReverseFormatter & >( receiver . GetFormatter() );
    try
    {
        const JSONValueRef values = JSON::parse( src );
        const JSONObject &obj = values -> toObject();

        formatter . set_separator ( ' ' );
        extract_and_set( obj, formatter, "syslog_prefix" );
        extract_and_set( obj, formatter, "timestamp" );
        formatter . set_separator ( '|' );
        extract_and_set( obj, formatter, "ip" );
        extract_and_set( obj, formatter, "owner" );
        extract_and_set( obj, formatter, "method" );
        extract_and_set( obj, formatter, "bucket" );
        extract_and_set( obj, formatter, "unknown1" );
        extract_and_set( obj, formatter, "requestHdrSize" );
        extract_and_set( obj, formatter, "requestBodySize" );
        extract_and_set( obj, formatter, "responseHdrSize" );
        extract_and_set( obj, formatter, "responseBodySize" );
        extract_and_set( obj, formatter, "totalSize" );
        extract_and_set( obj, formatter, "unknown2" );
        extract_and_set( obj, formatter, "path" );
        extract_and_set( obj, formatter, "httpStatus" );
        extract_and_set( obj, formatter, "reqId" );
        extract_and_set( obj, formatter, "unknown3" );
        extract_and_set( obj, formatter, "eTag" );
        extract_and_set( obj, formatter, "errorCode" );
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
