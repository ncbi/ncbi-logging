#include "OP_Interface.hpp"

#include "op_parser.hpp"
#include "op_scanner.hpp"
#include <ncbi/json.hpp>

#include "Formatters.hpp"
#include "AGENT_Interface.hpp"
#include "URL_Interface.hpp"

extern YY_BUFFER_STATE op_scan_bytes( const char * input, size_t size, yyscan_t yyscanner );

using namespace NCBI::Logging;
using namespace std;
using namespace ncbi;

OPReceiver::OPReceiver( ReceiverInterface::FormatterRef fmt )
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

ReceiverInterface::Category OPReceiver::post_process( void )
{
    ReceiverInterface::Category cat_res;
    {
        AGENTReceiver agt( m_fmt );
        AGENTParseBlock pb ( agt );
        // handle failure of parser (set cat to ugly) here and everywhere in post-process
        pb.format_specific_parse( agent_for_postprocess.c_str(), agent_for_postprocess.size() );
        agent_for_postprocess.clear();
        cat_res = agt.GetCategory();
    }

    if ( cat_res == ReceiverInterface::cat_good )
    {
        URLReceiver url( m_fmt );
        URLParseBlock pb ( url );
        pb.format_specific_parse( url_for_postprocess.c_str(), url_for_postprocess.size() );

        url_for_postprocess.clear();
        cat_res = url.GetCategory();
        url . finalize(); // this is special to url-parsing
    }
    return cat_res;
}

namespace NCBI
{
    namespace Logging
    {
        class OPParseBlock : public ParseBlockInterface
        {
        public:
            OPParseBlock( ReceiverInterface::FormatterRef fmt );
            virtual ~OPParseBlock();
            virtual ReceiverInterface & GetReceiver() { return m_receiver; }
            virtual bool format_specific_parse( const char * line, size_t line_size );
            virtual void SetDebug( bool onOff );

            yyscan_t m_sc;
            OPReceiver m_receiver;
        };

        class OPReverseBlock : public ParseBlockInterface
        {
        public:
            OPReverseBlock( ReceiverInterface::FormatterRef fmt );
            virtual ~OPReverseBlock();
            virtual ReceiverInterface & GetReceiver() { return m_receiver; }
            virtual bool format_specific_parse( const char * line, size_t line_size );
            virtual void SetDebug( bool onOff );

            OPReceiver m_receiver;
        };

    }
}

/* ----------- OPParseBlockFactory ----------- */
OPParseBlockFactory::~OPParseBlockFactory() {}

std::unique_ptr<ParseBlockInterface>
OPParseBlockFactory::MakeParseBlock() const
{
    ReceiverInterface::FormatterRef fmt;
    if ( m_fast )
        fmt = std::make_unique<JsonFastFormatter>();
    else
        fmt = std::make_unique<JsonLibFormatter>();
    return std::make_unique<OPParseBlock>( fmt );
}

/* ----------- OPReverseBlockFactory ----------- */
OPReverseBlockFactory::~OPReverseBlockFactory() {}

std::unique_ptr<ParseBlockInterface>
OPReverseBlockFactory::MakeParseBlock() const
{
    ReceiverInterface::FormatterRef fmt = std::make_unique<ReverseFormatter>();
    // return a revers-parseblock....
    return std::make_unique<OPReverseBlock>( fmt );
}

/* ----------- OPParseBlock----------- */
OPParseBlock::OPParseBlock( ReceiverInterface::FormatterRef fmt )
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
OPParseBlock::format_specific_parse( const char * line, size_t line_size )
{
    YY_BUFFER_STATE bs = op_scan_bytes( line, line_size, m_sc );
    int ret = op_parse( m_sc, & m_receiver );
    op__delete_buffer( bs, m_sc );
    return ret == 0;
}

/* ----------- OPReverseBlock ----------- */
OPReverseBlock::OPReverseBlock( ReceiverInterface::FormatterRef fmt )
: m_receiver ( fmt )
{ // no need to do anything here
}

OPReverseBlock::~OPReverseBlock()
{ // no need to do anything here
}

void
OPReverseBlock::SetDebug( bool onOff )
{ // no need to do anything here
}

// how to deal with the value if it contains spaces
// sp_auto  ... find out if values has spaces, quote the whole value if it has them, if not not quotes
// sp_force ... do not inspect value for spaces, quote the value always
// sp_off   ... do not inspect value for spaces, never quote the value
enum QuoteSpaces
{ sp_auto, sp_force, sp_off };

static void
extract_and_set( const JSONObject &obj, FormatterInterface &formatter, const char * fieldname,
    QuoteSpaces quote_spaces = sp_auto, bool empty_is_dash = false )
{
    const JSONValue &entry = obj . getValue ( fieldname );
    const String &S = entry . toString();
    if ( S . isEmpty() )
    {
        if ( empty_is_dash )
            formatter . addNameValue( fieldname, "-" );
    }
    else
    {
        switch ( quote_spaces )
        {
            case sp_auto:
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
                break;
            case sp_force:
                {
                    std::stringstream ss;
                    ss . put ( '"' );
                    ss . write( S . data(), S . size() );
                    ss . put ( '"' );
                    formatter . addNameValue( fieldname, ss . str() );
                }
                break;
            case sp_off:
                formatter . addNameValue( fieldname, S.toSTLString() );
                break;
        }
    }
}

static void
extract_and_set_request( const JSONObject &obj, FormatterInterface &formatter )
{
    std::stringstream ss;

    ss . put( '"' );

    const String & method = obj . getValue ( "method" ) . toString();
    ss . write( method . data(), method . size() );

    const String & path = obj . getValue ( "path" ) . toString();
    if ( ! path.isEmpty() )
    {
        ss . put( ' ' );
        ss . write( path . data(), path . size() );
    }

    const String & vers = obj . getValue ( "vers" ) . toString();
    if ( ! vers.isEmpty() )
    {
        ss . put( ' ' );
        ss . write( vers . data(), vers . size() );
    }

    ss . put( '"' );

    formatter . addNameValue( "request", ss.str() );
}


bool
OPReverseBlock::format_specific_parse( const char * line, size_t line_size )
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

        extract_and_set( obj, formatter, "ip" );
        formatter . addNameValue( "", "-" );
        extract_and_set( obj, formatter, "user", sp_auto, true );
        extract_and_set( obj, formatter, "time", sp_off );
        extract_and_set( obj, formatter, "server" );
        extract_and_set_request( obj, formatter );
        extract_and_set( obj, formatter, "res_code" );
        extract_and_set( obj, formatter, "res_len", sp_off, true );
        extract_and_set( obj, formatter, "req_time", sp_off );
        extract_and_set( obj, formatter, "referer", sp_force );
        extract_and_set( obj, formatter, "agent", sp_force );
        extract_and_set( obj, formatter, "forwarded", sp_force );

        {
            stringstream ss;
            ss << "port=" << obj . getValue ( "port" ) . toString() . toSTLString();
            formatter . addNameValue( "port", ss.str() );
        }

        {
            stringstream ss;
            ss << "rl=" << obj . getValue ( "req_len" ) . toString() . toSTLString();
            formatter . addNameValue( "req_len", ss.str() );
        }

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
