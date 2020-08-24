#include <gtest/gtest.h>

#include <sstream>
#include <vector>

#include "TW_Interface.hpp"
#include "Formatters.hpp"
#include "CatWriters.hpp"

using namespace std;
using namespace NCBI::Logging;

#define INIT_TSTR(t, s)     do { (t).p = s; (t).n = strlen(s); (t).escaped = false; } while (false)
#define INIT_TSTR_ESC(t, s) do { (t).p = s; (t).n = strlen(s); (t).escaped = true; } while (false)

TEST( LogTWEventTest, Create )
{
    std::unique_ptr<FormatterInterface> f = make_unique<JsonLibFormatter>();
    TWReceiver e ( f );
}

TEST( LogTWEventTest, Setters )
{
    std::unique_ptr<FormatterInterface> f = make_unique<JsonLibFormatter>();
    TWReceiver e ( f );
    
    t_str v;

    INIT_TSTR( v, "77619/000/0000/R" );     e.set( TWReceiver::id1, v );
    INIT_TSTR( v, "CC952F33EE2FDBD1" );     e.set( TWReceiver::id2, v );
    INIT_TSTR( v, "0008/0008" );            e.set( TWReceiver::id3, v );
    INIT_TSTR( v, "2020-06-11T23:59:58.079949" ); e.set( TWReceiver::time, v );
    INIT_TSTR( v, "traceweb22" );           e.set( TWReceiver::server, v );
    INIT_TSTR( v, "185.151.196.174" );      e.set( ReceiverInterface::ip, v );
    INIT_TSTR( v, "CC952F33EE2FDBD1_0000SID" );   e.set( TWReceiver::sid, v );
    INIT_TSTR( v, "sra" );                  e.set( TWReceiver::service, v );
    INIT_TSTR( v, "extra " );               e.set( TWReceiver::event, v );
    INIT_TSTR( v, "issued_subhit=m_1" );    e.set( TWReceiver::msg, v );

    ASSERT_EQ ( 
        "{\"event\":\"extra \",\"id1\":\"77619/000/0000/R\",\"id2\":\"CC952F33EE2FDBD1\",\"id3\":\"0008/0008\",\"ip\":\"185.151.196.174\",\"msg\":\"issued_subhit=m_1\",\"server\":\"traceweb22\",\"service\":\"sra\",\"sid\":\"CC952F33EE2FDBD1_0000SID\",\"time\":\"2020-06-11T23:59:58.079949\"}",
        e.GetFormatter().format() );    
}

class TWEventFixture : public ::testing::Test
{
    public :
        void try_to_parse( std::string line, bool debug = false )
        {
            istringstream ss( line );
            TWParseBlockFactory pbFact;
            SingleThreadedParser p( ss, s_outputs, pbFact );
            p . setDebug( debug );
            p . parse(); // does the parsing and generates the report
        }

        std::string try_to_parse_good( std::string line, bool debug = false )
        {
            try_to_parse( line, debug );
            return s_outputs.get_good();
        }

        std::string try_to_parse_bad( std::string line, bool debug = false )
        {
            try_to_parse( line, debug );
            return s_outputs.get_bad();
        }

        std::string try_to_parse_ugly( std::string line, bool debug = false )
        {
            try_to_parse( line, debug );
            return s_outputs.get_ugly();
        }

        std::string try_to_parse_review( std::string line, bool debug = false )
        {
            try_to_parse( line, debug );
            return s_outputs.get_review();
        }

        std::string extract_value( const std::string & src, std::string key )
        {
            try
            {
                const ncbi::String input ( src );
                ncbi::JSONObjectRef obj = ncbi::JSON::parseObject ( input );
                const ncbi::String name ( key );
                ncbi::JSONValue & val = obj -> getValue( name );
                return val.toString().toSTLString();
            }
            catch(const ncbi::Exception & ex)
            {
                cerr << "extract_value():" << ex.what() << endl;
                throw;
            }
        }

        StringCatWriter s_outputs;
};

TEST_F( TWEventFixture, LineRejecting )
{
    std::string res = try_to_parse_ugly( "line1 blah\nline2\nline3\n" );
    ASSERT_EQ( 
        "{\"_line_nr\":1,\"_unparsed\":\"line1 blah\"}\n"
        "{\"_line_nr\":2,\"_unparsed\":\"line2\"}\n"
        "{\"_line_nr\":3,\"_unparsed\":\"line3\"}\n", res );
}

