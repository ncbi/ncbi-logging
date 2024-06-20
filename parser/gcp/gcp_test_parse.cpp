#include <gtest/gtest.h>

#include <sstream>
#include <vector>

#include "GCP_Interface.hpp"
#include "Formatters.hpp"
#include "CatWriters.hpp"
#include "parse_test_fixture.hpp"

using namespace std;
using namespace NCBI::Logging;

#define INIT_TSTR(t, s)     do { (t).p = s; (t).n = strlen(s); } while (false)

TEST(GCPReceiverTest, Create)
{
    GCPReceiver e ( make_shared<JsonLibFormatter>() );
}

TEST(LogGCPEventTest, Setters)
{
    GCPReceiver e ( make_shared<JsonLibFormatter>() );

    t_str v;
    INIT_TSTR( v, "i_p");
    e.set( ReceiverInterface::ip, v );
    INIT_TSTR( v, "ref");
    e.set( ReceiverInterface::referer, v );
    INIT_TSTR( v, "unp");
    e.set( ReceiverInterface::unparsed, v );
    INIT_TSTR( v, "agt");
    e.set( ReceiverInterface::agent, v );

    INIT_TSTR( v, "m" ); e.set( GCPReceiver::method, v );
    INIT_TSTR( v, "p" ); e.set( GCPReceiver::path, v );
    INIT_TSTR( v, "v" ); e.set( GCPReceiver::vers, v );

    INIT_TSTR( v, "tim"); e.set(GCPReceiver::time, v);
    INIT_TSTR( v, "ipt"); e.set(GCPReceiver::ip_type, v);
    INIT_TSTR( v, "ipr"); e.set(GCPReceiver::ip_region, v);
    INIT_TSTR( v, "uri"); e.set(GCPReceiver::uri, v);
    INIT_TSTR( v, "sta"); e.set(GCPReceiver::status, v);
    INIT_TSTR( v, "rqb"); e.set(GCPReceiver::request_bytes, v);
    INIT_TSTR( v, "rsb"); e.set(GCPReceiver::result_bytes, v);
    INIT_TSTR( v, "tt");  e.set(GCPReceiver::time_taken, v);
    INIT_TSTR( v, "h");   e.set(GCPReceiver::host, v);
    INIT_TSTR( v, "rid"); e.set(GCPReceiver::request_id, v);
    INIT_TSTR( v, "ope"); e.set(GCPReceiver::operation, v);
    INIT_TSTR( v, "buc"); e.set(GCPReceiver::bucket, v);

    ASSERT_EQ (
        "{\"agent\":\"agt\",\"bucket\":\"buc\",\"host\":\"h\",\"ip\":\"i_p\",\"ip_region\":\"ipr\",\"ip_type\":\"ipt\",\"method\":\"m\",\"operation\":\"ope\",\"path\":\"p\",\"referer\":\"ref\",\"request_bytes\":\"rqb\",\"request_id\":\"rid\",\"result_bytes\":\"rsb\",\"status\":\"sta\",\"time\":\"tim\",\"time_taken\":\"tt\",\"unparsed\":\"unp\",\"uri\":\"uri\",\"vers\":\"v\"}",
        e.GetFormatter().format() );
}

class GCPTestFixture : public ParseTestFixture< GCPParseBlockFactory >
{
};

TEST_F( GCPTestFixture, Setters_BadUTF8 )
{
    GCPReceiver e ( make_shared<JsonLibFormatter>() );

    t_str v;
    INIT_TSTR( v, "7dd\xFFga" );
    e.set( GCPReceiver::host, v );
    ASSERT_EQ( ReceiverInterface::cat_review, e.GetCategory() );
    string text = e . GetFormatter() . format();
    ASSERT_EQ( "badly formed UTF-8 character in 'host'", extract_value( text, "_error" ) );
    ASSERT_EQ( "7dd\\uffffffffga", extract_value( text, "host" ) );
}

