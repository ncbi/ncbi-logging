#include <gtest/gtest.h>

#include "CL_Interface.hpp"
#include "Formatters.hpp"
#include "parse_test_fixture.hpp"

using namespace std;
using namespace NCBI::Logging;

#define INIT_TSTR(t, s)     do { (t).p = s; (t).n = strlen(s); } while (false)

TEST(CLReceiverTest, Create)
{
    CLReceiver e ( make_shared<JsonLibFormatter>() );
}

class CLParseTestFixture : public ParseTestFixture< CLParseBlockFactory >
{
};

TEST_F( CLParseTestFixture, lonely_eol )
{
    std::string res = try_to_parse_ugly( "\n" );
    ASSERT_EQ( "{\"_line_nr\":1,\"_unparsed\":\"\"}\n", res );
}

TEST_F( CLParseTestFixture, line )
{
    std::string res = try_to_parse_good(
"Oct 20 15:27:38 cloudian-node-120.be-md.ncbi.nlm.nih.gov 1 2020-10-20T15:27:37.592-04:00 cloudian-node-120.be-md.ncbi.nlm.nih.gov S3REQ 4738 - [mdc@18060 REQID=\"45200f16-ad2e-1945-87cb-d8c49756ebf4\"] 2020-10-20 15:27:36,999|10.10.26.17|trace|getObject|sra-pub-run-5||252|0|256|20971520|20972028|593542|ERR527068%2Ffile.1|206|45200f16-ad2e-1945-87cb-d8c49756ebf4|c89056a1918b4ea6534c912deee13f11-154|0|" );

    ASSERT_EQ( "Oct 20 15:27:38 cloudian-node-120.be-md.ncbi.nlm.nih.gov 1 2020-10-20T15:27:37.592-04:00 cloudian-node-120.be-md.ncbi.nlm.nih.gov S3REQ 4738 - [mdc@18060 REQID=\"45200f16-ad2e-1945-87cb-d8c49756ebf4\"]",   extract_value( res, "syslog_prefix" ) );
    ASSERT_EQ( "2020-10-20 15:27:36,999",   extract_value( res, "timestamp" ) );
    ASSERT_EQ( "10.10.26.17",               extract_value( res, "ip" ) );
    ASSERT_EQ( "trace",                     extract_value( res, "owner" ) );
    ASSERT_EQ( "getObject",                 extract_value( res, "method" ) );
    ASSERT_EQ( "sra-pub-run-5",             extract_value( res, "bucket" ) );
    ASSERT_EQ( "",                          extract_value( res, "unknown1" ) );
    ASSERT_EQ( "252",                       extract_value( res, "requestHdrSize" ) );
    ASSERT_EQ( "0",                         extract_value( res, "requestBodySize" ) );
    ASSERT_EQ( "256",                       extract_value( res, "responseHdrSize" ) );
    ASSERT_EQ( "20971520",                  extract_value( res, "responseBodySize" ) );
    ASSERT_EQ( "20972028",                  extract_value( res, "totalSize" ) );
    ASSERT_EQ( "593542",                    extract_value( res, "unknown2" ) );
    ASSERT_EQ( "ERR527068%2Ffile.1",        extract_value( res, "path" ) );
    ASSERT_EQ( "206",                       extract_value( res, "httpStatus" ) );
    ASSERT_EQ( "45200f16-ad2e-1945-87cb-d8c49756ebf4",
                                            extract_value( res, "reqId" ) );
    ASSERT_EQ( "c89056a1918b4ea6534c912deee13f11-154",
                                            extract_value( res, "unknown3" ) );
    ASSERT_EQ( "0",                         extract_value( res, "eTag" ) );
    ASSERT_EQ( "",                          extract_value( res, "errorCode" ) );

    // verify URL post-processing
    ASSERT_EQ( "ERR527068", extract_value( res, "accession" ) );
    ASSERT_EQ( "file",      extract_value( res, "filename" ) );
    ASSERT_EQ( ".1",        extract_value( res, "extension" ) );
}

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}