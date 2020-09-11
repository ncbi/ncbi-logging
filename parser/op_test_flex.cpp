#include <gtest/gtest.h>

#include <string>

#include "op_parser.hpp"
#include "op_scanner.hpp"

using namespace std;

TEST ( TestFlex, InitDestroy )
{
    yyscan_t sc;
    ASSERT_EQ( 0, op_lex_init( &sc ) );
    ASSERT_EQ( 0, op_lex_destroy( sc ) );
}

class OP_TestFlexFixture : public ::testing::Test
{
public:
    virtual void SetUp()
    {
        op_lex_init( &sc );
    }

    virtual void TearDown()
    {
        op_lex_destroy( sc );
    }

    int StartScan( const char * input, size_t size )
    {
        op__scan_bytes( input, size, sc );
        return op_lex( & token, sc );
    }

    int StartScan( const char * input )
    {
        op__scan_bytes( input, strlen( input ), sc );
        return op_lex( & token, sc );
    }

    int StartScanInURL_State(const char * input, bool debug = false )
    {
        op__scan_bytes( input, strlen( input ), sc );
        op_set_debug ( debug, sc );
        op_start_URL( sc );
        return op_lex( & token, sc );
    }

    int NextTokenType()
    {
        return op_lex( & token, sc );
    }

    string TokenValue() const { return string( token . s . p, token . s . n ); }

    void TestIPV6 ( const char * addr )
    {
        ASSERT_EQ( IPV6, StartScan(addr) );
        ASSERT_EQ( addr, TokenValue() );
    }

    void TestQuotedToken( int token_type, const char * token )
    {
        string quoted = "\"";
        quoted += token;
        quoted += "\"";
        ASSERT_EQ( QUOTE, StartScan( quoted.c_str() ) );
        ASSERT_EQ( token_type, NextTokenType() );
        ASSERT_EQ( string( token ), TokenValue() );
    }
    yyscan_t sc;
    YYSTYPE token;
};

TEST_F ( OP_TestFlexFixture, EndOfFile )       { ASSERT_EQ( 0,     StartScan("") ); }
TEST_F ( OP_TestFlexFixture, Space )           { ASSERT_EQ( SPACE, StartScan("   ") ); }
TEST_F ( OP_TestFlexFixture, Tab )              { ASSERT_EQ( SPACE, StartScan("\t ") ); }
TEST_F ( OP_TestFlexFixture, Port )            { ASSERT_EQ( PORT,  StartScan("port=") ); }
TEST_F ( OP_TestFlexFixture, RL )              { ASSERT_EQ( RL,    StartScan("rl=") ); }
TEST_F ( OP_TestFlexFixture, Quote )           { ASSERT_EQ( QUOTE, StartScan("\"") ); }

TEST_F ( OP_TestFlexFixture, Quotes )
{
    ASSERT_EQ( QUOTE, StartScan("\"\"") );
    ASSERT_EQ( QUOTE, NextTokenType() );ASSERT_EQ( 0, NextTokenType() );
}
TEST_F ( OP_TestFlexFixture, QuotedSpace )
{
    ASSERT_EQ( QUOTE, StartScan("\" \"") );
    ASSERT_EQ( SPACE, NextTokenType() );
}
TEST_F ( OP_TestFlexFixture, QuotedMethod )         { TestQuotedToken( METHOD, "OPTIONS" ); }
TEST_F ( OP_TestFlexFixture, PropfindMethod )       { TestQuotedToken( METHOD, "PROPFIND" ); }
TEST_F ( OP_TestFlexFixture, QuotedVers )           { TestQuotedToken( VERS, "HTTP/2" ); }
TEST_F ( OP_TestFlexFixture, QuotedVers2_0 )        { TestQuotedToken( VERS, "HTTP/2.0" ); }
TEST_F ( OP_TestFlexFixture, QuotedVersNoNumber )   { TestQuotedToken( VERS, "HTTP/" ); }
TEST_F ( OP_TestFlexFixture, QuotedVersMixedcase )  { TestQuotedToken( VERS, "HttP/1.0" ); }
TEST_F ( OP_TestFlexFixture, QuotedVersAnyNumber )  { TestQuotedToken( VERS, "HTTP/3.14" ); }

