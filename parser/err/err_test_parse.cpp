#include <gtest/gtest.h>

#include <sstream>
#include <vector>

#include "ERR_Interface.hpp"
#include "Formatters.hpp"
#include "CatWriters.hpp"
#include "parse_test_fixture.hpp"

using namespace std;
using namespace NCBI::Logging;

#define INIT_TSTR(t, s)     do { (t).p = s; (t).n = strlen(s); } while (false)

TEST( LogERREventTest, Create )
{
    ERRReceiver e ( make_shared<JsonLibFormatter>() );
}

class ERRParseTestFixture : public ParseTestFixture< ERRParseBlockFactory >
{
};

TEST_F( ERRParseTestFixture, lonely_eol )
{
    std::string res = try_to_parse_ugly( "\n" );
    ASSERT_EQ( "{\"_line_nr\":1,\"_unparsed\":\"\"}\n", res );
}

TEST_F( ERRParseTestFixture, line )
{
    std::string res = try_to_parse_good( "2020/10/01 20:15:27 [alert] 61030#0: *650699 open() \"/netmnt/traces04//sra75/SRR/008255/SRR8453128\" failed (2: No such file or directory), client: 218.219.98.126, server: srafiles21.be-md.ncbi.nlm.nih.gov, request: \"GET /traces/sra75/SRR/008255/SRR8453128 HTTP/1.1\", host: \"sra-downloadb.be-md.ncbi.nlm.nih.gov\"" );

    ASSERT_EQ( "2020/10/01 20:15:27", extract_value( res, "datetime" ) );
    ASSERT_EQ( "[alert]", extract_value( res, "severity" ) );
    ASSERT_EQ( "61030#0:", extract_value( res, "pid" ) );
    ASSERT_EQ( "*650699 open() \"/netmnt/traces04//sra75/SRR/008255/SRR8453128\" failed (2: No such file or directory), client: 218.219.98.126, server: srafiles21.be-md.ncbi.nlm.nih.gov, request: \"GET /traces/sra75/SRR/008255/SRR8453128 HTTP/1.1\", host: \"sra-downloadb.be-md.ncbi.nlm.nih.gov\"", extract_value( res, "msg" ) );
}

TEST_F( ERRParseTestFixture, file_meta )
{
    const char * input = "file-meta ncbi_location=be-md&ncbi_role=production&applog_db_rotate_host=srafiles21";
    std::string res = try_to_parse_ignored( input );
    ASSERT_EQ( res, "{\"_line_nr\":1,\"_unparsed\":\"file-meta ncbi_location=be-md&ncbi_role=production&applog_db_rotate_host=srafiles21\"}\n" );
}

TEST_F( ERRParseTestFixture, not_file_meta )
{
    const char * input = "something ncbi_location=be-md&ncbi_role=production&applog_db_rotate_host=srafiles21";
    std::string res = try_to_parse_review( input );
    ASSERT_EQ( res, "{\"_line_nr\":1,\"_unparsed\":\"something ncbi_location=be-md&ncbi_role=production&applog_db_rotate_host=srafiles21\"}\n" );
}

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}
