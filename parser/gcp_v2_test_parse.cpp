#include <gtest/gtest.h>

#include <sstream>
#include <vector>

#include "GCP_Interface.hpp"
#include "Formatters.hpp"
#include "CatWriters.hpp"

using namespace std;
using namespace NCBI::Logging;

#define INIT_TSTR(t, s)     do { (t).p = s; (t).n = strlen(s); (t).escaped = false; } while (false)
#define INIT_TSTR_ESC(t, s) do { (t).p = s; (t).n = strlen(s); (t).escaped = true; } while (false)

TEST(LogGCPEventTest, Create)
{
    std::unique_ptr<FormatterInterface> f = make_unique<JsonLibFormatter>();
    LogGCPEvent e ( f );
}

TEST(LogOPEventTest, Setters)
{
    std::unique_ptr<FormatterInterface> f = make_unique<JsonLibFormatter>();
    LogGCPEvent e ( f );
    
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
        acc_before
    };
    e.setRequest( r );

    INIT_TSTR( v, "tim"); e.set(LogGCPEvent::time, v);
    INIT_TSTR( v, "ipt"); e.set(LogGCPEvent::ip_type, v);
    INIT_TSTR( v, "ipr");e.set(LogGCPEvent::ip_region, v);
    INIT_TSTR( v, "uri"); e.set(LogGCPEvent::uri, v);
    INIT_TSTR( v, "sta"); e.set(LogGCPEvent::status, v);
    INIT_TSTR( v, "rqb"); e.set(LogGCPEvent::request_bytes, v);
    INIT_TSTR( v, "rsb"); e.set(LogGCPEvent::result_bytes, v);
    INIT_TSTR( v, "tt"); e.set(LogGCPEvent::time_taken, v);
    INIT_TSTR( v, "h"); e.set(LogGCPEvent::host, v);
    INIT_TSTR( v, "rid"); e.set(LogGCPEvent::request_id, v);
    INIT_TSTR( v, "ope"); e.set(LogGCPEvent::operation, v);
    INIT_TSTR( v, "buc"); e.set(LogGCPEvent::bucket, v);

    stringstream out;
    ASSERT_EQ ( 
        "{\"accession\":\"a\",\"agent\":\"o\",\"bucket\":\"buc\",\"extension\":\"e\",\"filename\":\"f\",\"host\":\"h\",\"ip\":\"i_p\",\"ip_region\":\"ipr\",\"ip_type\":\"ipt\",\"method\":\"m\",\"operation\":\"ope\",\"path\":\"p\",\"referer\":\"ref\",\"request_bytes\":\"rqb\",\"request_id\":\"rid\",\"result_bytes\":\"rsb\",\"status\":\"sta\",\"time\":\"tim\",\"time_taken\":\"tt\",\"unparsed\":\"unp\",\"uri\":\"uri\",\"vdb_libc\":\"v_l\",\"vdb_os\":\"v_o\",\"vdb_phid_compute_env\":\"v_c\",\"vdb_phid_guid\":\"v_g\",\"vdb_phid_session_id\":\"v_s\",\"vdb_release\":\"v_r\",\"vdb_tool\":\"v_t\",\"vers\":\"v\"}", 
        e.GetFormatter().format( out ).str() );    
}

