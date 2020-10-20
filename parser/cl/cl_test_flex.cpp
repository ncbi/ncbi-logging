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

    int StartScan( const char * input )
    {
        cl__scan_bytes( input, strlen( input ), sc );
        return cl_lex( & token, sc );
    }

    int NextTokenType()
    {
        return cl_lex( & token, sc );
    }

    string TokenValue() const { return string( token . s . p, token . s . n ); }

    yyscan_t sc;
    YYSTYPE token;
};

TEST_F ( CL_TestFlexFixture, EndOfFile )       { ASSERT_EQ( 0,     StartScan("") ); }

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}