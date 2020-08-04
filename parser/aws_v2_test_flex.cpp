#include <gtest/gtest.h>

#include <string>

#include "aws_v2_parser.hpp"
#include "aws_v2_scanner.hpp"

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

    int StartScanInURL_State(const char * input, bool debug = false )
    {
        aws__scan_string( input, sc );
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
    ASSERT_FALSE ( token . s . escaped ); // no '\' inside
    #undef str
}
TEST_F ( AWS_TestFlexFixture, QuotedNonAscii )
{   // TODO properly address UTF-8 codepoints, 
    #define str "и"
    ASSERT_EQ( QUOTE, StartScan("\"" str "\"") );
    ASSERT_EQ( UNRECOGNIZED, NextTokenType() );
    // TODO we cannot compare yet the proper TakenValue....
    // ASSERT_EQ( str, TokenValue() );
    #undef str
}
TEST_F ( AWS_TestFlexFixture, QuotedEscapedQuote )
{   // skip non-ascii characters
    ASSERT_EQ( QUOTE, StartScan("\"\\\"\"") );  /* "\"" */
    ASSERT_EQ( QSTR, NextTokenType() ); 
    ASSERT_TRUE ( token . s . escaped );
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
    ASSERT_EQ( SLASH, StartScanInURL_State( input ) ); 
    ASSERT_EQ( ACCESSION, NextTokenType() ); ASSERT_EQ( "SRR9154112", TokenValue() );
    ASSERT_EQ( SLASH, NextTokenType( ) ); 
    ASSERT_EQ( PERCENT, NextTokenType() ); ASSERT_EQ( "%", TokenValue() );
    ASSERT_EQ( PATHSTR, NextTokenType() ); ASSERT_EQ( "2A", TokenValue() );
    ASSERT_EQ( PATHEXT, NextTokenType() ); ASSERT_EQ( ".fastq", TokenValue() );
}

TEST_F ( AWS_TestFlexFixture, Agent )
{
    const char * input = "\"linux64 sra-toolkit fasterq-dump.2.10.7 (phid=noc86d2998,libc=2.17)\"";

    aws__scan_string( input, sc );
    //aws_set_debug ( 1, sc );
    aws_start_UserAgent( sc );
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

TEST_F ( AWS_TestFlexFixture, TLS_Version )
{
    const char * input = "TLSv1.2";

    aws__scan_string( input, sc );
    //aws_set_debug ( 1, sc );
    aws_start_TLS_vers( sc );
    ASSERT_EQ( TLS_VERSION, NextTokenType() ); 
    ASSERT_EQ( input, TokenValue() );
}

TEST_F ( AWS_TestFlexFixture, Host_ID )
{
    const char * input = 
"AIDAISBTTLPGXGH6YFFAY LzYGhqEwXn5Xiuil9tI6JtK2PiIo+SC6Ute3Isq2qEmt/t0Z7qFkyD0mp1ZIc43bm0qSX4tBbbc=";

    aws__scan_string( input, sc );
    //aws_set_debug ( 1, sc );
    aws_start_host_id( sc );
    ASSERT_EQ( X_AMZ_ID_2, NextTokenType() ); ASSERT_EQ( "AIDAISBTTLPGXGH6YFFAY", TokenValue() );
    ASSERT_EQ( SPACE, NextTokenType() );
    ASSERT_EQ( S3_EXT_REQ_ID, NextTokenType() ); ASSERT_EQ( "LzYGhqEwXn5Xiuil9tI6JtK2PiIo+SC6Ute3Isq2qEmt/t0Z7qFkyD0mp1ZIc43bm0qSX4tBbbc=", TokenValue() );
}

TEST_F ( AWS_TestFlexFixture, KEYSTR_KEYEXT )
{
    const char * input = "AIDA&I\\SBTT%LPG=XGH6?YFFAY.A&B\\C%D=F?X";

    aws__scan_string( input, sc );
    //aws_set_debug ( 1, sc );
    aws_start_key( sc );
    ASSERT_EQ( PATHSTR, NextTokenType() );
    ASSERT_EQ( "AIDA&I\\SBTT%LPG=XGH6?YFFAY", TokenValue() );
    ASSERT_EQ( PATHEXT, NextTokenType() );
    ASSERT_EQ( ".A&B\\C%D=F?X", TokenValue() );
}

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}