class LogGCPEventFixture : public ::testing::Test
{
    public :
        void try_to_parse( std::string line, bool debug = false )
        {
            istringstream ss( line );
            GCPParseBlockFactory pbFact( false );
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

TEST_F( LogGCPEventFixture, LineRejecting )
{
    std::string res = try_to_parse_ugly( "line1 blah\nline2\nline3\n" );
    ASSERT_EQ( 
        "{\"_line_nr\":1,\"_unparsed\":\"line1 blah\"}\n"
        "{\"_line_nr\":2,\"_unparsed\":\"line2\"}\n"
        "{\"_line_nr\":3,\"_unparsed\":\"line3\"}\n", res );
}

TEST_F( LogGCPEventFixture, ErrorRecovery )
{
    try_to_parse( 
        "line1 blah\n" 
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "{\"_line_nr\":1,\"_unparsed\":\"line1 blah\"}\n",
                s_outputs.get_ugly() );
    ASSERT_EQ( "{\"accession\":\"\",\"agent\":\"\",\"bucket\":\"\",\"extension\":\"\",\"filename\":\"\",\"host\":\"\",\"ip\":\"\",\"ip_region\":\"\",\"ip_type\":\"\",\"method\":\"\",\"operation\":\"\",\"path\":\"\",\"referer\":\"\",\"request_bytes\":\"\",\"request_id\":\"\",\"result_bytes\":\"\",\"status\":\"\",\"time\":\"1\",\"time_taken\":\"\",\"uri\":\"\",\"vdb_libc\":\"\",\"vdb_os\":\"\",\"vdb_phid_compute_env\":\"\",\"vdb_phid_guid\":\"\",\"vdb_phid_session_id\":\"\",\"vdb_release\":\"\",\"vdb_tool\":\"\",\"vers\":\"\"}\n", 
                s_outputs.get_good() );
}

TEST_F( LogGCPEventFixture, unrecognized_char )
{
    std::string res = try_to_parse_ugly( "1 \07" );
    ASSERT_EQ( "{\"_line_nr\":1,\"_unparsed\":\"1 \\u0007\"}\n", res );
}

TEST_F( LogGCPEventFixture, time )
{
    string res = try_to_parse_good(
        "\"123\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "123", extract_value( res, "time" ) );
}

TEST_F( LogGCPEventFixture, ipv4 )
{
    string res = try_to_parse_good(
        "\"1\",\"18.207.254.142\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "18.207.254.142", extract_value( res, "ip" ) );
}

TEST_F( LogGCPEventFixture, ipv6 )
{
    string res = try_to_parse_good(
        "\"1\",\"0123:4567:89ab:cdef::1.2.3.4\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "0123:4567:89ab:cdef::1.2.3.4", extract_value( res, "ip" ) );
}

TEST_F( LogGCPEventFixture, ip_type )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"42\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "42", extract_value( res, "ip_type" ) );
}

TEST_F( LogGCPEventFixture, ip_region )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"area42\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "area42", extract_value( res, "ip_region" ) );
}

TEST_F( LogGCPEventFixture, status )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"200\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "200", extract_value( res, "status" ) );
}

TEST_F( LogGCPEventFixture, req_bytes )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"201\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "201", extract_value( res, "request_bytes" ) );
}

TEST_F( LogGCPEventFixture, res_bytes )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"5000\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "5000", extract_value( res, "result_bytes" ) );
}

TEST_F( LogGCPEventFixture, time_taken )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"10\",\"\",\"\",\"\",\"\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "10", extract_value( res, "time_taken" ) );
}

TEST_F( LogGCPEventFixture, host )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"host0\",\"\",\"\",\"\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "host0", extract_value( res, "host" ) );
}

TEST_F( LogGCPEventFixture, referer )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"referrer\",\"\",\"\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "referrer", extract_value( res, "referer" ) );
}

TEST_F( LogGCPEventFixture, req_id_int )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"123\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "123", extract_value( res, "request_id" ) );
}

TEST_F( LogGCPEventFixture, req_id_str )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"abc\",\"\",\"\",\"\"\n");
    ASSERT_EQ( "abc", extract_value( res, "request_id" ) );
}

TEST_F( LogGCPEventFixture, operation )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"oper\",\"\",\"\"\n");
    ASSERT_EQ( "oper", extract_value( res, "operation" ) );
}

TEST_F( LogGCPEventFixture, bucket )
{
    string res = try_to_parse_good(
        "\"1\",\"\",\"\",\"\",\"GET\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"\",\"kickme\",\"\"\n");
    ASSERT_EQ( "kickme", extract_value( res, "bucket" ) );
}

//TODO method
//TODO url

#if 0

