#include <gtest/gtest.h>

#include <string>

#include "gcp_parser.hpp"
#include "gcp_scanner.hpp"

using namespace std;

TEST ( TestFlex, InitDestroy )
{
    yyscan_t sc;
    ASSERT_EQ( 0, gcp_lex_init( &sc ) );
    ASSERT_EQ( 0, gcp_lex_destroy( sc ) );
}

class GCP_TestFlexFixture : public ::testing::Test
{
public:
    virtual void SetUp()
    {
        gcp_lex_init( &sc );
    }

    virtual void TearDown()
    {
        gcp_lex_destroy( sc );
    }

    int StartScan( const char * input, size_t size )
    {
        gcp__scan_bytes( input, size, sc );
        return gcp_lex( & token, sc );
    }

    int StartScan( const char * input )
    {
        gcp__scan_bytes( input, strlen( input ), sc );
        return gcp_lex( & token, sc );
    }

    int NextTokenType()
    {
        return gcp_lex( & token, sc );
    }

    int StartPath(const char * input, bool debug = false )
    {
        gcp__scan_bytes( input, strlen( input ), sc );
        gcp_set_debug ( debug, sc );
        gcp_start_URL( sc );
        return gcp_lex( & token, sc );
    }

    int StartAgent(const char * input, bool debug = false )
    {
        gcp__scan_bytes( input, strlen( input ), sc );
        gcp_set_debug ( debug, sc );
        gcp_start_UserAgent( sc );
        return gcp_lex( & token, sc );
    }

    string TokenValue() const { return string( token  . p, token  . n ); }

    void TestIPV6 ( const char * addr )
    {
        string quoted = string("\"") + addr + "\"";
        ASSERT_EQ( QUOTE, StartScan(quoted.c_str()) );
        ASSERT_EQ( IPV6, NextTokenType() );
        ASSERT_EQ( addr, TokenValue() );
    }

    yyscan_t sc;
    YYSTYPE token;
};

TEST_F ( GCP_TestFlexFixture, EndOfFile )       { ASSERT_EQ( 0,     StartScan("") ); }
TEST_F ( GCP_TestFlexFixture, SkipSpace )       { ASSERT_EQ( 0,     StartScan("   ") ); }
TEST_F ( GCP_TestFlexFixture, SkipTab )         { ASSERT_EQ( 0,     StartScan(" \t ") ); }
TEST_F ( GCP_TestFlexFixture, Quote )           { ASSERT_EQ( QUOTE, StartScan("\"") ); }

TEST_F ( GCP_TestFlexFixture, Embedded_CtlChars )
{
    #define str "abc\x00\x01\x02...\x1f-def"
    ASSERT_EQ( QUOTE, StartScan("\"" str "\"", 16) );
    ASSERT_EQ( QSTR, NextTokenType() );
    ASSERT_EQ( 14, token . n );
    ASSERT_EQ( 0, memcmp( str, token . p, token . n ) );
    #undef str
}

TEST_F ( GCP_TestFlexFixture, Quotes )
{
    ASSERT_EQ( QUOTE, StartScan("\"\"") );
    ASSERT_EQ( QUOTE, NextTokenType() );ASSERT_EQ( 0, NextTokenType() );
}
TEST_F ( GCP_TestFlexFixture, QuotedString )
{
    #define str ".Bl0-_~!*'();:@&=+$,/%#[]"
    ASSERT_EQ( QUOTE, StartScan("\"" str "\"") );
    ASSERT_EQ( QSTR, NextTokenType() ); ASSERT_EQ( str, TokenValue() );
    #undef str
}
TEST_F ( GCP_TestFlexFixture, QuotedNonAscii )
{   // skip non-ascii characters
    #define str "и"
    ASSERT_EQ( QUOTE, StartScan("\"" str "\"") );
    ASSERT_EQ( QUOTE, NextTokenType() );
    #undef str
}
TEST_F ( GCP_TestFlexFixture, QuotedEscapedQuote )
{   // skip non-ascii characters
    ASSERT_EQ( QUOTE, StartScan("\"\\\"\"") );  /* "\"" */
    ASSERT_EQ( QSTR, NextTokenType() );
    ASSERT_EQ( "\\\"", TokenValue() ); // needs to be unescaped later
    ASSERT_EQ( QUOTE, NextTokenType() );
    #undef str
}
TEST_F ( GCP_TestFlexFixture, AgentDoubledQuote )
{
    ASSERT_EQ( QSTR, StartAgent("\"\"\"", true) );  /* """" */
    ASSERT_EQ( "\"\"", TokenValue() ); // needs to be unescaped later
    ASSERT_EQ( QUOTE, NextTokenType() );
    #undef str
}

TEST_F ( GCP_TestFlexFixture, IPV4 )
{
    const char * addr = "\"255.24.110.9\"";
    ASSERT_EQ( QUOTE, StartScan(addr) );
    ASSERT_EQ( IPV4, NextTokenType() );
    ASSERT_EQ( "255.24.110.9", TokenValue() );
}