TEST_F( GCPTestFixture, LineRejecting )
{
    std::string res = try_to_parse_ugly( "line1 blah\nline2\nline3\n" );
    ASSERT_EQ(
        "{\"_line_nr\":1,\"_unparsed\":\"line1 blah\"}\n"
        "{\"_line_nr\":2,\"_unparsed\":\"line2\"}\n"
        "{\"_line_nr\":3,\"_unparsed\":\"line3\"}\n", res );
}

TEST_F( GCPTestFixture, ErrorRecovery )
{
    try_to_parse(
        "line1 blah\n"
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"\n", true);
    ASSERT_EQ( "{\"_line_nr\":1,\"_unparsed\":\"line1 blah\"}\n",
                s_outputs.get_ugly() );
    ASSERT_EQ( "{\"accession\":\"\",\"agent\":\"\",\"bucket\":\"\",\"extension\":\"\",\"filename\":\"\",\"host\":\"\",\"ip\":\"\",\"ip_region\":\"\",\"ip_type\":\"\",\"method\":\"GET\",\"operation\":\"\",\"path\":\"\",\"referer\":\"\",\"request_bytes\":\"\",\"request_id\":\"\",\"result_bytes\":\"\",\"status\":\"\",\"time\":\"1\",\"time_taken\":\"\",\"uri\":\"\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"\"}\n",
                s_outputs.get_good() );
}

TEST_F( GCPTestFixture, unrecognized_char )
{
    std::string res = try_to_parse_ugly( "1 \07" );
    ASSERT_EQ( "{\"_line_nr\":1,\"_unparsed\":\"1 \\u0007\"}\n", res );
}

TEST_F( GCPTestFixture, lonely_eol )
{
    std::string res = try_to_parse_ugly( "\n" );
    ASSERT_EQ( "{\"_line_nr\":1,\"_unparsed\":\"\"}\n", res );
}

TEST_F( GCPTestFixture, parse_embedded_0 )
{
    string s("\"1\",\x00\"18.207.254.142\"", 21);
    std::string res = try_to_parse_ugly( s );
    ASSERT_EQ(
        "{\"_line_nr\":1,\"_unparsed\":\"\\\"1\\\",\\u0000\\\"18.207.254.142\\\"\",\"time\":\"1\"}\n",
        res
        );
}

TEST_F( GCPTestFixture, time )
{
    string res = try_to_parse_good(
        "\"123\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "123", extract_value( res, "time" ) );
}

TEST_F( GCPTestFixture, ipv4 )
{
    string res = try_to_parse_good(
        "\"1\",\"18.207.254.142\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "18.207.254.142", extract_value( res, "ip" ) );
}

TEST_F( GCPTestFixture, ipv6 )
{
    string res = try_to_parse_good(
        "\"1\",\"0123:4567:89ab:cdef::1.2.3.4\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "0123:4567:89ab:cdef::1.2.3.4", extract_value( res, "ip" ) );
}

TEST_F( GCPTestFixture, ip_type )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"42\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "42", extract_value( res, "ip_type" ) );
}

TEST_F( GCPTestFixture, ip_region )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"area42\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "area42", extract_value( res, "ip_region" ) );
}

TEST_F( GCPTestFixture, status )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"200\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "200", extract_value( res, "status" ) );
}

TEST_F( GCPTestFixture, req_bytes )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"201\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "201", extract_value( res, "request_bytes" ) );
}

TEST_F( GCPTestFixture, res_bytes )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"5000\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "5000", extract_value( res, "result_bytes" ) );
}

TEST_F( GCPTestFixture, time_taken )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"10\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "10", extract_value( res, "time_taken" ) );
}

TEST_F( GCPTestFixture, host )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"host0\",\"\",\"\",\"\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "host0", extract_value( res, "host" ) );
}

TEST_F( GCPTestFixture, referer )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"referrer\",\"\",\"\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "referrer", extract_value( res, "referer" ) );
}

TEST_F( GCPTestFixture, req_id_int )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"123\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "123", extract_value( res, "request_id" ) );
}

TEST_F( GCPTestFixture, req_id_str )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"abc\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "abc", extract_value( res, "request_id" ) );
}

