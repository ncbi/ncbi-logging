#include <gtest/gtest.h>

#include <sstream>
#include <vector>

#include "OP_Interface.hpp"
#include "Formatters.hpp"
#include "CatWriters.hpp"
#include "parse_test_fixture.hpp"

using namespace std;
using namespace NCBI::Logging;

#define INIT_TSTR(t, s)     do { (t).p = s; (t).n = strlen(s); (t).escaped = false; } while (false)
#define INIT_TSTR_ESC(t, s) do { (t).p = s; (t).n = strlen(s); (t).escaped = true; } while (false)

TEST(LogOPEventTest, Create)
{
    std::unique_ptr<FormatterInterface> f = make_unique<JsonLibFormatter>();
    OPReceiver e ( f );
}

TEST(LogOPEventTest, Setters)
{
    std::unique_ptr<FormatterInterface> f = make_unique<JsonLibFormatter>();
    OPReceiver e ( f );

    t_str v;
    INIT_TSTR( v, "i_p");
    e.set( ReceiverInterface::ip, v );
    INIT_TSTR( v, "ref");
    e.set( ReceiverInterface::referer, v );
    INIT_TSTR( v, "unp");
    e.set( ReceiverInterface::unparsed, v );
    t_agent a = {
        {"o", 1, false },
        {"v_o", 3, false },
        {"v_t", 3, false },
        {"v_r", 3, false },
        {"v_c", 3, false },
        {"v_g", 3, false },
        {"v_s", 3, false },
        {"v_l", 3, false }
    };
    e.setAgent( a );

    t_request r = {
        { "m", 1, false },
        { "p", 1, false },
        { "v", 1, false },
        { "a", 1, false },
        { "f", 1, false },
        { "e", 1, false },
        acc_before
    };
    e.setRequest( r );

    INIT_TSTR( v, "own"); e.set(OPReceiver::owner, v);
    INIT_TSTR( v, "tim"); e.set(OPReceiver::time, v);
    INIT_TSTR( v, "use"); e.set(OPReceiver::user, v);
    INIT_TSTR( v, "rqt");e.set(OPReceiver::req_time, v);
    INIT_TSTR( v, "fwd"); e.set(OPReceiver::forwarded, v);
    INIT_TSTR( v, "prt"); e.set(OPReceiver::port, v);
    INIT_TSTR( v, "rql"); e.set(OPReceiver::req_len, v);
    INIT_TSTR( v, "rqc"); e.set(OPReceiver::res_code, v);
    INIT_TSTR( v, "res"); e.set(OPReceiver::res_len, v);

    ASSERT_EQ (
        "{\"accession\":\"a\",\"agent\":\"o\",\"extension\":\"e\",\"filename\":\"f\",\"forwarded\":\"fwd\",\"ip\":\"i_p\",\"method\":\"m\",\"owner\":\"own\",\"path\":\"p\",\"port\":\"prt\",\"referer\":\"ref\",\"req_len\":\"rql\",\"req_time\":\"rqt\",\"res_code\":\"rqc\",\"res_len\":\"res\",\"time\":\"tim\",\"unparsed\":\"unp\",\"user\":\"use\",\"vdb_libc\":\"v_l\",\"vdb_os\":\"v_o\",\"vdb_phid_compute_env\":\"v_c\",\"vdb_phid_guid\":\"v_g\",\"vdb_phid_session_id\":\"v_s\",\"vdb_release\":\"v_r\",\"vdb_tool\":\"v_t\",\"vers\":\"v\"}",
        e.GetFormatter().format() );
}

class OPTestFixture : public ParseTestFixture< OPParseBlockFactory >
{
};

TEST_F( OPTestFixture, Setters_BadUTF8 )
{
    std::unique_ptr<FormatterInterface> f = make_unique<JsonLibFormatter>();
    OPReceiver e ( f );

    t_str v;
    INIT_TSTR( v, "7dd\xFFga" );
    e.set( OPReceiver::owner, v );
    ASSERT_EQ( ReceiverInterface::cat_review, e.GetCategory() );
    string text = e . GetFormatter() . format();
    ASSERT_EQ( "badly formed UTF-8 character in 'owner'", extract_value( text, "_error" ) );
    ASSERT_EQ( "7dd\\uffffffffga", extract_value( text, "owner" ) );
}