TEST_F ( GCP_TestFlexFixture, IPV6_1 )      { TestIPV6 ( "1:22:333:4444:5:6:7:8" ); }
TEST_F ( GCP_TestFlexFixture, IPV6_2 )      { TestIPV6 ( "1:22:333:4444:5:6:7::" ); }
TEST_F ( GCP_TestFlexFixture, IPV6_3 )      { TestIPV6 ( "1::FFFF" ); }
TEST_F ( GCP_TestFlexFixture, IPV6_4 )      { TestIPV6 ( "1:2::1:2" ); }
TEST_F ( GCP_TestFlexFixture, IPV6_5 )      { TestIPV6 ( "1:2::1:2:3" ); }
TEST_F ( GCP_TestFlexFixture, IPV6_6 )      { TestIPV6 ( "1:2::1:2:3:4" ); }
TEST_F ( GCP_TestFlexFixture, IPV6_7 )      { TestIPV6 ( "1:2::1:2:3:4:5" ); }
TEST_F ( GCP_TestFlexFixture, IPV6_8 )      { TestIPV6 ( "1::1:2:3:4:5:6" ); }
TEST_F ( GCP_TestFlexFixture, IPV6_9 )      { TestIPV6 ( "::1:2:3:4:5:6:7" ); }
TEST_F ( GCP_TestFlexFixture, IPV6_10_1 )   { TestIPV6 ( "fe80:%Z" ); }
TEST_F ( GCP_TestFlexFixture, IPV6_10_2 )   { TestIPV6 ( "fe80::1:2:3:4%blah" ); }
TEST_F ( GCP_TestFlexFixture, IPV6_11_1 )   { TestIPV6 ( "::1.2.3.4" ); }
TEST_F ( GCP_TestFlexFixture, IPV6_11_2 )   { TestIPV6 ( "::ffff:1.2.3.4" ); }
TEST_F ( GCP_TestFlexFixture, IPV6_11_3 )   { TestIPV6 ( "::ffff:0000:1.2.3.4" ); }
TEST_F ( GCP_TestFlexFixture, IPV6_12_1 )   { TestIPV6 ( "cdef::1.2.3.4" ); }
TEST_F ( GCP_TestFlexFixture, IPV6_12_2 )   { TestIPV6 ( "0123:4567:89ab:cdef::1.2.3.4" ); }

TEST_F ( GCP_TestFlexFixture, Path_State )
{
    ASSERT_EQ( PATHSTR, StartPath( "a" ) );
    ASSERT_EQ( "a", TokenValue() );
}

TEST_F ( GCP_TestFlexFixture, Path_StateReturn )
{
    const char * input = "\"a\",";
    gcp__scan_bytes( input, strlen( input ), sc );
    //gcp_set_debug ( 1, sc );
    gcp_start_URL( sc ); // send scanner into PATH state
    ASSERT_EQ( QUOTE, NextTokenType() );
    ASSERT_EQ( PATHSTR, NextTokenType() ); ASSERT_EQ( "a", TokenValue() );
    ASSERT_EQ( QUOTE, NextTokenType() );
    gcp_pop_state( sc ); // back to INITIAL state
    ASSERT_EQ( COMMA, NextTokenType() );
}

TEST_F ( GCP_TestFlexFixture, Agent )
{
    const char * input = "\"linux64 sra-toolkit fasterq-dump.2.10.7 (phid=noc86d2998,libc=2.17)\"";

    gcp__scan_bytes( input, strlen( input ), sc );
    gcp_start_UserAgent( sc );
    ASSERT_EQ( QUOTE, NextTokenType() );
    ASSERT_EQ( QSTR, NextTokenType() ); ASSERT_EQ( "linux64 sra-toolkit fasterq-dump.2.10.7 (phid=noc86d2998,libc=2.17)", TokenValue() );
    ASSERT_EQ( QUOTE, NextTokenType() );
}

TEST_F ( GCP_TestFlexFixture, Agent_2_part_version )
{
    const char * input = "fasterq-dump.2.10";

    gcp__scan_bytes( input, strlen( input ), sc );
    gcp_start_UserAgent( sc );
    ASSERT_EQ( QSTR, NextTokenType() ); ASSERT_EQ( "fasterq-dump.2.10", TokenValue() );
}

TEST_F ( GCP_TestFlexFixture, linefeed )
{
    const char * input = "\"blah\"\x0a";
    gcp__scan_bytes( input, strlen( input ), sc );
    //gcp_set_debug ( 1, sc );
    ASSERT_EQ( QUOTE, NextTokenType() );
    ASSERT_EQ( QSTR, NextTokenType() );
    ASSERT_EQ( QUOTE, NextTokenType() );
    ASSERT_EQ( 0, NextTokenType() );
}

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}
