#include <gtest/gtest.h>

#include "OP_Interface.cpp"

using namespace std;
using namespace NCBI::Logging;

TEST( OPReverseBlockTest, Create )
{
    OPReverseBlockFactory factory;
    auto pb = factory . MakeParseBlock();
    ASSERT_TRUE( pb );
}

TEST( OPReverseBlockTest, ReverseEmptyLine )
{
    // if an empty lines is given ( is in the json-source-file ) we want it to be in the unrecog-file
    OPReverseBlockFactory factory;
    auto pb = factory . MakeParseBlock();
    string line( "" );
    bool res = pb -> format_specific_parse( line.c_str(), line . size() );
    ASSERT_FALSE( res );
    ASSERT_EQ( ReceiverInterface::cat_ugly, pb->GetReceiver().GetCategory() );
}

TEST( OPReverseBlockTest, ReverseEmptyJson )
{
    // if an empty json is given ( is in the json-source-file ) we want it to be in the unrecog-file
    OPReverseBlockFactory factory;
    auto pb = factory . MakeParseBlock();
    string line( "{}" );
    bool res = pb -> format_specific_parse( line.c_str(), line . size() );
    ASSERT_FALSE( res );
}

TEST( OPReverseBlockTest, ReverseBadJson )
{
    // if an empty json is given ( is in the json-source-file ) we want it to be in the unrecog-file
    OPReverseBlockFactory factory;
    auto pb = factory . MakeParseBlock();
    string line( "{" );
    bool res = pb -> format_specific_parse( line.c_str(), line . size() );
    ASSERT_FALSE( res );
}

class OPReverseTestFixture : public ::testing::Test
{
public:
    string Parse( const string & line )
    {
        OPReverseBlockFactory factory;
        auto pb = factory . MakeParseBlock();
        if ( ! pb -> format_specific_parse( line.c_str(), line . size() ) )
        {
            return "OPReverseTestFixture: format_specific_parse() failed";
        }
        return pb -> GetReceiver() . GetFormatter() . format();
    }
};

TEST_F( OPReverseTestFixture, ReverseGoodJson )
{
    string line( "{\"accession\":\"SRR170543\",\"agent\":\"Mozilla/5.0\",\"extension\":\"\",\"filename\":\"SRR170543\",\"forwarded\":\"forwarded\",\"ip\":\"192.12.184.6\",\"method\":\"GET\",\"path\":\"/traces/sra6/SRR/000166/SRR170543\",\"port\":\"443\",\"referer\":\"referer\",\"req_len\":\"136\",\"req_time\":\"0.427\",\"res_code\":\"200\",\"res_len\":\"10262285\",\"server\":\"sra-download.ncbi.nlm.nih.gov\",\"time\":\"[27/Aug/2018:00:40:03 -0400]\",\"user\":\"user\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"HTTP/1.1\"}" );
    ASSERT_EQ( "192.12.184.6 - user [27/Aug/2018:00:40:03 -0400] sra-download.ncbi.nlm.nih.gov \"GET /traces/sra6/SRR/000166/SRR170543 HTTP/1.1\" 200 10262285 0.427 \"referer\" \"Mozilla/5.0\" \"forwarded\" port=443 rl=136", Parse(line) );
}

TEST_F( OPReverseTestFixture, ReverseOmitPath )
{
    string line( "{\"accession\":\"SRR170543\",\"agent\":\"Mozilla/5.0\",\"extension\":\"\",\"filename\":\"SRR170543\",\"forwarded\":\"forwarded\",\"ip\":\"192.12.184.6\",\"method\":\"GET\",\"path\":\"\",\"port\":\"443\",\"referer\":\"referer\",\"req_len\":\"136\",\"req_time\":\"0.427\",\"res_code\":\"200\",\"res_len\":\"10262285\",\"server\":\"sra-download.ncbi.nlm.nih.gov\",\"time\":\"[27/Aug/2018:00:40:03 -0400]\",\"user\":\"user\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"HTTP/1.1\"}" );
    ASSERT_EQ( "192.12.184.6 - user [27/Aug/2018:00:40:03 -0400] sra-download.ncbi.nlm.nih.gov \"GET HTTP/1.1\" 200 10262285 0.427 \"referer\" \"Mozilla/5.0\" \"forwarded\" port=443 rl=136", Parse(line) );
}

TEST_F( OPReverseTestFixture, ReverseOmitVersion )
{
    string line( "{\"accession\":\"SRR170543\",\"agent\":\"Mozilla/5.0\",\"extension\":\"\",\"filename\":\"SRR170543\",\"forwarded\":\"forwarded\",\"ip\":\"192.12.184.6\",\"method\":\"GET\",\"path\":\"/traces/sra6/SRR/000166/SRR170543\",\"port\":\"443\",\"referer\":\"referer\",\"req_len\":\"136\",\"req_time\":\"0.427\",\"res_code\":\"200\",\"res_len\":\"10262285\",\"server\":\"sra-download.ncbi.nlm.nih.gov\",\"time\":\"[27/Aug/2018:00:40:03 -0400]\",\"user\":\"user\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"\"}" );
    ASSERT_EQ( "192.12.184.6 - user [27/Aug/2018:00:40:03 -0400] sra-download.ncbi.nlm.nih.gov \"GET /traces/sra6/SRR/000166/SRR170543\" 200 10262285 0.427 \"referer\" \"Mozilla/5.0\" \"forwarded\" port=443 rl=136", Parse(line) );
}