// GCP
TEST_F ( TestParseFixture, GCP )
{
    const char * InputLine =
    "\"1589759989434690\","
    "\"35.245.177.170\","
    "\"1\","
    "\"\","
    "\"GET\","
    "\"/storage/v1/b/sra-pub-src-9/o/SRR1371108%2FCGAG_2.1.fastq.gz?fields=name&alt=json&userProject=nih-sra-datastore&projection=noAcl\","
    "\"404\","
    "\"0\","
    "\"291\","
    "\"27000\","
    "\"www.googleapis.com\","
    "\"\","
    "\"apitools gsutil/4.37 Python/2.7.13 (linux2) \\\"google\\\"-cloud-sdk/237.0.0 analytics/disabled,gzip(gfe)\","
    "\"AAANsUmaKBTw9gqOSHDOdr10MW802XI5jlNu87rTHuxhlRijModRQnNlwOd-Nxr0EHWq4iVXXEEn9LW4cHb7D6VK5gs\","
    "\"storage.objects.get\","
    "\"sra-pub-src-9\","
    "\"SRR1371108/CGAG_2.1.fastq.gz\""
    "\n";

    SLogGCPEvent e = parse_gcp( InputLine );

    ASSERT_EQ( 1589759989434690, e.time );
    ASSERT_EQ( "35.245.177.170", e.ip );
    ASSERT_EQ( 1, e.ip_type );
    ASSERT_EQ( "", e.ip_region );
    ASSERT_EQ( "GET", e.request.method );
    ASSERT_EQ( "/storage/v1/b/sra-pub-src-9/o/SRR1371108%2FCGAG_2.1.fastq.gz?fields=name&alt=json&userProject=nih-sra-datastore&projection=noAcl", e . uri );
    ASSERT_EQ( 404, e . status );
    ASSERT_EQ( 0, e . request_bytes );
    ASSERT_EQ( 291, e . result_bytes );
    ASSERT_EQ( 27000, e . time_taken );
    ASSERT_EQ( "www.googleapis.com", e . host );
    ASSERT_EQ( "", e . referer );
    ASSERT_EQ( "apitools gsutil/4.37 Python/2.7.13 (linux2) \\\"google\\\"-cloud-sdk/237.0.0 analytics/disabled,gzip(gfe)", e . agent . original);
    ASSERT_EQ( "AAANsUmaKBTw9gqOSHDOdr10MW802XI5jlNu87rTHuxhlRijModRQnNlwOd-Nxr0EHWq4iVXXEEn9LW4cHb7D6VK5gs", e . request_id );
    ASSERT_EQ( "storage.objects.get", e . operation );
    ASSERT_EQ( "sra-pub-src-9", e . bucket );
    ASSERT_EQ( "SRR1371108/CGAG_2.1.fastq.gz", e . request . path );
    ASSERT_EQ( "SRR1371108", e . request . accession );
    ASSERT_EQ( "CGAG_2", e . request . filename );
    ASSERT_EQ( ".1.fastq.gz", e . request . extension );
}

TEST_F ( TestParseFixture, GCP_EmptyAgent )
{
    const char * InputLine =
    "\"1591118933830501\",\"35.202.252.53\",\"1\",\"\",\"GET\",\"/sra-pub-run-8/SRR10303547/SRR10303547.1?GoogleAccessId=data-access-service%40nih-sra-datastore.iam.gserviceaccount.com&Expires=1591478828&userProject=nih-sra-datastore&Signature=ZNzj62MP4PWwSVvtkmsB97Lu33wQq4cFGLyWRJTcb%2F8h1BvVXi3lokOoT16ihScR%0At2EHti%2FgQ80VVMv9BGpAY%2FQ9HTqXeq57N53tMcjXQQMKFVttyXgIW89OLWO0UC0h%0AZdFq5AcKZywgnZql8z3RoaQi%2FPKdrdMO803tW%2Bxe%2Boy8sCd%2FyCXcG9jBrkGbdqPc%0A3xnyuycW1Va4LHIh4muGGdFSIqBk7oaLjkjLV54L8e4InzFMD3Kx0Q5raIlNadxx%0AIX%2B2hoPJuCdSh6IxEikrvUri%2Fd6i9Nqo%2BkZ%2BPSGtvlah9I9AafXrs3EAlwZkvc%2Bp%0AnjssKH8zalZQ5SmPpfHImQ%3D%3D%0A\",\"206\",\"0\",\"262144\",\"31000\",\"storage.googleapis.com\",\"\",\"\",\"AAANsUnhN-04LMubyLe-H4MQzIYbbqFwxT85S0jQpptnyQoxcHZP2JsKPbvPI9OK7TJkHIZRBcc4vvt6atty7aj6UoY\",\"GET_Object\",\"sra-pub-run-8\",\"SRR10303547/SRR10303547.1\""
    "\n";

    SLogGCPEvent e = parse_gcp( InputLine );
    ASSERT_EQ( "", e.agent.original );
    ASSERT_EQ( "", e.agent.vdb_os );
    ASSERT_EQ( "", e.agent.vdb_tool );
    ASSERT_EQ( "", e.agent.vdb_release );
    ASSERT_EQ( "", e.agent.vdb_phid_compute_env );
    ASSERT_EQ( "", e.agent.vdb_phid_guid );
    ASSERT_EQ( "", e.agent.vdb_phid_session_id );
    ASSERT_EQ( "", e.agent.vdb_libc );
}

TEST_F ( TestParseFixture, GCP_EmptyIP_EmptyURI )
{
    const char * InputLine =
    "\"1591062496651000\",\"\",\"\",\"\",\"PUT\",\"\",\"200\",\"\",\"\",\"\",\"\",\"\",\"GCS Lifecycle Management\",\"02478f5fe3deaa079d1a4f6033043a80\",\"PUT_Object\",\"sra-pub-src-3\",\"SRR1269314/s_3_11000.p1_export.txt.gzb_fixed.bam\""
    "\n";

    SLogGCPEvent e = parse_gcp( InputLine );
    ASSERT_EQ( "", e . ip );
    ASSERT_EQ( "", e . uri );
}

