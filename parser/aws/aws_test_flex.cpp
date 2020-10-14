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

    int StartScan( const char * input, size_t size )
    {
        aws__scan_bytes( input, size, sc );
        return aws_lex( & token, sc );
    }

    int StartScan( const char * input )
    {
        return StartScan( input, strlen( input ) );
    }

    int StartScanInURL_State(const char * input, bool debug = false )
    {
        aws__scan_bytes( input, strlen( input ), sc );
        aws_set_debug ( debug, sc );
        aws_start_URL( sc );
        return aws_lex( & token, sc );
    }

    int NextTokenType()
    {
        return aws_lex( & token, sc );
    }

    string TokenValue() const { return string( token . s . p, token . s . n ); }

    void TestIPV6 ( const char * addr )
    {
        aws_start_ipaddr( sc );
        ASSERT_EQ( IPV6, StartScan(addr) );
        ASSERT_EQ( addr, TokenValue() );
    }

    yyscan_t sc;
    YYSTYPE token;
};

TEST_F ( AWS_TestFlexFixture, EndOfFile )       { ASSERT_EQ( 0,     StartScan("") ); }
TEST_F ( AWS_TestFlexFixture, SkipSpace )       { ASSERT_EQ( SPACE, StartScan("   ") ); }
TEST_F ( AWS_TestFlexFixture, SkipTab )         { ASSERT_EQ( SPACE, StartScan(" \t ") ); }
TEST_F ( AWS_TestFlexFixture, Dash )            { ASSERT_EQ( DASH,  StartScan("-") ); }

TEST_F ( AWS_TestFlexFixture, OpenBracket )
{
    ASSERT_EQ( STR1,  StartScan("[") );
    ASSERT_EQ( "[", TokenValue() );
}

TEST_F ( AWS_TestFlexFixture, CloseBracket )
{
    ASSERT_EQ( STR1,  StartScan("]") );
    ASSERT_EQ( "]", TokenValue() );
}

TEST_F ( AWS_TestFlexFixture, MisplacedTime )
{
    ASSERT_EQ( STR1,  StartScan("[01/Jan/2000:11:22:33 +1234]") );
    ASSERT_EQ( "[01/Jan/2000:11:22:33", TokenValue() );
}

TEST_F ( AWS_TestFlexFixture, Quote )           { ASSERT_EQ( QUOTE, StartScan("\"") ); }

TEST_F ( AWS_TestFlexFixture, STR )
{
    ASSERT_EQ( STR, StartScan("abcdefghijklmnopqrstuvw") );
    ASSERT_EQ( "abcdefghijklmnopqrstuvw", TokenValue() );
}

TEST_F ( AWS_TestFlexFixture, Embedded_0 )
{
    ASSERT_EQ( STR, StartScan("abcdefghij\x00klmnopqrstuvw", 24 ) );
    ASSERT_EQ( "abcdefghij", TokenValue() );

    ASSERT_EQ( UNRECOGNIZED, NextTokenType() );
    ASSERT_EQ( 0, TokenValue()[0] );
    ASSERT_EQ( 1, TokenValue().size() );

    ASSERT_EQ( STR, NextTokenType() );
    ASSERT_EQ( "klmnopqrstuvw", TokenValue() );
}

TEST_F ( AWS_TestFlexFixture, Embedded_CtlChars )
{
    #define str "abc\x00\x01\x02...\x1f-def"
    ASSERT_EQ( QUOTE, StartScan("\"" str "\"", 16) );
    ASSERT_EQ( QSTR, NextTokenType() );
    ASSERT_EQ( 14, token . s . n );
    ASSERT_EQ( 0, memcmp( str, token . s . p, token . s . n ) );
    #undef str
}

