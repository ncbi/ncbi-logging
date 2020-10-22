#include <gtest/gtest.h>

#include <string>

#include "err_parser.hpp"
#include "err_scanner.hpp"

using namespace std;

void err_start_MSG( yyscan_t yyscanner );

TEST ( TestFlex, InitDestroy )
{
    yyscan_t sc;
    ASSERT_EQ( 0, err_lex_init( &sc ) );
    ASSERT_EQ( 0, err_lex_destroy( sc ) );
}

class ERR_TestFlexFixture : public ::testing::Test
{
public:
    virtual void SetUp()
    {
        err_lex_init( &sc );
    }

    virtual void TearDown()
    {
        err_lex_destroy( sc );
    }

    int StartScan( const char * input )
    {
        err__scan_bytes( input, strlen( input ), sc );
        return err_lex( & token, sc );
    }

    int StartScanF(const char * input, size_t size, std::function< void( yyscan_t ) > f, bool debug = false )
    {
        err__scan_bytes( input, size, sc );
        err_set_debug ( debug, sc );
        f( sc );
        return err_lex( & token, sc );
    }

    int NextTokenType()
    {
        return err_lex( & token, sc );
    }

    int NextTokenTypeF( std::function< void( yyscan_t ) > f )
    {
        f( sc );
        return err_lex( & token, sc );
    }

    string TokenValue() const { return string( token . s . p, token . s . n ); }

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

TEST_F ( ERR_TestFlexFixture, EndOfFile )   { ASSERT_EQ( 0,    StartScan( "" ) ); }

TEST_F ( ERR_TestFlexFixture, Space )       { token_test( SPACE, " " ); }
TEST_F ( ERR_TestFlexFixture, DateTime )    { token_test( DATETIME, "2020/10/15 00:03:45" ); }
TEST_F ( ERR_TestFlexFixture, Severity )    { token_test( SEVERITY, "[alert]" ); }
TEST_F ( ERR_TestFlexFixture, STR )         { token_test( STR, "61030#0:" ); }

TEST_F ( ERR_TestFlexFixture, Message )
{
    const char * input = "*650699 open() \"/netmnt/traces04//sra75/SRR/008255/SRR8453128\" failed (2: No such file or directory), client: 218.219.98.126, server: srafiles21.be-md.ncbi.nlm.nih.gov, request: \"GET /traces/sra75/SRR/008255/SRR8453128 HTTP/1.1\", host: \"sra-downloadb.be-md.ncbi.nlm.nih.gov\"";
    err__scan_bytes( input, strlen( input ), sc );
    err_start_MSG( sc );
    ASSERT_EQ( MSG, err_lex( & token, sc ) );
    ASSERT_EQ( input, TokenValue() );
}

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}