TEST_F ( TestParseFixture, GCP_EmptyIP_ReqId_Numeric )
{
    const char * InputLine =
    "\"1554306916471623\",\"35.245.77.223\",\"1\",\"\",\"GET\",\"/storage/v1/b/sra-pub-run-1/o/SRR002994%2FSRR002994.2?fields=name&alt=json&userProject=nih-sra-datastore&projection=noAcl\",\"404\",\"0\",\"0\",\"42000\",\"www.googleapis.com\",\"\",\"apitools gsutil/4.37 Python/2.7.13 (linux2) google-cloud-sdk/237.0.0 analytics/disabled,gzip(gfe)\",\"19919634438459959682894277668675\",\"storage.objects.get\",\"sra-pub-run-1\",\"SRR002994/SRR002994.2\""
    "\n";

    SLogGCPEvent e = parse_gcp( InputLine );
    ASSERT_EQ( "19919634438459959682894277668675", e.request_id );
}

TEST_F ( TestParseFixture, GCP_ErrorRecovery )
{
    const char * InputLine =
    "\"1554306916471623\",\"nonsense\",\"1\",\"\",\"GET\",\"/storage/v1/b/sra-pub-run-1/o/SRR002994%2FSRR002994.2?fields=name&alt=json&userProject=nih-sra-datastore&projection=noAcl\",\"404\",\"0\",\"0\",\"42000\",\"www.googleapis.com\",\"\",\"apitools gsutil/4.37 Python/2.7.13 (linux2) google-cloud-sdk/237.0.0 analytics/disabled,gzip(gfe)\",\"19919634438459959682894277668675\",\"storage.objects.get\",\"sra-pub-run-1\",\"SRR002994/SRR002994.2\""
    "\n"
    
    "\"1554306916471623\",\"35.245.77.223\",\"1\",\"\",\"GET\",\"/storage/v1/b/sra-pub-run-1/o/SRR002994%2FSRR002994.2?fields=name&alt=json&userProject=nih-sra-datastore&projection=noAcl\",\"404\",\"0\",\"0\",\"42000\",\"www.googleapis.com\",\"\",\"apitools gsutil/4.37 Python/2.7.13 (linux2) google-cloud-sdk/237.0.0 analytics/disabled,gzip(gfe)\",\"19919634438459959682894277668675\",\"storage.objects.get\",\"sra-pub-run-1\",\"SRR002994/SRR002994.2\""
    "\n"    ;

    SLogGCPEvent e = parse_gcp( InputLine );
    ASSERT_EQ ( 1, m_lines.m_rejected.size() ); // line 1
    ASSERT_EQ ( 1, m_lines.m_accepted.size() ); // line 2
    ASSERT_EQ ( 0, m_lines.m_unrecognized.size() );    
}

TEST_F ( TestParseFixture, GCP_rejected )
{
    const char * InputLine =
    "\"1588261829246636\",\"35.245.218.83\",\"1\",\"\",\"POST\",\"/resumable/upload/storage/v1/b/sra-pub-src-14/o?fields=generation%2CcustomerEncryption%2Cmd5Hash%2Ccrc32c%2Cetag%2Csize&alt=json&userProject=nih-sra-datastore&uploadType=resumable\",\"200\",\"1467742168\",\"158\",\"11330000\",\"www.googleapis.com\",\"\",\"apitools gsutil/4.37 Python/2.7.13 (linux2) google-cloud-sdk/237.0.0 analytics/disabled,gzip(gfe)\",\"AAANsUnxuPe3SnDN8Y2xbJ2y94VV3u924Bfq6MLxdYC5L6aemGMz3KGEFHWBlJnz96leDkMCkJZFJO-40Rw7wdV__fs\",\"storage.objects.insert\",\"sra-pub-src-14\",\"SRR1929577/{control_24h_biorep1}.fastq.gz\"";

    SLogGCPEvent e = parse_gcp( InputLine );
    ASSERT_EQ ( 1, m_lines.m_accepted.size() );
}

