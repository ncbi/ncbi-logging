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

    int StartScan( const char * input )
    {
        gcp__scan_string( input, sc );
        return gcp_lex( & token, sc );
    }

    int Scan()
    {
        return gcp_lex( & token, sc );
    }

    int StartPath(const char * input, bool debug = false )
    {
        gcp__scan_string( input, sc );
        gcp_set_debug ( debug, sc );
        gcp_start_URL( sc );
        return gcp_lex( & token, sc );
    }

    string Token() const { return string( token . s . p, token . s . n ); }

    void TestIPV6 ( const char * addr )
    {
        string quoted = string("\"") + addr + "\"";
        ASSERT_EQ( QUOTE, StartScan(quoted.c_str()) );
        ASSERT_EQ( IPV6, Scan() );
        ASSERT_EQ( addr, Token() );
    }

    yyscan_t sc;
    YYSTYPE token;
};

TEST_F ( GCP_TestFlexFixture, EndOfFile )       { ASSERT_EQ( 0,     StartScan("") ); }
TEST_F ( GCP_TestFlexFixture, SkipSpace )       { ASSERT_EQ( 0,     StartScan("   ") ); }
TEST_F ( GCP_TestFlexFixture, SkipTab )         { ASSERT_EQ( 0,     StartScan(" \t ") ); }
TEST_F ( GCP_TestFlexFixture, Quote )           { ASSERT_EQ( QUOTE, StartScan("\"") ); }

TEST_F ( GCP_TestFlexFixture, Quotes )
{
    ASSERT_EQ( QUOTE, StartScan("\"\"") );
    ASSERT_EQ( QUOTE, Scan() );ASSERT_EQ( 0, Scan() );
}
TEST_F ( GCP_TestFlexFixture, QuotedString )
{
    #define str ".Bl0-_~!*'();:@&=+$,/%#[]"
    ASSERT_EQ( QUOTE, StartScan("\"" str "\"") );
    ASSERT_EQ( QSTR, Scan() ); ASSERT_EQ( str, Token() );
    ASSERT_FALSE ( token . s . escaped ); // no '\' inside
    #undef str
}
TEST_F ( GCP_TestFlexFixture, QuotedNonAscii )
{   // skip non-ascii characters
    #define str "и"
    ASSERT_EQ( QUOTE, StartScan("\"" str "\"") );
    ASSERT_EQ( QUOTE, Scan() );
    #undef str
}
TEST_F ( GCP_TestFlexFixture, QuotedEscapedQuote )
{   // skip non-ascii characters
    ASSERT_EQ( QUOTE, StartScan("\"\\\"\"") );  /* "\"" */
    ASSERT_EQ( QSTR, Scan() ); 
    ASSERT_TRUE ( token . s . escaped );
    ASSERT_EQ( "\\\"", Token() ); // needs to be unescaped later
    ASSERT_EQ( QUOTE, Scan() );
    #undef str
}

TEST_F ( GCP_TestFlexFixture, IPV4 )
{
    const char * addr = "\"255.24.110.9\"";
    ASSERT_EQ( QUOTE, StartScan(addr) ); 
    ASSERT_EQ( IPV4, Scan() ); 
    ASSERT_EQ( "255.24.110.9", Token() );
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
    ASSERT_EQ( "a", Token() );
}

TEST_F ( GCP_TestFlexFixture, Path_StateReturn )
{
    const char * input = "\"a\",";
    gcp__scan_string( input, sc );
    //gcp_set_debug ( 1, sc );
    gcp_start_URL( sc ); // send scanner into PATH state
    ASSERT_EQ( QUOTE, Scan() );
    ASSERT_EQ( PATHSTR, Scan() ); ASSERT_EQ( "a", Token() );
    ASSERT_EQ( QUOTE, Scan() );
    gcp_stop_URL( sc ); // back to INITIAL state
    ASSERT_EQ( COMMA, Scan() );
}

TEST_F ( GCP_TestFlexFixture, Path_Accesssion )
{
    const char * input = "ERR4080068";
    ASSERT_EQ( ACCESSION, StartPath( input ) ); ASSERT_EQ( input, Token() );
}

TEST_F ( GCP_TestFlexFixture, Path_AccesssionSlash )
{
    const char * input = "ERR4080068/";
    ASSERT_EQ( ACCESSION, StartPath( input ) ); ASSERT_EQ( "ERR4080068", Token() );
    ASSERT_EQ( SLASH, Scan() );
}

TEST_F ( GCP_TestFlexFixture, Path_AccesssionSlashFilename )
{
    const char * input = "ERR4080068/HG04194_ATATGGAT-CTGTATTA_HFM5LDSXX_L002_001";
    ASSERT_EQ( ACCESSION, StartPath( input ) ); ASSERT_EQ( "ERR4080068", Token() );
    ASSERT_EQ( SLASH, Scan() );
    ASSERT_EQ( PATHSTR, Scan() ); 
    ASSERT_EQ( "HG04194_ATATGGAT-CTGTATTA_HFM5LDSXX_L002_001", Token() );
}

TEST_F ( GCP_TestFlexFixture, Path_AccesssionSlashFilename_Ext )
{
    const char * input = "ERR4080068/HG04194_ATATGGAT-CTGTATTA_HFM5LDSXX_L002_001.fq";
    ASSERT_EQ( ACCESSION, StartPath( input ) ); ASSERT_EQ( "ERR4080068", Token() );
    ASSERT_EQ( SLASH, Scan() );
    ASSERT_EQ( PATHSTR, Scan() ); 
    ASSERT_EQ( "HG04194_ATATGGAT-CTGTATTA_HFM5LDSXX_L002_001", Token() );
    ASSERT_EQ( PATHEXT, Scan() ); 
    ASSERT_EQ( ".fq", Token() );
}

TEST_F ( GCP_TestFlexFixture, Path_NameSlashAccesssionSlashFilename_Ext )
{
    const char * input = "SRP123456/ERR4080068/qwe.ff";
    ASSERT_EQ( PATHSTR, StartPath( input ) ); ASSERT_EQ( "SRP123456", Token() );
    ASSERT_EQ( SLASH, Scan() );
    ASSERT_EQ( ACCESSION, Scan() ); ASSERT_EQ( "ERR4080068", Token() );
    ASSERT_EQ( SLASH, Scan() );
    ASSERT_EQ( PATHSTR, Scan() ); ASSERT_EQ( "qwe", Token() );
    ASSERT_EQ( PATHEXT, Scan() ); ASSERT_EQ( ".ff", Token() );
}

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}
