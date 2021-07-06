#include <gtest/gtest.h>

#include "Formatters.hpp"
#include "CatWriters.hpp"
#include "AWS_Interface.hpp"
#include "parse_test_fixture.hpp"

#include <algorithm>

using namespace std;
using namespace NCBI::Logging;

#define INIT_TSTR(t, s)     do { (t).p = s; (t).n = strlen(s); } while (false)

TEST(AWSReceiverTest, Create)
{
    AWSReceiver e ( make_shared<JsonLibFormatter>() );
}

TEST(AWSReceiverTest, Setters)
{
    AWSReceiver e ( make_shared<JsonLibFormatter>() );

    t_str v;
    INIT_TSTR( v, "i_p"); e.set( ReceiverInterface::ip, v );
    INIT_TSTR( v, "ref"); e.set( ReceiverInterface::referer, v );
    INIT_TSTR( v, "unp"); e.set( ReceiverInterface::unparsed, v );
    INIT_TSTR( v, "agt"); e.set( ReceiverInterface::agent, v );

    INIT_TSTR( v, "m" ); e.set( AWSReceiver::method, v );
    INIT_TSTR( v, "p" ); e.set( AWSReceiver::path, v );
    INIT_TSTR( v, "v" ); e.set( AWSReceiver::vers, v );

    INIT_TSTR( v, "own"); e.set( AWSReceiver::owner, v);
    INIT_TSTR( v, "buc"); e.set( AWSReceiver::bucket, v);
    INIT_TSTR( v, "tim"); e.set( AWSReceiver::time, v);
    INIT_TSTR( v, "req"); e.set( AWSReceiver::requester, v);
    INIT_TSTR( v, "req_id");e.set( AWSReceiver::request_id, v);
    INIT_TSTR( v, "ope"); e.set( AWSReceiver::operation, v);
    INIT_TSTR( v, "key"); e.set( AWSReceiver::key, v);
    INIT_TSTR( v, "cod"); e.set( AWSReceiver::res_code, v);
    INIT_TSTR( v, "err"); e.set( AWSReceiver::error, v);
    INIT_TSTR( v, "res"); e.set( AWSReceiver::res_len, v);
    INIT_TSTR( v, "obj"); e.set( AWSReceiver::obj_size, v);
    INIT_TSTR( v, "tot"); e.set( AWSReceiver::total_time, v);
    INIT_TSTR( v, "tur"); e.set( AWSReceiver::turnaround_time, v);
    INIT_TSTR( v, "ver"); e.set( AWSReceiver::version_id, v);
    INIT_TSTR( v, "host");e.set( AWSReceiver::host_id, v);
    INIT_TSTR( v, "sig"); e.set( AWSReceiver::sig_ver, v);
    INIT_TSTR( v, "cip"); e.set( AWSReceiver::cipher_suite, v);
    INIT_TSTR( v, "aut"); e.set( AWSReceiver::auth_type, v);
    INIT_TSTR( v, "hos"); e.set( AWSReceiver::host_header, v);
    INIT_TSTR( v, "tls"); e.set( AWSReceiver::tls_version, v);

    ASSERT_EQ (
        "{\"agent\":\"agt\",\"auth_type\":\"aut\",\"bucket\":\"buc\",\"cipher_suite\":\"cip\",\"error\":\"err\",\"host_header\":\"hos\",\"host_id\":\"host\",\"ip\":\"i_p\",\"key\":\"key\",\"method\":\"m\",\"obj_size\":\"obj\",\"operation\":\"ope\",\"owner\":\"own\",\"path\":\"p\",\"referer\":\"ref\",\"request_id\":\"req_id\",\"requester\":\"req\",\"res_code\":\"cod\",\"res_len\":\"res\",\"sig_ver\":\"sig\",\"time\":\"tim\",\"tls_version\":\"tls\",\"total_time\":\"tot\",\"turnaround_time\":\"tur\",\"unparsed\":\"unp\",\"vers\":\"v\",\"version_id\":\"ver\"}",
        e.GetFormatter().format() );
}

class AWSTestFixture : public ParseTestFixture< AWSParseBlockFactory >
{
};

TEST_F( AWSTestFixture, lonely_eol )
{
    std::string res = try_to_parse_ugly( "\n" );
    ASSERT_EQ( "{\"_line_nr\":1,\"_unparsed\":\"\"}\n", res );
}

TEST_F( AWSTestFixture, Setters_BadUTF8 )
{
    AWSReceiver e ( make_shared<JsonLibFormatter>() );

    t_str v;
    INIT_TSTR( v, "7dd\xFFga" );
    e.set( AWSReceiver::owner, v );
    ASSERT_EQ( ReceiverInterface::cat_review, e.GetCategory() );
    string text = e . GetFormatter() . format();
    ASSERT_EQ( "badly formed UTF-8 character in 'owner'", extract_value( text, "_error" ) );
    ASSERT_EQ( "7dd\\uffffffffga", extract_value( text, "owner" ) );
}

