#include "GCP_Interface.hpp"

#include "gcp_v2_parser.hpp"
#include "gcp_v2_scanner.hpp"

extern YY_BUFFER_STATE gcp_scan_reset( const char * input, yyscan_t yyscanner );

using namespace NCBI::Logging;
using namespace std;

GCPReceiver::GCPReceiver( unique_ptr<FormatterInterface> & fmt )
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

namespace NCBI
{
    namespace Logging
    {
        class GCPParseBlock : public ParseBlockInterface
        {
        public:
            GCPParseBlock( std::unique_ptr<FormatterInterface> & fmt );
            virtual ~GCPParseBlock();
            virtual ReceiverInterface & GetReceiver() { return m_receiver; }
            virtual bool Parse( const std::string & line );
            virtual void SetDebug( bool onOff );

            yyscan_t m_sc;
            GCPReceiver m_receiver;
        };
    }
}

GCPParseBlockFactory::~GCPParseBlockFactory() {}

std::unique_ptr<ParseBlockInterface>
GCPParseBlockFactory::MakeParseBlock() const
{
    std::unique_ptr<FormatterInterface> fmt;
    if ( m_fast )
        fmt = std::make_unique<JsonFastFormatter>();
    else
        fmt = std::make_unique<JsonLibFormatter>();
    return std::make_unique<GCPParseBlock>( fmt );
}

GCPParseBlock::GCPParseBlock( std::unique_ptr<FormatterInterface> & fmt )
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
GCPParseBlock::Parse( const string & line )
{
    YY_BUFFER_STATE bs = gcp_scan_reset( line.c_str(), m_sc );
    int ret = gcp_parse( m_sc, & m_receiver );
    //TODO: if header, validate; set cat to ignore (good) or review (bad)
    if ( m_receiver.GetCategory() == ReceiverInterface::cat_ignored )
    {
        if ( 0 != strncmp( DefaultHeader.c_str(), line.c_str(), DefaultHeader.size() ) )
        {
            m_receiver.SetCategory( ReceiverInterface::cat_review );
        }
    }
    gcp__delete_buffer( bs, m_sc );
    return ret == 0;
}