TEST_F( GCPTestFixture, operation )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"oper\",\"\",\"\"\n");
    ASSERT_EQ( "oper", extract_value( res, "operation" ) );
}

TEST_F( GCPTestFixture, bucket )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"kickme\",\"\"\n");
    ASSERT_EQ( "kickme", extract_value( res, "bucket" ) );
}

TEST_F( GCPTestFixture, method )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "GET", extract_value( res, "method" ) );
}

TEST_F( GCPTestFixture, accession_from_object )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"SRR002994/qwe.2\"\n" );
    ASSERT_EQ( "GET", extract_value( res, "method" ) );
    ASSERT_EQ( "SRR002994", extract_value( res, "accession" ) );
    ASSERT_EQ( "qwe", extract_value( res, "filename" ) );
    ASSERT_EQ( ".2", extract_value( res, "extension" ) );
}

TEST_F( GCPTestFixture, accession_from_object_as_file )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"SRR002994/SRR002994.2\"\n");
    ASSERT_EQ( "SRR002994/SRR002994.2", extract_value( res, "path" ) );
    ASSERT_EQ( "SRR002994", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR002994", extract_value( res, "filename" ) );
    ASSERT_EQ( ".2", extract_value( res, "extension" ) );
}

TEST_F( GCPTestFixture, accession_from_object_no_file )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"SRR002994/.2\"\n");
    ASSERT_EQ( "SRR002994/.2", extract_value( res, "path" ) );
    ASSERT_EQ( "SRR002994", extract_value( res, "accession" ) );
    ASSERT_EQ( "", extract_value( res, "filename" ) );
    ASSERT_EQ( ".2", extract_value( res, "extension" ) );
}

TEST_F( GCPTestFixture, accession_from_object_no_extension )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"SRR002994/2\"\n");
    ASSERT_EQ( "SRR002994/2", extract_value( res, "path" ) );
    ASSERT_EQ( "SRR002994", extract_value( res, "accession" ) );
    ASSERT_EQ( "2", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( GCPTestFixture, accession_from_url )
{   // no accession found in the object, go to URL
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"/storage/SRR002994/qwe.2\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"qwe.2222222\"\n");
    ASSERT_EQ( "GET", extract_value( res, "method" ) );
    ASSERT_EQ( "/storage/SRR002994/qwe.2", extract_value( res, "uri" ) );
    ASSERT_EQ( "qwe.2222222", extract_value( res, "path" ) );
    ASSERT_EQ( "SRR002994", extract_value( res, "accession" ) );
    ASSERT_EQ( "qwe", extract_value( res, "filename" ) );
    ASSERT_EQ( ".2", extract_value( res, "extension" ) );
}

TEST_F( GCPTestFixture, accession_from_url_in_params )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"/storage/v1/b/sra-pub-src-8/o?projection=noAcl&versions=False&fields=prefixes%2CnextPageToken%2Citems%2Fname&userProject=nih-sra-datastore&prefix=SRR1755353%2FMetazome_Annelida_timecourse_sample_0097.fastq.gz&anothrPrefix=SRR77777777&maxResults=1000&delimiter=%2F&alt=json\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"qwe.2222222\"\n");
    ASSERT_EQ( "GET", extract_value( res, "method" ) );
    ASSERT_EQ( "/storage/v1/b/sra-pub-src-8/o?projection=noAcl&versions=False&fields=prefixes%2CnextPageToken%2Citems%2Fname&userProject=nih-sra-datastore&prefix=SRR1755353%2FMetazome_Annelida_timecourse_sample_0097.fastq.gz&anothrPrefix=SRR77777777&maxResults=1000&delimiter=%2F&alt=json", extract_value( res, "uri" ) );
    ASSERT_EQ( "qwe.2222222", extract_value( res, "path" ) );
    ASSERT_EQ( "SRR1755353", extract_value( res, "accession" ) );
    ASSERT_EQ( "Metazome_Annelida_timecourse_sample_0097", extract_value( res, "filename" ) );
    ASSERT_EQ( ".fastq.gz", extract_value( res, "extension" ) );
}