TEST_F( AWSTestFixture, parse_just_dashes )
{
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - - - - - - - - - -" );
    ASSERT_EQ(
        "{\"accession\":\"\",\"agent\":\"\",\"auth_type\":\"\",\"bucket\":\"\",\"cipher_suite\":\"\",\"error\":\"\",\"extension\":\"\",\"filename\":\"\",\"host_header\":\"\",\"host_id\":\"\",\"ip\":\"\",\"key\":\"\",\"method\":\"\",\"obj_size\":\"\",\"operation\":\"\",\"owner\":\"\",\"path\":\"\",\"referer\":\"\",\"request_id\":\"\",\"requester\":\"\",\"res_code\":\"\",\"res_len\":\"\",\"sig_ver\":\"\",\"time\":\"\",\"tls_version\":\"\",\"total_time\":\"\",\"turnaround_time\":\"\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"\",\"version_id\":\"\"}\n",
        res
        );
}

TEST_F( AWSTestFixture, parse_embedded_0 )
{
    string s("- - - \x00 - - - - - - - - - - - - - - - - - - - - -", 49);
    std::string res = try_to_parse_ugly( s );
    ASSERT_EQ(
        "{\"_line_nr\":1,\"_unparsed\":\"- - - \\u0000 - - - - - - - - - - - - - - - - - - - - -\",\"bucket\":\"\",\"owner\":\"\",\"time\":\"\"}\n",
        res
        );
}

TEST_F( AWSTestFixture, parse_quoted_dashes )
{
    std::string res = try_to_parse_good( "\"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\"" );
    ASSERT_EQ(
        "{\"accession\":\"\",\"agent\":\"-\",\"auth_type\":\"\",\"bucket\":\"\",\"cipher_suite\":\"\",\"error\":\"\",\"extension\":\"\",\"filename\":\"\",\"host_header\":\"\",\"host_id\":\"\",\"ip\":\"\",\"key\":\"\",\"method\":\"\",\"obj_size\":\"\",\"operation\":\"\",\"owner\":\"\",\"path\":\"\",\"referer\":\"\",\"request_id\":\"\",\"requester\":\"\",\"res_code\":\"\",\"res_len\":\"\",\"sig_ver\":\"\",\"time\":\"\",\"tls_version\":\"\",\"total_time\":\"\",\"turnaround_time\":\"\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"\",\"version_id\":\"\"}\n",
        res
        );
}

TEST_F( AWSTestFixture, LineRejecting )
{
    std::string res = try_to_parse_ugly( "line1 blah\nline2\nline3\n" );
    ASSERT_EQ(
        "{\"_line_nr\":1,\"_unparsed\":\"line1 blah\",\"bucket\":\"blah\",\"owner\":\"line1\"}\n"
        "{\"_line_nr\":2,\"_unparsed\":\"line2\",\"owner\":\"line2\"}\n"
        "{\"_line_nr\":3,\"_unparsed\":\"line3\",\"owner\":\"line3\"}\n", res );
}

TEST_F( AWSTestFixture, LineRejecting_with_embedded_zero )
{
    const char * txt = "line1 \000blah\nline2\n";
    std::string res = try_to_parse_ugly( std::string( txt, 18 ) );
    ASSERT_EQ(
        "{\"_line_nr\":1,\"_unparsed\":\"line1 \\u0000blah\",\"owner\":\"line1\"}\n"
        "{\"_line_nr\":2,\"_unparsed\":\"line2\",\"owner\":\"line2\"}\n", res );
}

