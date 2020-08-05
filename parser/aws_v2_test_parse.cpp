#include <gtest/gtest.h>

#include "Formatters.hpp"
#include "CatWriters.hpp"
#include "AWS_Interface.hpp"

using namespace std;
using namespace NCBI::Logging;

#define INIT_TSTR(t, s)     do { (t).p = s; (t).n = strlen(s); (t).escaped = false; } while (false)
#define INIT_TSTR_ESC(t, s) do { (t).p = s; (t).n = strlen(s); (t).escaped = true; } while (false)

TEST(LogAWSEventTest, Create)
{
    JsonLibFormatter f;
    LogAWSEvent e ( & f );
}

TEST(LogAWSEventTest, Setters)
{
    JsonLibFormatter f;
    LogAWSEvent e ( f );
    
    t_str v;
    INIT_TSTR( v, "i_p");
    e.set( LogLinesInterface::ip, v );
    INIT_TSTR( v, "ref");
    e.set( LogLinesInterface::referer, v );
    INIT_TSTR( v, "unp");
    e.set( LogLinesInterface::unparsed, v );
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
        { "s", 1, false },
        acc_before
    };
    e.setRequest( r );

    INIT_TSTR( v, "own"); e.set(LogAWSEvent::owner, v);
    INIT_TSTR( v, "buc"); e.set(LogAWSEvent::bucket, v);
    INIT_TSTR( v, "tim"); e.set(LogAWSEvent::time, v);
    INIT_TSTR( v, "req"); e.set(LogAWSEvent::requester, v);
    INIT_TSTR( v, "req_id");e.set(LogAWSEvent::request_id, v);
    INIT_TSTR( v, "ope"); e.set(LogAWSEvent::operation, v);
    INIT_TSTR( v, "key"); e.set(LogAWSEvent::key, v);
    INIT_TSTR( v, "cod"); e.set(LogAWSEvent::res_code, v);
    INIT_TSTR( v, "err"); e.set(LogAWSEvent::error, v);
    INIT_TSTR( v, "res"); e.set(LogAWSEvent::res_len, v);
    INIT_TSTR( v, "obj"); e.set(LogAWSEvent::obj_size, v);
    INIT_TSTR( v, "tot"); e.set(LogAWSEvent::total_time, v);
    INIT_TSTR( v, "tur"); e.set(LogAWSEvent::turnaround_time, v);
    INIT_TSTR( v, "ver"); e.set(LogAWSEvent::version_id, v);
    INIT_TSTR( v, "host");e.set(LogAWSEvent::host_id, v);
    INIT_TSTR( v, "sig"); e.set(LogAWSEvent::sig_ver, v);
    INIT_TSTR( v, "cip"); e.set(LogAWSEvent::cipher_suite, v);
    INIT_TSTR( v, "aut"); e.set(LogAWSEvent::auth_type, v);
    INIT_TSTR( v, "hos"); e.set(LogAWSEvent::host_header, v);
    INIT_TSTR( v, "tls"); e.set(LogAWSEvent::tls_version, v);

    stringstream out;
    ASSERT_EQ ( 
        "{\"accession\":\"a\",\"agent\":\"o\",\"auth_type\":\"aut\",\"bucket\":\"buc\",\"cipher_suite\":\"cip\",\"error\":\"err\",\"extension\":\"e\",\"filename\":\"f\",\"host_header\":\"hos\",\"host_id\":\"host\",\"ip\":\"i_p\",\"key\":\"key\",\"method\":\"m\",\"obj_size\":\"obj\",\"operation\":\"ope\",\"owner\":\"own\",\"path\":\"p\",\"referer\":\"ref\",\"request_id\":\"req_id\",\"requester\":\"req\",\"res_code\":\"cod\",\"res_len\":\"res\",\"sig_ver\":\"sig\",\"time\":\"tim\",\"tls_version\":\"tls\",\"total_time\":\"tot\",\"turnaround_time\":\"tur\",\"unparsed\":\"unp\",\"vdb_libc\":\"v_l\",\"vdb_os\":\"v_o\",\"vdb_phid_compute_env\":\"v_c\",\"vdb_phid_guid\":\"v_g\",\"vdb_phid_session_id\":\"v_s\",\"vdb_release\":\"v_r\",\"vdb_tool\":\"v_t\",\"vers\":\"v\",\"version_id\":\"ver\"}", 
        e.GetFormatter().format( out ).str() );    
}

