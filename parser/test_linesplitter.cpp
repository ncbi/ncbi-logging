#include <gtest/gtest.h>
#include <thread>

#include "ReceiverInterface.hpp"

using namespace std;
using namespace NCBI::Logging;

TEST ( TestStdLineSplitter, Ctr )
{
    StdLineSplitter ls( std::cin );
    ASSERT_EQ( 0, ls.size() );
}

class LineSplitter_Fixture : public ::testing::Test
{
    public :
        std::stringstream ss;
};

TEST_F ( LineSplitter_Fixture, split_empty_string )
{
    StdLineSplitter ls( ss );
    ASSERT_FALSE( ls.getLine() );
    ASSERT_EQ( 0, ls.size() );
}

TEST_F ( LineSplitter_Fixture, split_string_without_nl )
{
    const char * txt = "the first line";
    ss << txt;
    StdLineSplitter ls( ss );
    ASSERT_TRUE( ls.getLine() );
    ASSERT_EQ( 0, strcmp( txt, ls.data() ) );
    ASSERT_EQ( strlen( txt ), ls.size() );
    ASSERT_FALSE( ls.getLine() );
    ASSERT_EQ( 0, ls.size() );
}

TEST_F ( LineSplitter_Fixture, split_string_with_nl )
{
    const char * txt = "the first line";
    ss << txt << std::endl;
    StdLineSplitter ls( ss );
    ASSERT_TRUE( ls.getLine() );
    ASSERT_EQ( 0, strcmp( txt, ls.data() ) );
    ASSERT_EQ( strlen( txt ), ls.size() );
    ASSERT_FALSE( ls.getLine() );
    ASSERT_EQ( 0, ls.size() );
}

TEST_F ( LineSplitter_Fixture, split_string_2_lines )
{
    const char * txt1 = "the first line";
    const char * txt2 = "the second one";
    ss << txt1 << std::endl << txt2;
    StdLineSplitter ls( ss );
    ASSERT_TRUE( ls.getLine() );
    ASSERT_EQ( 0, strcmp( txt1, ls.data() ) );
    ASSERT_EQ( strlen( txt1 ), ls.size() );
    ASSERT_TRUE( ls.getLine() );
    ASSERT_EQ( 0, strcmp( txt2, ls.data() ) );
    ASSERT_EQ( strlen( txt2 ), ls.size() );
    ASSERT_FALSE( ls.getLine() );
    ASSERT_EQ( 0, ls.size() );
}

TEST_F ( LineSplitter_Fixture, split_string_with_zero )
{
    const char * txt = "the first \000 line";
    std::string s( txt, 16 );
    ss << s;
    StdLineSplitter ls( ss );
    ASSERT_TRUE( ls.getLine() );
    ASSERT_EQ( s, std::string( ls.data(), ls.size() ) );
    ASSERT_EQ( 16, ls.size() );
    ASSERT_FALSE( ls.getLine() );
    ASSERT_EQ( 0, ls.size() );
}

TEST_F ( LineSplitter_Fixture, split_string_2_lines_with_zero )
{
    const char * txt1 = "the first \000 line";
    const char * txt2 = "the second one";    
    std::string s( txt1, 16 );
    ss << s << std::endl << txt2;
    StdLineSplitter ls( ss );
    ASSERT_TRUE( ls.getLine() );
    ASSERT_EQ( s, std::string( ls.data(), ls.size() ) );
    ASSERT_EQ( 16, ls.size() );
    ASSERT_TRUE( ls.getLine() );
    ASSERT_EQ( 0, strcmp( txt2, ls.data() ) );
    ASSERT_EQ( strlen( txt2 ), ls.size() );
    ASSERT_FALSE( ls.getLine() );
    ASSERT_EQ( 0, ls.size() );
}
