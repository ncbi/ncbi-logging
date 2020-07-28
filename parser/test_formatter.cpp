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
    stringstream s;
};

TEST_F( JsonLibFormatter_Fixture, FormatEmpty )
{
    f.format(s);
    ASSERT_EQ("{}", s.str());
}

TEST_F( JsonLibFormatter_Fixture, Format_t_str )
{
    const t_str str = { "v", 1, false };
    f.addNameValue("a", str);
    f.format(s);
    ASSERT_EQ("{\"a\":\"v\"}", s.str());

    // make sure the data was deleted
    stringstream s1;
    f.format(s1);
    ASSERT_EQ("{}", s1.str());
}

TEST_F( JsonLibFormatter_Fixture, Format_t_str_quote )
{
    const t_str str = { "v\"", 2, true };
    f.addNameValue("a", str);
    f.format(s);
    ASSERT_EQ("{\"a\":\"v\\\"\"}", s.str());
}

TEST_F( JsonLibFormatter_Fixture, Format_int )
{
    f.addNameValue("a", 12345);
    f.format(s);
    ASSERT_EQ("{\"a\":12345}", s.str());
}

TEST_F( JsonLibFormatter_Fixture, Format_neg_int )
{
    f.addNameValue("a", -12345);
    f.format(s);
    ASSERT_EQ("{\"a\":-12345}", s.str());
}