TEST_F ( AWS_TestFlexFixture, Quotes )
{
    ASSERT_EQ( QUOTE, StartScan("\"\"") );
    ASSERT_EQ( QUOTE, NextTokenType() );
    ASSERT_EQ( 0, NextTokenType() );
}
TEST_F ( AWS_TestFlexFixture, QuotedSpace )
{
    ASSERT_EQ( QUOTE, StartScan("\" a\tb\"") );
    ASSERT_EQ( SPACE, NextTokenType() ); ASSERT_EQ( " ", TokenValue() );
    ASSERT_EQ( QSTR, NextTokenType() );  ASSERT_EQ( "a", TokenValue() );
    ASSERT_EQ( SPACE, NextTokenType() ); ASSERT_EQ( "\t", TokenValue() );
    ASSERT_EQ( QSTR, NextTokenType() );  ASSERT_EQ( "b", TokenValue() );
}
TEST_F ( AWS_TestFlexFixture, QuotedQuestion )
{
    ASSERT_EQ( QUOTE, StartScan("\"?\"") );
    ASSERT_EQ( QSTR, NextTokenType() );
}
TEST_F ( AWS_TestFlexFixture, QuotedString )
{
    #define str ".Bl0-_~!*'();:@&=+$,/%#[]"
    ASSERT_EQ( QUOTE, StartScan("\"" str "\"") );
    ASSERT_EQ( QSTR, NextTokenType() ); ASSERT_EQ( str, TokenValue() );
    #undef str
}
TEST_F ( AWS_TestFlexFixture, QuotedNonAscii )
{
    #define str "и"
    ASSERT_EQ( QUOTE, StartScan("\"" str "\"") );
    ASSERT_EQ( UNRECOGNIZED, NextTokenType() );
    // unicode tokens come out of the lexer one byte a time...
    ASSERT_EQ( str[ 0 ], TokenValue()[ 0 ] );
    ASSERT_EQ( UNRECOGNIZED, NextTokenType() );
    ASSERT_EQ( str[ 1 ], TokenValue()[ 0 ] );
    #undef str
}
TEST_F ( AWS_TestFlexFixture, QuotedEscapedQuote )
{
    ASSERT_EQ( QUOTE, StartScan("\"\\\"\"") );  /* "\"" */
    ASSERT_EQ( QSTR, NextTokenType() );
    ASSERT_EQ( "\\\"", TokenValue() ); // needs to be unescaped later
    ASSERT_EQ( QUOTE, NextTokenType() );
    #undef str
}

TEST_F ( AWS_TestFlexFixture, IPV4 )
{
    const char * addr = "255.24.110.9";
    aws_start_ipaddr( sc );
    ASSERT_EQ( IPV4, StartScan(addr) );
    ASSERT_EQ( addr, TokenValue() );
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
    ASSERT_EQ( PATHSTR, StartScanInURL_State( "a" ) );
    ASSERT_EQ( "a", TokenValue() );
}

TEST_F ( AWS_TestFlexFixture, Path_StateReturn )
{
    ASSERT_EQ( PATHSTR, StartScanInURL_State( "a b" ) );
    ASSERT_EQ( "a", TokenValue() );
    // " " sends us back to the default state
    ASSERT_EQ( SPACE, NextTokenType() );
    // now should be back in the default state
    ASSERT_EQ( STR, NextTokenType() ); ASSERT_EQ( "b", TokenValue() );
}

TEST_F ( AWS_TestFlexFixture, Path_Accesssion )
{
    const char * input = "/SRR9154112%2F%2A.fastq";
    ASSERT_EQ( PATHSTR, StartScanInURL_State( input ) );
    ASSERT_EQ( input, TokenValue() );
}

TEST_F ( AWS_TestFlexFixture, Agent )
{
    const char * input = "\"linux64 blah blah\"";

    aws__scan_bytes( input, strlen( input ), sc );
    aws_start_UserAgent( sc );
    ASSERT_EQ( QUOTE, NextTokenType() );
    ASSERT_EQ( STR, NextTokenType() ); ASSERT_EQ( "linux64 blah blah", TokenValue() );
    ASSERT_EQ( QUOTE, NextTokenType() );
}

TEST_F ( AWS_TestFlexFixture, Agent_Dash )
{
    const char * input = "\"sra-toolkit\"";

    aws__scan_bytes( input, strlen( input ), sc );
    aws_start_UserAgent( sc );
    ASSERT_EQ( QUOTE, NextTokenType() );
    ASSERT_EQ( STR, NextTokenType() ); ASSERT_EQ( "sra-toolkit", TokenValue() );
    ASSERT_EQ( QUOTE, NextTokenType() );
}

