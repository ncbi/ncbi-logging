#include <gtest/gtest.h>

#include "AWS_Interface.cpp"

using namespace std;
using namespace NCBI::Logging;

TEST( AWSReverseBlockTest, Create )
{
    AWSReverseBlockFactory factory;
    auto pb = factory . MakeParseBlock();
    ASSERT_TRUE( pb );
}

TEST( AWSReverseBlockTest, ReverseEmptyLine )
{
    // if an empty lines is given ( is in the json-source-file ) we want it to be in the unrecog-file
    AWSReverseBlockFactory factory;
    auto pb = factory . MakeParseBlock();
    string line( "" );
    bool res = pb -> format_specific_parse( line.c_str(), line . size() );
    ASSERT_FALSE( res );
    ASSERT_EQ( ReceiverInterface::cat_ugly, pb->GetReceiver().GetCategory() );
}

TEST( AWSReverseBlockTest, ReverseEmptyJson )
{
    // if an empty json is given ( is in the json-source-file ) we want it to be in the unrecog-file
    AWSReverseBlockFactory factory;
    auto pb = factory . MakeParseBlock();
    string line( "{}" );
    bool res = pb -> format_specific_parse( line.c_str(), line . size() );
    ASSERT_FALSE( res );
}

TEST( AWSReverseBlockTest, ReverseBadJson )
{
    // if an empty json is given ( is in the json-source-file ) we want it to be in the unrecog-file
    AWSReverseBlockFactory factory;
    auto pb = factory . MakeParseBlock();
    string line( "{" );
    bool res = pb -> format_specific_parse( line.c_str(), line . size() );
    ASSERT_FALSE( res );
}

class AWSReverseTestFixture : public ::testing::Test
{
public:
    string Parse( const string & line )
    {
        AWSReverseBlockFactory factory;
        auto pb = factory . MakeParseBlock();
        if ( ! pb -> format_specific_parse( line.c_str(), line . size() ) )
        {
            return "AWSReverseTestFixture: format_specific_parse() failed";
        }
        return pb -> GetReceiver() . GetFormatter() . format();
    }
};

TEST_F( AWSReverseTestFixture, ReverseGoodJson )
{
    string line( "{\"accepted\":\"true\",\"accession\":\"SRR10868496\",\"agent\":\"aws-cli/1.18.13 Python/2.7.18 Linux/4.14.186-110.268.amzn1.x86_64 botocore/1.15.13\",\"auth_type\":\"AuthHeader\",\"bucket\":\"sra-pub-run-2\",\"cipher_suite\":\"ECDHE-RSA-AES128-GCM-SHA256\",\"error\":\"\",\"extension\":\".1\",\"filename\":\"SRR10868496\",\"host_header\":\"sra-pub-run-2.s3.amazonaws.com\",\"host_id\":\"t8FlcJOJAG3BrX2fV+sKmIEJORnHX3sEx/vrIOT6TBGVn/Hj+ZyjP3y0jhr10w6kwfF6J4A02Ec=\",\"ip\":\"3.236.215.75\",\"key\":\"SRR10868496/SRR10868496.1\",\"method\":\"PUT\",\"obj_size\":\"8388608\",\"operation\":\"REST.PUT.PART\",\"owner\":\"922194806485875312b252374a3644f1feecd16802a50d4729885c1d11e1fd37\",\"path\":\"/SRR10868496/SRR10868496.1?partNumber=5312&uploadId=eCBqLXk7jmcJG1FqaYKg5saIW0au0xZ3deTnVUDp8Xg4j981rHK6X9HUNrsntwo.F9szQ_KY1Nbjykzj45NP5rhbId5W.wyWxp_JV5Wok6lrs1KmDAjwIcTokTU01Rb7lNSxiAYxjqgtDRo9vaD1QA--\",\"referer\":\"\",\"request_id\":\"CA5DAE26B8645A60\",\"requester\":\"arn:aws:sts::783971887864:assumed-role/sra-developer-instance-profile-role/i-020cde5f07895bac2\",\"res_code\":\"200\",\"res_len\":\"\",\"sig_ver\":\"SigV4\",\"source\":\"S3\",\"time\":\"[02/Sep/2020:06:00:57 +0000]\",\"tls_version\":\"TLSv1.2\",\"total_time\":\"940\",\"turnaround_time\":\"105\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"HTTP/1.1\",\"version_id\":\"\"}" );
    ASSERT_EQ( "922194806485875312b252374a3644f1feecd16802a50d4729885c1d11e1fd37 sra-pub-run-2 [02/Sep/2020:06:00:57 +0000] 3.236.215.75 arn:aws:sts::783971887864:assumed-role/sra-developer-instance-profile-role/i-020cde5f07895bac2 CA5DAE26B8645A60 REST.PUT.PART SRR10868496/SRR10868496.1 \"PUT /SRR10868496/SRR10868496.1?partNumber=5312&uploadId=eCBqLXk7jmcJG1FqaYKg5saIW0au0xZ3deTnVUDp8Xg4j981rHK6X9HUNrsntwo.F9szQ_KY1Nbjykzj45NP5rhbId5W.wyWxp_JV5Wok6lrs1KmDAjwIcTokTU01Rb7lNSxiAYxjqgtDRo9vaD1QA-- HTTP/1.1\" 200 - - 8388608 940 105 - \"aws-cli/1.18.13 Python/2.7.18 Linux/4.14.186-110.268.amzn1.x86_64 botocore/1.15.13\" - t8FlcJOJAG3BrX2fV+sKmIEJORnHX3sEx/vrIOT6TBGVn/Hj+ZyjP3y0jhr10w6kwfF6J4A02Ec= SigV4 ECDHE-RSA-AES128-GCM-SHA256 AuthHeader sra-pub-run-2.s3.amazonaws.com TLSv1.2", Parse(line) );
}