TEST_F( OPReverseTestFixture, ReverseMethodOnly )
{
    string line( "{\"accession\":\"SRR170543\",\"agent\":\"Mozilla/5.0\",\"extension\":\"\",\"filename\":\"SRR170543\",\"forwarded\":\"forwarded\",\"ip\":\"192.12.184.6\",\"method\":\"GET\",\"path\":\"\",\"port\":\"443\",\"referer\":\"referer\",\"req_len\":\"136\",\"req_time\":\"0.427\",\"res_code\":\"200\",\"res_len\":\"10262285\",\"server\":\"sra-download.ncbi.nlm.nih.gov\",\"time\":\"[27/Aug/2018:00:40:03 -0400]\",\"user\":\"user\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"\"}" );
    ASSERT_EQ( "192.12.184.6 - user [27/Aug/2018:00:40:03 -0400] sra-download.ncbi.nlm.nih.gov \"GET\" 200 10262285 0.427 \"referer\" \"Mozilla/5.0\" \"forwarded\" port=443 rl=136", Parse(line) );
}

TEST_F( OPReverseTestFixture, ReverseNoServer )
{
    string line( "{\"accession\":\"SRR170543\",\"agent\":\"Mozilla/5.0\",\"extension\":\"\",\"filename\":\"SRR170543\",\"forwarded\":\"forwarded\",\"ip\":\"192.12.184.6\",\"method\":\"GET\",\"path\":\"/traces/sra6/SRR/000166/SRR170543\",\"port\":\"443\",\"referer\":\"referer\",\"req_len\":\"136\",\"req_time\":\"0.427\",\"res_code\":\"200\",\"res_len\":\"10262285\",\"server\":\"\",\"time\":\"[27/Aug/2018:00:40:03 -0400]\",\"user\":\"user\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"HTTP/1.1\"}" );
    ASSERT_EQ( "192.12.184.6 - user [27/Aug/2018:00:40:03 -0400] \"GET /traces/sra6/SRR/000166/SRR170543 HTTP/1.1\" 200 10262285 0.427 \"referer\" \"Mozilla/5.0\" \"forwarded\" port=443 rl=136", Parse(line) );
}

TEST_F( OPReverseTestFixture, ReverseEmptyUserIsDash )
{
    string line( "{\"accession\":\"SRR170543\",\"agent\":\"Mozilla/5.0\",\"extension\":\"\",\"filename\":\"SRR170543\",\"forwarded\":\"forwarded\",\"ip\":\"192.12.184.6\",\"method\":\"GET\",\"path\":\"/traces/sra6/SRR/000166/SRR170543\",\"port\":\"443\",\"referer\":\"referer\",\"req_len\":\"136\",\"req_time\":\"0.427\",\"res_code\":\"200\",\"res_len\":\"10262285\",\"server\":\"sra-download.ncbi.nlm.nih.gov\",\"time\":\"[27/Aug/2018:00:40:03 -0400]\",\"user\":\"\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"HTTP/1.1\"}" );
    ASSERT_EQ( "192.12.184.6 - - [27/Aug/2018:00:40:03 -0400] sra-download.ncbi.nlm.nih.gov \"GET /traces/sra6/SRR/000166/SRR170543 HTTP/1.1\" 200 10262285 0.427 \"referer\" \"Mozilla/5.0\" \"forwarded\" port=443 rl=136", Parse(line) );
}

TEST_F( OPReverseTestFixture, ReverseEmptyReslenIsDash )
{
    string line( "{\"accession\":\"SRR170543\",\"agent\":\"Mozilla/5.0\",\"extension\":\"\",\"filename\":\"SRR170543\",\"forwarded\":\"forwarded\",\"ip\":\"192.12.184.6\",\"method\":\"GET\",\"path\":\"/traces/sra6/SRR/000166/SRR170543\",\"port\":\"443\",\"referer\":\"referer\",\"req_len\":\"136\",\"req_time\":\"0.427\",\"res_code\":\"200\",\"res_len\":\"\",\"server\":\"sra-download.ncbi.nlm.nih.gov\",\"time\":\"[27/Aug/2018:00:40:03 -0400]\",\"user\":\"user\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"HTTP/1.1\"}" );
    ASSERT_EQ( "192.12.184.6 - user [27/Aug/2018:00:40:03 -0400] sra-download.ncbi.nlm.nih.gov \"GET /traces/sra6/SRR/000166/SRR170543 HTTP/1.1\" 200 - 0.427 \"referer\" \"Mozilla/5.0\" \"forwarded\" port=443 rl=136", Parse(line) );
}

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}
