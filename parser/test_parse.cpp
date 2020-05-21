#include <gtest/gtest.h>

#include <sstream>

#include "log_lines.hpp"

using namespace std;
using namespace NCBI::Logging;

struct TestLogLines : public LogLines
{
    virtual int unrecognized( const std::string & text ) { return 0; }
    virtual int acceptLine( const LogEvent & event )
    {
        lastEvent = event;
        return 0;
    }
    virtual int rejectLine( const LogEvent & event ) { return 0; }

    LogEvent lastEvent;
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

    static string ToString( const t_str & in )
    {
        return string( in.p == nullptr ? "" : in.p, in.n );
    }

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
        const LogEvent & e = m_lines.lastEvent;
        ASSERT_EQ( "158.111.236.250", e.ip );

        ASSERT_EQ( "", e.user );

        ASSERT_EQ( 1, e.time.day );
        ASSERT_EQ( 1, e.time.month );
        ASSERT_EQ( 2020, e.time.year );
        ASSERT_EQ( 2, e.time.hour );
        ASSERT_EQ( 50, e.time.minute );
        ASSERT_EQ( 24, e.time.second );
        ASSERT_EQ( -500, e.time.offset );

        ASSERT_EQ( "sra-download.ncbi.nlm.nih.gov", ToString( e.request.server ) );
        //TODO: verify the remaining members
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
        const LogEvent & e = m_lines.lastEvent;
        ASSERT_EQ( "158.111.236.250", e.ip );
        ASSERT_EQ( "userid", e.user );
        ASSERT_EQ( 1, e.time.day );
    }
}

#if UNFINISHED
TEST_F ( TestParseFixture, OnPremise_Request_Params )
{
    const char * InputLine =
"158.111.236.250 - userid [01/Jan/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET /traces/sra34/SRR/003923/SRR4017927?param1=value HTTP/1.1\" 206 32768 0.000 \"-\" \"linux64 sra-toolkit fastq-dump.2.9.1\" \"-\" port=443 rl=293\n";
    std::istringstream input ( InputLine );
    {
        LogParser p( m_lines, input );
        //p.setDebug(true);
        ASSERT_TRUE( p.parse() );
        const LogEvent & e = m_lines.lastEvent;

    }
}
#endif

//TODO: multiple lines
//TODO: AWS
//TODO: GCP
//TODO: bad lines
//TODO: various line endings


extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}