TEST_F( AWSReverseTestFixture, ReverseAllEmpty )
{
    string line( "{\"accepted\":\"\",\"accession\":\"\",\"agent\":\"\",\"auth_type\":\"\",\"bucket\":\"\",\"cipher_suite\":\"\",\"error\":\"\",\"extension\":\"\",\"filename\":\"\",\"host_header\":\"\",\"host_id\":\"\",\"ip\":\"\",\"key\":\"\",\"method\":\"\",\"obj_size\":\"\",\"operation\":\"\",\"owner\":\"\",\"path\":\"\",\"referer\":\"\",\"request_id\":\"\",\"requester\":\"\",\"res_code\":\"\",\"res_len\":\"\",\"sig_ver\":\"\",\"source\":\"\",\"time\":\"\",\"tls_version\":\"\",\"total_time\":\"\",\"turnaround_time\":\"\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"\",\"version_id\":\"\"}" );
    ASSERT_EQ( "- - - - - - - - - - - - - - - - - - - - - - - -", Parse(line) );
}

TEST_F( AWSReverseTestFixture, ReverseOmitMethod )
{
    string line( "{\"accepted\":\"\",\"accession\":\"\",\"agent\":\"\",\"auth_type\":\"\",\"bucket\":\"\",\"cipher_suite\":\"\",\"error\":\"\",\"extension\":\"\",\"filename\":\"\",\"host_header\":\"\",\"host_id\":\"\",\"ip\":\"\",\"key\":\"\",\"method\":\"\",\"obj_size\":\"\",\"operation\":\"\",\"owner\":\"\",\"path\":\"path\",\"referer\":\"\",\"request_id\":\"\",\"requester\":\"\",\"res_code\":\"\",\"res_len\":\"\",\"sig_ver\":\"\",\"source\":\"\",\"time\":\"\",\"tls_version\":\"\",\"total_time\":\"\",\"turnaround_time\":\"\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"vers\",\"version_id\":\"\"}" );
    ASSERT_EQ( "- - - - - - - - \"path vers\" - - - - - - - - - - - - - - -", Parse(line) );
}

TEST_F( AWSReverseTestFixture, ReverseOmitPath )
{
    string line( "{\"accepted\":\"\",\"accession\":\"\",\"agent\":\"\",\"auth_type\":\"\",\"bucket\":\"\",\"cipher_suite\":\"\",\"error\":\"\",\"extension\":\"\",\"filename\":\"\",\"host_header\":\"\",\"host_id\":\"\",\"ip\":\"\",\"key\":\"\",\"method\":\"method\",\"obj_size\":\"\",\"operation\":\"\",\"owner\":\"\",\"path\":\"\",\"referer\":\"\",\"request_id\":\"\",\"requester\":\"\",\"res_code\":\"\",\"res_len\":\"\",\"sig_ver\":\"\",\"source\":\"\",\"time\":\"\",\"tls_version\":\"\",\"total_time\":\"\",\"turnaround_time\":\"\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"vers\",\"version_id\":\"\"}" );
    ASSERT_EQ( "- - - - - - - - \"method vers\" - - - - - - - - - - - - - - -", Parse(line) );
}

TEST_F( AWSReverseTestFixture, ReverseOmitVersion )
{
    string line( "{\"accepted\":\"\",\"accession\":\"\",\"agent\":\"\",\"auth_type\":\"\",\"bucket\":\"\",\"cipher_suite\":\"\",\"error\":\"\",\"extension\":\"\",\"filename\":\"\",\"host_header\":\"\",\"host_id\":\"\",\"ip\":\"\",\"key\":\"\",\"method\":\"method\",\"obj_size\":\"\",\"operation\":\"\",\"owner\":\"\",\"path\":\"path\",\"referer\":\"\",\"request_id\":\"\",\"requester\":\"\",\"res_code\":\"\",\"res_len\":\"\",\"sig_ver\":\"\",\"source\":\"\",\"time\":\"\",\"tls_version\":\"\",\"total_time\":\"\",\"turnaround_time\":\"\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"\",\"version_id\":\"\"}" );
    ASSERT_EQ( "- - - - - - - - \"method path\" - - - - - - - - - - - - - - -", Parse(line) );
}

