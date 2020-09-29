#include <gtest/gtest.h>

#include "Formatters.hpp"

using namespace std;
using namespace NCBI::Logging;

TEST( JsonLib_Formatter, Create )
{
    JsonLibFormatter f;
}

class JsonLibFormatter_Fixture : public ::testing::Test
{
public:
    virtual void SetUp() {}
    virtual void TearDown() {}

    void TestControlChar( unsigned char ch, string expect )
    {
        f.addNameValue( "a", string ( "some" ) + string ( 1, ch ) + "text" );
        string exp = string ( "{\"a\":\"some" ) + expect + "text\"}";
        ASSERT_EQ( exp, f.format() );
    }

    JsonLibFormatter f;
};

TEST_F( JsonLibFormatter_Fixture, FormatEmpty )
{
    ASSERT_EQ("{}", f.format());
}

TEST_F( JsonLibFormatter_Fixture, Format_t_str )
{
    const t_str str = { "v", 1, false };
    f.addNameValue("a", str);
    ASSERT_EQ("{\"a\":\"v\"}", f.format());

    // make sure the data was deleted
    ASSERT_EQ("{}", f.format());
}

TEST_F( JsonLibFormatter_Fixture, Format_string )
{
    const string str ( "v" );
    f.addNameValue("a", str);
    ASSERT_EQ("{\"a\":\"v\"}", f.format());
}

TEST_F( JsonLibFormatter_Fixture, Format_t_str_quote )
{
    const t_str str = { "v\"", 2, true };
    f.addNameValue("a", str);
    ASSERT_EQ("{\"a\":\"v\\\"\"}", f.format());
}

TEST_F( JsonLibFormatter_Fixture, Format_string_quote )
{
    const string str = ( "v\"" );
    f.addNameValue("a", str);
    ASSERT_EQ("{\"a\":\"v\\\"\"}", f.format());
}

TEST_F( JsonLibFormatter_Fixture, Format_int )
{
    f.addNameValue("a", 12345);
    ASSERT_EQ("{\"a\":12345}", f.format());
}

TEST_F( JsonLibFormatter_Fixture, Format_neg_int )
{
    f.addNameValue("a", -12345);
    ASSERT_EQ("{\"a\":-12345}", f.format());
}

TEST_F (JsonLibFormatter_Fixture, Quote)
{
    f.addNameValue( "key", "\"" );
    ASSERT_EQ ( "{\"key\":\"\\\"\"}", f.format() );
}

TEST_F (JsonLibFormatter_Fixture, Backslash)
{
    f.addNameValue( "key", "\\" );
    ASSERT_EQ ( "{\"key\":\"\\\\\"}", f.format() );
}

TEST_F (JsonLibFormatter_Fixture, StringEscaped)
{
    f.addNameValue( "key", "\\\"" );
    ASSERT_EQ ( "{\"key\":\"\\\\\\\"\"}", f.format() );
}

TEST_F (JsonLibFormatter_Fixture, Format_array)
{
    f.addArray( "key" );
    const t_str s1 = { "123", 3, false };
    f.addArrayValue( s1 );
    const t_str s2 = { "456", 3, false };
    f.addArrayValue( s2 );
    f.closeArray();
    ASSERT_EQ ( "{\"key\":[\"123\",\"456\"]}", f.format() );
}

TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_0 ) { TestControlChar( 0, "\\u0000"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_1 ) { TestControlChar( 1, "\\u0001"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_2 ) { TestControlChar( 2, "\\u0002"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_3 ) { TestControlChar( 3, "\\u0003"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_4 ) { TestControlChar( 4, "\\u0004"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_5 ) { TestControlChar( 5, "\\u0005"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_6 ) { TestControlChar( 6, "\\u0006"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_7 ) { TestControlChar( 7, "\\u0007"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_8 ) { TestControlChar( 8, "\\b"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_9 ) { TestControlChar( 9, "\\t"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_10 ) { TestControlChar( 10, "\\n"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_11 ) { TestControlChar( 11, "\\u000b"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_12 ) { TestControlChar( 12, "\\f"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_13 ) { TestControlChar( 13, "\\r"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_14 ) { TestControlChar( 14, "\\u000e"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_15 ) { TestControlChar( 15, "\\u000f"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_16 ) { TestControlChar( 16, "\\u0010"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_17 ) { TestControlChar( 17, "\\u0011"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_18 ) { TestControlChar( 18, "\\u0012"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_19 ) { TestControlChar( 19, "\\u0013"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_20 ) { TestControlChar( 20, "\\u0014"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_21 ) { TestControlChar( 21, "\\u0015"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_22 ) { TestControlChar( 22, "\\u0016"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_23 ) { TestControlChar( 23, "\\u0017"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_24 ) { TestControlChar( 24, "\\u0018"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_25 ) { TestControlChar( 25, "\\u0019"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_26 ) { TestControlChar( 26, "\\u001a"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_27 ) { TestControlChar( 27, "\\u001b"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_28 ) { TestControlChar( 28, "\\u001c"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_29 ) { TestControlChar( 29, "\\u001d"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_30 ) { TestControlChar( 30, "\\u001e"); }
TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char_31 ) { TestControlChar( 31, "\\u001f"); }

/* -------------------------------------------------------------- */

class JsonFastFormatter_Fixture : public ::testing::Test
{
public:
    virtual void SetUp() {}
    virtual void TearDown() {}

    void TestControlChar( unsigned char ch, string expect )
    {
        f.addNameValue( "a", string ( "some" ) + string ( 1, ch ) + "text" );
        string exp = string ( "{\"a\":\"some" ) + expect + "text\"}";
        ASSERT_EQ( exp, f.format() );
    }

    JsonFastFormatter f;
};

TEST_F( JsonFastFormatter_Fixture, FormatEmpty )
{
    ASSERT_EQ( "{}", f.format() );
}

TEST_F( JsonFastFormatter_Fixture, Format_t_str )
{
    const t_str str = { "v", 1, false };
    f.addNameValue("a", str);
    ASSERT_EQ("{\"a\":\"v\"}", f.format());

    // make sure the data was deleted
    ASSERT_EQ("{}", f.format());
}

TEST_F( JsonFastFormatter_Fixture, Format_string )
{
    const string str ( "v" );
    f.addNameValue("a", str);
    ASSERT_EQ("{\"a\":\"v\"}", f.format());
}

TEST_F( JsonFastFormatter_Fixture, Format_t_str_quote )
{
    const t_str str = { "v\"", 2, true };
    f.addNameValue("a", str);
    ASSERT_EQ("{\"a\":\"v\\\"\"}", f.format());
}

TEST_F( JsonFastFormatter_Fixture, Format_string_quote )
{
    const string str = ( "v\"" );
    f.addNameValue("a", str);
    ASSERT_EQ("{\"a\":\"v\\\"\"}", f.format());
}

TEST_F( JsonFastFormatter_Fixture, Format_int )
{
    f.addNameValue("a", 12345);
    ASSERT_EQ("{\"a\":12345}", f.format());
}

TEST_F( JsonFastFormatter_Fixture, Format_neg_int )
{
    f.addNameValue("a", -12345);
    ASSERT_EQ("{\"a\":-12345}", f.format());
}

TEST_F (JsonFastFormatter_Fixture, Quote)
{
    f.addNameValue( "key", "\"" );
    ASSERT_EQ ( "{\"key\":\"\\\"\"}", f.format() );
}

TEST_F (JsonFastFormatter_Fixture, Backslash)
{
    f.addNameValue( "key", "\\" );
    ASSERT_EQ ( "{\"key\":\"\\\\\"}", f.format() );
}

TEST_F (JsonFastFormatter_Fixture, StringEscaped)
{
    f.addNameValue( "key", "\\\"" );
    ASSERT_EQ ( "{\"key\":\"\\\\\\\"\"}", f.format() );
}

TEST_F (JsonFastFormatter_Fixture, Format_array)
{
    f.addArray( "key" );
    const t_str s1 = { "123", 3, false };
    f.addArrayValue( s1 );
    const t_str s2 = { "456", 3, false };
    f.addArrayValue( s2 );
    f.closeArray();
    ASSERT_EQ ( "{\"key\":[\"123\",\"456\"]}", f.format() );
}

TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_0 ) { TestControlChar( 0, "\\u0000"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_1 ) { TestControlChar( 1, "\\u0001"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_2 ) { TestControlChar( 2, "\\u0002"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_3 ) { TestControlChar( 3, "\\u0003"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_4 ) { TestControlChar( 4, "\\u0004"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_5 ) { TestControlChar( 5, "\\u0005"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_6 ) { TestControlChar( 6, "\\u0006"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_7 ) { TestControlChar( 7, "\\u0007"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_8 ) { TestControlChar( 8, "\\b"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_9 ) { TestControlChar( 9, "\\t"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_10 ) { TestControlChar( 10, "\\n"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_11 ) { TestControlChar( 11, "\\u000b"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_12 ) { TestControlChar( 12, "\\f"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_13 ) { TestControlChar( 13, "\\r"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_14 ) { TestControlChar( 14, "\\u000e"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_15 ) { TestControlChar( 15, "\\u000f"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_16 ) { TestControlChar( 16, "\\u0010"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_17 ) { TestControlChar( 17, "\\u0011"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_18 ) { TestControlChar( 18, "\\u0012"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_19 ) { TestControlChar( 19, "\\u0013"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_20 ) { TestControlChar( 20, "\\u0014"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_21 ) { TestControlChar( 21, "\\u0015"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_22 ) { TestControlChar( 22, "\\u0016"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_23 ) { TestControlChar( 23, "\\u0017"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_24 ) { TestControlChar( 24, "\\u0018"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_25 ) { TestControlChar( 25, "\\u0019"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_26 ) { TestControlChar( 26, "\\u001a"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_27 ) { TestControlChar( 27, "\\u001b"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_28 ) { TestControlChar( 28, "\\u001c"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_29 ) { TestControlChar( 29, "\\u001d"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_30 ) { TestControlChar( 30, "\\u001e"); }
TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char_31 ) { TestControlChar( 31, "\\u001f"); }

TEST( ReverseFormatterTest, Create )
{
    ReverseFormatter fmt;
}

TEST( ReverseFormatterTest, FormatEmptyObj )
{
    ReverseFormatter fmt;
    ASSERT_EQ( "", fmt.format() );
}

TEST( ReverseFormatterTest, FormatStrings )
{
    ReverseFormatter fmt;
    fmt . addNameValue( "ignored", "value1" );
    fmt . addNameValue( "ignored", "value2" );
    fmt . addNameValue( "ignored", "value3" );
    fmt . addNameValue( "ignored", 100 );
    t_str s{ "tvalue", 6, false };
    fmt . addNameValue( "ignored", s );
    ASSERT_EQ( "value1 value2 value3 100 tvalue", fmt.format() );
    ASSERT_EQ( "", fmt.format() );
}

TEST( ReverseFormatterTest, FormatStrings_comma_separated )
{
    ReverseFormatter fmt( ',' );
    fmt . addNameValue( "ignored", "value1" );
    fmt . addNameValue( "ignored", "value2" );
    fmt . addNameValue( "ignored", "value3" );
    fmt . addNameValue( "ignored", 100 );
    t_str s{ "tvalue", 6, false };
    fmt . addNameValue( "ignored", s );
    ASSERT_EQ( "value1,value2,value3,100,tvalue", fmt.format() );
    ASSERT_EQ( "", fmt.format() );
}

TEST( ReverseFormatterTest, FormatNulltstr )
{
    ReverseFormatter fmt;
    fmt . addNameValue( "ignored", "value1" );
    t_str s = { nullptr, 0, false };
    fmt . addNameValue( "ignored", s );
    ASSERT_EQ( "value1", fmt.format() );
}