TEST_F ( TestParseFixture, GCP_UnparsedInput_WhenRejected )
{
    const char * InputLine =
    "\"1554306916471623\",\"nonsense\",\"1\",\"\",\"GET\",\"/storage/v1/b/sra-pub-run-1/o/SRR002994%2FSRR002994.2?fields=name&alt=json&userProject=nih-sra-datastore&projection=noAcl\",\"404\",\"0\",\"0\",\"42000\",\"www.googleapis.com\",\"\",\"apitools gsutil/4.37 Python/2.7.13 (linux2) google-cloud-sdk/237.0.0 analytics/disabled,gzip(gfe)\",\"19919634438459959682894277668675\",\"storage.objects.get\",\"sra-pub-run-1\",\"SRR002994/SRR002994.2\"";

    std::istringstream inputstream( InputLine );
    {
        GCP_Parser p( m_lines, inputstream );
        p.parse();
        ASSERT_EQ ( 1, m_lines.m_rejected.size() ); 
        ASSERT_EQ ( string (InputLine), m_lines.m_rejected.front().unparsed);
        ASSERT_EQ ( 0, m_lines.m_accepted.size() ); 
        ASSERT_EQ ( 0, m_lines.m_unrecognized.size() );    
    }
}

TEST_F ( TestParseFixture, GCP_Object_URL_Acc_File_Ext )
{
    const char * InputLine =
    "\"1554306916471623\",\"35.245.218.83\",\"1\",\"\",\"GET\",\"/storage/v1/b/sra-pub-run-1/o/SRR002994%2FSRR002994.2?fields=name&alt=json&userProject=nih-sra-datastore&projection=noAcl\",\"404\",\"0\",\"0\",\"42000\",\"www.googleapis.com\",\"\",\"apitools gsutil/4.37 Python/2.7.13 (linux2) google-cloud-sdk/237.0.0 analytics/disabled,gzip(gfe)\",\"19919634438459959682894277668675\",\"storage.objects.get\",\"sra-pub-run-1\","
    "\"SRR002994/qwe.2\"";

    std::istringstream inputstream( InputLine );
    {
        GCP_Parser p( m_lines, inputstream );
        SLogGCPEvent e = parse_gcp( InputLine );
        ASSERT_EQ( "SRR002994/qwe.2", e . request . path );
        ASSERT_EQ( "SRR002994", e . request . accession );
        ASSERT_EQ( "qwe", e . request . filename );
        ASSERT_EQ( ".2", e . request . extension );
    }
}

TEST_F ( TestParseFixture, GCP_Object_URL_Acc_Acc_Ext )
{
    const char * InputLine =
    "\"1554306916471623\",\"35.245.218.83\",\"1\",\"\",\"GET\",\"/storage/v1/b/sra-pub-run-1/o/SRR002994%2FSRR002994.2?fields=name&alt=json&userProject=nih-sra-datastore&projection=noAcl\",\"404\",\"0\",\"0\",\"42000\",\"www.googleapis.com\",\"\",\"apitools gsutil/4.37 Python/2.7.13 (linux2) google-cloud-sdk/237.0.0 analytics/disabled,gzip(gfe)\",\"19919634438459959682894277668675\",\"storage.objects.get\",\"sra-pub-run-1\","
    "\"SRR002994/SRR002994.2\"";

    std::istringstream inputstream( InputLine );
    {
        GCP_Parser p( m_lines, inputstream );
        SLogGCPEvent e = parse_gcp( InputLine );
        ASSERT_EQ( "SRR002994/SRR002994.2", e . request . path );
        ASSERT_EQ( "SRR002994", e . request . accession );
        ASSERT_EQ( "SRR002994", e . request . filename );
        ASSERT_EQ( ".2", e . request . extension );
    }
}

TEST_F ( TestParseFixture, GCP_Object_URL_Acc__Ext )
{
    const char * InputLine =
    "\"1554306916471623\",\"35.245.218.83\",\"1\",\"\",\"GET\",\"/storage/v1/b/sra-pub-run-1/o/SRR002994%2FSRR002994.2?fields=name&alt=json&userProject=nih-sra-datastore&projection=noAcl\",\"404\",\"0\",\"0\",\"42000\",\"www.googleapis.com\",\"\",\"apitools gsutil/4.37 Python/2.7.13 (linux2) google-cloud-sdk/237.0.0 analytics/disabled,gzip(gfe)\",\"19919634438459959682894277668675\",\"storage.objects.get\",\"sra-pub-run-1\","
    "\"SRR002994/.2\"";

    std::istringstream inputstream( InputLine );
    {
        GCP_Parser p( m_lines, inputstream );
        SLogGCPEvent e = parse_gcp( InputLine );
        ASSERT_EQ( "SRR002994/.2", e . request . path );
        ASSERT_EQ( "SRR002994", e . request . accession );
        ASSERT_EQ( "", e . request . filename );
        ASSERT_EQ( ".2", e . request . extension ); // an explicitly empty filename, so do not use the accession to populate it
    }
}