TEST_F( AWSReverseTestFixture, ReverseMethodOnly )
{
    string line( "{\"accepted\":\"\",\"accession\":\"\",\"agent\":\"\",\"auth_type\":\"\",\"bucket\":\"\",\"cipher_suite\":\"\",\"error\":\"\",\"extension\":\"\",\"filename\":\"\",\"host_header\":\"\",\"host_id\":\"\",\"ip\":\"\",\"key\":\"\",\"method\":\"method\",\"obj_size\":\"\",\"operation\":\"\",\"owner\":\"\",\"path\":\"\",\"referer\":\"\",\"request_id\":\"\",\"requester\":\"\",\"res_code\":\"\",\"res_len\":\"\",\"sig_ver\":\"\",\"source\":\"\",\"time\":\"\",\"tls_version\":\"\",\"total_time\":\"\",\"turnaround_time\":\"\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"\",\"version_id\":\"\"}" );
    ASSERT_EQ( "- - - - - - - - \"method\" - - - - - - - - - - - - - - -", Parse(line) );
}
TEST_F( AWSReverseTestFixture, ReversePathOnly )
{
    string line( "{\"accepted\":\"\",\"accession\":\"\",\"agent\":\"\",\"auth_type\":\"\",\"bucket\":\"\",\"cipher_suite\":\"\",\"error\":\"\",\"extension\":\"\",\"filename\":\"\",\"host_header\":\"\",\"host_id\":\"\",\"ip\":\"\",\"key\":\"\",\"method\":\"\",\"obj_size\":\"\",\"operation\":\"\",\"owner\":\"\",\"path\":\"path\",\"referer\":\"\",\"request_id\":\"\",\"requester\":\"\",\"res_code\":\"\",\"res_len\":\"\",\"sig_ver\":\"\",\"source\":\"\",\"time\":\"\",\"tls_version\":\"\",\"total_time\":\"\",\"turnaround_time\":\"\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"\",\"version_id\":\"\"}" );
    ASSERT_EQ( "- - - - - - - - \"path\" - - - - - - - - - - - - - - -", Parse(line) );
}
TEST_F( AWSReverseTestFixture, ReverseVersOnly )
{
    string line( "{\"accepted\":\"\",\"accession\":\"\",\"agent\":\"\",\"auth_type\":\"\",\"bucket\":\"\",\"cipher_suite\":\"\",\"error\":\"\",\"extension\":\"\",\"filename\":\"\",\"host_header\":\"\",\"host_id\":\"\",\"ip\":\"\",\"key\":\"\",\"method\":\"\",\"obj_size\":\"\",\"operation\":\"\",\"owner\":\"\",\"path\":\"\",\"referer\":\"\",\"request_id\":\"\",\"requester\":\"\",\"res_code\":\"\",\"res_len\":\"\",\"sig_ver\":\"\",\"source\":\"\",\"time\":\"\",\"tls_version\":\"\",\"total_time\":\"\",\"turnaround_time\":\"\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"vers\",\"version_id\":\"\"}" );
    ASSERT_EQ( "- - - - - - - - \"vers\" - - - - - - - - - - - - - - -", Parse(line) );
}

TEST_F( AWSReverseTestFixture, Reverse2PartHostId )
{
    string line( "{\"accepted\":\"\",\"accession\":\"\",\"agent\":\"\",\"auth_type\":\"\",\"bucket\":\"\",\"cipher_suite\":\"\",\"error\":\"\",\"extension\":\"\",\"filename\":\"\",\"host_header\":\"\",\"host_id\":\"AIDAISBTTLPGXGH6YFFAY LzYGhqEwXn5Xiuil9tI6JtK2PiIo+SC6Ute3Isq2qEmt/t0Z7qFkyD0mp1ZIc43bm0qSX4tBbbc=\",\"ip\":\"\",\"key\":\"\",\"method\":\"\",\"obj_size\":\"\",\"operation\":\"\",\"owner\":\"\",\"path\":\"\",\"referer\":\"\",\"request_id\":\"\",\"requester\":\"\",\"res_code\":\"\",\"res_len\":\"\",\"sig_ver\":\"\",\"source\":\"\",\"time\":\"\",\"tls_version\":\"\",\"total_time\":\"\",\"turnaround_time\":\"\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"\",\"version_id\":\"\"}" );
    ASSERT_EQ( "- - - - - - - - - - - - - - - - - - AIDAISBTTLPGXGH6YFFAY LzYGhqEwXn5Xiuil9tI6JtK2PiIo+SC6Ute3Isq2qEmt/t0Z7qFkyD0mp1ZIc43bm0qSX4tBbbc= - - - - -", Parse(line) );
}

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}
