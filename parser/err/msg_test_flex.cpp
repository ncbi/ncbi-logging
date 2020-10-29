#include <gtest/gtest.h>

#include <string>

#include "msg_parser.hpp"
#include "msg_scanner.hpp"

using namespace std;

void msg_start_VALUE( yyscan_t yyscanner );
void msg_start_REQUEST( yyscan_t yyscanner );

TEST ( TestFlex, InitDestroy )
{
    yyscan_t sc;
    ASSERT_EQ( 0, msg_lex_init( &sc ) );
    ASSERT_EQ( 0, msg_lex_destroy( sc ) );
}

class MSG_TestFlexFixture : public ::testing::Test
{
public:
    virtual void SetUp()
    {
        msg_lex_init( &sc );
    }

    virtual void TearDown()
    {
        msg_lex_destroy( sc );
    }

    int StartScan( const char * input )
    {
        msg__scan_bytes( input, strlen( input ), sc );
        return msg_lex( & token, sc );
    }

    int StartScanF(const char * input, size_t size, std::function< void( yyscan_t ) > f, bool debug = false )
    {
        msg__scan_bytes( input, size, sc );
        msg_set_debug ( debug, sc );
        f( sc );
        return msg_lex( & token, sc );
    }

    int NextTokenType()
    {
        return msg_lex( & token, sc );
    }

    int NextTokenTypeF( std::function< void( yyscan_t ) > f )
    {
        f( sc );
        return msg_lex( & token, sc );
    }

    string TokenValue() const { return string( token . p, token . n ); }

    void token_test( int tt, const char * txt, size_t size, std::function< void( yyscan_t ) > f )
    {
        ASSERT_EQ( tt, StartScanF( txt, size, f ) );
        ASSERT_EQ( txt, TokenValue() );
    }
    void token_test( int tt, const char * txt, std::function< void( yyscan_t ) > f )
    {
        ASSERT_EQ( tt, StartScanF( txt, strlen( txt ), f ) );
        ASSERT_EQ( txt, TokenValue() );
    }

    void token_test( int tt, const char * txt )
    {
        ASSERT_EQ( tt, StartScan( txt ) );
        ASSERT_EQ( txt, TokenValue() );
    }

    yyscan_t sc;
    YYSTYPE token;
};

TEST_F ( MSG_TestFlexFixture, EndOfFile )   { ASSERT_EQ( 0,    StartScan( "" ) ); }
TEST_F ( MSG_TestFlexFixture, Space )       { token_test( SPACE, " " ); }
TEST_F ( MSG_TestFlexFixture, STR )         { token_test( STR, "61030#0:" ); }
TEST_F ( MSG_TestFlexFixture, CLIENT )      { token_test( CLIENT, "client:" ); }
TEST_F ( MSG_TestFlexFixture, SERVER )      { token_test( SERVER, "server:" ); }
TEST_F ( MSG_TestFlexFixture, REQUEST )     { token_test( REQUEST, "request:" ); }
TEST_F ( MSG_TestFlexFixture, HOST )        { token_test( HOST, "host:" ); }
TEST_F ( MSG_TestFlexFixture, VALUESTR )
{
    const char * input = "6103,";
    msg__scan_bytes( input, strlen( input ), sc );
    msg_start_VALUE( sc );
    ASSERT_EQ( STR, msg_lex( & token, sc ) );
    ASSERT_EQ( "6103", TokenValue() ); // commas are not consumed
}

TEST_F ( MSG_TestFlexFixture, request )
{
    const char * input = "\"GET /traces/sra75/SRR/008255/SRR8453128 HTTP/1.1\"";
    msg__scan_bytes( input, strlen( input ), sc );
// msg_set_debug ( 1, sc );

    msg_start_REQUEST( sc );
    ASSERT_EQ( QUOTE, NextTokenType() );
    ASSERT_EQ( STR, NextTokenType() );
    ASSERT_EQ( "GET", TokenValue() );
    ASSERT_EQ( SPACE, NextTokenType() );
    ASSERT_EQ( STR, NextTokenType() );
    ASSERT_EQ( "/traces/sra75/SRR/008255/SRR8453128", TokenValue() );
    ASSERT_EQ( SPACE, NextTokenType() );
    ASSERT_EQ( STR, NextTokenType() );
    ASSERT_EQ( "HTTP/1.1", TokenValue() );
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