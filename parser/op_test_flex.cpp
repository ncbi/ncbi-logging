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

class TestFlexFixture : public ::testing::Test
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

    int StartScan( const char * input )
    {
        op__scan_string( input, sc );
        return op_lex( & token, sc );
    }

    int Scan()
    {
        return op_lex( & token, sc );
    }

    string Token() const { return string( token . s . p, token . s . n ); }

    void TestIPV6 ( const char * addr )
    {
        ASSERT_EQ( IPV6, StartScan(addr) );
        ASSERT_EQ( addr, Token() );
    }

    yyscan_t sc;
    YYSTYPE token;
};

TEST_F ( TestFlexFixture, EndOfFile )       { ASSERT_EQ( 0,     StartScan("") ); }
TEST_F ( TestFlexFixture, SkipSpace )       { ASSERT_EQ( 0,     StartScan("   ") ); }
TEST_F ( TestFlexFixture, SkipTab )         { ASSERT_EQ( 0,     StartScan(" \t ") ); }
TEST_F ( TestFlexFixture, CR_LF )           { ASSERT_EQ( LF,    StartScan("\r\n") ); }
TEST_F ( TestFlexFixture, Dot )             { ASSERT_EQ( DOT,   StartScan(".") ); }
TEST_F ( TestFlexFixture, Dash )            { ASSERT_EQ( DASH,  StartScan("-") ); }
TEST_F ( TestFlexFixture, OpenBracket )     { ASSERT_EQ( OB,    StartScan("[") ); }
TEST_F ( TestFlexFixture, CloceBracket )    { ASSERT_EQ( CB,    StartScan("]") ); }
TEST_F ( TestFlexFixture, Slash )           { ASSERT_EQ( SLASH, StartScan("/") ); }
TEST_F ( TestFlexFixture, Colon )           { ASSERT_EQ( COLON, StartScan(":") ); }
TEST_F ( TestFlexFixture, Port )            { ASSERT_EQ( PORT,  StartScan("port=") ); }
TEST_F ( TestFlexFixture, RL )              { ASSERT_EQ( RL,    StartScan("rl=") ); }
TEST_F ( TestFlexFixture, Quote )           { ASSERT_EQ( QUOTE, StartScan("\"") ); }

TEST_F ( TestFlexFixture, Quotes )
{
    ASSERT_EQ( QUOTE, StartScan("\"\"") );
    ASSERT_EQ( QUOTE, Scan() );ASSERT_EQ( 0, Scan() );
}
TEST_F ( TestFlexFixture, QuotedSpace )
{
    ASSERT_EQ( QUOTE, StartScan("\" \"") );
    ASSERT_EQ( SPACE, Scan() );
}
TEST_F ( TestFlexFixture, QuotedQuestion )
{
    ASSERT_EQ( QUOTE, StartScan("\"?\"") );
    ASSERT_EQ( QMARK, Scan() );
}
TEST_F ( TestFlexFixture, QuotedMethod )
{
    ASSERT_EQ( QUOTE, StartScan("\"OPTIONS\"") );
    ASSERT_EQ( METHOD, Scan() ); ASSERT_EQ( "OPTIONS", Token() );
}
TEST_F ( TestFlexFixture, QuotedVers )
{
    ASSERT_EQ( QUOTE, StartScan("\"HTTP/2\"") );
    ASSERT_EQ( VERS, Scan() ); ASSERT_EQ( "HTTP/2", Token() );
}
TEST_F ( TestFlexFixture, QuotedString )
{
    #define str ".Bl0-_~!*'();:@&=+$,/%#[]"
    ASSERT_EQ( QUOTE, StartScan("\"" str "\"") );
    ASSERT_EQ( QSTR, Scan() ); ASSERT_EQ( str, Token() );
    ASSERT_FALSE ( token . s . escaped ); // no '\' inside
    #undef str
}
TEST_F ( TestFlexFixture, QuotedNonAscii )
{   // skip non-ascii characters
    #define str "и"
    ASSERT_EQ( QUOTE, StartScan("\"" str "\"") );
    ASSERT_EQ( QUOTE, Scan() );
    #undef str
}
TEST_F ( TestFlexFixture, QuotedEscapedQuote )
{   // skip non-ascii characters
    ASSERT_EQ( QUOTE, StartScan("\"\\\"\"") );  /* "\"" */
    ASSERT_EQ( QSTR, Scan() ); 
    ASSERT_TRUE ( token . s . escaped );
    ASSERT_EQ( "\\\"", Token() ); // needs to be unescaped later
    ASSERT_EQ( QUOTE, Scan() );
    #undef str
}

TEST_F ( TestFlexFixture, IPV4 )
{
    const char * addr = "255.24.110.9";
    ASSERT_EQ( IPV4, StartScan(addr) ); ASSERT_EQ( addr, Token() );
}

TEST_F ( TestFlexFixture, IPV6_1 )      { TestIPV6 ( "1:22:333:4444:5:6:7:8" ); }
TEST_F ( TestFlexFixture, IPV6_2 )      { TestIPV6 ( "1:22:333:4444:5:6:7::" ); }
TEST_F ( TestFlexFixture, IPV6_3 )      { TestIPV6 ( "1::FFFF" ); }
TEST_F ( TestFlexFixture, IPV6_4 )      { TestIPV6 ( "1:2::1:2" ); }
TEST_F ( TestFlexFixture, IPV6_5 )      { TestIPV6 ( "1:2::1:2:3" ); }
TEST_F ( TestFlexFixture, IPV6_6 )      { TestIPV6 ( "1:2::1:2:3:4" ); }
TEST_F ( TestFlexFixture, IPV6_7 )      { TestIPV6 ( "1:2::1:2:3:4:5" ); }
TEST_F ( TestFlexFixture, IPV6_8 )      { TestIPV6 ( "1::1:2:3:4:5:6" ); }
TEST_F ( TestFlexFixture, IPV6_9 )      { TestIPV6 ( "::1:2:3:4:5:6:7" ); }
TEST_F ( TestFlexFixture, IPV6_10_1 )   { TestIPV6 ( "fe80:%Z" ); }
TEST_F ( TestFlexFixture, IPV6_10_2 )   { TestIPV6 ( "fe80::1:2:3:4%blah" ); }
TEST_F ( TestFlexFixture, IPV6_11_1 )   { TestIPV6 ( "::1.2.3.4" ); }
TEST_F ( TestFlexFixture, IPV6_11_2 )   { TestIPV6 ( "::ffff:1.2.3.4" ); }
TEST_F ( TestFlexFixture, IPV6_11_3 )   { TestIPV6 ( "::ffff:0000:1.2.3.4" ); }
TEST_F ( TestFlexFixture, IPV6_12_1 )   { TestIPV6 ( "cdef::1.2.3.4" ); }
TEST_F ( TestFlexFixture, IPV6_12_2 )   { TestIPV6 ( "0123:4567:89ab:cdef::1.2.3.4" ); }

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}