TEST_F ( TestParseFixture, GCP_Object_URL_Acc_File_ )
{
    const char * InputLine =
    "\"1554306916471623\",\"35.245.218.83\",\"1\",\"\",\"GET\",\"/storage/v1/b/sra-pub-run-1/o/SRR002994%2FSRR002994.2?fields=name&alt=json&userProject=nih-sra-datastore&projection=noAcl\",\"404\",\"0\",\"0\",\"42000\",\"www.googleapis.com\",\"\",\"apitools gsutil/4.37 Python/2.7.13 (linux2) google-cloud-sdk/237.0.0 analytics/disabled,gzip(gfe)\",\"19919634438459959682894277668675\",\"storage.objects.get\",\"sra-pub-run-1\","
    "\"SRR002994/2\"";

    std::istringstream inputstream( InputLine );
    {
        GCP_Parser p( m_lines, inputstream );
        SLogGCPEvent e = parse_gcp( InputLine );
        ASSERT_EQ( "SRR002994/2", e . request . path );
        ASSERT_EQ( "SRR002994", e . request . accession );
        ASSERT_EQ( "2", e . request . filename );
        ASSERT_EQ( "", e . request . extension );
    }
}

TEST_F ( TestParseFixture, GCP_Object_URL_NoAccession )
{
    const char * InputLine =
    "\"1554306916471623\",\"35.245.218.83\",\"1\",\"\",\"GET\",\"/storage/v1/b/sra-pub-run-1/o/SRR002994%2FSRR002994.2?fields=name&alt=json&userProject=nih-sra-datastore&projection=noAcl\",\"404\",\"0\",\"0\",\"42000\",\"www.googleapis.com\",\"\",\"apitools gsutil/4.37 Python/2.7.13 (linux2) google-cloud-sdk/237.0.0 analytics/disabled,gzip(gfe)\",\"19919634438459959682894277668675\",\"storage.objects.get\",\"sra-pub-run-1\","
    "\"qwe.2222222\"";

    std::istringstream inputstream( InputLine );
    SLogGCPEvent e = parse_gcp( InputLine );
    // no accession in the object field ("qwe.2"), so we parse the original request
    ASSERT_EQ( "/storage/v1/b/sra-pub-run-1/o/SRR002994%2FSRR002994.2?fields=name&alt=json&userProject=nih-sra-datastore&projection=noAcl", e . request . path ); 
    ASSERT_EQ( "SRR002994", e . request . accession );
    ASSERT_EQ( "SRR002994", e . request . filename );
    ASSERT_EQ( ".2", e . request . extension ); // not .2222222 from the object field
}

TEST_F ( TestParseFixture, GCP_object_empty_and_accession_in_path_parameters )
{
    const char * InputLine =
    "\"1589021828012489\",\"35.186.177.30\",\"1\",\"\",\"GET\",\"/storage/v1/b/sra-pub-src-8/o?projection=noAcl&versions=False&fields=prefixes%2CnextPageToken%2Citems%2Fname&userProject=nih-sra-datastore&prefix=SRR1755353%2FMetazome_Annelida_timecourse_sample_0097.fastq.gz&anothrPrefix=SRR77777777&maxResults=1000&delimiter=%2F&alt=json\",\"200\",\"0\",\"3\",\"27000\",\"www.googleapis.com\",\"\",\"apitools gsutil/4.37 Python/2.7.13 (linux2) google-cloud-sdk/237.0.0 analytics/disabled,gzip(gfe)\",\"AAANsUm2OYFw_ABbdF3IlhsUe5t4NuypE_5950pi4Ox34TnnKVjsaHA7cvogPp3T5-ePgGqCBLiMsai2BLhDcRhZ95E\",\"\",\"sra-pub-src-8\",\"\"";

    SLogGCPEvent e = parse_gcp( InputLine );
    ASSERT_EQ( "/storage/v1/b/sra-pub-src-8/o?projection=noAcl&versions=False&fields=prefixes%2CnextPageToken%2Citems%2Fname&userProject=nih-sra-datastore&prefix=SRR1755353%2FMetazome_Annelida_timecourse_sample_0097.fastq.gz&anothrPrefix=SRR77777777&maxResults=1000&delimiter=%2F&alt=json", e . request . path );
    ASSERT_EQ( "SRR1755353", e . request . accession );
    ASSERT_EQ( "Metazome_Annelida_timecourse_sample_0097", e . request . filename );
    ASSERT_EQ( ".fastq.gz", e . request . extension );
}