TEST_F( AWSTestFixture, ErrorRecovery )
{
    try_to_parse(
        "line1 blah\n"
        "- - - - - - - - - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "{\"_line_nr\":1,\"_unparsed\":\"line1 blah\",\"bucket\":\"blah\",\"owner\":\"line1\"}\n",
                s_outputs.get_ugly() );
    ASSERT_EQ( "{\"accession\":\"\",\"agent\":\"\",\"auth_type\":\"\",\"bucket\":\"\",\"cipher_suite\":\"\",\"error\":\"\",\"extension\":\"\",\"filename\":\"\",\"host_header\":\"\",\"host_id\":\"\",\"ip\":\"\",\"key\":\"\",\"method\":\"\",\"obj_size\":\"\",\"operation\":\"\",\"owner\":\"\",\"path\":\"\",\"referer\":\"\",\"request_id\":\"\",\"requester\":\"\",\"res_code\":\"\",\"res_len\":\"\",\"sig_ver\":\"\",\"time\":\"\",\"tls_version\":\"\",\"total_time\":\"\",\"turnaround_time\":\"\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"\",\"version_id\":\"\"}\n",
                s_outputs.get_good() );
}

TEST_F( AWSTestFixture, parse_owner )
{
    std::string res = try_to_parse_good( "7dd4dcfe9b004fb7433c61af3e87972f2e9477fa7f0760a02827f771b41b3455 - - - - - - - - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "7dd4dcfe9b004fb7433c61af3e87972f2e9477fa7f0760a02827f771b41b3455",
                extract_value( res, "owner" ) );
}

TEST_F( AWSTestFixture, parse_owner_str1 )
{
    std::string res = try_to_parse_good( "123%@& - - - - - - - - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "123%@&", extract_value( res, "owner" ) );
}

TEST_F( AWSTestFixture, parse_bucket )
{
    std::string res = try_to_parse_good( "- abucket - - - - - - - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "abucket", extract_value( res, "bucket" ) );
}

TEST_F( AWSTestFixture, parse_time )
{
    std::string res = try_to_parse_good( "- - [09/May/2020:22:07:21 +0000] - - - - - - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "[09/May/2020:22:07:21 +0000]", extract_value( res, "time" ) );
}

TEST_F( AWSTestFixture, parse_ip4 )
{
    std::string res = try_to_parse_good( "- - - 18.207.254.142 - - - - - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "18.207.254.142", extract_value( res, "ip" ) );
}

TEST_F( AWSTestFixture, parse_ip6 )
{
    std::string res = try_to_parse_good( "- - - 0123:4567:89ab:cdef::1.2.3.4 - - - - - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "0123:4567:89ab:cdef::1.2.3.4", extract_value( res, "ip" ) );
}

TEST_F( AWSTestFixture, parse_requester )
{
    std::string res = try_to_parse_good( "- - - - arn:aws:sts::783971887864 - - - - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "arn:aws:sts::783971887864", extract_value( res, "requester" ) );
}

TEST_F( AWSTestFixture, parse_request_id )
{
    std::string res = try_to_parse_good( "- - - - - B6301F55C2486C74 - - - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "B6301F55C2486C74", extract_value( res, "request_id" ) );
}

TEST_F( AWSTestFixture, parse_operation )
{
    std::string res = try_to_parse_good( "- - - - - - REST.PUT.PART - - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "REST.PUT.PART", extract_value( res, "operation" ) );
}

TEST_F( AWSTestFixture, parse_key )
{
    std::string res = try_to_parse_good( "- - - - - - - ERR792423/5141526_s1_p0.bas.h5.1 - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "ERR792423/5141526_s1_p0.bas.h5.1", extract_value( res, "key" ) );
    ASSERT_EQ( "ERR792423", extract_value( res, "accession" ) );
    ASSERT_EQ( "5141526_s1_p0", extract_value( res, "filename" ) );
    ASSERT_EQ( ".bas.h5.1", extract_value( res, "extension" ) );
}

TEST_F( AWSTestFixture, parse_key_with_qmark )
{
    std::string res = try_to_parse_good( "- - - - - - - ERR792423/5141526_s1_p0.bas.h5.1?ab - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "ERR792423/5141526_s1_p0.bas.h5.1?ab", extract_value( res, "key" ) );
    ASSERT_EQ( "ERR792423", extract_value( res, "accession" ) );
    ASSERT_EQ( "5141526_s1_p0", extract_value( res, "filename" ) );
    ASSERT_EQ( ".bas.h5.1", extract_value( res, "extension" ) );
}

TEST_F( AWSTestFixture, parse_key_with_pct )
{
    std::string res = try_to_parse_good( "- - - - - - - ERR792423/5141526_%s1_p0.bas.h5.1%ab - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "ERR792423/5141526_%s1_p0.bas.h5.1%ab", extract_value( res, "key" ) );
    ASSERT_EQ( "ERR792423", extract_value( res, "accession" ) );
    ASSERT_EQ( "5141526_%s1_p0", extract_value( res, "filename" ) );
    ASSERT_EQ( ".bas.h5.1%ab", extract_value( res, "extension" ) );
}

TEST_F( AWSTestFixture, parse_key_with_eq )
{
    std::string res = try_to_parse_good( "- - - - - - - ERR792423/5141526_=s1_p0.bas.h5.1=b - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "ERR792423/5141526_=s1_p0.bas.h5.1=b", extract_value( res, "key" ) );
    ASSERT_EQ( "ERR792423", extract_value( res, "accession" ) );
    ASSERT_EQ( "5141526_=s1_p0", extract_value( res, "filename" ) );
    ASSERT_EQ( ".bas.h5.1=b", extract_value( res, "extension" ) );
}

TEST_F( AWSTestFixture, parse_key_with_amp )
{
    std::string res = try_to_parse_good( "- - - - - - - ERR792423/5141526_&s1_p0.bas.h5.1&b - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "ERR792423/5141526_&s1_p0.bas.h5.1&b", extract_value( res, "key" ) );
    ASSERT_EQ( "ERR792423", extract_value( res, "accession" ) );
    ASSERT_EQ( "5141526_&s1_p0", extract_value( res, "filename" ) );
    ASSERT_EQ( ".bas.h5.1&b", extract_value( res, "extension" ) );
}

TEST_F( AWSTestFixture, parse_key_multiple_accessions )
{   // pick the last accession
    std::string res = try_to_parse_good( "- - - - - - - SRX123456/ERR792423/5141526_s1_p0.bas.h5.1 - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "SRX123456/ERR792423/5141526_s1_p0.bas.h5.1", extract_value( res, "key" ) );
    ASSERT_EQ( "ERR792423", extract_value( res, "accession" ) );
    ASSERT_EQ( "5141526_s1_p0", extract_value( res, "filename" ) );
    ASSERT_EQ( ".bas.h5.1", extract_value( res, "extension" ) );
}

TEST_F( AWSTestFixture, parse_key_multiple_path_segments )
{   // pick the last segment
    std::string res = try_to_parse_good( "- - - - - - - SRR123456/abc.12/5141526_s1_p0.bas.h5.1 - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "SRR123456/abc.12/5141526_s1_p0.bas.h5.1", extract_value( res, "key" ) );
    ASSERT_EQ( "SRR123456", extract_value( res, "accession" ) );
    ASSERT_EQ( "5141526_s1_p0", extract_value( res, "filename" ) );
    ASSERT_EQ( ".bas.h5.1", extract_value( res, "extension" ) );
}

TEST_F( AWSTestFixture, parse_key_no_extension )
{
    std::string res = try_to_parse_good( "- - - - - - - SRR123456/abc.12/5141526_s1_p0 - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "SRR123456/abc.12/5141526_s1_p0", extract_value( res, "key" ) );
    ASSERT_EQ( "SRR123456", extract_value( res, "accession" ) );
    ASSERT_EQ( "5141526_s1_p0", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( AWSTestFixture, parse_key_only_extension )
{
    std::string res = try_to_parse_good( "- - - - - - - SRR123456/abc.12/.bas.h5.1 - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "SRR123456/abc.12/.bas.h5.1", extract_value( res, "key" ) );
    ASSERT_EQ( "SRR123456", extract_value( res, "accession" ) );
    ASSERT_EQ( "", extract_value( res, "filename" ) );
    ASSERT_EQ( ".bas.h5.1", extract_value( res, "extension" ) );
}

TEST_F( AWSTestFixture, parse_key_accession_is_filename )
{
    std::string res = try_to_parse_good( "- - - - - - - SRR123456.1 - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "SRR123456.1", extract_value( res, "key" ) );
    ASSERT_EQ( "SRR123456", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR123456", extract_value( res, "filename" ) );
    ASSERT_EQ( ".1", extract_value( res, "extension" ) );
}

TEST_F( AWSTestFixture, parse_request )
{
    std::string res = try_to_parse_good( "- - - - - - - - \"GET ?list-type=2&delimiter=%2F&prefix=SRR11060177%2FSRR99999999/filename.1&morefilenames.moreextensions.1&name=SRR000123&encoding-type=url HTTP/1.1\" - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "GET", extract_value( res, "method" ) );
    ASSERT_EQ( "?list-type=2&delimiter=%2F&prefix=SRR11060177%2FSRR99999999/filename.1&morefilenames.moreextensions.1&name=SRR000123&encoding-type=url", extract_value( res, "path" ) );
    ASSERT_EQ( "HTTP/1.1", extract_value( res, "vers" ) );
    ASSERT_EQ( "SRR99999999", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( ".1", extract_value( res, "extension" ) );
}

TEST_F( AWSTestFixture, parse_request_no_ver )
{
    std::string res = try_to_parse_good( "- - - - - - - - \"GET url \" - - - - - - - - - - - - - - -" );

    ASSERT_EQ( "GET", extract_value( res, "method" ) );
    ASSERT_EQ( "url", extract_value( res, "path" ) );
    ASSERT_EQ( "", extract_value( res, "vers" ) );
}

TEST_F( AWSTestFixture, parse_request_no_space_ver )
{
    std::string res = try_to_parse_good( "- - - - - - - - \"GET url\" - - - - - - - - - - - - - - -" );

    ASSERT_EQ( "GET", extract_value( res, "method" ) );
    ASSERT_EQ( "url", extract_value( res, "path" ) );
    ASSERT_EQ( "", extract_value( res, "vers" ) );
}

TEST_F( AWSTestFixture, parse_request_no_url )
{
    std::string res = try_to_parse_good( "- - - - - - - - \"GET\" - - - - - - - - - - - - - - -" );

    ASSERT_EQ( "GET", extract_value( res, "method" ) );
    ASSERT_EQ( "", extract_value( res, "path" ) );
    ASSERT_EQ( "", extract_value( res, "vers" ) );
}

TEST_F( AWSTestFixture, parse_request_no_method )
{   // not supported for now
    std::string res = try_to_parse_ugly( "- - - - - - - - \"url\" - - - - - - - - - - - - - - -" );
}

TEST_F( AWSTestFixture, parse_key_and_request )
{   // the key wins, if
    std::string res = try_to_parse_good( "- - - - - - - ERR792423/5141526_s1_p0.bas.h5.1 \"GET ?list-type=2&delimiter=%2F&prefix=SRR11060177%2FSRR99999999/filename.1&morefilenames.moreextensions.1&name=SRR000123&encoding-type=url HTTP/1.1\" - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "GET", extract_value( res, "method" ) );
    ASSERT_EQ( "?list-type=2&delimiter=%2F&prefix=SRR11060177%2FSRR99999999/filename.1&morefilenames.moreextensions.1&name=SRR000123&encoding-type=url", extract_value( res, "path" ) );
    ASSERT_EQ( "HTTP/1.1", extract_value( res, "vers" ) );
    // the extracted values come from the key
    ASSERT_EQ( "ERR792423", extract_value( res, "accession" ) );
    ASSERT_EQ( "5141526_s1_p0", extract_value( res, "filename" ) );
    ASSERT_EQ( ".bas.h5.1", extract_value( res, "extension" ) );
}

TEST_F( AWSTestFixture, parse_res_code )
{
    std::string res = try_to_parse_good( "- - - - - - - - - 200 - - - - - - - - - - - - - -" );
    ASSERT_EQ( "200", extract_value( res, "res_code" ) );
}

TEST_F( AWSTestFixture, parse_error )
{
    std::string res = try_to_parse_good( "- - - - - - - - - - NoSuchKey - - - - - - - - - - - - -" );
    ASSERT_EQ( "NoSuchKey", extract_value( res, "error" ) );
}

TEST_F( AWSTestFixture, parse_res_len )
{
    std::string res = try_to_parse_good( "- - - - - - - - - - - 1024 - - - - - - - - - - - -" );
    ASSERT_EQ( "1024", extract_value( res, "res_len" ) );
}

TEST_F( AWSTestFixture, parse_obj_size )
{
    std::string res = try_to_parse_good( "- - - - - - - - - - - - 1024 - - - - - - - - - - -" );
    ASSERT_EQ( "1024", extract_value( res, "obj_size" ) );
}

TEST_F( AWSTestFixture, parse_total_time )
{
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - 1024 - - - - - - - - - -" );
    ASSERT_EQ( "1024", extract_value( res, "total_time" ) );
}

TEST_F( AWSTestFixture, parse_turnaround_time )
{
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - 1024 - - - - - - - - -" );
    ASSERT_EQ( "1024", extract_value( res, "turnaround_time" ) );
}

TEST_F( AWSTestFixture, parse_referrer_simple )
{
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - referrrer - - - - - - - -" );
    ASSERT_EQ( "referrrer", extract_value( res, "referer" ) );
}

TEST_F( AWSTestFixture, parse_referrer_compound )
{
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - \"referring referrrer\" - - - - - - - -" );
    ASSERT_EQ( "referring referrrer", extract_value( res, "referer" ) );
}

TEST_F( AWSTestFixture, parse_agent )
{
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - - \"linux64 sra-toolkit test-sra.2.8.2 (phid=noc7737000,libc=2.17)\" - - - - - - -" );
    ASSERT_EQ( "linux64 sra-toolkit test-sra.2.8.2 (phid=noc7737000,libc=2.17)", extract_value( res, "agent" ) );
}

TEST_F( AWSTestFixture, parse_agent_empty )
{
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - - \"\" - - - - - - -" );
    ASSERT_EQ( "", extract_value( res, "agent" ) );
}

TEST_F( AWSTestFixture, parse_version_id )
{
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - - - 3HL4kqtJvjVBH40Nrjfkd - - - - - -" );
    ASSERT_EQ( "3HL4kqtJvjVBH40Nrjfkd", extract_value( res, "version_id" ) );
}

TEST_F( AWSTestFixture, parse_2_part_hostId )
{
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - - - - AIDAISBTTLPGXGH6YFFAY LzYGhqEwXn5Xiuil9tI6JtK2PiIo+SC6Ute3Isq2qEmt/t0Z7qFkyD0mp1ZIc43bm0qSX4tBbbc= - - - - -" );
    ASSERT_EQ( "AIDAISBTTLPGXGH6YFFAY LzYGhqEwXn5Xiuil9tI6JtK2PiIo+SC6Ute3Isq2qEmt/t0Z7qFkyD0mp1ZIc43bm0qSX4tBbbc=", extract_value( res, "host_id" ) );
}

TEST_F( AWSTestFixture, parse_1_part_hostId_a )
{
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - - - - AIDAISBTTLPGXGH6YFFAY - - - - -" );
    ASSERT_EQ( "AIDAISBTTLPGXGH6YFFAY", extract_value( res, "host_id" ) );
}

TEST_F( AWSTestFixture, parse_1_part_hostId_b )
{
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - - - - LzYGhqEwXn5Xiuil9tI6JtK2PiIo+SC6Ute3Isq2qEmt/t0Z7qFkyD0mp1ZIc43bm0qSX4tBbbc= - - - - -" );
    ASSERT_EQ( "LzYGhqEwXn5Xiuil9tI6JtK2PiIo+SC6Ute3Isq2qEmt/t0Z7qFkyD0mp1ZIc43bm0qSX4tBbbc=", extract_value( res, "host_id" ) );
}

TEST_F( AWSTestFixture, parse_sig_ver )
{
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - - - - - SigV4 - - - -" );
    ASSERT_EQ( "SigV4", extract_value( res, "sig_ver" ) );
}

TEST_F( AWSTestFixture, parse_cipher )
{
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - - - - - - ECDHE-RSA-AES128-GCM-SHA256 - - -" );
    ASSERT_EQ( "ECDHE-RSA-AES128-GCM-SHA256", extract_value( res, "cipher_suite" ) );
}

TEST_F( AWSTestFixture, parse_auth )
{
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - - - - - - - AuthHeader - -" );
    ASSERT_EQ( "AuthHeader", extract_value( res, "auth_type" ) );
}

TEST_F( AWSTestFixture, parse_host_hdr )
{
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - - - - - - - - sra-pub-src-14.s3.amazonaws.com -" );
    ASSERT_EQ( "sra-pub-src-14.s3.amazonaws.com", extract_value( res, "host_header" ) );
}

TEST_F( AWSTestFixture, parse_tls )
{
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - - - - - - - - - TLSv1.2" );
    ASSERT_EQ( "TLSv1.2", extract_value( res, "tls_version" ) );
}

TEST_F( AWSTestFixture, repeated_time_shifts_tls_vers )
{
    std::string input = "922194806485875312b252374a3644f1feecd16802a50d4729885c1d11e1fd37 sra-pub-run-7 [05/Jan/2020:23:48:44 +0000] 130.14.28.32 rn:aws:iam::783971922194806485875312b252374a3644f1feecd16802a50d4729885c1d11e1fd37 sra-pub-run-6 [05/Jan/2020:23:10:20 +0000] 130.14.28.140 arn:aws:iam::651740271041:user/sddp-1-ncbi-verifier A5D9D5749E3175C8 REST.GET.BUCKET - \"GET /?prefix=SRR7087357%2FSRR7087357.1&encoding-type=url HTTP/1.1\" 403 AccessDenied 243 - 14 - \"-\" \"aws-cli/1.16.87 Python/3.7.3 Linux/3.10.0-1062.7.1.el7.x86_64 botocore/1.12.77\" - kgI+NnID7YrclgTiImbWYaTJ9nRbiOsmQw/SnkihnWcpM67sWFjyx6T/YH9CnrlJV2wZit4LbSo= SigV4 ECDHE-RSA-AES128-GCM-SHA256 AuthHeader sra-pub-run-6.s3.amazonaws.com TLSv1.2";
    std::string res = try_to_parse_bad( input );
}

TEST_F( AWSTestFixture, percent_in_key )
{
    std::string input = "922194806485875312b252374a3644f1feecd16802a50d4729885c1d11e1fd37 sra-pub-src-13 [27/Jul/2020:22:56:40 +0000] 35.175.201.205 arn:aws:sts::783971887864:assumed-role/sra-developer-instance-profile-role/i-0b6b034f8f9b482fb 8C244990662F952E REST.PUT.PART SRR12318371/run2249_lane2_read2_indexN703%253DDropSeq_Potter_BrainWT_8_17_17_N703.fastq.gz.1 \"PUT /SRR12318371/run2249_lane2_read2_indexN703%3DDropSeq_Potter_BrainWT_8_17_17_N703.fastq.gz.1?partNumber=4&uploadId=qpR5BCc2akFqypS4spNO3VQiVAFHRYcSPNoZgZCJSQVh71SLNrUQLeARnEaBGhGGnrZihGRLY1T1pJ055KFXvVa7RYMLm1VM.t6ZKJt2zY16rP0q_wgAcu2aY4eNfrFWOe0GEl7xjYv2ZBe99zmqMg-- HTTP/1.1\" 200 - - 8388608 743 47 \"-\" \"aws-cli/1.16.102 Python/2.7.16 Linux/4.14.171-105.231.amzn1.x86_64 botocore/1.12.92\" - Tfy76PKfrEHlL2+E2a1wC87RDinpRfa1tngc77ETpa94hBUbT/GxSWE/SqHWK8iC8DLTqIfRn3w= SigV4 ECDHE-RSA-AES128-GCM-SHA256 AuthHeader sra-pub-src-13.s3.amazonaws.com TLSv1.2";
    std::string res = try_to_parse_good( input );
    ASSERT_EQ( "SRR12318371/run2249_lane2_read2_indexN703%253DDropSeq_Potter_BrainWT_8_17_17_N703.fastq.gz.1", extract_value( res, "key" ) );
}

TEST_F( AWSTestFixture, extra_data )
{
    // because the state is to-review the post-processing is not called, that is the reason why access/filename/extension are missing
    std::string res = try_to_parse_review( "- - - - - - - - - - - - - - - - - - - - - - - - more tokens follow" );
    ASSERT_EQ(
        "{\"_error\":\"Extra fields discovered\",\"_extra\":\"more tokens follow\",\"_line_nr\":1,\"_unparsed\":\"- - - - - - - - - - - - - - - - - - - - - - - - more tokens follow\",\"agent\":\"\",\"auth_type\":\"\",\"bucket\":\"\",\"cipher_suite\":\"\",\"error\":\"\",\"host_header\":\"\",\"host_id\":\"\",\"ip\":\"\",\"key\":\"\",\"method\":\"\",\"obj_size\":\"\",\"operation\":\"\",\"owner\":\"\",\"path\":\"\",\"referer\":\"\",\"request_id\":\"\",\"requester\":\"\",\"res_code\":\"\",\"res_len\":\"\",\"sig_ver\":\"\",\"time\":\"\",\"tls_version\":\"\",\"total_time\":\"\",\"turnaround_time\":\"\",\"vers\":\"\",\"version_id\":\"\"}\n",
        res
        );
}

TEST_F( AWSTestFixture, quote_in_agent )
{
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - - \"linux64 sra-toolkit \\\"test-sra.2.8.2 (phid=noc7737000,libc=2.17)\" - - - - - - -" );
    ASSERT_EQ( "linux64 sra-toolkit \\\"test-sra.2.8.2 (phid=noc7737000,libc=2.17)", extract_value( res, "agent" ) );
}

TEST_F( AWSTestFixture, quote_in_path )
{
    std::string res = try_to_parse_good( "- - - - - - - - \"GET a\\\" HTTP/1.1\" - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "a\\\"", extract_value( res, "path" ) );
}

TEST_F( AWSTestFixture, investigate )
{
    std::string res = try_to_parse_good( "922194806485875312b252374a3644f1feecd16802a50d4729885c1d11e1fd37 sra-pub-run-1 [20/Mar/2019:19:44:01 +0000] 130.14.20.103 arn:aws:iam::783971887864:user/ignatovi 92D3B218463A63EA REST.GET.TAGGING - \"GET /sra-pub-run-1?tagging= HTTP/1.1\" 404 NoSuchTagSet 293 - 59 - \"-\" \"S3Console/0.4, aws-internal/3 aws-sdk-java/1.11.509 Linux/4.9.137-0.1.ac.218.74.329.metal1.x86_64 OpenJDK_64-Bit_Server_VM/25.202-b08 java/1.8.0_202\" - AIDAISBTTLPGXGH6YFFAY LzYGhqEwXn5Xiuil9tI6JtK2PiIo+SC6Ute3Isq2qEmt/t0Z7qFkyD0mp1ZIc43bm0qSX4tBbbc= SigV4 ECDHE-RSA-AES128-SHA AuthHeader s3.amazonaws.com TLSv1.2");
    ASSERT_EQ( "130.14.20.103", extract_value( res, "ip" ) );
}

TEST_F( AWSTestFixture, two_lines_with_and_without_accessions )
{
    std::string src1 = "- - - - - - - SRR123456.1 - - - - - - - - - - - - - - - -";
    std::string src2 = "- - - - - - - - - - - - - - - - - - - - - - - -";
    std::string src = src1 + "\n" + src2;
    std::string res = try_to_parse_good( src );

    ASSERT_EQ( "{\"accession\":\"SRR123456\",\"agent\":\"\",\"auth_type\":\"\",\"bucket\":\"\",\"cipher_suite\":\"\",\"error\":\"\",\"extension\":\".1\",\"filename\":\"SRR123456\",\"host_header\":\"\",\"host_id\":\"\",\"ip\":\"\",\"key\":\"SRR123456.1\",\"method\":\"\",\"obj_size\":\"\",\"operation\":\"\",\"owner\":\"\",\"path\":\"\",\"referer\":\"\",\"request_id\":\"\",\"requester\":\"\",\"res_code\":\"\",\"res_len\":\"\",\"sig_ver\":\"\",\"time\":\"\",\"tls_version\":\"\",\"total_time\":\"\",\"turnaround_time\":\"\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"\",\"version_id\":\"\"}\n{\"accession\":\"\",\"agent\":\"\",\"auth_type\":\"\",\"bucket\":\"\",\"cipher_suite\":\"\",\"error\":\"\",\"extension\":\"\",\"filename\":\"\",\"host_header\":\"\",\"host_id\":\"\",\"ip\":\"\",\"key\":\"\",\"method\":\"\",\"obj_size\":\"\",\"operation\":\"\",\"owner\":\"\",\"path\":\"\",\"referer\":\"\",\"request_id\":\"\",\"requester\":\"\",\"res_code\":\"\",\"res_len\":\"\",\"sig_ver\":\"\",\"time\":\"\",\"tls_version\":\"\",\"total_time\":\"\",\"turnaround_time\":\"\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"\",\"version_id\":\"\"}\n", res );
}

TEST_F( AWSTestFixture, dash_as_extra_field )
{   // the line is good, the dash gos into "_extra"
    std::string res = try_to_parse_good(
"384e684d1d83331f62ec686c0aae5c0513b488404eca7a6744bbeb7f5c6a7834 sra-ca-run-4 [29/Jun/2021:22:48:34 +0000] 34.226.216.239 arn:aws:sts::184059545989:assumed-role/sra-developer-instance-profile-role/i-07cffb826ea1814dd WC0X4C0EB5NQJQGB REST.GET.BUCKET - \"GET /?list-type=2&delimiter=%2F&prefix=SRR13431598%2FSRR13431598.2&encoding-type=url HTTP/1.1\" 200 - 324 - 25 24 \"-\" \"aws-cli/1.18.147 Python/2.7.18 Linux/4.14.231-173.361.amzn2.x86_64 botocore/1.18.6\" - Sp3lYc2YbhTGkIh700Tmw/4m3Bt7AawTZlRpEY5GoFSByNYqyHiOfJF5MgLTC/MhtmyV21E84+Y= SigV4 ECDHE-RSA-AES128-GCM-SHA256 AuthHeader sra-ca-run-4.s3.amazonaws.com TLSv1.2 -"
    );
        ASSERT_EQ( "-", extract_value( res, "_extra" ) );
}

TEST_F( AWSTestFixture, MultiThreading )
{
    std::string input(
        "1 - - - - - - - - - - - - - - - - - - - - - - x\n" // ugly
        "2 - - - - - - - - - - - - - - - - - - - - - - x\n" // ugly
        "3 - - - - - - - - - - - - - - - - - - - - - - -\n" // good
        "4 - - - - - - - - - - - - - - - - - - - - - - x\n" // ugly
    );

    FILE * ss = fmemopen( (void*) input.c_str(), input.size(), "r");
    CLineSplitter splitter( ss );
    AWSParseBlockFactory pbFact;
    //pbFact.setFast(false);
    MultiThreadedDriver p( splitter, s_outputs, 100, 2, pbFact );
    p . parse_all_lines();

    ASSERT_LT( 0, s_outputs.get_good().size() );
    ASSERT_EQ( 0, s_outputs.get_bad().size() );
    ASSERT_EQ( 0, s_outputs.get_review().size() );

    vector<string> lines;
    std::string s = s_outputs.get_ugly();
    std::string separator = "\n";
    while( s.find( separator ) != std::string::npos )
    {
        auto separatorIndex = s.find( separator );
        lines.push_back( s.substr( 0, separatorIndex ) );
        s = s.substr( separatorIndex + 1, s.length() );
    }
    if ( s.length() > 0 )
        lines.push_back( s );

    ASSERT_EQ( 3, lines.size() );
    sort( lines.begin(), lines.end() );

    ASSERT_EQ( "1", extract_value( lines[0], "owner" ) );
    ASSERT_EQ( "2", extract_value( lines[1], "owner" ) );
    ASSERT_EQ( "4", extract_value( lines[2], "owner" ) );
    ASSERT_EQ( "1", extract_value( lines[0], "_line_nr" ) );
    ASSERT_EQ( "2", extract_value( lines[1], "_line_nr" ) );
    ASSERT_EQ( "4", extract_value( lines[2], "_line_nr" ) );

    fclose( ss );
}

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}