/*
TEST_F( LogOPEventFixture, ErrorRecovery )
{
    try_to_parse( 
        "line1 blah\n" 
        "18.207.254.142 - - [01/Jan/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "{\"_line_nr\":1,\"_unparsed\":\"line1 blah\"}\n",
                s_outputs.get_ugly() );
    ASSERT_EQ( "{\"accession\":\"\",\"agent\":\"\",\"extension\":\"\",\"filename\":\"\",\"forwarded\":\"-\",\"ip\":\"18.207.254.142\",\"method\":\"GET\",\"path\":\"\",\"port\":\"4\",\"referer\":\"-\",\"req_len\":\"5\",\"req_time\":\"3\",\"res_code\":\"1\",\"res_len\":\"2\",\"server\":\"sra-download.ncbi.nlm.nih.gov\",\"time\":\"[01/Jan/2020:02:50:24 -0500]\",\"user\":\"\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"\"}\n", 
                s_outputs.get_good() );
}

TEST_F( LogOPEventFixture, unrecognized_char )
{
    std::string res = try_to_parse_ugly( "line1 \07" );
    ASSERT_EQ( "{\"_line_nr\":1,\"_unparsed\":\"line1 \\u0007\"}\n", res );
}

TEST_F( LogOPEventFixture, parse_ip4 )
{
    std::string res = try_to_parse_good( "18.207.254.142 - - [01/Jan/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5\n" );
    ASSERT_EQ( "18.207.254.142", extract_value( res, "ip" ) );
}

TEST_F( LogOPEventFixture, parse_ip6 )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - - [01/Jan/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "0123:4567:89ab:cdef::1.2.3.4", extract_value( res, "ip" ) );
}

TEST_F( LogOPEventFixture, parse_user_dash )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - - [01/Jan/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "", extract_value( res, "user" ) );
}

TEST_F( LogOPEventFixture, parse_user )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "usr", extract_value( res, "user" ) );
}

TEST_F( LogOPEventFixture, parse_time )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "[01/Jan/2020:02:50:24 -0500]", extract_value( res, "time" ) );
}

TEST_F( LogOPEventFixture, parse_server_quoted )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "sra-download.ncbi.nlm.nih.gov", extract_value( res, "server" ) );
}

TEST_F( LogOPEventFixture, parse_server_unquoted )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] sra-download.ncbi.nlm.nih.gov \"GET\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "sra-download.ncbi.nlm.nih.gov", extract_value( res, "server" ) );
}

TEST_F( LogOPEventFixture, parse_noserver_request_full )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] \"GET url HTTP/1.1\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "", extract_value( res, "server" ) );
    ASSERT_EQ( "GET", extract_value( res, "method" ) );
    ASSERT_EQ( "url", extract_value( res, "path" ) );
    ASSERT_EQ( "HTTP/1.1", extract_value( res, "vers" ) );
}

TEST_F( LogOPEventFixture, parse_server_request_method_only )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "srv", extract_value( res, "server" ) );
    ASSERT_EQ( "GET", extract_value( res, "method" ) );
    ASSERT_EQ( "", extract_value( res, "path" ) );
    ASSERT_EQ( "", extract_value( res, "vers" ) );
}

TEST_F( LogOPEventFixture, parse_server_request_method_url )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "srv", extract_value( res, "server" ) );
    ASSERT_EQ( "GET", extract_value( res, "method" ) );
    ASSERT_EQ( "url", extract_value( res, "path" ) );
    ASSERT_EQ( "", extract_value( res, "vers" ) );
}

TEST_F( LogOPEventFixture, parse_server_request_method_url_space )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url \" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "srv", extract_value( res, "server" ) );
    ASSERT_EQ( "GET", extract_value( res, "method" ) );
    ASSERT_EQ( "url", extract_value( res, "path" ) );
    ASSERT_EQ( "", extract_value( res, "vers" ) );
}

TEST_F( LogOPEventFixture, parse_server_request_full_extra )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url HTTP/1.1 qstr# qstr_esc\\\" POST\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "srv", extract_value( res, "server" ) );
    ASSERT_EQ( "GET", extract_value( res, "method" ) );
    ASSERT_EQ( "url", extract_value( res, "path" ) );
    ASSERT_EQ( "HTTP/1.1", extract_value( res, "vers" ) );
}

TEST_F( LogOPEventFixture, parse_server_bad_request )
{   // TODO: report as bad or review
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"me bad\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "srv", extract_value( res, "server" ) );
    ASSERT_THROW( extract_value( res, "method" ), ncbi::Exception );
    ASSERT_THROW( extract_value( res, "path" ), ncbi::Exception );
    ASSERT_THROW( extract_value( res, "vers" ), ncbi::Exception );
}

TEST_F( LogOPEventFixture, parse_request_accession )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] \"GET /srapub/SRR5385591.sra HTTP/1.1\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "/srapub/SRR5385591.sra", extract_value( res, "path" ) );
    ASSERT_EQ( "SRR5385591", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR5385591", extract_value( res, "filename" ) );
    ASSERT_EQ( ".sra", extract_value( res, "extension" ) );
}

TEST_F( LogOPEventFixture, parse_request_accession_with_params )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] \"GET /srapub/SRR5385591.sra?qqq/SRR000123?qqq.ext HTTP/1.1\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "/srapub/SRR5385591.sra?qqq/SRR000123?qqq.ext", extract_value( res, "path" ) );
    ASSERT_EQ( "SRR5385591", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR5385591", extract_value( res, "filename" ) );
    ASSERT_EQ( ".sra", extract_value( res, "extension" ) );
}

TEST_F( LogOPEventFixture, parse_request_accession_with_params_no_ext )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] \"GET /srapub/SRR5385591?qqq HTTP/1.1\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "/srapub/SRR5385591?qqq", extract_value( res, "path" ) );
    ASSERT_EQ( "SRR5385591", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR5385591", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( LogOPEventFixture, parse_request_accession_with_params_no_file )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] \"GET .ext?qqq HTTP/1.1\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( ".ext?qqq", extract_value( res, "path" ) );
    ASSERT_EQ( "", extract_value( res, "accession" ) );
    ASSERT_EQ( "", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext", extract_value( res, "extension" ) );
}

TEST_F( LogOPEventFixture, parse_request_accession_in_params )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] \"GET /?/srapub/SRR5385591.sra HTTP/1.1\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "/?/srapub/SRR5385591.sra", extract_value( res, "path" ) );
    ASSERT_EQ( "", extract_value( res, "accession" ) );
    ASSERT_EQ( "", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( LogOPEventFixture, parse_result_code )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url HTTP/1.1\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "1", extract_value( res, "res_code" ) );
}

TEST_F( LogOPEventFixture, parse_result_len )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url HTTP/1.1\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "2", extract_value( res, "res_len" ) );
}

TEST_F( LogOPEventFixture, parse_result_len_dash )
{   //TODO: review? bad?
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url HTTP/1.1\" 1 - 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "", extract_value( res, "res_len" ) );
}

TEST_F( LogOPEventFixture, parse_req_time_float )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url HTTP/1.1\" 1 2 3.14 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "3.14", extract_value( res, "req_time" ) );
}

TEST_F( LogOPEventFixture, parse_req_time_int )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url HTTP/1.1\" 1 2 3 \"-\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "3", extract_value( res, "req_time" ) );
}

TEST_F( LogOPEventFixture, parse_referer )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url HTTP/1.1\" 1 2 3 \"qstr# HTTP/1.1 qstr+esc\\\" PUT\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "qstr# HTTP/1.1 qstr+esc\\\" PUT", extract_value( res, "referer" ) );
}

TEST_F( LogOPEventFixture, parse_referer_empty )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url HTTP/1.1\" 1 2 3 \"\" \"\" \"-\" port=4 rl=5" );
    ASSERT_EQ( "", extract_value( res, "referer" ) );
}

TEST_F( LogOPEventFixture, parse_agent )
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

TEST_F( LogOPEventFixture, parse_agent_no_os )
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

TEST_F( LogOPEventFixture, parse_agent_empty )
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

TEST_F( LogOPEventFixture, parse_forwarded )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url HTTP/1.1\" 1 2 3 \"\" \"\" \"fwd\" port=4 rl=5" );
    ASSERT_EQ( "fwd", extract_value( res, "forwarded" ) );
}

TEST_F( LogOPEventFixture, parse_port )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url HTTP/1.1\" 1 2 3 \"\" \"\" \"fwd\" port=4 rl=5" );
    ASSERT_EQ( "4", extract_value( res, "port" ) );
}

TEST_F( LogOPEventFixture, parse_req_len )
{
    std::string res = try_to_parse_good( "0123:4567:89ab:cdef::1.2.3.4 - usr [01/Jan/2020:02:50:24 -0500] srv \"GET url HTTP/1.1\" 1 2 3 \"\" \"\" \"fwd\" port=4 rl=5" );
    ASSERT_EQ( "5", extract_value( res, "req_len" ) );
}
*/

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}
