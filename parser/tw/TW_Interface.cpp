#include "TW_Interface.hpp"

#include "tw_parser.hpp"
#include "tw_scanner.hpp"
#include <ncbi/json.hpp>

#include "Formatters.hpp"
#include "JWT_Interface.hpp"

extern YY_BUFFER_STATE tw_scan_bytes( const char * input, size_t size, yyscan_t yyscanner );

using namespace NCBI::Logging;
using namespace std;
using namespace ncbi;

TWReceiver::TWReceiver( ReceiverInterface::FormatterRef fmt )
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
    case msg :  setMember( "msg", v );
                msg_for_postprocess = std::string( v.p, v.n );
                break;
    //CASE( msg )
    default: ReceiverInterface::set((ReceiverInterface::Members)m, v);
    }
#undef CASE
    if ( m_cat == cat_unknown )
        m_cat = cat_good;
}

ReceiverInterface::Category TWReceiver::post_process( void )
{
    JWTReceiver jwt( m_fmt );
    JWTParseBlock pb ( jwt );
    pb.format_specific_parse( msg_for_postprocess.c_str(), msg_for_postprocess.size() );
    msg_for_postprocess . clear();
    return jwt.GetCategory();
}

namespace NCBI
{
    namespace Logging
    {
        class TWParseBlock : public ParseBlockInterface
        {
        public:
            TWParseBlock( ReceiverInterface::FormatterRef fmt );
            virtual ~TWParseBlock();
            virtual ReceiverInterface & GetReceiver() { return m_receiver; }
            virtual bool format_specific_parse( const char * line, size_t line_size );
            virtual void SetDebug( bool onOff );

            yyscan_t m_sc;
            TWReceiver m_receiver;
        };

        class TWReverseBlock : public ParseBlockInterface
        {
        public:
            TWReverseBlock( ReceiverInterface::FormatterRef fmt );
            virtual ~TWReverseBlock();
            virtual ReceiverInterface & GetReceiver() { return m_receiver; }
            virtual bool format_specific_parse( const char * line, size_t line_size );
            virtual void SetDebug( bool onOff );

            TWReceiver m_receiver;
        };
    }
}

TWParseBlockFactory::~TWParseBlockFactory() {}

std::unique_ptr<ParseBlockInterface>
TWParseBlockFactory::MakeParseBlock() const
{
    ReceiverInterface::FormatterRef fmt;
    if ( m_fast )
        fmt = std::make_unique<JsonFastFormatter>();
    else
        fmt = std::make_unique<JsonLibFormatter>();
    return std::make_unique<TWParseBlock>( fmt );
}

TWParseBlock::TWParseBlock( ReceiverInterface::FormatterRef fmt )
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
TWParseBlock::format_specific_parse( const char * line, size_t line_size )
{
    YY_BUFFER_STATE bs = tw_scan_bytes( line, line_size, m_sc );
    int ret = tw_parse( m_sc, & m_receiver );
    tw__delete_buffer( bs, m_sc );
    return ret == 0;
}

/* ----------- TWReverseBlockFactory ----------- */
TWReverseBlockFactory::~TWReverseBlockFactory() {}

std::unique_ptr<ParseBlockInterface>
TWReverseBlockFactory::MakeParseBlock() const
{
    ReceiverInterface::FormatterRef fmt = std::make_unique<ReverseFormatter>();
    // return a revers-parseblock....
    return std::make_unique<TWReverseBlock>( fmt );
}

/* ----------- TWReverseBlock ----------- */
TWReverseBlock::TWReverseBlock( ReceiverInterface::FormatterRef fmt )
: m_receiver ( fmt )
{ // no need to do anything here
}

TWReverseBlock::~TWReverseBlock()
{ // no need to do anything here
}

void
TWReverseBlock::SetDebug( bool onOff )
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
TWReverseBlock::format_specific_parse( const char * line, size_t line_size )
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

        extract_and_set( obj, formatter, "id1" );
        extract_and_set( obj, formatter, "id2" );
        extract_and_set( obj, formatter, "id3" );
        extract_and_set( obj, formatter, "time" );
        extract_and_set( obj, formatter, "server" );
        extract_and_set( obj, formatter, "ip" );
        extract_and_set( obj, formatter, "sid" );
        extract_and_set( obj, formatter, "service" );
        extract_and_set( obj, formatter, "event" );
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