TEST_F( GCPTestFixture, bad_object )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"SRR004257&\"\n" );
    ASSERT_EQ( "SRR004257&", extract_value( res, "path" ) );
    ASSERT_EQ( "", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR004257&", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( GCPTestFixture, header )
{
    string s = try_to_parse_ignored(
        "\"time_micros\",\"c_ip\",\"c_ip_type\",\"c_ip_region\",\"cs_method\",\"cs_uri\",\"sc_status\",\"cs_bytes\",\"sc_bytes\",\"time_taken_micros\",\"cs_host\",\"cs_referer\",\"cs_user_agent\",\"s_request_id\",\"cs_operation\",\"cs_bucket\",\"cs_object\""
    );
    ASSERT_NE( "", s );
}

TEST_F( GCPTestFixture, header_bad )
{
    string s = try_to_parse_review( "\"blah\"" );
    ASSERT_NE( "", s );
}

TEST_F( GCPTestFixture, LOGMON_208 )
{
    string res = try_to_parse_good(
        "\"1650240002834549\",\"34.86.77.8\",\"1\",\"\",\"GET\",\"/sra-pub-run-7/SRR000002/SRR000002.3?X-Goog-Algorithm=GOOG4-RSA-SHA256&X-Goog-Credential=data-access-service%40nih-sra-datastore.iam.gserviceaccount.com%2F20220418%2Fauto%2Fstorage%2Fgoog4_request&X-Goog-Date=20220418T000002Z&X-Goog-Expires=360000&X-Goog-SignedHeaders=host&userProject=nih-sra-datastore&X-Goog-Signature=b2641e3eb68d942584a955284f6314a9b79973128b18fe5037c9166bd252fcd1b51445a814f554ca6c6414ee5577855fde8e8f1fbe44f467ed19d927fe46edd931143bd6f39c7dd29d42e56cc18ad385cc21ba011d974372cb11757d27cb35d1747aec172f1f48685b59cd039fc6c860486b21b9205eb05389c54fa78e5b38477f216c9928f816b97274bd38674d3f7b33f0899b36cdeb040898f24627404e296b3566610143dccbd57f8ef0fb2037b58b6dbf45f924c5c64cf657e96fb531b38ee3f9529ec62255e64107b7771258ffa81df866ff3cc1013866bdc45d73709624018c8466d4b0fed9e4761c146e75f28560d5bb964b00a2a14cb91ac50a5417\",\"206\",\"0\",\"262144\",\"89000\",\"storage.googleapis.com\",\"\",\"\"\"linux64\"\" sra-toolkit vdb-dump.3-head (phid=bGc77b715a,libc=2.28,bmap=nob)\",\"ADPycdvACa6n_nW35L2ltwqjWbblm7_fFNEFwq2QbMFHVjal-nZh3mLLVRc4qmz3TPtF_C1j0IHAC5EumKoF4AHFOClJYw\",\"GET_Object\",\"sra-pub-run-7\",\"SRR000002/SRR000002.3\"" );
    ASSERT_EQ( "\"\"linux64\"\" sra-toolkit vdb-dump.3-head (phid=bGc77b715a,libc=2.28,bmap=nob)",
               extract_value( res, "agent" ) );
    // un-escaped
    ASSERT_EQ( string( "\"linux64\" sra-toolkit vdb-dump.3-head (phid=bGc77b715a,libc=2.28,bmap=nob)" ),
               static_cast<const GCPReceiver&>( pb -> GetReceiver() ) . agent_for_postprocess );
}

TEST_F( GCPTestFixture, VDB_5563 )
{   // allow empty method
    string res = try_to_parse_good(
"\"1714445214573584\",\"\",\"\",\"\",\"\",\"\",\"200\",\"\",\"\",\"\",\"\",\"\",\"GCS Lifecycle Management\",\"d8c3fedc1503d12e1e1b797027e52ed3\",\"\",\"sra-pub-crun-7\",\"\"" );
    ASSERT_NE( "", res );
}

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}
