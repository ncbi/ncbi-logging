#include <gtest/gtest.h>

extern "C"
{
    #include "log1_parser.h"
    #include "log1_scanner.h"
}

TEST ( TestFlex, InitDestroy )
{
    yyscan_t sc;
    ASSERT_EQ( 0, log1_lex_init( &sc ) );
    ASSERT_EQ( 0, log1_lex_destroy( sc ) );
}

class TestFlexFixture : public ::testing::Test
{
public:
    virtual void SetUp()
    {
        log1_lex_init( &sc );
    }

    virtual void TearDown()
    {
        log1_lex_destroy( sc );
    }

    int Scan( const char * input )
    {
        log1__scan_string( input, sc );
        return log1_lex( nullptr, sc );
    }

    yyscan_t sc;
};

TEST_F ( TestFlexFixture, EndOfFile )   { ASSERT_EQ( 0,     Scan("") ); }
TEST_F ( TestFlexFixture, SkipSpace )   { ASSERT_EQ( 0,     Scan("   ") ); }
TEST_F ( TestFlexFixture, SkipTab )     { ASSERT_EQ( 0,     Scan(" \t ") ); }
TEST_F ( TestFlexFixture, CR_LF )       { ASSERT_EQ( LF,    Scan("\r\n") ); }
TEST_F ( TestFlexFixture, Dot )         { ASSERT_EQ( DOT,    Scan(".") ); }
TEST_F ( TestFlexFixture, Dash )        { ASSERT_EQ( DASH,    Scan("-") ); }
TEST_F ( TestFlexFixture, OB )          { ASSERT_EQ( OB,    Scan("[") ); }
TEST_F ( TestFlexFixture, CB )          { ASSERT_EQ( CB,    Scan("]") ); }

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}