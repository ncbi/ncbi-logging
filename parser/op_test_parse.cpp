#include <gtest/gtest.h>

#include <sstream>
#include <vector>

#include "log_lines.hpp"
#include "helper.hpp"

using namespace std;
using namespace NCBI::Logging;

struct SRequest
{
    string server;
    string method;
    string path;
    string params;
    string vers;

    SRequest& operator= ( const t_request &req )
    {
       server = ToString( req.server );
       method = ToString( req.method );
       path = ToString( req.path );
       params = ToString( req.params );
       vers = ToString( req.vers );
       return *this;
    }
};

struct SLogOPEvent
{
    string      ip;
    t_timepoint time;
    SRequest    request;
    int64_t     res_code;
    int64_t     res_len;
    string      referer;
    string      agent;

    string      user;
    string      req_time;
    int64_t     port;
    int64_t     req_len;
    string      forwarded;

    string      unparsed;

    SLogOPEvent( const LogOPEvent &ev )
    {
        ip          = ToString( ev . ip );
        time        = ev . time;
        request     = ev . request;
        res_code    = ev . res_code;
        res_len     = ev . res_len;
        referer     = ToString( ev . referer );
        agent       = ToString( ev . agent );

        user        = ToString( ev . user );
        req_time    = ToString( ev . req_time );
        forwarded   = ToString( ev . forwarded );
        port        = ev . port;
        req_len     = ev . req_len;
    }
};

/*
struct SLogAWSEvent : public SCommonLogEvent
{
    string      owner;
    string      bucket;
    string      requester;
    string      request_id;
    string      operation;
    string      key;
    string      error;
    int64_t     obj_size;
    int64_t     total_time;
    string      version_id;
    string      host_id;
    string      cipher_suite;
    string      auth_type;
    string      host_header;
    string      tls_version;

    SLogAWSEvent( const LogAWSEvent &ev ) : SCommonLogEvent( ev )
    {
        SCommonLogEvent::operator=( ev );

        owner       = ToString( ev . owner );
        bucket      = ToString( ev . bucket );
        requester   = ToString( ev . requester );
        request_id  = ToString( ev . request_id );
        operation   = ToString( ev . operation );
        key         = ToString( ev . key );
        error       = ToString( ev . error );
        obj_size    = ev . obj_size;
        total_time  = ev . total_time;
        version_id  = ToString( ev . version_id );
        host_id     = ToString( ev . host_id );
        cipher_suite = ToString( ev . cipher_suite );
        auth_type   = ToString( ev . auth_type );
        host_header = ToString( ev . host_header );
        tls_version = ToString( ev . tls_version );
    }
};
*/
struct TestLogLines : public OP_LogLines
{
    virtual int unrecognized( const t_str & text )
    {
        m_unrecognized . push_back( ToString( text ) );
        return 0;
    }

    virtual int acceptLine( const LogOPEvent & event )
    {
        m_accepted . push_back ( SLogOPEvent( event ) );
        return 0;
    }

    virtual int rejectLine( const LogOPEvent & event )
    {
        m_rejected . push_back ( SLogOPEvent( event ) );
        return 0;
    }

    vector< string >        m_unrecognized;
    vector < SLogOPEvent>   m_accepted;
    vector < SLogOPEvent>   m_rejected;

    virtual ~TestLogLines()
    {
    }
};

TEST ( TestParse, InitDestroy )
{
    TestLogLines lines;
    OP_Parser p( lines );
}

class TestParseFixture : public ::testing::Test
{
public:
    virtual void SetUp() {}
    virtual void TearDown() {}

    const SLogOPEvent parse_str( const char * input )
    {
        std::istringstream inputstream( input );
        {
            OP_Parser p( m_lines, inputstream );
            if ( !p.parse() ) throw logic_error( "parsing failed" );
            if ( m_lines.m_accepted.empty() ) throw logic_error( "last_m_accepted is null" );
            return m_lines . m_accepted.back();
        }
    }

    TestLogLines m_lines;
};

TEST_F ( TestParseFixture, Empty )
{
    std::istringstream input ( "" );
    {
        OP_Parser p( m_lines, input );
        ASSERT_TRUE( p.parse() );
    }
}

TEST_F ( TestParseFixture, OnPremise_NoUser )
{
    const char * InputLine =
"158.111.236.250 - - [01/Jan/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET /traces/sra34/SRR/003923/SRR4017927 HTTP/1.1\" 206 32768 0.000 \"-\" \"linux64 sra-toolkit fastq-dump.2.9.1\" \"-\" port=443 rl=293\n";
    SLogOPEvent e = parse_str( InputLine );

    ASSERT_EQ( "158.111.236.250", e.ip );

    ASSERT_EQ( "", e.user );

    ASSERT_EQ( 1, e.time.day );
    ASSERT_EQ( 1, e.time.month );
    ASSERT_EQ( 2020, e.time.year );
    ASSERT_EQ( 2, e.time.hour );
    ASSERT_EQ( 50, e.time.minute );
    ASSERT_EQ( 24, e.time.second );
    ASSERT_EQ( -500, e.time.offset );

    ASSERT_EQ( "sra-download.ncbi.nlm.nih.gov", e.request.server );

    ASSERT_EQ( 206, e.res_code );
    ASSERT_EQ( 32768, e.res_len );

    ASSERT_EQ( "0.000", e.req_time );
    ASSERT_EQ( "-", e.referer );
    ASSERT_EQ( "linux64 sra-toolkit fastq-dump.2.9.1", e.agent );
    ASSERT_EQ( "-", e.forwarded );

    ASSERT_EQ( 443, e.port );
    ASSERT_EQ( 293, e.req_len );
}