TEST_F( OPTestFixture, Setters_GoodUTF8_lib )
{
    std::unique_ptr<FormatterInterface> f = make_unique<JsonLibFormatter>();
    OPReceiver e ( f );

    t_str v;
    auto s = u8"попробуем 产品公司求购 ";
    INIT_TSTR( v, s );
    e.set( OPReceiver::owner, v );
    //string text = e . GetFormatter() . format();
    //ASSERT_EQ( s, extract_value( text, "owner" ) );
}

TEST_F( OPTestFixture, LineRejecting )
{
    std::string res = try_to_parse_ugly( "line1 blah\nline2\nline3\n" );
    ASSERT_EQ(
        "{\"_line_nr\":1,\"_unparsed\":\"line1 blah\"}\n"
        "{\"_line_nr\":2,\"_unparsed\":\"line2\"}\n"
        "{\"_line_nr\":3,\"_unparsed\":\"line3\"}\n", res );
}

TEST_F( OPTestFixture, ErrorRecovery )
{
    try_to_parse(
        "line1 blah\n"
        "18.207.254.142 - - [01/Jan/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "{\"_line_nr\":1,\"_unparsed\":\"line1 blah\"}\n",
                s_outputs.get_ugly() );
    ASSERT_EQ( "{\"accession\":\"\",\"agent\":\"\",\"extension\":\"\",\"filename\":\"\",\"forwarded\":\"-\",\"ip\":\"18.207.254.142\",\"method\":\"GET\",\"path\":\"\",\"port\":\"4\",\"referer\":\"-\",\"req_len\":\"5\",\"req_time\":\"3\",\"res_code\":\"1\",\"res_len\":\"2\",\"server\":\"sra-download.ncbi.nlm.nih.gov\",\"time\":\"[01/Jan/2020:02:50:24 -0500]\",\"user\":\"\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"\"}\n",
                s_outputs.get_good() );
}

TEST_F( OPTestFixture, unrecognized_char )
{
    std::string res = try_to_parse_ugly( "line1 \07" );
    ASSERT_EQ( "{\"_line_nr\":1,\"_unparsed\":\"line1 \\u0007\"}\n", res );
}

TEST_F( OPTestFixture, lonely_eol )
{
    std::string res = try_to_parse_ugly( "\n" );
    ASSERT_EQ( "{\"_line_nr\":1,\"_unparsed\":\"\"}\n", res );
}

TEST_F( OPTestFixture, parse_embedded_0 )
{   //TODO: send the line to Review
    string s("18.207.254.142 - \x00 - ", 21);
    std::string res = try_to_parse_ugly( s );
    ASSERT_EQ(
        "{\"_line_nr\":1,\"_unparsed\":\"18.207.254.142 - \\u0000 - \",\"ip\":\"18.207.254.142\"}\n",
        res
        );
}

TEST_F( OPTestFixture, parse_ip4 )
{
    std::string res = try_to_parse_good( "18.207.254.142 - - [01/Jan/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5\n" );
    ASSERT_EQ( "18.207.254.142", extract_value( res, "ip" ) );
}

TEST_F( OPTestFixture, parse_ip6 )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - - [01/Jan/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "0123:4567:89ab:cdef::1.2.3.4", extract_value( res, "ip" ) );
}

TEST_F( OPTestFixture, parse_user_dash )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - - [01/Jan/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "", extract_value( res, "user" ) );
}

TEST_F( OPTestFixture, parse_user )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "usr", extract_value( res, "user" ) );
}

TEST_F( OPTestFixture, parse_time )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "[01/Jan/2020:02:50:24 -0500]", extract_value( res, "time" ) );
}

TEST_F( OPTestFixture, parse_server_quoted )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "sra-download.ncbi.nlm.nih.gov", extract_value( res, "server" ) );
}

TEST_F( OPTestFixture, parse_server_unquoted )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] sra-download.ncbi.nlm.nih.gov \"GET\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "sra-download.ncbi.nlm.nih.gov", extract_value( res, "server" ) );
}

TEST_F( OPTestFixture, parse_noserver_request_full )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] \"GET url HTTP/1.1\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "", extract_value( res, "server" ) );
    ASSERT_EQ( "GET", extract_value( res, "method" ) );
    ASSERT_EQ( "url", extract_value( res, "path" ) );
    ASSERT_EQ( "HTTP/1.1", extract_value( res, "vers" ) );
}

