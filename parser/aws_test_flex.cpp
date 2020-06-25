#include <gtest/gtest.h>

#include <string>

#include "aws_parser.hpp"
#include "aws_scanner.hpp"

using namespace std;

TEST ( TestFlex, InitDestroy )
{
    yyscan_t sc;
    ASSERT_EQ( 0, aws_lex_init( &sc ) );
    ASSERT_EQ( 0, aws_lex_destroy( sc ) );
}

class AWS_TestFlexFixture : public ::testing::Test
{
public:
    virtual void SetUp()
    {
        aws_lex_init( &sc );
    }

    virtual void TearDown()
    {
        aws_lex_destroy( sc );
    }

    int StartScan( const char * input )
    {
        aws__scan_string( input, sc );
        return aws_lex( & token, sc );
    }

    int StartPath(const char * input, bool debug = false )
    {
        aws__scan_string( input, sc );
        aws_set_debug ( debug, sc );
        aws_start_URL( sc );
        return aws_lex( & token, sc );
    }

    int Scan()
    {
        return aws_lex( & token, sc );
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

TEST_F ( AWS_TestFlexFixture, EndOfFile )       { ASSERT_EQ( 0,     StartScan("") ); }
TEST_F ( AWS_TestFlexFixture, SkipSpace )       { ASSERT_EQ( SPACE, StartScan("   ") ); }
TEST_F ( AWS_TestFlexFixture, SkipTab )         { ASSERT_EQ( SPACE, StartScan(" \t ") ); }
TEST_F ( AWS_TestFlexFixture, CR_LF )           { ASSERT_EQ( LF,    StartScan("\r\n") ); }
TEST_F ( AWS_TestFlexFixture, Dash )            { ASSERT_EQ( DASH,  StartScan("-") ); }
TEST_F ( AWS_TestFlexFixture, OpenBracket )     { ASSERT_EQ( OB,    StartScan("[") ); }
TEST_F ( AWS_TestFlexFixture, Quote )           { ASSERT_EQ( QUOTE, StartScan("\"") ); }

TEST_F ( AWS_TestFlexFixture, STR )
{
    ASSERT_EQ( STR, StartScan("abcdefghijklmnopqrstuvw") );
    ASSERT_EQ( "abcdefghijklmnopqrstuvw", Token() );
}

TEST_F ( AWS_TestFlexFixture, Quotes )
{
    ASSERT_EQ( QUOTE, StartScan("\"\"") );
    ASSERT_EQ( QUOTE, Scan() );
    ASSERT_EQ( 0, Scan() );
}
TEST_F ( AWS_TestFlexFixture, QuotedSpace )
{
    ASSERT_EQ( QUOTE, StartScan("\" \"") );
    ASSERT_EQ( SPACE, Scan() );
}
TEST_F ( AWS_TestFlexFixture, QuotedQuestion )
{
    ASSERT_EQ( QUOTE, StartScan("\"?\"") );
    ASSERT_EQ( QSTR, Scan() );
}
TEST_F ( AWS_TestFlexFixture, QuotedString )
{
    #define str ".Bl0-_~!*'();:@&=+$,/%#[]"
    ASSERT_EQ( QUOTE, StartScan("\"" str "\"") );
    ASSERT_EQ( QSTR, Scan() ); ASSERT_EQ( str, Token() );
    ASSERT_FALSE ( token . s . escaped ); // no '\' inside
    #undef str
}
TEST_F ( AWS_TestFlexFixture, QuotedNonAscii )
{   // skip non-ascii characters
    #define str "и"
    ASSERT_EQ( QUOTE, StartScan("\"" str "\"") );
    ASSERT_EQ( QUOTE, Scan() );
    #undef str
}
TEST_F ( AWS_TestFlexFixture, QuotedEscapedQuote )
{   // skip non-ascii characters
    ASSERT_EQ( QUOTE, StartScan("\"\\\"\"") );  /* "\"" */
    ASSERT_EQ( QSTR, Scan() ); 
    ASSERT_TRUE ( token . s . escaped );
    ASSERT_EQ( "\\\"", Token() ); // needs to be unescaped later
    ASSERT_EQ( QUOTE, Scan() );
    #undef str
}

TEST_F ( AWS_TestFlexFixture, IPV4 )
{
    const char * addr = "255.24.110.9";
    ASSERT_EQ( IPV4, StartScan(addr) ); ASSERT_EQ( addr, Token() );
}

TEST_F ( AWS_TestFlexFixture, IPV6_1 )      { TestIPV6 ( "1:22:333:4444:5:6:7:8" ); }
TEST_F ( AWS_TestFlexFixture, IPV6_2 )      { TestIPV6 ( "1:22:333:4444:5:6:7::" ); }
TEST_F ( AWS_TestFlexFixture, IPV6_3 )      { TestIPV6 ( "1::FFFF" ); }
TEST_F ( AWS_TestFlexFixture, IPV6_4 )      { TestIPV6 ( "1:2::1:2" ); }
TEST_F ( AWS_TestFlexFixture, IPV6_5 )      { TestIPV6 ( "1:2::1:2:3" ); }
TEST_F ( AWS_TestFlexFixture, IPV6_6 )      { TestIPV6 ( "1:2::1:2:3:4" ); }
TEST_F ( AWS_TestFlexFixture, IPV6_7 )      { TestIPV6 ( "1:2::1:2:3:4:5" ); }
TEST_F ( AWS_TestFlexFixture, IPV6_8 )      { TestIPV6 ( "1::1:2:3:4:5:6" ); }
TEST_F ( AWS_TestFlexFixture, IPV6_9 )      { TestIPV6 ( "::1:2:3:4:5:6:7" ); }
TEST_F ( AWS_TestFlexFixture, IPV6_10_1 )   { TestIPV6 ( "fe80:%Z" ); }
TEST_F ( AWS_TestFlexFixture, IPV6_10_2 )   { TestIPV6 ( "fe80::1:2:3:4%blah" ); }
TEST_F ( AWS_TestFlexFixture, IPV6_11_1 )   { TestIPV6 ( "::1.2.3.4" ); }
TEST_F ( AWS_TestFlexFixture, IPV6_11_2 )   { TestIPV6 ( "::ffff:1.2.3.4" ); }
TEST_F ( AWS_TestFlexFixture, IPV6_11_3 )   { TestIPV6 ( "::ffff:0000:1.2.3.4" ); }
TEST_F ( AWS_TestFlexFixture, IPV6_12_1 )   { TestIPV6 ( "cdef::1.2.3.4" ); }
TEST_F ( AWS_TestFlexFixture, IPV6_12_2 )   { TestIPV6 ( "0123:4567:89ab:cdef::1.2.3.4" ); }

TEST_F ( AWS_TestFlexFixture, Path_State )
{
    ASSERT_EQ( PATHSTR, StartPath( "a" ) ); 
    ASSERT_EQ( "a", Token() );
}

TEST_F ( AWS_TestFlexFixture, Path_StateReturn )
{
    const char * input = "\"a\" ";
    aws__scan_string( input, sc );
    //aws_set_debug ( 1, sc );
    aws_start_URL( sc ); // send scanner into PATH state
    ASSERT_EQ( QUOTE, Scan() );
    ASSERT_EQ( PATHSTR, Scan() ); ASSERT_EQ( "a", Token() );
    ASSERT_EQ( QUOTE, Scan() );
    aws_stop_URL( sc ); // back to INITIAL state
    ASSERT_EQ( SPACE, Scan() );
}

TEST_F ( AWS_TestFlexFixture, Path_Accesssion )
{
    const char * input = "/SRR9154112/%2A.fastq%2A";
    ASSERT_EQ( SLASH, StartPath( input ) ); 
    ASSERT_EQ( ACCESSION, Scan() ); ASSERT_EQ( "SRR9154112", Token() );
    ASSERT_EQ( SLASH, Scan( ) ); 
    ASSERT_EQ( PATHSTR, Scan() ); ASSERT_EQ( "%2A", Token() );
    ASSERT_EQ( PATHEXT, Scan() ); ASSERT_EQ( ".fastq%2A", Token() );
}
extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}