TEST_F ( TestParseFixture, OnPremise_User )
{
    const char * InputLine =
"158.111.236.250 - userid [01/Jan/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET /traces/sra34/SRR/003923/SRR4017927 HTTP/1.1\" 206 32768 0.000 \"-\" \"linux64 sra-toolkit fastq-dump.2.9.1\" \"-\" port=443 rl=293\n";
    SLogOPEvent e = parse_str( InputLine );

    ASSERT_EQ( "158.111.236.250", e.ip );
    ASSERT_EQ( "userid", e.user );
    ASSERT_EQ( 1, e.time.day );
}

TEST_F ( TestParseFixture, OnPremise_Request_Params )
{
    const char * InputLine =
"158.111.236.250 - userid [01/Jan/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET /traces/sra34/SRR/003923/SRR4017927?param1=value HTTP/1.1\" 206 32768 0.000 \"-\" \"linux64 sra-toolkit fastq-dump.2.9.1\" \"-\" port=443 rl=293\n";
    SLogOPEvent e = parse_str( InputLine );

    ASSERT_EQ( "param1=value", e.request.params );
}

TEST_F ( TestParseFixture, OnPremise_OnlyIP )
{
    const char * InputLine = "158.111.236.250\n";
    std::istringstream inputstream( InputLine );
    {
        OP_Parser p( m_lines, inputstream );
        p.setDebug( false );
        ASSERT_TRUE( p.parse() );
        ASSERT_EQ( 1, m_lines.m_rejected.size() );
        ASSERT_EQ( "158.111.236.250", m_lines.m_rejected.back().ip );
    }
}

TEST_F ( TestParseFixture, OnPremise_unrecognized )
{
    const char * InputLine = "total nonesense\n";
    std::istringstream inputstream( InputLine );
    {
        OP_Parser p( m_lines, inputstream );
        ASSERT_TRUE( p.parse() );
        ASSERT_EQ( 1, m_lines . m_unrecognized . size() );
        ASSERT_EQ( "total nonesense", m_lines . m_unrecognized[ 0 ] );
    }
}

TEST_F ( TestParseFixture, OnPremise_multiple_nonesense )
{
    const char * InputLine = "total nonesense\nmore nonesense\neven more\n";
    std::istringstream inputstream( InputLine );
    {
        OP_Parser p( m_lines, inputstream );
        ASSERT_TRUE( p.parse() );
        ASSERT_EQ( 3, m_lines . m_unrecognized . size() );
        ASSERT_EQ( "total nonesense", m_lines . m_unrecognized[ 0 ] );
        ASSERT_EQ( "more nonesense", m_lines . m_unrecognized[ 1 ] );
        ASSERT_EQ( "even more", m_lines . m_unrecognized[ 2 ] );
    }
}

TEST_F ( TestParseFixture, OnPremise_MultiLine )
{ 
    const char * InputLine =
"158.111.236.250 - - [01/Feb/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET /traces/sra34/SRR/003923/SRR4017927 HTTP/1.1\" 206 32768 0.000 \"-\" \"linux64 sra-toolkit fastq-\\\"dump.2.9.1\\\"\" \"-\" port=443 rl=293\n"
"158.111.236.250 - - [01/Jan/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET /traces/sra34/SRR/003923/SRR4017927 HTTP/1.1\" 206 32768 0.000 \"-\" \"linux64 sra-toolkit fastq-\\\"dump.2.9.1\\\"\" \"-\" port=443 rl=293\n";

    std::istringstream inputstream( InputLine );
    {
        OP_Parser p( m_lines, inputstream );
        ASSERT_TRUE ( p.parse() );
        ASSERT_EQ( 2, m_lines.m_accepted.size() );
    }
}

TEST_F ( TestParseFixture, OnPremise_ErrorLine )
{   // consume the error line completely, be prepared to parse the next line with the cleared status of the scanner
    const char * InputLine =
"total nonesense\n"
"158.111.236.250 - - [01/Jan/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET /traces/sra34/SRR/003923/SRR4017927 HTTP/1.1\" 206 32768 0.000 \"-\" \"linux64 sra-toolkit fastq-\\\"dump.2.9.1\\\"\" \"-\" port=443 rl=293\n";

    std::istringstream inputstream( InputLine );
    {
        OP_Parser p( m_lines, inputstream );
        ASSERT_TRUE ( p.parse() );
        ASSERT_EQ ( 0, m_lines.m_rejected.size() );
        ASSERT_EQ ( 1, m_lines.m_accepted.size() ); // line 2
        ASSERT_EQ ( 1, m_lines.m_unrecognized.size() ); // line 1
    }
}

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}