TEST_F ( OP_TestFlexFixture, Embedded_CtlChars )
{
    #define str "abc\x00\x01\x02...\x1f-def"
    ASSERT_EQ( QUOTE, StartScan("\"" str "\"", 16) );
    ASSERT_EQ( QSTR, NextTokenType() );
    ASSERT_EQ( 14, token . s . n );
    ASSERT_EQ( 0, memcmp( str, token . s . p, token . s . n ) );
    ASSERT_FALSE ( token . s . escaped ); // no '\' inside
    #undef str
}
TEST_F ( OP_TestFlexFixture, QuotedString )
{
    #define str ".Bl0-_~!*'();:@&=+$,/%#[]?\\<>|`{}^"
    ASSERT_EQ( QUOTE, StartScan("\"" str "\"") );
    ASSERT_EQ( QSTR, NextTokenType() ); ASSERT_EQ( str, TokenValue() );
    ASSERT_FALSE ( token . s . escaped ); // no '\' inside
    #undef str
}
TEST_F ( OP_TestFlexFixture, QuotedNonAscii )
{   // skip non-ascii characters
    #define str "и"
    ASSERT_EQ( QUOTE, StartScan("\"" str "\"") );
    ASSERT_EQ( QUOTE, NextTokenType() );
    #undef str
}
TEST_F ( OP_TestFlexFixture, QuotedEscapedQuote )
{
    ASSERT_EQ( QUOTE, StartScan("\"\\\"\"") );  /* "\"" */
    ASSERT_EQ( QSTR, NextTokenType() );
    ASSERT_TRUE ( token . s . escaped );
    ASSERT_EQ( "\\\"", TokenValue() ); // needs to be unescaped later
    ASSERT_EQ( QUOTE, NextTokenType() );
    #undef str
}

TEST_F ( OP_TestFlexFixture, IPV4 )
{
    const char * addr = "255.24.110.9";
    ASSERT_EQ( IPV4, StartScan(addr) ); ASSERT_EQ( addr, TokenValue() );
}

TEST_F ( OP_TestFlexFixture, IPV6_1 )      { TestIPV6 ( "1:22:333:4444:5:6:7:8" ); }
TEST_F ( OP_TestFlexFixture, IPV6_2 )      { TestIPV6 ( "1:22:333:4444:5:6:7::" ); }
TEST_F ( OP_TestFlexFixture, IPV6_3 )      { TestIPV6 ( "1::FFFF" ); }
TEST_F ( OP_TestFlexFixture, IPV6_4 )      { TestIPV6 ( "1:2::1:2" ); }
TEST_F ( OP_TestFlexFixture, IPV6_5 )      { TestIPV6 ( "1:2::1:2:3" ); }
TEST_F ( OP_TestFlexFixture, IPV6_6 )      { TestIPV6 ( "1:2::1:2:3:4" ); }
TEST_F ( OP_TestFlexFixture, IPV6_7 )      { TestIPV6 ( "1:2::1:2:3:4:5" ); }
TEST_F ( OP_TestFlexFixture, IPV6_8 )      { TestIPV6 ( "1::1:2:3:4:5:6" ); }
TEST_F ( OP_TestFlexFixture, IPV6_9 )      { TestIPV6 ( "::1:2:3:4:5:6:7" ); }
TEST_F ( OP_TestFlexFixture, IPV6_10_1 )   { TestIPV6 ( "fe80:%Z" ); }
TEST_F ( OP_TestFlexFixture, IPV6_10_2 )   { TestIPV6 ( "fe80::1:2:3:4%blah" ); }
TEST_F ( OP_TestFlexFixture, IPV6_11_1 )   { TestIPV6 ( "::1.2.3.4" ); }
TEST_F ( OP_TestFlexFixture, IPV6_11_2 )   { TestIPV6 ( "::ffff:1.2.3.4" ); }
TEST_F ( OP_TestFlexFixture, IPV6_11_3 )   { TestIPV6 ( "::ffff:0000:1.2.3.4" ); }
TEST_F ( OP_TestFlexFixture, IPV6_12_1 )   { TestIPV6 ( "cdef::1.2.3.4" ); }
TEST_F ( OP_TestFlexFixture, IPV6_12_2 )   { TestIPV6 ( "0123:4567:89ab:cdef::1.2.3.4" ); }