TEST_F ( TestParseFixture, GCP_object_empty_only_accession_in_request )
{
    const char * InputLine =
    "\"1589021828012489\",\"35.186.177.30\",\"1\",\"\",\"GET\",\"/storage/v1/b/sra-pub-src-8/o?projection=noAcl&versions=False&fields=prefixes%2CnextPageToken%2Citems%2Fname&userProject=nih-sra-datastore&prefix=SRR1755353\",\"200\",\"0\",\"3\",\"27000\",\"www.googleapis.com\",\"\",\"apitools gsutil/4.37 Python/2.7.13 (linux2) google-cloud-sdk/237.0.0 analytics/disabled,gzip(gfe)\",\"AAANsUm2OYFw_ABbdF3IlhsUe5t4NuypE_5950pi4Ox34TnnKVjsaHA7cvogPp3T5-ePgGqCBLiMsai2BLhDcRhZ95E\",\"\",\"sra-pub-src-8\",\"\"";

    SLogGCPEvent e = parse_gcp( InputLine );
    ASSERT_EQ( "/storage/v1/b/sra-pub-src-8/o?projection=noAcl&versions=False&fields=prefixes%2CnextPageToken%2Citems%2Fname&userProject=nih-sra-datastore&prefix=SRR1755353", e . request . path );
    ASSERT_EQ( "SRR1755353", e . request . accession );
    ASSERT_EQ( "SRR1755353", e . request . filename );
    ASSERT_EQ( "", e . request . extension );
}

TEST_F ( TestParseFixture, GCP_object_filename_has_equal_sign )
{
    const char * InputLine =
    "\"1589943868942178\",\"35.199.59.195\",\"1\",\"\",\"GET\",\"/storage/v1/b/sra-pub-src-10/o/SRR11453435%2Frun1912_lane2_read1_indexN705-S506%3D122016_Cell94-F12.fastq.gz.1?fields=name&alt=json&userProject=nih-sra-datastore&projection=noAcl\",\"404\",\"0\",\"387\",\"29000\",\"www.googleapis.com\",\"\",\"apitools gsutil/4.37 Python/2.7.13 (linux2) google-cloud-sdk/237.0.0 analytics/disabled,gzip(gfe)\",\"AAANsUkOJYv3sKNYJZeo1FanJIcIecdxEsYa4TBKKLK88UHXdQa6nlqSjqzqMZJ7nJlD7QBNKdJwEHvmXpyVam9Kwak\",\"storage.objects.get\",\"sra-pub-src-10\",\"SRR11453435/run1912_lane2_read1_indexN705-S506=122016_Cell94-F12.fastq.gz.1\"";

    SLogGCPEvent e = parse_gcp( InputLine );
    ASSERT_EQ( "SRR11453435", e . request . accession );
    ASSERT_EQ( "run1912_lane2_read1_indexN705-S506=122016_Cell94-F12", e . request . filename );
    ASSERT_EQ( ".fastq.gz.1", e . request . extension );
}

TEST_F ( TestParseFixture, GCP_object_ext_has_equal_sign )
{
    const char * InputLine =
    "\"1589943868942178\",\"35.199.59.195\",\"1\",\"\",\"GET\",\"/storage/v1/b/sra-pub-src-10/o/SRR11453435%2Frun1912_lane2_read1_indexN705-S506%3D122016_Cell94-F12.fastq.gz.1?fields=name&alt=json&userProject=nih-sra-datastore&projection=noAcl\",\"404\",\"0\",\"387\",\"29000\",\"www.googleapis.com\",\"\",\"apitools gsutil/4.37 Python/2.7.13 (linux2) google-cloud-sdk/237.0.0 analytics/disabled,gzip(gfe)\",\"AAANsUkOJYv3sKNYJZeo1FanJIcIecdxEsYa4TBKKLK88UHXdQa6nlqSjqzqMZJ7nJlD7QBNKdJwEHvmXpyVam9Kwak\",\"storage.objects.get\",\"sra-pub-src-10\",\"SRR11453435/run1912_lane2_read1_indexN705-S506_122016_Cell94-F12.fastq.gz=1\"";

    SLogGCPEvent e = parse_gcp( InputLine );
    ASSERT_EQ( "SRR11453435", e . request . accession );
    ASSERT_EQ( "run1912_lane2_read1_indexN705-S506_122016_Cell94-F12", e . request . filename );
    ASSERT_EQ( ".fastq.gz=1", e . request . extension );
}

