#include "ERR_Interface.hpp"

#include "err_parser.hpp"
#include "err_scanner.hpp"
#include <ncbi/json.hpp>

#include "Formatters.hpp"
#include "MSG_Interface.hpp"
#include "URL_Interface.hpp"

extern YY_BUFFER_STATE err_scan_bytes( const char * input, size_t size, yyscan_t yyscanner );

using namespace NCBI::Logging;
using namespace std;
using namespace ncbi;

ERRReceiver::ERRReceiver( ReceiverInterface::FormatterRef fmt )
: ReceiverInterface ( fmt )
{
}

void ERRReceiver::set( ERR_Members m, const t_str & v )
{
#define CASE(mem) case mem: setMember( #mem, v ); break;
    switch( m )
    {
    CASE( datetime )
    CASE( severity )
    CASE( pid )
    CASE( cat )
    case msg :  setMember( "msg", v );
                msg_for_postprocess = std::string( v.p, v.n );
                break;
    default: ReceiverInterface::set((ReceiverInterface::Members)m, v);
    }
#undef CASE
    if ( m_cat == cat_unknown )
        m_cat = cat_good;
}

bool
ERRReceiver::SetErrorCategory( const char * toFind, const char * catName )
{
    if ( msg_for_postprocess.find( toFind ) != string::npos )
    {
        t_str cat_value = { catName, strlen( catName ) };
        set( cat, cat_value );
        return true;
    }
    return false;
}


ReceiverInterface::Category ERRReceiver::post_process( void )
{
    MSGReceiver msg( m_fmt );
    MSGParseBlock pb ( msg );

    // extract client/server/host/request/etc from the free-form message
    pb.format_specific_parse( msg_for_postprocess.c_str(), msg_for_postprocess.size() );

    // categorize on error message
    if ( msg.GetCategory() == ReceiverInterface::cat_good )
    {
        if      ( SetErrorCategory( "open()",  "openFailed" ) ) {}
        else if ( SetErrorCategory( "unlink()", "unlinkFailed" ) ) {}
        else if ( SetErrorCategory( "forbidden", "forbidden" ) ) {}
        else if ( SetErrorCategory( "ignore long locked inactive cache entry", "inactive" ) ) {}
        else if ( SetErrorCategory( "upstream prematurely closed connection while reading upstream", "upstream" ) ) {}
        else
        {
            t_str cat_value = { "unknown", 7 };
            set( cat, cat_value );
        }
        ;
    }
    msg_for_postprocess . clear();

    // extract accession/filename/extension
    if ( msg.GetCategory() == ReceiverInterface::cat_good )
    {
        URLReceiver url( m_fmt );
        URLParseBlock pb ( url );
        pb.format_specific_parse( msg.path_for_url_parser.c_str(), msg.path_for_url_parser.size() );
        url . finalize(); // this is special to url-parsing
    }

    return GetCategory(); // ignore the result category of post-processing
}

namespace NCBI
{
    namespace Logging
    {
        class ERRParseBlock : public ParseBlockInterface
        {
        public:
            ERRParseBlock( ReceiverInterface::FormatterRef fmt );
            virtual ~ERRParseBlock();
            virtual ReceiverInterface & GetReceiver() { return m_receiver; }
            virtual bool format_specific_parse( const char * line, size_t line_size );
            virtual void SetDebug( bool onOff );

            yyscan_t m_sc;
            ERRReceiver m_receiver;
        };

        class ERRReverseBlock : public ParseBlockInterface
        {
        public:
            ERRReverseBlock( ReceiverInterface::FormatterRef fmt );
            virtual ~ERRReverseBlock();
            virtual ReceiverInterface & GetReceiver() { return m_receiver; }
            virtual bool format_specific_parse( const char * line, size_t line_size );
            virtual void SetDebug( bool onOff );

            ERRReceiver m_receiver;
        };
    }
}

ERRParseBlockFactory::~ERRParseBlockFactory() {}

std::unique_ptr<ParseBlockInterface>
ERRParseBlockFactory::MakeParseBlock() const
{
    ReceiverInterface::FormatterRef fmt;
    if ( m_fast )
        fmt = std::make_unique<JsonFastFormatter>();
    else
        fmt = std::make_unique<JsonLibFormatter>();
    return std::make_unique<ERRParseBlock>( fmt );
}

ERRParseBlock::ERRParseBlock( ReceiverInterface::FormatterRef fmt )
: m_receiver ( fmt )
{
    err_lex_init( &m_sc );
}

ERRParseBlock::~ERRParseBlock()
{
    err_lex_destroy( m_sc );
}

void
ERRParseBlock::SetDebug( bool onOff )
{
    err_debug = onOff ? 1 : 0;            // bison (op_debug is global)
    err_set_debug( onOff ? 1 : 0, m_sc );   // flex
}

bool
ERRParseBlock::format_specific_parse( const char * line, size_t line_size )
{
    YY_BUFFER_STATE bs = err_scan_bytes( line, line_size, m_sc );
    int ret = err_parse( m_sc, & m_receiver );
    err__delete_buffer( bs, m_sc );
    return ret == 0;
}

/* ----------- TWReverseBlockFactory ----------- */
ERRReverseBlockFactory::~ERRReverseBlockFactory() {}

std::unique_ptr<ParseBlockInterface>
ERRReverseBlockFactory::MakeParseBlock() const
{
    ReceiverInterface::FormatterRef fmt = std::make_unique<ReverseFormatter>();
    // return a revers-parseblock....
    return std::make_unique<ERRReverseBlock>( fmt );
}

/* ----------- TWReverseBlock ----------- */
ERRReverseBlock::ERRReverseBlock( ReceiverInterface::FormatterRef fmt )
: m_receiver ( fmt )
{ // no need to do anything here
}

ERRReverseBlock::~ERRReverseBlock()
{ // no need to do anything here
}

void
ERRReverseBlock::SetDebug( bool onOff )
{ // no need to do anything here
}

static void
extract_and_set( const JSONObject &obj, FormatterInterface &formatter, const char * fieldname )
{
    const JSONValue &entry = obj . getValue ( fieldname );
    const String &S = entry . toString();
    t_str s = { S . data(), S . size() };
    formatter . addNameValue( fieldname, s );
}

bool
ERRReverseBlock::format_specific_parse( const char * line, size_t line_size )
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

        extract_and_set( obj, formatter, "datetime" );
        extract_and_set( obj, formatter, "severity" );
        extract_and_set( obj, formatter, "pid" );
        extract_and_set( obj, formatter, "msg" );

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