TEST_F( OPTestFixture, parse_server_request_method_only )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "srv", extract_value( res, "server" ) );
    ASSERT_EQ( "GET", extract_value( res, "method" ) );
    ASSERT_EQ( "", extract_value( res, "path" ) );
    ASSERT_EQ( "", extract_value( res, "vers" ) );
}

TEST_F( OPTestFixture, parse_server_request_method_url )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "srv", extract_value( res, "server" ) );
    ASSERT_EQ( "GET", extract_value( res, "method" ) );
    ASSERT_EQ( "url", extract_value( res, "path" ) );
    ASSERT_EQ( "", extract_value( res, "vers" ) );
}

TEST_F( OPTestFixture, parse_server_request_method_url_space )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url \" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "srv", extract_value( res, "server" ) );
    ASSERT_EQ( "GET", extract_value( res, "method" ) );
    ASSERT_EQ( "url", extract_value( res, "path" ) );
    ASSERT_EQ( "", extract_value( res, "vers" ) );
}

TEST_F( OPTestFixture, parse_server_request_full_extra )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url HTTP/1.1 qstr# qstr_esc\\\" POST\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "srv", extract_value( res, "server" ) );
    ASSERT_EQ( "GET", extract_value( res, "method" ) );
    ASSERT_EQ( "url", extract_value( res, "path" ) );
    ASSERT_EQ( "HTTP/1.1", extract_value( res, "vers" ) );
}

TEST_F( OPTestFixture, parse_server_bad_request )
{
    std::string res = try_to_parse_review( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"me bad\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "{\"_error\":\"Invalid request\",\"_line_nr\":1,\"_unparsed\":\"0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \\\"me bad\\\" 1 2 3 \\\"-\\\" \\\"\\\" \\\"-\\\" port=4 rl=5\",\"agent\":\"\",\"forwarded\":\"-\",\"ip\":\"0123:4567:89ab:cdef::1.2.3.4\",\"port\":\"4\",\"referer\":\"-\",\"req_len\":\"5\",\"req_time\":\"3\",\"res_code\":\"1\",\"res_len\":\"2\",\"server\":\"srv\",\"time\":\"[01/Jan/2020:02:50:24 -0500]\",\"user\":\"usr\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\"}\n", res);
}

TEST_F( OPTestFixture, parse_request_accession )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] \"GET /srapub/SRR5385591.sra HTTP/1.1\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "/srapub/SRR5385591.sra", extract_value( res, "path" ) );
    ASSERT_EQ( "SRR5385591", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR5385591", extract_value( res, "filename" ) );
    ASSERT_EQ( ".sra", extract_value( res, "extension" ) );
}

TEST_F( OPTestFixture, parse_request_accession_with_params )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] \"GET /srapub/SRR5385591.sra?qqq/SRR000123?qqq.ext HTTP/1.1\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "/srapub/SRR5385591.sra?qqq/SRR000123?qqq.ext", extract_value( res, "path" ) );
    ASSERT_EQ( "SRR5385591", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR5385591", extract_value( res, "filename" ) );
    ASSERT_EQ( ".sra", extract_value( res, "extension" ) );
}

TEST_F( OPTestFixture, parse_request_accession_with_params_no_ext )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] \"GET /srapub/SRR5385591?qqq HTTP/1.1\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "/srapub/SRR5385591?qqq", extract_value( res, "path" ) );
    ASSERT_EQ( "SRR5385591", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR5385591", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( OPTestFixture, parse_request_accession_with_params_no_file )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] \"GET .ext?qqq HTTP/1.1\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( ".ext?qqq", extract_value( res, "path" ) );
    ASSERT_EQ( "", extract_value( res, "accession" ) );
    ASSERT_EQ( "", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext", extract_value( res, "extension" ) );
}

TEST_F( OPTestFixture, parse_request_accession_in_params )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] \"GET /?/srapub/SRR5385591.sra HTTP/1.1\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "/?/srapub/SRR5385591.sra", extract_value( res, "path" ) );
    ASSERT_EQ( "", extract_value( res, "accession" ) );
    ASSERT_EQ( "", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( OPTestFixture, parse_result_code )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url HTTP/1.1\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "1", extract_value( res, "res_code" ) );
}

TEST_F( OPTestFixture, parse_result_len )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url HTTP/1.1\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "2", extract_value( res, "res_len" ) );
}

TEST_F( OPTestFixture, parse_result_len_dash )
{   //TODO: review? bad?
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url HTTP/1.1\" 1 - 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "", extract_value( res, "res_len" ) );
}