TEST_F ( AWS_TestFlexFixture, Agent_Escaped_quote )
{
    const char * input = "\"linux64 \\\"sra\"";
    aws__scan_bytes( input, strlen( input ), sc );
    aws_start_UserAgent( sc );
    ASSERT_EQ( QUOTE, NextTokenType() );
    ASSERT_EQ( STR, NextTokenType() ); ASSERT_EQ( "linux64 \\\"sra", TokenValue() );
    ASSERT_EQ( QUOTE, NextTokenType() );
}

TEST_F ( AWS_TestFlexFixture, Agent_Empty )
{
    const char * input = "\"\"";
    aws__scan_bytes( input, strlen( input ), sc );
    aws_start_UserAgent( sc );
    ASSERT_EQ( QUOTE, NextTokenType() );
    ASSERT_EQ( QUOTE, NextTokenType() );
}

TEST_F ( AWS_TestFlexFixture, Agent_NakedDash )
{
    const char * input = "- - -";
    aws__scan_bytes( input, strlen( input ), sc );
    aws_start_UserAgent( sc );
    ASSERT_EQ( DASH, NextTokenType() ); ASSERT_EQ( "-", TokenValue() );
}

TEST_F ( AWS_TestFlexFixture, TLS_Version )
{
    const char * input = "TLSv1.2";

    aws__scan_bytes( input, strlen( input ), sc );
    //aws_set_debug ( 1, sc );
    aws_start_TLS_vers( sc );
    ASSERT_EQ( TLS_VERSION, NextTokenType() );
    ASSERT_EQ( input, TokenValue() );
}

TEST_F ( AWS_TestFlexFixture, Host_ID )
{
    const char * input =
"AIDAISBTTLPGXGH6YFFAY LzYGhqEwXn5Xiuil9tI6JtK2PiIo+SC6Ute3Isq2qEmt/t0Z7qFkyD0mp1ZIc43bm0qSX4tBbbc=";

    aws__scan_bytes( input, strlen( input ), sc );
    //aws_set_debug ( 1, sc );
    aws_start_host_id( sc );
    ASSERT_EQ( X_AMZ_ID_2, NextTokenType() ); ASSERT_EQ( "AIDAISBTTLPGXGH6YFFAY", TokenValue() );
    ASSERT_EQ( SPACE, NextTokenType() );
    ASSERT_EQ( S3_EXT_REQ_ID, NextTokenType() ); ASSERT_EQ( "LzYGhqEwXn5Xiuil9tI6JtK2PiIo+SC6Ute3Isq2qEmt/t0Z7qFkyD0mp1ZIc43bm0qSX4tBbbc=", TokenValue() );
}

TEST_F ( AWS_TestFlexFixture, KEYSTR )
{
    const char * input = "AIDA&I\\SBTT%LPG=XGH6?YFFAY.A&B\\C%D=F?X STOP";

    aws__scan_bytes( input, strlen( input ), sc );
    //aws_set_debug ( 1, sc );
    aws_start_key( sc );
    ASSERT_EQ( PATHSTR, NextTokenType() );
    ASSERT_EQ( "AIDA&I\\SBTT%LPG=XGH6?YFFAY.A&B\\C%D=F?X", TokenValue() );
    ASSERT_EQ( SPACE, NextTokenType() );
//    ASSERT_EQ( PATHEXT, NextTokenType() );
//    ASSERT_EQ( ".A&B\\C%D=F?X", TokenValue() );
}

TEST_F ( AWS_TestFlexFixture, RestOfLine )
{
    const char * input = "- and now the extra tokens";

    aws__scan_bytes( input, strlen( input ), sc );
    //aws_set_debug ( 1, sc );
    ASSERT_EQ( DASH, NextTokenType() );
    aws_start_rest_of( sc );
    ASSERT_EQ( STR, NextTokenType() );
    ASSERT_EQ( " and now the extra tokens", TokenValue() );
}

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}