TEST_F ( TestParseFixture, GCP_object_filename_has_spaces_and_ampersands )
{
    const char * InputLine =
    "\"1589988801294070\",\"34.86.94.122\",\"1\",\"\",\"GET\",\"/storage/v1/b/sra-pub-src-12/o/SRR11509941%2FLZ017%26LZ018%26LZ019_2%20sample%20taq.fastq.gz.1?fields=name&alt=json&userProject=nih-sra-datastore&projection=noAcl\",\"404\",\"0\",\"343\",\"24000\",\"www.googleapis.com\",\"\",\"apitools gsutil/4.37 Python/2.7.13 (linux2) google-cloud-sdk/237.0.0 analytics/disabled,gzip(gfe)\",\"AAANsUloSsCf-idOT-SgYBGO8s8SdXw4zhnQtvdmCtC-ctONXnJ59iRUMEE5ToS7MhsA3Rh38QNfoGSckZswCZLSmrM\",\"storage.objects.get\",\"sra-pub-src-12\",\"SRR11509941/LZ017&LZ018&LZ019_2 sample taq.fast&q.gz.1\"";

    SLogGCPEvent e = parse_gcp( InputLine );
    ASSERT_EQ( "SRR11509941", e . request . accession );
    ASSERT_EQ( "LZ017&LZ018&LZ019_2 sample taq", e . request . filename );
    ASSERT_EQ( ".fast&q.gz.1", e . request . extension );
}

TEST_F ( TestParseFixture, GCP_double_accession )
{
    const char * InputLine =
    "\"1590718096562025\",\"130.14.28.7\",\"1\",\"\",\"GET\",\"/storage/v1/b/sra-pub-sars-cov2/o/sra-src%2FSRR004257%2FSRR004257?fields=updated%2Cname%2CtimeCreated%2Csize&alt=json&userProject=nih-sra-datastore&projection=noAcl\",\"404\",\"0\",\"297\",\"40000\",\"storage.googleapis.com\",\"\",\"apitools gsutil/4.46 Python/2.7.5 (linux2) google-cloud-sdk/274.0.1 analytics/disabled,gzip(gfe)\",\"AAANsUl0Ofxs9M0aVz_qBWEFs-oNbk42zNRcrU6KXN5hGz3odbZ9v3_Hr_XAMIkNgsd-iYKmTR3RnQqr37E8jeEFJsE\",\"storage.objects.get\",\"sra-pub-sars-cov2\",\"sra-src/SRR004257/SRR004257\"";

    SLogGCPEvent e = parse_gcp( InputLine );
    ASSERT_EQ( "SRR004257", e . request . accession );
    ASSERT_EQ( "SRR004257", e . request . filename );
    ASSERT_EQ( "", e . request . extension );
}

TEST_F ( TestParseFixture, GCP_bad_object )
{
    const char * InputLine =
    "\"1590718096562025\",\"130.14.28.7\",\"1\",\"\",\"GET\",\"\",\"404\",\"0\",\"297\",\"40000\",\"storage.googleapis.com\",\"\",\"\",\"\",\"storage.objects.get\",\"sra-pub-sars-cov2\","
    "\"SRR004257&\"";

    SLogGCPEvent e = parse_gcp( InputLine );
    ASSERT_EQ( "", e . request . path );
    ASSERT_EQ( "", e . request . accession );
    ASSERT_EQ( "", e . request . filename );
    ASSERT_EQ( "", e . request . extension );
}

TEST_F ( TestParseFixture, GCP_user_agent )
{   
    const char * InputLine =
    "\"1590718096562025\",\"130.14.28.7\",\"1\",\"\",\"GET\",\"/storage/v1/b/sra-pub-sars-cov2/o/sra-src%2FSRR004257%2FSRR004257?fields=updated%2Cname%2CtimeCreated%2Csize&alt=json&userProject=nih-sra-datastore&projection=noAcl\",\"404\",\"0\",\"297\",\"40000\",\"storage.googleapis.com\",\"\",\"linux64 sra-toolkit test-sra.2.8.2 (phid=noc7737000,libc=2.17)\",\"AAANsUl0Ofxs9M0aVz_qBWEFs-oNbk42zNRcrU6KXN5hGz3odbZ9v3_Hr_XAMIkNgsd-iYKmTR3RnQqr37E8jeEFJsE\",\"storage.objects.get\",\"sra-pub-sars-cov2\",\"sra-src/SRR004257/SRR004257\"";

    SLogGCPEvent e = parse_gcp( InputLine );
    ASSERT_EQ( "linux64 sra-toolkit test-sra.2.8.2 (phid=noc7737000,libc=2.17)", e.agent.original );
    ASSERT_EQ( "linux64", e.agent.vdb_os );
    ASSERT_EQ( "test-sra", e.agent.vdb_tool );
    ASSERT_EQ( "2.8.2", e.agent.vdb_release );
    ASSERT_EQ( "noc", e.agent.vdb_phid_compute_env );
    ASSERT_EQ( "7737", e.agent.vdb_phid_guid );
    ASSERT_EQ( "000", e.agent.vdb_phid_session_id );
    ASSERT_EQ( "2.17", e.agent.vdb_libc );
}
#endif

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}
