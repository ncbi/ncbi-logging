#include <gtest/gtest.h>

#include <sstream>
#include <vector>

#include "log_lines.hpp"

using namespace std;
using namespace NCBI::Logging;

static string ToString( const t_str & in )
{
    return string( in.p == nullptr ? "" : in.p, in.n );
}

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

struct SLogEvent
{
    string      ip;
    string      user;
    t_timepoint time;
    SRequest    request;
    int64_t     res_code;
    int64_t     res_len;
    string      req_time;
    string      referer;
    string      agent;
    string      forwarded;
    int64_t     port;
    int64_t     req_len;

    SLogEvent& operator= ( const LogEvent &ev )
    {
        ip = ToString( ev . ip );
        user = ToString( ev . user );
        time = ev . time;
        request = ev . request;
        res_code = ev . res_code;
        res_len = ev . res_len;
        req_time = ToString( ev . req_time );
        referer = ToString( ev . referer );
        agent = ToString( ev . agent );
        forwarded = ToString( ev . forwarded );
        port = ev . port;
        req_len = ev . req_len;
        return *this; 
    }
};

struct TestLogLines : public LogLines
{
    virtual int unrecognized( const t_str & text )
    {
        m_unrecognized . push_back( ToString( text ) );
        return 0;
    }
    virtual int acceptLine( const LogEvent & event )
    {
        lastEvent = event;
        return 0;
    }
    virtual int rejectLine( const LogEvent & event )
    {
        lastRejected = event;
        return 0;
    }

    vector< string > m_unrecognized;
    SLogEvent lastEvent;
    SLogEvent lastRejected;
};

TEST ( TestParse, InitDestroy )
{
    TestLogLines lines;
    LogParser p( lines );
}

class TestParseFixture : public ::testing::Test
{
public:
    virtual void SetUp() {}
    virtual void TearDown() {}

    TestLogLines m_lines;
};

TEST_F ( TestParseFixture, Empty )
{
    std::istringstream input ( "" );
    {
        LogParser p( m_lines, input );
        ASSERT_TRUE( p.parse() );
    }
}

TEST_F ( TestParseFixture, OnPremise_NoUser )
{
    const char * InputLine =
"158.111.236.250 - - [01/Jan/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET /traces/sra34/SRR/003923/SRR4017927 HTTP/1.1\" 206 32768 0.000 \"-\" \"linux64 sra-toolkit fastq-dump.2.9.1\" \"-\" port=443 rl=293\n";
    std::istringstream input ( InputLine );
    {
        LogParser p( m_lines, input );
        ASSERT_TRUE( p.parse() );
        const SLogEvent & e = m_lines.lastEvent;
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
}

TEST_F ( TestParseFixture, OnPremise_User )
{
    const char * InputLine =
"158.111.236.250 - userid [01/Jan/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET /traces/sra34/SRR/003923/SRR4017927 HTTP/1.1\" 206 32768 0.000 \"-\" \"linux64 sra-toolkit fastq-dump.2.9.1\" \"-\" port=443 rl=293\n";
    std::istringstream input ( InputLine );
    {
        LogParser p( m_lines, input );
        //p.setDebug(true);
        ASSERT_TRUE( p.parse() );
        const SLogEvent & e = m_lines.lastEvent;
        ASSERT_EQ( "158.111.236.250", e.ip );
        ASSERT_EQ( "userid", e.user );
        ASSERT_EQ( 1, e.time.day );
    }
}

TEST_F ( TestParseFixture, OnPremise_Request_Params )
{
    const char * InputLine =
"158.111.236.250 - userid [01/Jan/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET /traces/sra34/SRR/003923/SRR4017927?param1=value HTTP/1.1\" 206 32768 0.000 \"-\" \"linux64 sra-toolkit fastq-dump.2.9.1\" \"-\" port=443 rl=293\n";
    std::istringstream input ( InputLine );
    {
        LogParser p( m_lines, input );
        //p.setDebug(true);
        ASSERT_TRUE( p.parse() );
        const SLogEvent & e = m_lines.lastEvent;
        ASSERT_EQ( "param1=value", e.request.params );
    }
}

TEST_F ( TestParseFixture, OnPremise_OnlyIP )
{
    const char * InputLine = "158.111.236.250\n";
    std::istringstream input ( InputLine );
    {
        LogParser p( m_lines, input );
        //p.setDebug(true);
        ASSERT_TRUE( p.parse() );
        ASSERT_EQ( "158.111.236.250", m_lines . lastRejected . ip );
    }
}

TEST_F ( TestParseFixture, OnPremise_unrecognized )
{
    const char * InputLine = "total nonesense\n";
    std::istringstream input ( InputLine );
    {
        LogParser p( m_lines, input );
        //p.setDebug(true);
        ASSERT_TRUE( p.parse() );
        ASSERT_EQ( 1, m_lines . m_unrecognized . size() );
        ASSERT_EQ( "total nonesense", m_lines . m_unrecognized[ 0 ] );
    }
}

TEST_F ( TestParseFixture, OnPremise_multiple_nonesense )
{
    const char * InputLine = "total nonesense\nmore nonesense\neven more\n";
    std::istringstream input ( InputLine );
    {
        LogParser p( m_lines, input );
        //p.setDebug(true);
        ASSERT_TRUE( p.parse() );
        ASSERT_EQ( 3, m_lines . m_unrecognized . size() );
        ASSERT_EQ( "total nonesense", m_lines . m_unrecognized[ 0 ] );
        ASSERT_EQ( "more nonesense", m_lines . m_unrecognized[ 1 ] );
        ASSERT_EQ( "even more", m_lines . m_unrecognized[ 2 ] );
    }
}

//TODO: what about rejected lines with partial information recognized?
//TODO: multiple lines
//TODO: AWS
//TODO: GCP
//TODO: various line endings


extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}