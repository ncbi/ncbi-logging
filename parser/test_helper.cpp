#include <gtest/gtest.h>

#include "helper.hpp"

using namespace std;
using namespace NCBI::Logging;

TEST ( TestHelper, MonthJan ) { t_str m = { "Jan", 3, false };ASSERT_EQ( 1, month_int(& m ) ); }
TEST ( TestHelper, MonthFeb ) { t_str m = { "Feb", 3, false };ASSERT_EQ( 2, month_int(& m ) ); }
TEST ( TestHelper, MonthMar ) { t_str m = { "Mar", 3, false };ASSERT_EQ( 3, month_int(& m ) ); }
TEST ( TestHelper, MonthApr ) { t_str m = { "Apr", 3, false };ASSERT_EQ( 4, month_int(& m ) ); }
TEST ( TestHelper, MonthMay ) { t_str m = { "May", 3, false };ASSERT_EQ( 5, month_int(& m ) ); }
TEST ( TestHelper, MonthJun ) { t_str m = { "Jun", 3, false };ASSERT_EQ( 6, month_int(& m ) ); }
TEST ( TestHelper, MonthJul ) { t_str m = { "Jul", 3, false };ASSERT_EQ( 7, month_int(& m ) ); }
TEST ( TestHelper, MonthAug ) { t_str m = { "Aug", 3, false };ASSERT_EQ( 8, month_int(& m ) ); }
TEST ( TestHelper, MonthSep ) { t_str m = { "Sep", 3, false };ASSERT_EQ( 9, month_int(& m ) ); }
TEST ( TestHelper, MonthOct ) { t_str m = { "Oct", 3, false };ASSERT_EQ( 10, month_int(& m ) ); }
TEST ( TestHelper, MonthNov ) { t_str m = { "Nov", 3, false };ASSERT_EQ( 11, month_int(& m ) ); }
TEST ( TestHelper, MonthDec ) { t_str m = { "Dec", 3, false };ASSERT_EQ( 12, month_int(& m ) ); } 

TEST ( TestHelper, MonthFoo ) { t_str m = { "Foo", 3, false };ASSERT_EQ( 0, month_int(& m ) ); } 

TEST (TestHelper, StringEmpty)
{
    t_str s = { nullptr, 0, false };
    ASSERT_EQ( string(), ToString( s ) );
}
TEST (TestHelper, StringNonEmpty)
{
    t_str s = { "stuff", 4, false };
    ASSERT_EQ( string("stuf"), ToString( s ) );
}
TEST (TestHelper, StringEscaped)
{
    t_str s = { "\\\"", 2, true };
    ASSERT_EQ( string("\""), ToString( s ) );
}
TEST (TestHelper, StringEscaped_Multiple)
{
    t_str s = { "abc\\\"\\\"", 7, true };
    ASSERT_EQ( string("abc\"\""), ToString( s ) );
}
TEST (TestHelper, StringEscaped_TrailingSlash)
{
    t_str s = { "\\", 1, true };
    ASSERT_EQ( string("\\"), ToString( s ) );
}
TEST (TestHelper, StringEscaped_NotEscapedCharacter)
{
    t_str s = { "\\n", 2, true };
    ASSERT_EQ( string("\\n"), ToString( s ) );
}

TEST (TestHelper, ToInt64_1)
{
    t_str s = { "123", 3, false };
    ASSERT_EQ( 123, ToInt64( s ) );
}

TEST (TestHelper, ToInt64_2)
{
    t_str s = { "-123", 4, false };
    ASSERT_EQ( -123, ToInt64( s ) );
}

TEST (TestHelper, ToInt64_3)
{
    t_str s = { "", 0, false };
    ASSERT_EQ( 0, ToInt64( s ) );
}

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}
