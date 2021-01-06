#include <gtest/gtest.h>

#include <string>

#include "cl_path_parser.hpp"
#include "cl_path_scanner.hpp"

using namespace std;

TEST ( CLPATH_TestFlex, InitDestroy )
{
    yyscan_t sc;
    ASSERT_EQ( 0, cl_path_lex_init( &sc ) );
    ASSERT_EQ( 0, cl_path_lex_destroy( sc ) );
}

class CLPATH_TestFlexFixture : public ::testing::Test
{
public:
    virtual void SetUp()
    {
        cl_path_lex_init( &sc );
    }

    virtual void TearDown()
    {
        cl_path_lex_destroy( sc );
    }

    int StartScan( const char * input, size_t size )
    {
        cl_path__scan_bytes( input, size, sc );
        return cl_path_lex( & token, sc );
    }

    int StartScan( const char * input )
    {
        return StartScan( input, strlen( input ) );
    }

    int NextTokenType()
    {
        return cl_path_lex( & token, sc );
    }

    string TokenValue() const { return string( token . str . p, token . str . n ); }

    yyscan_t sc;
    YYSTYPE token;
};

TEST_F ( CLPATH_TestFlexFixture, EndOfFile )       { ASSERT_EQ( 0,     StartScan( "" ) ); }

/* SRA accession names */
TEST_F ( CLPATH_TestFlexFixture, Accession6 )      { ASSERT_EQ( ACCESSION, StartScan( "SRR123456" ) ); ASSERT_EQ( "SRR123456", TokenValue() ); }
TEST_F ( CLPATH_TestFlexFixture, Accession7 )      { ASSERT_EQ( ACCESSION, StartScan( "DRR1234567" ) ); ASSERT_EQ( "DRR1234567", TokenValue() ); }

/* WGS accession names*/
TEST_F ( CLPATH_TestFlexFixture, WGS_noPrefix_4letters_2digits ) { ASSERT_EQ( ACCESSION, StartScan( "AAAB01" ) ); ASSERT_EQ( "AAAB01", TokenValue() ); }
TEST_F ( CLPATH_TestFlexFixture, WGS_noPrefix_4letters_10digits ) { ASSERT_EQ( ACCESSION, StartScan( "ABCDEF0123456789" ) ); ASSERT_EQ( "ABCDEF0123456789", TokenValue() ); }
TEST_F ( CLPATH_TestFlexFixture, WGS_Prefix_4letters_2digits ) { ASSERT_EQ( ACCESSION, StartScan( "NZ_AAAB01" ) ); ASSERT_EQ( "NZ_AAAB01", TokenValue() ); }


/* Refseq and similar names */
TEST_F ( CLPATH_TestFlexFixture, RefSeq_1letter_5digits )              { ASSERT_EQ( ACCESSION, StartScan( "J01415" ) ); ASSERT_EQ( "J01415", TokenValue() ); }
TEST_F ( CLPATH_TestFlexFixture, RefSeq_2letters_6digits )             { ASSERT_EQ( ACCESSION, StartScan( "CM000071" ) ); ASSERT_EQ( "CM000071", TokenValue() ); }
TEST_F ( CLPATH_TestFlexFixture, RefSeq_Prefix_2letters_6digits )      { ASSERT_EQ( ACCESSION, StartScan( "NZ_CM000071" ) ); ASSERT_EQ( "NZ_CM000071", TokenValue() ); }
TEST_F ( CLPATH_TestFlexFixture, RefSeq_Prefix_0letters_9digits )      { ASSERT_EQ( ACCESSION, StartScan( "NW_003315935" ) ); ASSERT_EQ( "NW_003315935", TokenValue() ); }
TEST_F ( CLPATH_TestFlexFixture, RefSeq_2letters_9digits )      { ASSERT_EQ( ACCESSION, StartScan( "NA000008777" ) ); ASSERT_EQ( "NA000008777", TokenValue() ); }
TEST_F ( CLPATH_TestFlexFixture, RefSeq_hs37d5 )                       { ASSERT_EQ( ACCESSION, StartScan( "hs37d5" ) ); ASSERT_EQ( "hs37d5", TokenValue() ); }

TEST_F ( CLPATH_TestFlexFixture, Slash1 )          { ASSERT_EQ( SLASH,     StartScan( "/" ) ); }
TEST_F ( CLPATH_TestFlexFixture, Slash2 )          { ASSERT_EQ( SLASH,     StartScan( "%2F" ) ); }

TEST_F ( CLPATH_TestFlexFixture, PathStr )         { ASSERT_EQ( PATHSTR,   StartScan( "abc&12=34" ) ); ASSERT_EQ( "abc&12=34", TokenValue() ); }

TEST_F ( CLPATH_TestFlexFixture, Percent )         { ASSERT_EQ( PERCENT,     StartScan( "%" ) ); }
TEST_F ( CLPATH_TestFlexFixture, PathStrWithPct )
{
    ASSERT_EQ( PATHSTR,   StartScan( "a%c/12=34" ) );
    ASSERT_EQ( "a", TokenValue() );
    ASSERT_EQ( PERCENT, NextTokenType() );
    ASSERT_EQ( PATHSTR, NextTokenType() );
    ASSERT_EQ( "c", TokenValue() );
}
TEST_F ( CLPATH_TestFlexFixture, PathStrWithURLEncoding )
{
    ASSERT_EQ( PATHSTR,   StartScan( "a%1f/12=34" ) );
    ASSERT_EQ( "a%1f", TokenValue() );
}

TEST_F ( CLPATH_TestFlexFixture, PathDot )
{
    ASSERT_EQ( DOT,   StartScan( ".txt" ) );
    ASSERT_EQ( EXTSTR, NextTokenType() );
    ASSERT_EQ( "txt", TokenValue() );
}

TEST_F ( CLPATH_TestFlexFixture, PathTokens )
{
    ASSERT_EQ( SLASH, StartScan( "/part1/part2/leaf.txt/a" ) );
    ASSERT_EQ( PATHSTR, NextTokenType() );
    ASSERT_EQ( "part1", TokenValue() );
    ASSERT_EQ( SLASH, NextTokenType() );
    ASSERT_EQ( PATHSTR, NextTokenType() );
    ASSERT_EQ( "part2", TokenValue() );
    ASSERT_EQ( SLASH, NextTokenType() );
    ASSERT_EQ( PATHSTR, NextTokenType() );
    ASSERT_EQ( "leaf", TokenValue() );
    ASSERT_EQ( DOT,   NextTokenType() );
    ASSERT_EQ( EXTSTR, NextTokenType() );
    ASSERT_EQ( "txt", TokenValue() );
    ASSERT_EQ( SLASH, NextTokenType() ); // scanner is back to the default state
    ASSERT_EQ( PATHSTR, NextTokenType() );
    ASSERT_EQ( "a", TokenValue() );
}

// ? and # are treated as end of input
TEST_F ( CLPATH_TestFlexFixture, Qmark )           { ASSERT_EQ( 0,  StartScan( "?" ) ); }
TEST_F ( CLPATH_TestFlexFixture, QSeparator1 )     { ASSERT_EQ( 0,  StartScan( "?&" ) ); }

TEST_F ( CLPATH_TestFlexFixture, Hash1 )           { ASSERT_EQ( 0,      StartScan( "#" ) ); }
TEST_F ( CLPATH_TestFlexFixture, Hash2 )           { ASSERT_EQ( 0,     StartScan( "#?" ) ); }

