#include <gtest/gtest.h>

#include <string>

#include "tw_parser.hpp"
#include "tw_scanner.hpp"

using namespace std;

TEST ( TestFlex, InitDestroy )
{
    yyscan_t sc;
    ASSERT_EQ( 0, tw_lex_init( &sc ) );
    ASSERT_EQ( 0, tw_lex_destroy( sc ) );
}

class TW_TestFlexFixture : public ::testing::Test
{
public:
    virtual void SetUp()
    {
        tw_lex_init( &sc );
    }

    virtual void TearDown()
    {
        tw_lex_destroy( sc );
    }

    int StartScan( const char * input )
    {
        tw__scan_bytes( input, strlen( input ), sc );
        return tw_lex( & token, sc );
    }

    int StartScanF(const char * input, size_t size, std::function< void( yyscan_t ) > f, bool debug = false )
    {
        tw__scan_bytes( input, size, sc );
        tw_set_debug ( debug, sc );
        f( sc );
        return tw_lex( & token, sc );
    }

    int NextTokenType()
    {
        return tw_lex( & token, sc );
    }

    int NextTokenTypeF( std::function< void( yyscan_t ) > f )
    {
        f( sc );
        return tw_lex( & token, sc );
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

TEST_F ( TW_TestFlexFixture, EndOfFile )       { ASSERT_EQ( 0,    StartScan( "" ) ); }
TEST_F ( TW_TestFlexFixture, SEP1 )            { ASSERT_EQ( SEP,  StartScan( " " ) ); }
TEST_F ( TW_TestFlexFixture, SEP2 )            { ASSERT_EQ( SEP,  StartScan( "\t" ) ); }
TEST_F ( TW_TestFlexFixture, SEP3 )            { ASSERT_EQ( SEP,  StartScan( " \t " ) ); }

TEST_F ( TW_TestFlexFixture, ID1_1 )    { token_test( ID1, "77619/000/0000/R" ); }
TEST_F ( TW_TestFlexFixture, ID1_2 )    { token_test( ID1, "77945/000/0000/PB" ); }

TEST_F ( TW_TestFlexFixture, ID1_3 )
{
    ASSERT_EQ( UNRECOGNIZED, StartScan( "XXX") );
}

TEST_F ( TW_TestFlexFixture, ID2 )      { token_test( ID2, "CC952F33EE2FDBD1", tw_start_ID2 ); }

TEST_F ( TW_TestFlexFixture, ID1_ID2 )
{
    ASSERT_EQ( ID1, StartScan( "77945/000/0000/PB CC952F33EE2FDBD1" ) );
    ASSERT_EQ( "77945/000/0000/PB", TokenValue() );
    ASSERT_EQ( SEP, NextTokenType() );
    ASSERT_EQ( ID2, NextTokenTypeF( tw_start_ID2 ) );
    ASSERT_EQ( "CC952F33EE2FDBD1", TokenValue() );
}

TEST_F ( TW_TestFlexFixture, ID3 )      { token_test( ID3, "0009/0009", tw_start_ID3 ); }
TEST_F ( TW_TestFlexFixture, TIME6 )     { token_test( TIME, "2020-06-11T23:59:58.079949", tw_start_TIME ); }
TEST_F ( TW_TestFlexFixture, TIME3 )     { token_test( TIME, "2020-06-11T23:59:58.079", tw_start_TIME ); }
TEST_F ( TW_TestFlexFixture, SERVER )   { token_test( SERVER, "traceweb22", tw_start_SERVER ); }
TEST_F ( TW_TestFlexFixture, IP1 )      { token_test( IPADDR, "185.151.196.174", tw_start_IPADDR ); }
TEST_F ( TW_TestFlexFixture, IP2 )      { token_test( IPADDR, "UNK_CLIENT", tw_start_IPADDR ); }
TEST_F ( TW_TestFlexFixture, IP3 )      { token_test( IPADDR, "2001:250:3412:c081:756d:9a01:87ef:dc69", tw_start_IPADDR ); }
TEST_F ( TW_TestFlexFixture, SID_1 )    { token_test( SID, "QCC952F33EE2FDBD1_Q00AFSID", tw_start_SID ); }
TEST_F ( TW_TestFlexFixture, SID_2 )    { token_test( SID, "UNK_SESSION", tw_start_SID ); }
TEST_F ( TW_TestFlexFixture, SID_3 )    { token_test( SID, "dde5586852447f7817f0a4f2d6452c7631585809371", tw_start_SID ); }
TEST_F ( TW_TestFlexFixture, SRV_1 )    { token_test( SERVICE, "sra", tw_start_SERVICE ); }
TEST_F ( TW_TestFlexFixture, SRV_2 )    { token_test( SERVICE, "sra_data_locator", tw_start_SERVICE ); }
TEST_F ( TW_TestFlexFixture, SRV_3)    { token_test( SERVICE, "SRA_Data_locator", tw_start_SERVICE ); }
TEST_F ( TW_TestFlexFixture, EVENT_1 )  { token_test( EVENT, "extra", tw_start_EVENT ); }
TEST_F ( TW_TestFlexFixture, EVENT_2 )  { token_test( EVENT, "Trace:", tw_start_EVENT ); }

TEST_F ( TW_TestFlexFixture, MSG )
{
    const char * txt = "CONNECT \"/netopt/ncbi_tools64/c++.by-date/20200713/GCC730-Release64MT/../src/connect/ncbi_localip.c\", line 238: UNK_FUNC --- Loading local IP specs from \"/etc/ncbi/local_ips_v2\"";
    token_test( MSG, txt, tw_start_MSG );
}

TEST_F ( TW_TestFlexFixture, UTF8 )
{
    const char * txt = u8"попробуем 产品公司求购";
    token_test( MSG, txt, tw_start_MSG );
}

TEST_F ( TW_TestFlexFixture, CtrlChars )
{
    const char * txt = "\x01\x06\x10\x1e\x1f";
    token_test( MSG, txt, tw_start_MSG );
    ASSERT_EQ( 5, token . n );
}

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}