#include <gtest/gtest.h>
#include <thread>
#include <cstdio>

#include "LineSplitters.hpp"

using namespace std;
using namespace NCBI::Logging;

class BaseLineSplitter_Fixture : public ::testing::Test
{
    public :
        BaseLineSplitter_Fixture() : ls( nullptr ) {}

        virtual ~BaseLineSplitter_Fixture()
        {
            delete( ls );
        }

        virtual void MakeSplitter( const string & txt ) = 0;

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

        LineSplitterInterface * ls;
};


TEST ( TestStdLineSplitter, Ctr )
{
    StdLineSplitter ls( std::cin );
    ASSERT_EQ( 0, ls.size() );
}

class StdLineSplitter_Fixture : public BaseLineSplitter_Fixture
{
    public :
        StdLineSplitter_Fixture()  {}

        virtual void MakeSplitter( const string & txt )
        {
            ss . write( txt.data(), txt.size() );
            ls = new StdLineSplitter( ss );
        }

        std::stringstream ss;
};

TEST_F ( StdLineSplitter_Fixture, split_empty_string )
{
    MakeSplitter( "" );
    ASSERT_FALSE( ls -> getLine() );
    ASSERT_EQ( 0, ls -> size() );
}

TEST_F ( StdLineSplitter_Fixture, split_string_without_nl )
{
    std::string txt( "the first line" );
    MakeSplitter( txt );
    VerifyLine( txt );
    VerifyEmpty();
}

TEST_F ( StdLineSplitter_Fixture, split_string_with_nl )
{
    std::string txt( "the first line" );
    MakeSplitter( txt + "\n" );
    VerifyLine( txt );
    VerifyEmpty();   
}

TEST_F ( StdLineSplitter_Fixture, split_string_2_lines )
{
    std::string txt1( "the first line" );
    std::string txt2( "the second one" );
    MakeSplitter( txt1 + "\n" + txt2 );
    VerifyLine( txt1 );
    VerifyLine( txt2 );
    VerifyEmpty();
}

TEST_F ( StdLineSplitter_Fixture, split_string_with_zero )
{
    std::string txt( "the first \000 line", 16 );
    MakeSplitter( txt );
    VerifyLine( txt );
    ASSERT_EQ( 16, ls -> size() );
    VerifyEmpty();
}

TEST_F ( StdLineSplitter_Fixture, split_string_2_lines_with_zero )
{
    std::string txt1( "the first \000 line", 16 );
    std::string txt2( "the second one" );
    MakeSplitter( txt1 + "\n" + txt2 );
    VerifyLine( txt1 );
    ASSERT_EQ( 16, ls -> size() );
    VerifyLine( txt2 );    
    VerifyEmpty();
}

/////// CLineSplitters

TEST ( TestCLineSplitter, Ctr )
{
    CLineSplitter ls( stdin );
    ASSERT_EQ( 0, ls.size() );
}

class CLineSplitter_Fixture : public BaseLineSplitter_Fixture
{
    public :
        ~CLineSplitter_Fixture()
        {
            if ( nullptr != f )
                fclose( f );
        }

        virtual  void MakeSplitter( const string & txt )
        {
            buf = txt;
            f = fmemopen( (void*)buf.c_str(), buf.length(), "r" );
            ASSERT_NE( nullptr, f );
            ls = new CLineSplitter( f );
        }

        FILE * f = nullptr;
        string buf;
};

TEST_F ( CLineSplitter_Fixture, split_empty_string )
{
    MakeSplitter( string() );
    VerifyEmpty();
}

TEST_F ( CLineSplitter_Fixture, split_nullptr_file )
{
    ls = new CLineSplitter( nullptr );
    VerifyEmpty();
}

TEST_F ( CLineSplitter_Fixture, split_string_without_nl )
{
    std::string txt = "the first line";
    MakeSplitter( txt );
    VerifyLine( txt );
    VerifyEmpty();
}

TEST_F ( CLineSplitter_Fixture, split_string_with_nl )
{
    std::string txt = "the first     line";
    MakeSplitter( string ( txt ) + "\n" );
    VerifyLine( txt );
    VerifyEmpty();
}

TEST_F ( CLineSplitter_Fixture, split_string_2_lines )
{
    const char * txt1 = "the first line";
    const char * txt2 = "the second one";
    MakeSplitter( string ( txt1 ) + "\n" + txt2 );
    VerifyLine( txt1 );
    VerifyLine( txt2 );
    VerifyEmpty();
}

TEST_F ( CLineSplitter_Fixture, split_string_with_zero )
{
    std::string s ( "the first \000 line", 16 );
    MakeSplitter( s );
    VerifyLine( s );
    VerifyEmpty();
}

TEST_F ( CLineSplitter_Fixture, split_string_2_lines_with_zero )
{
    std::string txt1( "the first \000 line", 16 );
    std::string txt2( "the second one" );
    MakeSplitter( txt1 + "\n" + txt2 );
    VerifyLine( txt1 );
    ASSERT_EQ( 16, ls -> size() );    
    VerifyLine( txt2 );
    VerifyEmpty();
}

//// BuffLineSplitter

TEST ( TestBufLineSplitter, Ctr )
{
    BufLineSplitter ls( "abc", 3 );
    ASSERT_EQ( 0, ls.size() );
}

TEST ( TestBufLineSplitter, WithNullPtr )
{
    BufLineSplitter ls( nullptr, 0 );
    ASSERT_EQ( 0, ls.size() );
}

class BufLineSplitter_Fixture : public BaseLineSplitter_Fixture
{
    public :
        ~BufLineSplitter_Fixture() {  }

        virtual  void MakeSplitter( const string & txt )
        {
            buf = txt;
            ls = new BufLineSplitter( buf.c_str(), buf.size() );
        }

        string buf;
};

TEST_F ( BufLineSplitter_Fixture, split_empty_string )
{
    MakeSplitter( string() );
    VerifyEmpty();
}

TEST_F ( BufLineSplitter_Fixture, split_nullptr_file )
{
    ls = new CLineSplitter( nullptr );
    VerifyEmpty();
}

TEST_F ( BufLineSplitter_Fixture, split_string_without_nl )
{
    std::string txt = "the first line";
    MakeSplitter( txt );
    VerifyLine( txt );
    VerifyEmpty();
}

TEST_F ( BufLineSplitter_Fixture, split_string_with_nl )
{
    std::string txt = "the first     line";
    MakeSplitter( string ( txt ) + "\n" );
    VerifyLine( txt );
    VerifyEmpty();
}

TEST_F ( BufLineSplitter_Fixture, split_string_2_lines )
{
    const char * txt1 = "the first line";
    const char * txt2 = "the second one";
    MakeSplitter( string ( txt1 ) + "\n" + txt2 );
    VerifyLine( txt1 );
    VerifyLine( txt2 );
    VerifyEmpty();
}

TEST_F ( BufLineSplitter_Fixture, split_string_with_zero )
{
    std::string txt( "the first \000 line", 16 );
    MakeSplitter( txt );
    VerifyLine( txt );
    VerifyEmpty();
}

TEST_F ( BufLineSplitter_Fixture, split_string_2_lines_with_zero )
{
    std::string txt1( "the first \000 line", 16 );
    std::string txt2( "the second one" );
    MakeSplitter( txt1 + "\n" + txt2 );
    VerifyLine( txt1 );
    VerifyLine( txt2 );
    VerifyEmpty();
}