class LogAWSEventFixture : public ::testing::Test
{
    public :
        void try_to_parse( std::string line, bool debug = false )
        {
            istringstream ss( line );
            JsonLibFormatter jsonFmt; 
            LogAWSEvent receiver( jsonFmt );
            AWSParser p( ss, receiver, s_outputs );
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

TEST_F( LogAWSEventFixture, parse_just_dashes )
{
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( 
        "{\"accession\":\"\",\"agent\":\"\",\"auth_type\":\"\",\"bucket\":\"\",\"cipher_suite\":\"\",\"error\":\"\",\"extension\":\"\",\"filename\":\"\",\"host_header\":\"\",\"host_id\":\"\",\"ip\":\"\",\"key\":\"\",\"method\":\"\",\"obj_size\":\"\",\"operation\":\"\",\"owner\":\"\",\"path\":\"\",\"referer\":\"\",\"request_id\":\"\",\"requester\":\"\",\"res_code\":\"\",\"res_len\":\"\",\"sig_ver\":\"\",\"time\":\"\",\"tls_version\":\"\",\"total_time\":\"\",\"turnaround_time\":\"\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"\",\"version_id\":\"\"}\n",
        res
        );
}

TEST_F( LogAWSEventFixture, parse_quoted_dashes )
{
    std::string res = try_to_parse_good( "\"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\"" );
    ASSERT_EQ( 
        "{\"accession\":\"\",\"agent\":\"\",\"auth_type\":\"\",\"bucket\":\"\",\"cipher_suite\":\"\",\"error\":\"\",\"extension\":\"\",\"filename\":\"\",\"host_header\":\"\",\"host_id\":\"\",\"ip\":\"\",\"key\":\"\",\"method\":\"\",\"obj_size\":\"\",\"operation\":\"\",\"owner\":\"\",\"path\":\"\",\"referer\":\"\",\"request_id\":\"\",\"requester\":\"\",\"res_code\":\"\",\"res_len\":\"\",\"sig_ver\":\"\",\"time\":\"\",\"tls_version\":\"\",\"total_time\":\"\",\"turnaround_time\":\"\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"\",\"version_id\":\"\"}\n",
        res
        );
}

TEST_F( LogAWSEventFixture, LineRejecting )
{
    std::string res = try_to_parse_ugly( "line1 blah\nline2\nline3\n" );
    ASSERT_EQ( 
        "{\"_line_nr\":1,\"_unparsed\":\"line1 blah\",\"bucket\":\"blah\",\"owner\":\"line1\"}\n"
        "{\"_line_nr\":2,\"_unparsed\":\"line2\",\"owner\":\"line2\"}\n"
        "{\"_line_nr\":3,\"_unparsed\":\"line3\",\"owner\":\"line3\"}\n", res );
}

TEST_F( LogAWSEventFixture, ErrorRecovery )
{
    try_to_parse( 
        "line1 blah\n" 
        "- - - - - - - - - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "{\"_line_nr\":1,\"_unparsed\":\"line1 blah\",\"bucket\":\"blah\",\"owner\":\"line1\"}\n",
                s_outputs.get_ugly() );
    ASSERT_EQ( "{\"accession\":\"\",\"agent\":\"\",\"auth_type\":\"\",\"bucket\":\"\",\"cipher_suite\":\"\",\"error\":\"\",\"extension\":\"\",\"filename\":\"\",\"host_header\":\"\",\"host_id\":\"\",\"ip\":\"\",\"key\":\"\",\"method\":\"\",\"obj_size\":\"\",\"operation\":\"\",\"owner\":\"\",\"path\":\"\",\"referer\":\"\",\"request_id\":\"\",\"requester\":\"\",\"res_code\":\"\",\"res_len\":\"\",\"sig_ver\":\"\",\"time\":\"\",\"tls_version\":\"\",\"total_time\":\"\",\"turnaround_time\":\"\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"\",\"version_id\":\"\"}\n", 
                s_outputs.get_good() );
}

TEST_F( LogAWSEventFixture, unrecognized_char )
{
    std::string res = try_to_parse_ugly( "line1 \07" );
    ASSERT_EQ( "{\"_line_nr\":1,\"_unparsed\":\"line1 \\u0007\",\"owner\":\"line1\"}\n", res );
}

TEST_F( LogAWSEventFixture, parse_owner )
{
    std::string res = try_to_parse_good( "7dd4dcfe9b004fb7433c61af3e87972f2e9477fa7f0760a02827f771b41b3455 - - - - - - - - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "7dd4dcfe9b004fb7433c61af3e87972f2e9477fa7f0760a02827f771b41b3455",
                extract_value( res, "owner" ) );
}

TEST_F( LogAWSEventFixture, parse_owner_str1 )
{
    std::string res = try_to_parse_good( "123%@& - - - - - - - - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "123%@&", extract_value( res, "owner" ) );
}

TEST_F( LogAWSEventFixture, parse_bucket )
{
    std::string res = try_to_parse_good( "- abucket - - - - - - - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "abucket", extract_value( res, "bucket" ) );
}

TEST_F( LogAWSEventFixture, parse_time )
{
    std::string res = try_to_parse_good( "- - [09/May/2020:22:07:21 +0000] - - - - - - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "[09/May/2020:22:07:21 +0000]", extract_value( res, "time" ) );
}

TEST_F( LogAWSEventFixture, parse_ip4 )
{
    std::string res = try_to_parse_good( "- - - 18.207.254.142 - - - - - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "18.207.254.142", extract_value( res, "ip" ) );
}

TEST_F( LogAWSEventFixture, parse_ip6 )
{
    std::string res = try_to_parse_good( "- - - 0123:4567:89ab:cdef::1.2.3.4 - - - - - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "0123:4567:89ab:cdef::1.2.3.4", extract_value( res, "ip" ) );
}

TEST_F( LogAWSEventFixture, parse_requester )
{
    std::string res = try_to_parse_good( "- - - - arn:aws:sts::783971887864 - - - - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "arn:aws:sts::783971887864", extract_value( res, "requester" ) );
}

TEST_F( LogAWSEventFixture, parse_request_id )
{
    std::string res = try_to_parse_good( "- - - - - B6301F55C2486C74 - - - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "B6301F55C2486C74", extract_value( res, "request_id" ) );
}

TEST_F( LogAWSEventFixture, parse_operation )
{
    std::string res = try_to_parse_good( "- - - - - - REST.PUT.PART - - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "REST.PUT.PART", extract_value( res, "operation" ) );
}

TEST_F( LogAWSEventFixture, parse_key )
{
    std::string res = try_to_parse_good( "- - - - - - - ERR792423/5141526_s1_p0.bas.h5.1 - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "ERR792423/5141526_s1_p0.bas.h5.1", extract_value( res, "key" ) );
    ASSERT_EQ( "ERR792423", extract_value( res, "accession" ) );
    ASSERT_EQ( "5141526_s1_p0", extract_value( res, "filename" ) );
    ASSERT_EQ( ".bas.h5.1", extract_value( res, "extension" ) );
}

TEST_F( LogAWSEventFixture, parse_key_with_qmark )
{
    std::string res = try_to_parse_good( "- - - - - - - ERR792423/5141526_s1_p0.bas.h5.1?ab - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "ERR792423/5141526_s1_p0.bas.h5.1?ab", extract_value( res, "key" ) );
    ASSERT_EQ( "ERR792423", extract_value( res, "accession" ) );
    ASSERT_EQ( "5141526_s1_p0", extract_value( res, "filename" ) );
    ASSERT_EQ( ".bas.h5.1?ab", extract_value( res, "extension" ) );
}

TEST_F( LogAWSEventFixture, parse_key_with_pct )
{
    std::string res = try_to_parse_good( "- - - - - - - ERR792423/5141526_%s1_p0.bas.h5.1%ab - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "ERR792423/5141526_%s1_p0.bas.h5.1%ab", extract_value( res, "key" ) );
    ASSERT_EQ( "ERR792423", extract_value( res, "accession" ) );
    ASSERT_EQ( "5141526_%s1_p0", extract_value( res, "filename" ) );
    ASSERT_EQ( ".bas.h5.1%ab", extract_value( res, "extension" ) );
}

TEST_F( LogAWSEventFixture, parse_key_with_eq )
{
    std::string res = try_to_parse_good( "- - - - - - - ERR792423/5141526_=s1_p0.bas.h5.1=b - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "ERR792423/5141526_=s1_p0.bas.h5.1=b", extract_value( res, "key" ) );
    ASSERT_EQ( "ERR792423", extract_value( res, "accession" ) );
    ASSERT_EQ( "5141526_=s1_p0", extract_value( res, "filename" ) );
    ASSERT_EQ( ".bas.h5.1=b", extract_value( res, "extension" ) );
}

TEST_F( LogAWSEventFixture, parse_key_with_amp )
{
    std::string res = try_to_parse_good( "- - - - - - - ERR792423/5141526_&s1_p0.bas.h5.1&b - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "ERR792423/5141526_&s1_p0.bas.h5.1&b", extract_value( res, "key" ) );
    ASSERT_EQ( "ERR792423", extract_value( res, "accession" ) );
    ASSERT_EQ( "5141526_&s1_p0", extract_value( res, "filename" ) );
    ASSERT_EQ( ".bas.h5.1&b", extract_value( res, "extension" ) );
}

TEST_F( LogAWSEventFixture, parse_key_multiple_accessions )
{   // pick the last accession
    std::string res = try_to_parse_good( "- - - - - - - SRX123456/ERR792423/5141526_s1_p0.bas.h5.1 - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "SRX123456/ERR792423/5141526_s1_p0.bas.h5.1", extract_value( res, "key" ) );
    ASSERT_EQ( "ERR792423", extract_value( res, "accession" ) );
    ASSERT_EQ( "5141526_s1_p0", extract_value( res, "filename" ) );
    ASSERT_EQ( ".bas.h5.1", extract_value( res, "extension" ) );
}

TEST_F( LogAWSEventFixture, parse_key_no_accessions )
{   // pick the last accession
    std::string res = try_to_parse_good( "- - - - - - - abc.12/5141526_s1_p0.bas.h5.1 - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "abc.12/5141526_s1_p0.bas.h5.1", extract_value( res, "key" ) );
    ASSERT_EQ( "", extract_value( res, "accession" ) );
    ASSERT_EQ( "", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( LogAWSEventFixture, parse_key_multiple_path_segments )
{   // pick the last segment
    std::string res = try_to_parse_good( "- - - - - - - SRR123456/abc.12/5141526_s1_p0.bas.h5.1 - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "SRR123456/abc.12/5141526_s1_p0.bas.h5.1", extract_value( res, "key" ) );
    ASSERT_EQ( "SRR123456", extract_value( res, "accession" ) );
    ASSERT_EQ( "5141526_s1_p0", extract_value( res, "filename" ) );
    ASSERT_EQ( ".bas.h5.1", extract_value( res, "extension" ) );
}

TEST_F( LogAWSEventFixture, parse_key_no_extension )
{   
    std::string res = try_to_parse_good( "- - - - - - - SRR123456/abc.12/5141526_s1_p0 - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "SRR123456/abc.12/5141526_s1_p0", extract_value( res, "key" ) );
    ASSERT_EQ( "SRR123456", extract_value( res, "accession" ) );
    ASSERT_EQ( "5141526_s1_p0", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( LogAWSEventFixture, parse_key_only_extension )
{   
    std::string res = try_to_parse_good( "- - - - - - - SRR123456/abc.12/.bas.h5.1 - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "SRR123456/abc.12/.bas.h5.1", extract_value( res, "key" ) );
    ASSERT_EQ( "SRR123456", extract_value( res, "accession" ) );
    ASSERT_EQ( "", extract_value( res, "filename" ) );
    ASSERT_EQ( ".bas.h5.1", extract_value( res, "extension" ) );
}

TEST_F( LogAWSEventFixture, parse_key_accession_is_filename )
{   
    std::string res = try_to_parse_good( "- - - - - - - SRR123456.1 - - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "SRR123456.1", extract_value( res, "key" ) );
    ASSERT_EQ( "SRR123456", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR123456", extract_value( res, "filename" ) );
    ASSERT_EQ( ".1", extract_value( res, "extension" ) );
}

TEST_F( LogAWSEventFixture, parse_request )
{   
    std::string res = try_to_parse_good( "- - - - - - - - \"GET ?list-type=2&delimiter=%2F&prefix=SRR11060177%2FSRR99999999/filename.1&morefilenames.moreextensions.1&name=SRR000123&encoding-type=url HTTP/1.1\" - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "GET", extract_value( res, "method" ) );
    ASSERT_EQ( "?list-type=2&delimiter=%2F&prefix=SRR11060177%2FSRR99999999/filename.1&morefilenames.moreextensions.1&name=SRR000123&encoding-type=url", extract_value( res, "path" ) );
    ASSERT_EQ( "HTTP/1.1", extract_value( res, "vers" ) );
    ASSERT_EQ( "SRR11060177", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( ".1", extract_value( res, "extension" ) );
}

TEST_F( LogAWSEventFixture, parse_request_no_ver )
{   
    std::string res = try_to_parse_good( "- - - - - - - - \"GET url \" - - - - - - - - - - - - - - -" );

    ASSERT_EQ( "GET", extract_value( res, "method" ) );
    ASSERT_EQ( "url", extract_value( res, "path" ) );
    ASSERT_EQ( "", extract_value( res, "vers" ) );
}

TEST_F( LogAWSEventFixture, parse_request_no_space_ver )
{   
    std::string res = try_to_parse_good( "- - - - - - - - \"GET url\" - - - - - - - - - - - - - - -" );

    ASSERT_EQ( "GET", extract_value( res, "method" ) );
    ASSERT_EQ( "url", extract_value( res, "path" ) );
    ASSERT_EQ( "", extract_value( res, "vers" ) );
}

TEST_F( LogAWSEventFixture, parse_request_no_url )
{   
    std::string res = try_to_parse_good( "- - - - - - - - \"GET\" - - - - - - - - - - - - - - -" );

    ASSERT_EQ( "GET", extract_value( res, "method" ) );
    ASSERT_EQ( "", extract_value( res, "path" ) );
    ASSERT_EQ( "", extract_value( res, "vers" ) );
}

TEST_F( LogAWSEventFixture, parse_request_no_method )
{   // not supported for now
    std::string res = try_to_parse_ugly( "- - - - - - - - \"url\" - - - - - - - - - - - - - - -" );
}

TEST_F( LogAWSEventFixture, parse_key_and_request )
{   // the key wins
    std::string res = try_to_parse_good( "- - - - - - - ERR792423/5141526_s1_p0.bas.h5.1 \"GET ?list-type=2&delimiter=%2F&prefix=SRR11060177%2FSRR99999999/filename.1&morefilenames.moreextensions.1&name=SRR000123&encoding-type=url HTTP/1.1\" - - - - - - - - - - - - - - -" );
    ASSERT_EQ( "GET", extract_value( res, "method" ) );
    ASSERT_EQ( "?list-type=2&delimiter=%2F&prefix=SRR11060177%2FSRR99999999/filename.1&morefilenames.moreextensions.1&name=SRR000123&encoding-type=url", extract_value( res, "path" ) );
    ASSERT_EQ( "HTTP/1.1", extract_value( res, "vers" ) );
    // the extracted values come from the key
    ASSERT_EQ( "ERR792423", extract_value( res, "accession" ) );
    ASSERT_EQ( "5141526_s1_p0", extract_value( res, "filename" ) );
    ASSERT_EQ( ".bas.h5.1", extract_value( res, "extension" ) );
}

TEST_F( LogAWSEventFixture, parse_res_code )
{   
    std::string res = try_to_parse_good( "- - - - - - - - - 200 - - - - - - - - - - - - - -" );
    ASSERT_EQ( "200", extract_value( res, "res_code" ) );
}

TEST_F( LogAWSEventFixture, parse_error )
{   
    std::string res = try_to_parse_good( "- - - - - - - - - - NoSuchKey - - - - - - - - - - - - -" );
    ASSERT_EQ( "NoSuchKey", extract_value( res, "error" ) );
}

TEST_F( LogAWSEventFixture, parse_res_len )
{   
    std::string res = try_to_parse_good( "- - - - - - - - - - - 1024 - - - - - - - - - - - -" );
    ASSERT_EQ( "1024", extract_value( res, "res_len" ) );
}

TEST_F( LogAWSEventFixture, parse_obj_size )
{   
    std::string res = try_to_parse_good( "- - - - - - - - - - - - 1024 - - - - - - - - - - -" );
    ASSERT_EQ( "1024", extract_value( res, "obj_size" ) );
}

TEST_F( LogAWSEventFixture, parse_total_time )
{   
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - 1024 - - - - - - - - - -" );
    ASSERT_EQ( "1024", extract_value( res, "total_time" ) );
}

TEST_F( LogAWSEventFixture, parse_turnaround_time )
{   
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - 1024 - - - - - - - - -" );
    ASSERT_EQ( "1024", extract_value( res, "turnaround_time" ) );
}

TEST_F( LogAWSEventFixture, parse_referrer_simple )
{   
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - referrrer - - - - - - - -" );
    ASSERT_EQ( "referrrer", extract_value( res, "referer" ) );
}

TEST_F( LogAWSEventFixture, parse_referrer_compound )
{   
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - \"referring referrrer\" - - - - - - - -" );
    ASSERT_EQ( "referring referrrer", extract_value( res, "referer" ) );
}

TEST_F( LogAWSEventFixture, parse_agent )
{   
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - - \"linux64 sra-toolkit test-sra.2.8.2 (phid=noc7737000,libc=2.17)\" - - - - - - -" );
    ASSERT_EQ( "linux64 sra-toolkit test-sra.2.8.2 (phid=noc7737000,libc=2.17)", extract_value( res, "agent" ) );
    ASSERT_EQ( "linux64", extract_value( res, "vdb_os" ) );
    ASSERT_EQ( "test-sra", extract_value( res, "vdb_tool" ) );
    ASSERT_EQ( "2.8.2", extract_value( res, "vdb_release" ) );
    ASSERT_EQ( "noc", extract_value( res, "vdb_phid_compute_env" ) );
    ASSERT_EQ( "7737", extract_value( res, "vdb_phid_guid" ) );
    ASSERT_EQ( "000", extract_value( res, "vdb_phid_session_id" ) );
    ASSERT_EQ( "2.17", extract_value( res, "vdb_libc" ) );
}

TEST_F( LogAWSEventFixture, parse_agent_no_os )
{   
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - - \"sra-toolkit test-sra.2.8.2 (phid=noc7737000,libc=2.17)\" - - - - - - -" );
    ASSERT_EQ( "sra-toolkit test-sra.2.8.2 (phid=noc7737000,libc=2.17)", extract_value( res, "agent" ) );   
    ASSERT_EQ( "", extract_value( res, "vdb_os" ) );
    ASSERT_EQ( "test-sra", extract_value( res, "vdb_tool" ) );
    ASSERT_EQ( "2.8.2", extract_value( res, "vdb_release" ) );
    ASSERT_EQ( "noc", extract_value( res, "vdb_phid_compute_env" ) );
    ASSERT_EQ( "7737", extract_value( res, "vdb_phid_guid" ) );
    ASSERT_EQ( "000", extract_value( res, "vdb_phid_session_id" ) );
    ASSERT_EQ( "2.17", extract_value( res, "vdb_libc" ) );
}

TEST_F( LogAWSEventFixture, parse_agent_os_in_middle )
{   
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - - \"sra-toolkit test-sra.2.8.2 (phid=noc7737000,libc=2.17)linux64\" - - - - - - -" );
    ASSERT_EQ( "sra-toolkit test-sra.2.8.2 (phid=noc7737000,libc=2.17)linux64", extract_value( res, "agent" ) );   
    ASSERT_EQ( "", extract_value( res, "vdb_os" ) );
    ASSERT_EQ( "test-sra", extract_value( res, "vdb_tool" ) );
    ASSERT_EQ( "2.8.2", extract_value( res, "vdb_release" ) );
    ASSERT_EQ( "noc", extract_value( res, "vdb_phid_compute_env" ) );
    ASSERT_EQ( "7737", extract_value( res, "vdb_phid_guid" ) );
    ASSERT_EQ( "000", extract_value( res, "vdb_phid_session_id" ) );
    ASSERT_EQ( "2.17", extract_value( res, "vdb_libc" ) );
}

TEST_F( LogAWSEventFixture, parse_agent_empty )
{   
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - - \"\" - - - - - - -" );
    ASSERT_EQ( "", extract_value( res, "agent" ) );   
}

TEST_F( LogAWSEventFixture, parse_version_id )
{   
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - - - 3HL4kqtJvjVBH40Nrjfkd - - - - - -" );
    ASSERT_EQ( "3HL4kqtJvjVBH40Nrjfkd", extract_value( res, "version_id" ) );
}

TEST_F( LogAWSEventFixture, parse_2_part_hostId )
{   
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - - - - AIDAISBTTLPGXGH6YFFAY LzYGhqEwXn5Xiuil9tI6JtK2PiIo+SC6Ute3Isq2qEmt/t0Z7qFkyD0mp1ZIc43bm0qSX4tBbbc= - - - - -" );
    ASSERT_EQ( "AIDAISBTTLPGXGH6YFFAY LzYGhqEwXn5Xiuil9tI6JtK2PiIo+SC6Ute3Isq2qEmt/t0Z7qFkyD0mp1ZIc43bm0qSX4tBbbc=", extract_value( res, "host_id" ) );
}

TEST_F( LogAWSEventFixture, parse_1_part_hostId_a )
{   
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - - - - AIDAISBTTLPGXGH6YFFAY - - - - -" );
    ASSERT_EQ( "AIDAISBTTLPGXGH6YFFAY", extract_value( res, "host_id" ) );
}

TEST_F( LogAWSEventFixture, parse_1_part_hostId_b )
{   
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - - - - LzYGhqEwXn5Xiuil9tI6JtK2PiIo+SC6Ute3Isq2qEmt/t0Z7qFkyD0mp1ZIc43bm0qSX4tBbbc= - - - - -" );
    ASSERT_EQ( "LzYGhqEwXn5Xiuil9tI6JtK2PiIo+SC6Ute3Isq2qEmt/t0Z7qFkyD0mp1ZIc43bm0qSX4tBbbc=", extract_value( res, "host_id" ) );
}

TEST_F( LogAWSEventFixture, parse_sig_ver )
{   
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - - - - - SigV4 - - - -" );
    ASSERT_EQ( "SigV4", extract_value( res, "sig_ver" ) );
}

TEST_F( LogAWSEventFixture, parse_cipher )
{   
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - - - - - - ECDHE-RSA-AES128-GCM-SHA256 - - -" );
    ASSERT_EQ( "ECDHE-RSA-AES128-GCM-SHA256", extract_value( res, "cipher_suite" ) );
}

TEST_F( LogAWSEventFixture, parse_auth )
{   
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - - - - - - - AuthHeader - -" );
    ASSERT_EQ( "AuthHeader", extract_value( res, "auth_type" ) );
}

TEST_F( LogAWSEventFixture, parse_host_hdr )
{   
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - - - - - - - - sra-pub-src-14.s3.amazonaws.com -" );
    ASSERT_EQ( "sra-pub-src-14.s3.amazonaws.com", extract_value( res, "host_header" ) );
}

TEST_F( LogAWSEventFixture, parse_tls )
{   
    std::string res = try_to_parse_good( "- - - - - - - - - - - - - - - - - - - - - - - TLSv1.2" );
    ASSERT_EQ( "TLSv1.2", extract_value( res, "tls_version" ) );
}

TEST_F( LogAWSEventFixture, repeated_time_shifts_tls_vers )
{
    std::string input = "922194806485875312b252374a3644f1feecd16802a50d4729885c1d11e1fd37 sra-pub-run-7 [05/Jan/2020:23:48:44 +0000] 130.14.28.32 rn:aws:iam::783971922194806485875312b252374a3644f1feecd16802a50d4729885c1d11e1fd37 sra-pub-run-6 [05/Jan/2020:23:10:20 +0000] 130.14.28.140 arn:aws:iam::651740271041:user/sddp-1-ncbi-verifier A5D9D5749E3175C8 REST.GET.BUCKET - \"GET /?prefix=SRR7087357%2FSRR7087357.1&encoding-type=url HTTP/1.1\" 403 AccessDenied 243 - 14 - \"-\" \"aws-cli/1.16.87 Python/3.7.3 Linux/3.10.0-1062.7.1.el7.x86_64 botocore/1.12.77\" - kgI+NnID7YrclgTiImbWYaTJ9nRbiOsmQw/SnkihnWcpM67sWFjyx6T/YH9CnrlJV2wZit4LbSo= SigV4 ECDHE-RSA-AES128-GCM-SHA256 AuthHeader sra-pub-run-6.s3.amazonaws.com TLSv1.2";
    std::string res = try_to_parse_bad( input );
}

TEST_F( LogAWSEventFixture, percent_in_key )
{
    std::string input = "922194806485875312b252374a3644f1feecd16802a50d4729885c1d11e1fd37 sra-pub-src-13 [27/Jul/2020:22:56:40 +0000] 35.175.201.205 arn:aws:sts::783971887864:assumed-role/sra-developer-instance-profile-role/i-0b6b034f8f9b482fb 8C244990662F952E REST.PUT.PART SRR12318371/run2249_lane2_read2_indexN703%253DDropSeq_Potter_BrainWT_8_17_17_N703.fastq.gz.1 \"PUT /SRR12318371/run2249_lane2_read2_indexN703%3DDropSeq_Potter_BrainWT_8_17_17_N703.fastq.gz.1?partNumber=4&uploadId=qpR5BCc2akFqypS4spNO3VQiVAFHRYcSPNoZgZCJSQVh71SLNrUQLeARnEaBGhGGnrZihGRLY1T1pJ055KFXvVa7RYMLm1VM.t6ZKJt2zY16rP0q_wgAcu2aY4eNfrFWOe0GEl7xjYv2ZBe99zmqMg-- HTTP/1.1\" 200 - - 8388608 743 47 \"-\" \"aws-cli/1.16.102 Python/2.7.16 Linux/4.14.171-105.231.amzn1.x86_64 botocore/1.12.92\" - Tfy76PKfrEHlL2+E2a1wC87RDinpRfa1tngc77ETpa94hBUbT/GxSWE/SqHWK8iC8DLTqIfRn3w= SigV4 ECDHE-RSA-AES128-GCM-SHA256 AuthHeader sra-pub-src-13.s3.amazonaws.com TLSv1.2";
    std::string res = try_to_parse_good( input );
    ASSERT_EQ( "SRR12318371/run2249_lane2_read2_indexN703%253DDropSeq_Potter_BrainWT_8_17_17_N703.fastq.gz.1", extract_value( res, "key" ) );
}

//TODO: reportField

#if 0
TEST (TestHelper, StringEscaped)
{
    JsonFormatter f;
    t_str v;
    INIT_TSTR_ESC( v, "\\\"");
    string s = f.format( "key", v );
    cout << endl << s << endl;
    ASSERT_EQ ( "{\"key\":\"\\\"\"}", s );
}
TEST (TestHelper, StringEscaped_Multiple)
{
    t_str s = { "abc\\\"\\\"", 7, true };
    ASSERT_EQ( string("abc\"\""), ToString( s ) );
}
TEST (TestHelper, StringEscaped_TrailingSlash)
{
    t_str s = { "\\", 1, true };
    ASSERT_EQ( string("\\"), ToString( s ) );
}
TEST (TestHelper, StringEscaped_NotEscapedCharacter)
{
    t_str s = { "\\n", 2, true };
    ASSERT_EQ( string("\\n"), ToString( s ) );
}
#endif

TEST_F( LogAWSEventFixture, MultiThreading )
{
    std::string input( 
        "1 - - - - - - - - - - - - - - - - - - - - - - x\n" // ugly
        "2 - - - - - - - - - - - - - - - - - - - - - - x\n" // ugly
        "3 - - - - - - - - - - - - - - - - - - - - - - -\n" // good
        "4 - - - - - - - - - - - - - - - - - - - - - - x\n" // ugly
    );

    istringstream ss( input );
    AWSMultiThreadedParser p( ss, s_outputs, 100, 2 );
    p . parse();

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
}

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}