TEST_F( OPTestFixture, parse_req_time_float )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url HTTP/1.1\" 1 2 3.14 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "3.14", extract_value( res, "req_time" ) );
}

TEST_F( OPTestFixture, parse_req_time_int )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url HTTP/1.1\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "3", extract_value( res, "req_time" ) );
}

TEST_F( OPTestFixture, parse_referer )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url HTTP/1.1\" 1 2 3 \"qstr# HTTP/1.1 qstr+esc\\\" PUT\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "qstr# HTTP/1.1 qstr+esc\\\" PUT", extract_value( res, "referer" ) );
}

TEST_F( OPTestFixture, parse_referer_empty )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url HTTP/1.1\" 1 2 3 \"\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "", extract_value( res, "referer" ) );
}

TEST_F( OPTestFixture, parse_agent )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url HTTP/1.1\" 1 2 3 \"\" \"linux64 sra-toolkit abi-dump.1.2 libc=1.2.3 phid=nocnognos agentstring ()\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "linux64 sra-toolkit abi-dump.1.2 libc=1.2.3 phid=nocnognos agentstring ()", extract_value( res, "agent" ) );
    ASSERT_EQ( "linux64", extract_value( res, "vdb_os" ) );
    ASSERT_EQ( "abi-dump", extract_value( res, "vdb_tool" ) );
    ASSERT_EQ( "1.2", extract_value( res, "vdb_release" ) );
    ASSERT_EQ( "noc", extract_value( res, "vdb_phid_compute_env" ) );
    ASSERT_EQ( "nog", extract_value( res, "vdb_phid_guid" ) );
    ASSERT_EQ( "nos", extract_value( res, "vdb_phid_session_id" ) );
    ASSERT_EQ( "1.2.3", extract_value( res, "vdb_libc" ) );
}

TEST_F( OPTestFixture, parse_agent_no_os )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url HTTP/1.1\" 1 2 3 \"\" \"sra-toolkit abi-dump.1.2 libc=1.2.3 phid=nocnognos agentstring ()\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "sra-toolkit abi-dump.1.2 libc=1.2.3 phid=nocnognos agentstring ()", extract_value( res, "agent" ) );
    ASSERT_EQ( "", extract_value( res, "vdb_os" ) );
    ASSERT_EQ( "abi-dump", extract_value( res, "vdb_tool" ) );
    ASSERT_EQ( "1.2", extract_value( res, "vdb_release" ) );
    ASSERT_EQ( "noc", extract_value( res, "vdb_phid_compute_env" ) );
    ASSERT_EQ( "nog", extract_value( res, "vdb_phid_guid" ) );
    ASSERT_EQ( "nos", extract_value( res, "vdb_phid_session_id" ) );
    ASSERT_EQ( "1.2.3", extract_value( res, "vdb_libc" ) );
}

TEST_F( OPTestFixture, parse_agent_empty )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url HTTP/1.1\" 1 2 3 \"\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "", extract_value( res, "agent" ) );
    ASSERT_EQ( "", extract_value( res, "vdb_os" ) );
    ASSERT_EQ( "", extract_value( res, "vdb_tool" ) );
    ASSERT_EQ( "", extract_value( res, "vdb_release" ) );
    ASSERT_EQ( "", extract_value( res, "vdb_phid_compute_env" ) );
    ASSERT_EQ( "", extract_value( res, "vdb_phid_guid" ) );
    ASSERT_EQ( "", extract_value( res, "vdb_phid_session_id" ) );
    ASSERT_EQ( "", extract_value( res, "vdb_libc" ) );
}

TEST_F( OPTestFixture, parse_forwarded )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url HTTP/1.1\" 1 2 3 \"\" \"\" \"fwd\" port=4 rl=5" );
    ASSERT_EQ( "fwd", extract_value( res, "forwarded" ) );
}

TEST_F( OPTestFixture, parse_port )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url HTTP/1.1\" 1 2 3 \"\" \"\" \"fwd\" port=4 rl=5" );
    ASSERT_EQ( "4", extract_value( res, "port" ) );
}

TEST_F( OPTestFixture, parse_req_len )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url HTTP/1.1\" 1 2 3 \"\" \"\" \"fwd\" port=4 rl=5" );
    ASSERT_EQ( "5", extract_value( res, "req_len" ) );
}

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}
