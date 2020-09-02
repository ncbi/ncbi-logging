#include <gtest/gtest.h>
#include <thread>
#include <cstdio>

#include "LineSplitters.hpp"

using namespace std;
using namespace NCBI::Logging;

TEST ( TestStdLineSplitter, Ctr )
{
    StdLineSplitter ls( std::cin );
    ASSERT_EQ( 0, ls.size() );
}

class StdLineSplitter_Fixture : public ::testing::Test
{
    public :
        StdLineSplitter_Fixture() : ls ( ss ) {}

        std::stringstream ss;
        StdLineSplitter ls;
};

TEST_F ( StdLineSplitter_Fixture, split_empty_string )
{
    ASSERT_FALSE( ls.getLine() );
    ASSERT_EQ( 0, ls.size() );
}

TEST_F ( StdLineSplitter_Fixture, split_string_without_nl )
{
    const char * txt = "the first line";
    ss << txt;
    ASSERT_TRUE( ls.getLine() );
    ASSERT_EQ( 0, strcmp( txt, ls.data() ) );
    ASSERT_EQ( strlen( txt ), ls.size() );
    ASSERT_FALSE( ls.getLine() );
    ASSERT_EQ( 0, ls.size() );
}

TEST_F ( StdLineSplitter_Fixture, split_string_with_nl )
{
    const char * txt = "the first line";
    ss << txt << std::endl;
    ASSERT_TRUE( ls.getLine() );
    ASSERT_EQ( 0, strcmp( txt, ls.data() ) );
    ASSERT_EQ( strlen( txt ), ls.size() );
    ASSERT_FALSE( ls.getLine() );
    ASSERT_EQ( 0, ls.size() );
}

TEST_F ( StdLineSplitter_Fixture, split_string_2_lines )
{
    const char * txt1 = "the first line";
    const char * txt2 = "the second one";
    ss << txt1 << std::endl << txt2;
    ASSERT_TRUE( ls.getLine() );
    ASSERT_EQ( 0, strcmp( txt1, ls.data() ) );
    ASSERT_EQ( strlen( txt1 ), ls.size() );
    ASSERT_TRUE( ls.getLine() );
    ASSERT_EQ( 0, strcmp( txt2, ls.data() ) );
    ASSERT_EQ( strlen( txt2 ), ls.size() );
    ASSERT_FALSE( ls.getLine() );
    ASSERT_EQ( 0, ls.size() );
}

TEST_F ( StdLineSplitter_Fixture, split_string_with_zero )
{
    const char * txt = "the first \000 line";
    std::string s( txt, 16 );
    ss << s;
    ASSERT_TRUE( ls.getLine() );
    ASSERT_EQ( s, std::string( ls.data(), ls.size() ) );
    ASSERT_EQ( 16, ls.size() );
    ASSERT_FALSE( ls.getLine() );
    ASSERT_EQ( 0, ls.size() );
}

TEST_F ( StdLineSplitter_Fixture, split_string_2_lines_with_zero )
{
    const char * txt1 = "the first \000 line";
    const char * txt2 = "the second one";
    std::string s( txt1, 16 );
    ss << s << std::endl << txt2;
    ASSERT_TRUE( ls.getLine() );
    ASSERT_EQ( s, std::string( ls.data(), ls.size() ) );
    ASSERT_EQ( 16, ls.size() );
    ASSERT_TRUE( ls.getLine() );
    ASSERT_EQ( 0, strcmp( txt2, ls.data() ) );
    ASSERT_EQ( strlen( txt2 ), ls.size() );
    ASSERT_FALSE( ls.getLine() );
    ASSERT_EQ( 0, ls.size() );
}

/////// CLineSplitters

TEST ( TestCLineSplitter, Ctr )
{
    CLineSplitter ls( stdin );
    ASSERT_EQ( 0, ls.size() );
}

class CLineSplitter_Fixture : public ::testing::Test
{
    public :
        ~CLineSplitter_Fixture()
        {
            fclose( f );
            delete ls;
        }

        void MakeSplitter( const string & txt )
        {
            buf = txt;
            f = fmemopen( (void*)buf.c_str(), buf.length(), "r");
            ASSERT_NE( nullptr, f );
            ls = new CLineSplitter( f );
        }

        void VerifyLine( const string & exp )
        {
            ASSERT_TRUE( ls -> getLine() );
            ASSERT_EQ( exp, string( ls -> data(), ls -> size() ) );
        }
        void VerifyEmpty()
        {
            ASSERT_FALSE( ls -> getLine() );
            ASSERT_EQ( 0, ls -> size() );
        }

        FILE * f = nullptr;
        CLineSplitter * ls;
        string buf;
};

TEST_F ( CLineSplitter_Fixture, split_empty_string )
{
    MakeSplitter( string() );

    ASSERT_FALSE( ls->getLine() );
    ASSERT_EQ( 0, ls->size() );
}

TEST_F ( CLineSplitter_Fixture, split_string_without_nl )
{
    const char * txt = "the first line";
    MakeSplitter( txt );

    ASSERT_TRUE( ls -> getLine() );
    ASSERT_EQ( string( txt ), string( ls -> data(), ls -> size() ) );
    VerifyEmpty();
}

TEST_F ( CLineSplitter_Fixture, split_string_with_nl )
{
    const char * txt = "the first     line";
    MakeSplitter( string ( txt ) + "\n" );

    ASSERT_TRUE( ls -> getLine() );
    ASSERT_EQ( string ( txt ), string( ls -> data(), ls -> size() ) );

    VerifyEmpty();
}

TEST_F ( CLineSplitter_Fixture, split_string_2_lines )
{
    const char * txt1 = "the first line";
    const char * txt2 = "the second one";
    MakeSplitter( string ( txt1 ) + "\n" + txt2 );

    ASSERT_TRUE( ls -> getLine() );
    ASSERT_EQ( string ( txt1 ), string( ls -> data(), ls -> size() ) );

    ASSERT_TRUE( ls -> getLine() );
    ASSERT_EQ( string ( txt2 ), string( ls -> data(), ls -> size() ) );

    ASSERT_FALSE( ls -> getLine() );
}

TEST_F ( CLineSplitter_Fixture, split_string_with_zero )
{
    const char * txt = "the first \000 line";
    std::string s ( txt, 16 );
    MakeSplitter( s );

    ASSERT_TRUE( ls -> getLine() );
    ASSERT_EQ( s, string( ls -> data(), ls -> size() ) );

    ASSERT_FALSE( ls -> getLine() );
}

TEST_F ( CLineSplitter_Fixture, split_string_2_lines_with_zero )
{
    const char * txt1 = "the first \000 line";
    const char * txt2 = "the second one";
    std::string s( txt1, 16 );
    MakeSplitter( s + "\n" + txt2 );

    ASSERT_TRUE( ls -> getLine() );
    ASSERT_EQ( s, string( ls -> data(), ls -> size() ) );

    ASSERT_TRUE( ls -> getLine() );
    ASSERT_EQ( string ( txt2 ), string( ls -> data(), ls -> size() ) );

    ASSERT_FALSE( ls -> getLine() );
}

