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

TEST_F( JsonLibFormatter_Fixture, Format_ctrl_char )
{
    f.addNameValue( "a", "some\007text" );
    ASSERT_EQ( "{\"a\":\"some\\u0007text\"}", f.format() );
}

/* -------------------------------------------------------------- */

class JsonFastFormatter_Fixture : public ::testing::Test
{
public:
    virtual void SetUp() {}
    virtual void TearDown() {}

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

TEST_F( JsonFastFormatter_Fixture, Format_ctrl_char )
{
    f.addNameValue( "a", "some\007text" );
    ASSERT_EQ( "{\"a\":\"some\\u0007text\"}", f.format() );
}