TEST_F ( OP_TestFlexFixture, Path_State )
{
    ASSERT_EQ( PATHSTR, StartScanInURL_State( "a" ) );
    ASSERT_EQ( "a", TokenValue() );
}

TEST_F ( OP_TestFlexFixture, Path_StateReturn )
{   // scanner state manipulation
    const char * input = "a b";
    op__scan_bytes( input, strlen( input ), sc );
    op_start_URL( sc ); // into PATH state
    ASSERT_EQ( PATHSTR, NextTokenType() );
    ASSERT_EQ( "a", TokenValue() );
    op_pop_state( sc );    // back to the default state
    ASSERT_EQ( SPACE, NextTokenType() );
    ASSERT_EQ( STR, NextTokenType() ); ASSERT_EQ( "b", TokenValue() );
}

TEST_F ( OP_TestFlexFixture, Path_Accesssion )
{
    const char * input = "/SRR9154112/%2A.fastq%2A";
    ASSERT_EQ( SLASH, StartScanInURL_State( input ) );
    ASSERT_EQ( ACCESSION, NextTokenType() ); ASSERT_EQ( "SRR9154112", TokenValue() );
    ASSERT_EQ( SLASH, NextTokenType( ) );
    ASSERT_EQ( PATHSTR, NextTokenType() ); ASSERT_EQ( "%2A", TokenValue() );
    ASSERT_EQ( PATHEXT, NextTokenType() ); ASSERT_EQ( ".fastq%2A", TokenValue() );
}

TEST_F ( OP_TestFlexFixture, Agent )
{
    const char * input = "\"linux64 sra-toolkit fasterq-dump.2.10.7 (phid=noc86d2998,libc=2.17)\"";

    op__scan_bytes( input, strlen( input ), sc );
    //op_set_debug ( 1, sc );
    op_start_UserAgent( sc );
    ASSERT_EQ( QUOTE, NextTokenType() );
    ASSERT_EQ( OS, NextTokenType() ); ASSERT_EQ( "linux64", TokenValue() );
    ASSERT_EQ( SPACE, NextTokenType() ); ASSERT_EQ( " ", TokenValue() );
    ASSERT_EQ( SRA_TOOLKIT, NextTokenType() ); ASSERT_EQ( "sra-toolkit", TokenValue() );
    ASSERT_EQ( SPACE, NextTokenType() );

    ASSERT_EQ( SRATOOLVERS, NextTokenType() ); ASSERT_EQ( "fasterq-dump.2.10.7", TokenValue() );

    ASSERT_EQ( SPACE, NextTokenType() );
    ASSERT_EQ( AGENTSTR, NextTokenType() ); ASSERT_EQ( "(", TokenValue() );
    ASSERT_EQ( PHIDVALUE, NextTokenType() ); ASSERT_EQ( "phid=noc86d2998", TokenValue() );
    ASSERT_EQ( AGENTSTR, NextTokenType() ); ASSERT_EQ( ",", TokenValue() );
    ASSERT_EQ( LIBCVERSION, NextTokenType() ); ASSERT_EQ( "libc=2.17", TokenValue() );
    ASSERT_EQ( AGENTSTR, NextTokenType() ); ASSERT_EQ( ")", TokenValue() );
    ASSERT_EQ( QUOTE, NextTokenType() );
}

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}