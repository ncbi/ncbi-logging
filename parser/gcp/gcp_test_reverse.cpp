#include <gtest/gtest.h>

#include "GCP_Interface.cpp"

using namespace std;
using namespace NCBI::Logging;

TEST( GCPReverseBlockTest, Create )
{
    GCPReverseBlockFactory factory;
    auto pb = factory . MakeParseBlock();
    ASSERT_TRUE( pb );
}

TEST( GCPReverseBlockTest, ReverseEmptyLine )
{
    // if an empty lines is given ( is in the json-source-file ) we want it to be in the unrecog-file
    GCPReverseBlockFactory factory;
    auto pb = factory . MakeParseBlock();
    string line( "" );
    bool res = pb -> format_specific_parse( line.c_str(), line . size() );
    ASSERT_FALSE( res );
    ASSERT_EQ( ReceiverInterface::cat_ugly, pb->GetReceiver().GetCategory() );
}

TEST( GCPReverseBlockTest, ReverseEmptyJson )
{
    // if an empty json is given ( is in the json-source-file ) we want it to be in the unrecog-file
    GCPReverseBlockFactory factory;
    auto pb = factory . MakeParseBlock();
    string line( "{}" );
    bool res = pb -> format_specific_parse( line.c_str(), line . size() );
    ASSERT_FALSE( res );
}

TEST( GCPReverseBlockTest, ReverseBadJson )
{
    // if an empty json is given ( is in the json-source-file ) we want it to be in the unrecog-file
    GCPReverseBlockFactory factory;
    auto pb = factory . MakeParseBlock();
    string line( "{" );
    bool res = pb -> format_specific_parse( line.c_str(), line . size() );
    ASSERT_FALSE( res );
}

class GCPReverseTestFixture : public ::testing::Test
{
public:
    string Parse( const string & line )
    {
        GCPReverseBlockFactory factory;
        auto pb = factory . MakeParseBlock();
        if ( ! pb -> format_specific_parse( line.c_str(), line . size() ) )
        {
            return "GCPReverseTestFixture: format_specific_parse() failed";
        }
        return pb -> GetReceiver() . GetFormatter() . format();
    }
};

TEST_F( GCPReverseTestFixture, ReverseGoodJson )
{
    string line( "{\"accepted\":\"true\",\"accession\":\"\",\"agent\":\"apitools gsutil/4.46 Python/2.7.5 (linux2) google-cloud-sdk/274.0.1 analytics/disabled,gzip(gfe)\",\"bucket\":\"sra-pub-run-1\",\"extension\":\"\",\"filename\":\"\",\"host\":\"storage.googleapis.com\",\"ip\":\"130.14.28.11\",\"ip_region\":\"region\",\"ip_type\":\"1\",\"method\":\"GET\",\"operation\":\"op\",\"path\":\"/storage/v1/b/sra-pub-run-1/o?pageToken=ChdTUlIxMTY5NDQ2L1NSUjExNjk0NDYuMQ%3D%3D&projection=noAcl&versions=False&fields=prefixes%2CnextPageToken%2Citems%2Fname&alt=json&userProject=nih-sra-datastore&maxResults=1000\",\"referer\":\"\",\"request_bytes\":\"0\",\"request_id\":\"AAANsUmuA_tXwMB2S3BS37sM-cw4Y-FPg301kI1gXCWgwGLx1ucGjI6W5a7UrGOEn2qETbxYRkndQ5tRSH15r-zED3o\",\"result_bytes\":\"53340\",\"source\":\"GS\",\"status\":\"200\",\"time\":\"1595830441696156\",\"time_taken\":\"50000\",\"uri\":\"/storage/v1/b/sra-pub-run-1/o?pageToken=ChdTUlIxMTY5NDQ2L1NSUjExNjk0NDYuMQ%3D%3D&projection=noAcl&versions=False&fields=prefixes%2CnextPageToken%2Citems%2Fname&alt=json&userProject=nih-sra-datastore&maxResults=1000\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"\"}" );

    ASSERT_EQ( "\"1595830441696156\",\"130.14.28.11\",\"1\",\"region\",\"GET\",\"/storage/v1/b/sra-pub-run-1/o?pageToken=ChdTUlIxMTY5NDQ2L1NSUjExNjk0NDYuMQ%3D%3D&projection=noAcl&versions=False&fields=prefixes%2CnextPageToken%2Citems%2Fname&alt=json&userProject=nih-sra-datastore&maxResults=1000\",\"200\",\"0\",\"53340\",\"50000\",\"storage.googleapis.com\",\"\",\"apitools gsutil/4.46 Python/2.7.5 (linux2) google-cloud-sdk/274.0.1 analytics/disabled,gzip(gfe)\",\"AAANsUmuA_tXwMB2S3BS37sM-cw4Y-FPg301kI1gXCWgwGLx1ucGjI6W5a7UrGOEn2qETbxYRkndQ5tRSH15r-zED3o\",\"op\",\"sra-pub-run-1\",\"/storage/v1/b/sra-pub-run-1/o?pageToken=ChdTUlIxMTY5NDQ2L1NSUjExNjk0NDYuMQ%3D%3D&projection=noAcl&versions=False&fields=prefixes%2CnextPageToken%2Citems%2Fname&alt=json&userProject=nih-sra-datastore&maxResults=1000\"", Parse(line) );
}

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}
