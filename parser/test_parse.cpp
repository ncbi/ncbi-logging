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

struct SLogOnPremEvent
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

    SLogOnPremEvent& operator= ( const LogOnPremEvent &ev )
    {
        ip          = ToString( ev . ip );
        user        = ToString( ev . user );
        time        = ev . time;
        request     = ev . request;
        res_code    = ev . res_code;
        res_len     = ev . res_len;
        req_time    = ToString( ev . req_time );
        referer     = ToString( ev . referer );
        agent       = ToString( ev . agent );
        forwarded   = ToString( ev . forwarded );
        port        = ev . port;
        req_len     = ev . req_len;
        return *this; 
    }
};

struct SLogAWSEvent
{
    string      owner;
    string      bucket;
    t_timepoint time;
    string      ip;
    string      requester;
    string      request_id;
    string      operation;
    string      key;
    SRequest    request;
    int64_t     status;
    string      error;
    int64_t     bytes_sent;
    int64_t     obj_size;
    int64_t     total_time;
    string      referer;
    string      agent;
    string      version_id;
    string      host_id;
    string      cipher_suite;
    string      auth_type;
    string      host_header;
    string      tls_version;

    SLogAWSEvent& operator= ( const LogAWSEvent &ev )
    {
        owner       = ToString( ev . owner );
        bucket      = ToString( ev . bucket );
        time        = ev . time;
        ip          = ToString( ev . ip );
        requester   = ToString( ev . requester );
        request_id  = ToString( ev . request_id );
        operation   = ToString( ev . operation );
        key         = ToString( ev . key );
    }
};

struct TestLogLines : public LogLines
{
    virtual int unrecognized( const t_str & text )
    {
        m_unrecognized . push_back( ToString( text ) );
        return 0;
    }

    virtual int acceptLine( const LogOnPremEvent & event )
    {
        lastEvent = event;
        return 0;
    }

    virtual int rejectLine( const LogOnPremEvent & event )
    {
        lastRejected = event;
        return 0;
    }

    virtual int acceptLine( const LogAWSEvent & event )
    {
        return 0;
    }

    virtual int rejectLine( const LogAWSEvent & event )
    {
        return 0;
    }

    vector< string > m_unrecognized;
    SLogOnPremEvent lastEvent;
    SLogOnPremEvent lastRejected;
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
        const SLogOnPremEvent & e = m_lines.lastEvent;
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
        const SLogOnPremEvent & e = m_lines.lastEvent;
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
        const SLogOnPremEvent & e = m_lines.lastEvent;
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

TEST_F ( TestParseFixture, AWS )
{
    const char * InputLine =
"922194806485875312b252374a3644f1feecd16802a50d4729885c1d11e1fd37 sra-pub-src-14 [09/May/2020:22:07:21 +0000] 18.207.254.142 arn:aws:sts::783971887864:assumed-role/sra-developer-instance-profile-role/i-0d76b79326eb0165a B6301F55C2486C74 REST.PUT.PART SRR9612637/DRGHT.TC.307_interleaved.fq.1 \"PUT /SRR9612637/DRGHT.TC.307_interleaved.fq.1?partNumber=1&uploadId=rl6yL37lb4xUuIa9RvC0ON4KgDqJNvtwLoquo_cALj95v4njBOTUHpISyEjOaMG30lVYAo5eR_UEXo4dVJjUJA3SfjJtKjg30rvVEpg._Z9DZZo8S6oUjXHGDCW15EVzLZcJMgRG6N7J8d.42.lMAw-- HTTP/1.1\" 200 - - 8388608 557 12 \"-\" \"aws-cli/1.16.102 Python/2.7.16 Linux/4.14.171-105.231.amzn1.x86_64 botocore/1.12.92\" - fV92QmqOf5ZNYPIj7KZeQWiqAOFqdFtMlOn82aRYjwQHt8QfsWfS3TTOft1Be+bY01d9TObk5Qg= SigV4 ECDHE-RSA-AES128-GCM-SHA256 AuthHeader sra-pub-src-14.s3.amazonaws.com TLSv1.2\n"

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
