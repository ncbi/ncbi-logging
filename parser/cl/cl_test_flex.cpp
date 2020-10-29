#include <gtest/gtest.h>

#include <string>

#include "cl_parser.hpp"
#include "cl_scanner.hpp"

using namespace std;

TEST ( TestFlex, InitDestroy )
{
    yyscan_t sc;
    ASSERT_EQ( 0, cl_lex_init( &sc ) );
    ASSERT_EQ( 0, cl_lex_destroy( sc ) );
}

class CL_TestFlexFixture : public ::testing::Test
{
public:
    virtual void SetUp()
    {
        cl_lex_init( &sc );
    }

    virtual void TearDown()
    {
        cl_lex_destroy( sc );
    }

    int StartScan( const char * input, size_t size )
    {
        cl__scan_bytes( input, size, sc );
        return cl_lex( & token, sc );
    }

    int StartScan( const char * input, bool debug = false )
    {
        cl__scan_bytes( input, strlen( input ), sc );
        cl_set_debug ( debug, sc );
        return cl_lex( & token, sc );
    }

    int NextTokenType()
    {
        return cl_lex( & token, sc );
    }

    string TokenValue() const { return string( token . p, token . n ); }

    yyscan_t sc;
    YYSTYPE token;
};

TEST_F ( CL_TestFlexFixture, EndOfFile )    { ASSERT_EQ( 0,     StartScan("") ); }
TEST_F ( CL_TestFlexFixture, Pipe )         { ASSERT_EQ( PIPE,  StartScan("|") ); }

TEST_F ( CL_TestFlexFixture, Space )
{
    const char * input = "   \t\t \t";
    ASSERT_EQ( SPACE, StartScan(input) );
    ASSERT_EQ( input, TokenValue() );
}

TEST_F ( CL_TestFlexFixture, DateTime1_2 )
{
    const char * input = "Oct 20 15:27:38";
    ASSERT_EQ( DATETIME1, StartScan(input) );
    ASSERT_EQ( input, TokenValue() );
}
TEST_F ( CL_TestFlexFixture, DateTime1_1 )
{
    const char * input = "Oct 1 2:3:4";
    ASSERT_EQ( DATETIME1, StartScan(input) );
    ASSERT_EQ( input, TokenValue() );
}

TEST_F ( CL_TestFlexFixture, DateTime2 )
{
    const char * input = "2020-10-20T15:27:37.592-04:00";
    ASSERT_EQ( DATETIME2, StartScan(input) );
    ASSERT_EQ( input, TokenValue() );
}

TEST_F ( CL_TestFlexFixture, DateTime3 )
{
    const char * input = "2020-10-20 15:27:36,999";
    ASSERT_EQ( DATETIME3, StartScan(input) );
    ASSERT_EQ( input, TokenValue() );
}

TEST_F ( CL_TestFlexFixture, String )
{
    const char * input = "cloudian-NODE-120.be-md.ncbi.nlm.nih.gov";
    ASSERT_EQ( STR, StartScan(input) );
    ASSERT_EQ( input, TokenValue() );
}

TEST_F ( CL_TestFlexFixture, StringNoPipe )
{
    const char * input = "cloudian-NODE-120.be-md.ncbi.nlm.n|||||ih.gov";
    ASSERT_EQ( STR, StartScan(input) );
    ASSERT_EQ( "cloudian-NODE-120.be-md.ncbi.nlm.n", TokenValue() );
}

TEST_F ( CL_TestFlexFixture, ReqId )
{
    const char * input = "[mdc@18060 REQID=\"45200f16-ad2e-1945-87cb-d8c49756ebf4\"]";
    ASSERT_EQ( REQID, StartScan(input) );
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