#include <gtest/gtest.h>

#include <sstream>
#include <vector>

#include "log_lines.hpp"
#include "helper.hpp"
#include "test_helper.hpp"

using namespace std;
using namespace NCBI::Logging;

struct SLogOPEvent
{
    string      ip;
    t_timepoint time;
    SRequest    request;
    int64_t     res_code;
    int64_t     res_len;
    string      referer;
    SAgent      agent;

    string      user;
    string      req_time;
    int64_t     port;
    int64_t     req_len;
    string      forwarded;

    string      unparsed;

    SLogOPEvent( const LogOPEvent &ev )
    {
        ip          = ToString( ev . ip );
        time        = ev . time;
        request     = ev . request;
        res_code    = ev . res_code;
        res_len     = ev . res_len;
        referer     = ToString( ev . referer );
        agent       = ev . agent;
        user        = ToString( ev . user );
        req_time    = ToString( ev . req_time );
        forwarded   = ToString( ev . forwarded );
        port        = ev . port;
        req_len     = ev . req_len;

        unparsed    = ToString( ev . unparsed );
    }
};

struct TestLogLines : public OP_LogLines
{
    virtual void unrecognized( const t_str & text )
    {
        m_unrecognized . push_back( ToString( text ) );
    }

    virtual void acceptLine( const LogOPEvent & event )
    {
        m_accepted . push_back ( SLogOPEvent( event ) );
    }

    virtual void rejectLine( const LogOPEvent & event )
    {
        m_rejected . push_back ( SLogOPEvent( event ) );
    }

    vector< string >        m_unrecognized;
    vector < SLogOPEvent>   m_accepted;
    vector < SLogOPEvent>   m_rejected;

    virtual ~TestLogLines()
    {
    }
};

TEST ( TestParse, InitDestroy )
{
    TestLogLines lines;
    OP_Parser p( lines, cin );
}

TEST ( TestParse, LineRejecting )
{
    const char * input = "1.1.1.1\n2.2.2.2\n3.3.3.3\n";
    std::istringstream inputstream( input );
    TestLogLines lines;
    OP_Parser p( lines, inputstream );
    p.parse();
    ASSERT_EQ( 3, lines.m_rejected.size() );
}

TEST ( TestParse, LineFilter )
{
    const char * input = "1.1.1.1\n2.2.2.2\n3.3.3.3\n";
    std::istringstream inputstream( input );
    TestLogLines lines;
    OP_Parser p( lines, inputstream );
    p . setLineFilter( 2 );
    p . parse();
    ASSERT_EQ( 1, lines . m_rejected.size() );
    ASSERT_EQ( "2.2.2.2", lines . m_rejected . front() . unparsed );
}

class TestParseFixture : public ::testing::Test
{
public:
    virtual void SetUp() {}
    virtual void TearDown() {}

    void parse_str( const char * input, bool _debug = false  )
    {
        std::istringstream inputstream( input );
        OP_Parser p( m_lines, inputstream );
        p.setDebug( _debug );
        p.parse();
    }

    const SLogOPEvent parse_and_accept( const char * input, bool _debug = false  )
    {
        parse_str ( input, _debug );
        if ( m_lines.m_accepted.empty() ) throw logic_error( "last_m_accepted is null" );
        if ( 0 != m_lines.m_rejected.size() ) throw logic_error( "the line was rejected" );
        if ( 1 != m_lines.m_accepted.size() ) throw logic_error( "the line was not accepted" );
        if ( 0 != m_lines.m_unrecognized.size() ) throw logic_error( "the line was not recognized" );
        return m_lines . m_accepted.back();
    }

    void check_rejected( const char * input, bool _debug = false )
    {
        parse_str ( input, _debug );
        if ( 1 != m_lines.m_rejected.size() ) throw logic_error( "the line was not rejected" );
        if ( 0 != m_lines.m_accepted.size() ) throw logic_error( "the line was  falsely accepted" );
        if ( 0 != m_lines.m_unrecognized.size() ) throw logic_error( "the line was not recognized" );
        if ( string( input ) != m_lines.m_rejected.front().unparsed ) throw logic_error( "the source line was lost" );
    }

    TestLogLines m_lines;
};

TEST_F ( TestParseFixture, Empty )
{
    std::istringstream input ( "" );
    OP_Parser( m_lines, input ).parse();
}

TEST_F ( TestParseFixture, OnPremise_NoUser )
{
    const char * InputLine =
"158.111.236.250 - - [01/Jan/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET /traces/sra34/SRR/003923/SRR4017927 HTTP/1.1\" 206 32768 0.000 \"-\" \"linux64 sra-toolkit fastq-dump.2.9.1\" \"-\" port=443 rl=293\n";
    SLogOPEvent e = parse_and_accept( InputLine );

    ASSERT_EQ( "158.111.236.250", e.ip );

    ASSERT_EQ( "", e.user );

    ASSERT_EQ( 1, e.time.day );
    ASSERT_EQ( 1, e.time.month );
    ASSERT_EQ( 2020, e.time.year );
    ASSERT_EQ( 2, e.time.hour );
    ASSERT_EQ( 50, e.time.minute );
    ASSERT_EQ( 24, e.time.second );
    ASSERT_EQ( -500, e.time.offset );

    ASSERT_EQ( "sra-download.ncbi.nlm.nih.gov", e.request.server );

    ASSERT_EQ( 206, e.res_code );
    ASSERT_EQ( 32768, e.res_len );

    ASSERT_EQ( "0.000", e.req_time );
    ASSERT_EQ( "-", e.referer );
    ASSERT_EQ( "linux64 sra-toolkit fastq-dump.2.9.1", e.agent.original );
    ASSERT_EQ( "-", e.forwarded );

    ASSERT_EQ( 443, e.port );
    ASSERT_EQ( 293, e.req_len );
}

TEST_F ( TestParseFixture, OnPremise_User )
{
    const char * InputLine =
"158.111.236.250 - userid [01/Jan/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET /traces/sra34/SRR/003923/SRR4017927 HTTP/1.1\" 206 32768 0.000 \"-\" \"linux64 sra-toolkit fastq-dump.2.9.1\" \"-\" port=443 rl=293\n";
    SLogOPEvent e = parse_and_accept( InputLine );

    ASSERT_EQ( "158.111.236.250", e.ip );
    ASSERT_EQ( "userid", e.user );
    ASSERT_EQ( 1, e.time.day );
}

TEST_F ( TestParseFixture, OnPremise_OnlyIP )
{
    const char * InputLine = "158.111.236.250\n";
    parse_str ( InputLine );
    ASSERT_EQ( 1, m_lines.m_rejected.size() );
    ASSERT_EQ( "158.111.236.250", m_lines.m_rejected.back().ip );
}

TEST_F ( TestParseFixture, OnPremise_unrecognized )
{
    const char * InputLine = "total nonesense\n";
    parse_str ( InputLine );
    ASSERT_EQ( 1, m_lines . m_unrecognized . size() );
    ASSERT_EQ( "total nonesense", m_lines . m_unrecognized[ 0 ] );
}

TEST_F ( TestParseFixture, OnPremise_multiple_nonesense )
{
    const char * InputLine = "total nonesense\nmore nonesense\neven more\n";
    parse_str ( InputLine );
    ASSERT_EQ( 3, m_lines . m_unrecognized . size() );
    ASSERT_EQ( "total nonesense", m_lines . m_unrecognized[ 0 ] );
    ASSERT_EQ( "more nonesense", m_lines . m_unrecognized[ 1 ] );
    ASSERT_EQ( "even more", m_lines . m_unrecognized[ 2 ] );
}

TEST_F ( TestParseFixture, OnPremise_MultiLine )
{ 
    const char * InputLine =
"158.111.236.250 - - [01/Feb/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET /traces/sra34/SRR/003923/SRR4017927 HTTP/1.1\" 206 32768 0.000 \"-\" \"linux64 sra-toolkit fastq-\\\"dump.2.9.1\\\"\" \"-\" port=443 rl=293\n"
"158.111.236.250 - - [01/Jan/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET /traces/sra34/SRR/003923/SRR4017927 HTTP/1.1\" 206 32768 0.000 \"-\" \"linux64 sra-toolkit fastq-\\\"dump.2.9.1\\\"\" \"-\" port=443 rl=293\n";

    parse_str ( InputLine );
    ASSERT_EQ( 2, m_lines.m_accepted.size() );
}

TEST_F ( TestParseFixture, OnPremise_ErrorLine )
{   // consume the error line completely, be prepared to parse the next line with the cleared status of the scanner
    const char * InputLine =
"158.111.236.249 - \"%\"\n"
"158.111.236.250 - - [01/Jan/2020:02:50:24 -0500] \"sra-download.ncbi.nlm.nih.gov\" \"GET /traces/sra34/SRR/003923/SRR4017927 HTTP/1.1\" 206 32768 0.000 \"-\" \"linux64 sra-toolkit fastq-\\\"dump.2.9.1\\\"\" \"-\" port=443 rl=293\n";

    parse_str ( InputLine );
    ASSERT_EQ ( 1, m_lines.m_rejected.size() ); // line 1
    ASSERT_EQ ( 1, m_lines.m_accepted.size() ); // line 2
    ASSERT_EQ ( 0, m_lines.m_unrecognized.size() ); 
}

TEST_F ( TestParseFixture, OnPremise_QmarkInReferer )
{   
    const char * InputLine =
"61.153.216.106 - - [07/Jun/2020:00:04:05 -0400] \"sra-download.ncbi.nlm.nih.gov\" \"GET /traces/sra47/SRR/010462/SRR10713958 HTTP/1.1\" 206 34758384 1959.489 \"https://trace.ncbi.nlm.nih.gov/Traces/sra/?run=SRR10713958#\" \"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/83.0.4103.97 Safari/537.36 Edg/83.0.478.45\" \"-\" port=443 rl=459\n";

    parse_and_accept( InputLine );
}

TEST_F ( TestParseFixture, OnPremise_NoVersion )
{   
    const char * InputLine =
"165.112.6.3 - - [07/Jun/2020:00:06:24 -0400] \"sra-download.ncbi.nlm.nih.gov\" \"GET /\" 400 248 0.000 \"-\" \"-\" \"-\" port=443 rl=7";

    parse_and_accept( InputLine );
}

TEST_F ( TestParseFixture, OnPremise_NoMethod )
{   
    const char * InputLine =
"13.59.252.14 - - [07/Jun/2020:01:09:52 -0400] \"sra-download.ncbi.nlm.nih.gov\" \"\\x16\\x03\\x01\\x00\\xCF\\x01\\x00\\x00\\xCB\\x03\\x01\" 400 150 0.011 \"-\" \"-\" \"-\" port=80 rl=0";

    parse_and_accept( InputLine );
}

TEST_F ( TestParseFixture, OnPremise_EmptyAgent )
{   
    const char * InputLine =
"146.118.64.48 - - [07/Jun/2020:02:47:10 -0400] \"sra-download.ncbi.nlm.nih.gov\" \"HEAD /traces/sra19/SRR/009763/SRR9997476 HTTP/1.1\" 200 0 0.000 \"-\" \"\" \"-\" port=443 rl=117";

    parse_and_accept( InputLine );
}

TEST_F ( TestParseFixture, OnPremise_SpaceAsProtcol )
{   
    const char * InputLine =
"10.154.195.11 - - [07/Jun/2020:08:32:05 -0400] \"sra-download.ncbi.nlm.nih.gov\" \"GET / \" 301 162 0.001 \"-\" \"-\" \"-\" port=80 rl=8";

    parse_and_accept( InputLine );
}

TEST_F ( TestParseFixture, OnPremise_JustMethod )
{   
    const char * InputLine =
"10.154.195.11 - - [07/Jun/2020:08:32:05 -0400] \"sra-download.ncbi.nlm.nih.gov\" \"GET\" 400 150 0.001 \"-\" \"-\" \"-\" port=80 rl=0";

    parse_and_accept( InputLine );

    ASSERT_EQ( "GET", m_lines.m_accepted.front().request.method );
}

TEST_F ( TestParseFixture, OnPremise_NoMethod_WithSpaces )
{   
    const char * InputLine =
"13.56.21.202 - - [07/Jun/2020:22:26:43 -0400] \"sra-download.ncbi.nlm.nih.gov\" \"\\x16\\x03\\x01\\x02\\x00\\x01\\x00\\x01\\xFC\\x03\\x03\\xD5\\xAC\\xA1\\xD1r\\x85\\x91)\\x92v\\xB0p\\x038B#\\xD9\\x007\\x08\\xCB\\x86\\x08\\x8C\\x1Cs\\x04 \\x99\\x8A\\xC0\\x12 a\\xD7\\xC9C\\xEE\\x93\\x91\\xBF\\xCEASvP\\x9F\\xDB\\x91\\x85\\x9D\\xB4\\xF7Ee\\x80%^\\xBD\\xF3\\x02\\xB4\\x00w\\xBE\\x00\\xD2\\x13\\x02\\x13\\x03\\x13\\x01\\xC0,\\xC00\\x00\\xA3\\x00\\x9F\\xCC\\xA9\\xCC\\xA8\\xCC\\xAA\\xC0\\xAF\\xC0\\xAD\\xC0\\xA3\\xC0\\x9F\\xC0]\\xC0a\\xC0W\\xC0S\\x00\\xA7\\xC0+\\xC0/\\x00\\xA2\\x00\\x9E\\xC0\\xAE\\xC0\\xAC\\xC0\\xA2\\xC0\\x9E\\xC0\\x5C\\xC0`\\xC0V\\xC0R\\x00\\xA6\\xC0$\\xC0(\\x00k\\x00j\\xC0s\\xC0w\\x00\\xC4\\x00\\xC3\\x00m\\x00\\xC5\\xC0#\\xC0'\\x00g\\x00@\\xC0r\\xC0v\\x00\\xBE\\x00\\xBD\\x00l\\x00\\xBF\\xC0\" 400 150 0.071 \"-\" \"-\" \"-\" port=80 rl=0";

    parse_and_accept( InputLine );
}

TEST_F ( TestParseFixture, OnPremise_BadMethod )
{   
    const char * InputLine =
"34.201.223.169 - - [07/Jun/2020:04:49:29 -0400] \"sra-download.ncbi.nlm.nih.gov\" \"HCHALE / HTTP/1.1\" 405 552 0.000 \"-\" \"Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; NIHInfoSec)\" \"-\" port=80 rl=319";

    parse_and_accept( InputLine );
}

TEST_F ( TestParseFixture, OnPremise_NoServer )
{   
    const char * InputLine =
"139.80.16.229 - - [01/Jan/2020:02:50:24 -0500] \"GET /traces/sra32/SRR/005807/SRR5946882 HTTP/1.1\" 206 32768 0.000 \"-\" \"linux64 sra-toolkit fastq-dump.2.9.3\" \"-\" port=443 rl=293";

    parse_and_accept( InputLine );
}

TEST_F ( TestParseFixture, OnPremise_cmdline_as_request )
{   
    const char * InputLine =
"159.226.149.175 - - [15/Aug/2018:10:31:47 -0400] \"sra-download.ncbi.nlm.nih.gov\" \"HEAD /srapub/SRR5385591.sra > SRR5385591.sra.out 2>&1 HTTP/1.1\" 404 0 0.000 \"-\" \"linux64 sra-toolkit test-sra.2.8.2\" \"-\" port=443 rl=164";
    parse_and_accept( InputLine );
}

TEST_F ( TestParseFixture, OnPremise_rejected_unparsed_set )
{   
    const char * InputLine = "139.80.16.229 - - [01/Jan/2020:02:50:24 -0500] ";
    check_rejected( InputLine );
}

TEST_F ( TestParseFixture, OnPremise_EmptyReferrer )
{   
    const char * InputLine =
"192.12.184.6 - - [27/Aug/2018:00:40:03 -0400] \"sra-download.ncbi.nlm.nih.gov\" \"GET /traces/sra6/SRR/000166/SRR170543 HTTP/1.1\" 200 10262285 0.427 \"\" \"Mozilla/5.0\" \"-\" port=443 rl=136";
    parse_and_accept( InputLine );
}

TEST_F ( TestParseFixture, OnPremise_one_more_problem )
{   
    const char * InputLine =
"34.200.223.129 - - [20/Sep/2018:13:32:54 -0400] \"sra-download.ncbi.nlm.nih.gov\" \"\\x16\\x03\\x01\\x00\\xF5\\x01\\x00\\x00\\xF1\\x03\\x03}\\xB0'\\xC8\\xCC\\xBA\\xF6h\\xE9Q^\\xBA\\xA3\\xF6\\xC5\\xD0\\xE6  U\\xE5\\xE5\\x81\\xC4?\\xA3M\\xD7[\\x00\\xA3;\\x00\\x00\\x82\\x003\\x009\\x005\\x00/\\xC0,\\xC00\\x00\\xA3\\x00\\x9F\\xCC\\xA9\\xCC\\xA8\\xCC\\xAA\\xC0\\xAF\\xC0\\xAD\\xC0\\xA3\\xC0\\x9F\\xC0+\\xC0/\\x00\\xA2\\x00\\x9E\\xC0\\xAE\\xC0\\xAC\\xC0\\xA2\\xC0\\x9E\\xC0$\\xC0(\\x00k\\x00j\\xC0s\\xC0w\\x00\\xC4\\x00\\xC3\\xC0#\\xC0'\\x00g\\x00@\\xC0r\\xC0v\\x00\\xBE\\x00\\xBD\\xC0\" 400 166 0.001 \"-\" \"-\" \"-\" port=80 rl=0";
    parse_and_accept( InputLine );
}

TEST_F ( TestParseFixture, OnPremise_acc_filename_and_ext )
{   
    const char * InputLine =
"159.226.149.175 - - [15/Aug/2018:10:31:47 -0400] \"sra-download.ncbi.nlm.nih.gov\" \"HEAD /srapub/SRR5385591.sra HTTP/1.1\" 404 0 0.000 \"-\" \"linux64 sra-toolkit test-sra.2.8.2\" \"-\" port=443 rl=164";

    SLogOPEvent e = parse_and_accept( InputLine );
    ASSERT_EQ( "/srapub/SRR5385591.sra", e . request . path );
    ASSERT_EQ( "HEAD", e . request . method );
    ASSERT_EQ( "SRR5385591", e . request . accession );
    ASSERT_EQ( "SRR5385591", e . request . filename );
    ASSERT_EQ( ".sra", e . request . extension );
    ASSERT_EQ( "HTTP/1.1", e . request . vers );
}

TEST_F ( TestParseFixture, OnPremise_acc_filename_and_ext_and_params )
{   
    const char * InputLine =
"159.226.149.175 - - [15/Aug/2018:10:31:47 -0400] \"sra-download.ncbi.nlm.nih.gov\" \"HEAD /srapub/SRR5385591.sra?qqq HTTP/1.1\" 404 0 0.000 \"-\" \"linux64 sra-toolkit test-sra.2.8.2\" \"-\" port=443 rl=164";

    SLogOPEvent e = parse_and_accept( InputLine );
    ASSERT_EQ( "/srapub/SRR5385591.sra?qqq", e . request . path );
    ASSERT_EQ( ".sra", e . request . extension );
    ASSERT_EQ( "HTTP/1.1", e . request . vers );
}

TEST_F ( TestParseFixture, OnPremise_acc_and_params )
{   
    const char * InputLine =
"159.226.149.175 - - [15/Aug/2018:10:31:47 -0400] \"sra-download.ncbi.nlm.nih.gov\" \"HEAD /srapub/SRR5385591?qqq HTTP/1.1\" 404 0 0.000 \"-\" \"linux64 sra-toolkit test-sra.2.8.2\" \"-\" port=443 rl=164";

    SLogOPEvent e = parse_and_accept( InputLine );
    ASSERT_EQ( "/srapub/SRR5385591?qqq", e . request . path );
    ASSERT_EQ( "", e . request . extension );
    ASSERT_EQ( "HTTP/1.1", e . request . vers );
}

TEST_F ( TestParseFixture, OnPremise_ext_and_params )
{   
    const char * InputLine =
"159.226.149.175 - - [15/Aug/2018:10:31:47 -0400] \"sra-download.ncbi.nlm.nih.gov\" \"HEAD /.ext?qqq HTTP/1.1\" 404 0 0.000 \"-\" \"linux64 sra-toolkit test-sra.2.8.2\" \"-\" port=443 rl=164";

    SLogOPEvent e = parse_and_accept( InputLine );
    ASSERT_EQ( "/.ext?qqq", e . request . path );
    ASSERT_EQ( "", e . request . accession );
    ASSERT_EQ( "", e . request . filename );
    ASSERT_EQ( ".ext", e . request . extension );
    ASSERT_EQ( "HTTP/1.1", e . request . vers );
}

TEST_F ( TestParseFixture, OnPremise_params_only )
{   
    const char * InputLine =
"159.226.149.175 - - [15/Aug/2018:10:31:47 -0400] \"sra-download.ncbi.nlm.nih.gov\" \"HEAD /?/srapub/SRR5385591.sra HTTP/1.1\" 404 0 0.000 \"-\" \"linux64 sra-toolkit test-sra.2.8.2\" \"-\" port=443 rl=164";

    SLogOPEvent e = parse_and_accept( InputLine );
    ASSERT_EQ( "/?/srapub/SRR5385591.sra", e . request . path );
    ASSERT_EQ( "", e . request . accession );
    ASSERT_EQ( "", e . request . filename );
    ASSERT_EQ( "", e . request . extension );
    ASSERT_EQ( "HTTP/1.1", e . request . vers );
}

// User-Agent header parsing

TEST_F ( TestParseFixture, OnPremise_empty_user_agent )
{
    const char * InputLine =
"159.226.149.175 - - [15/Aug/2018:10:31:47 -0400] \"sra-download.ncbi.nlm.nih.gov\" \"HEAD /?/srapub/SRR5385591.sra HTTP/1.1\" 404 0 0.000 \"-\" \"\" \"-\" port=443 rl=164";

    SLogOPEvent e = parse_and_accept( InputLine );
    ASSERT_EQ( "", e.agent.original );
    ASSERT_EQ( "", e.agent.vdb_os );
    ASSERT_EQ( "", e.agent.vdb_tool );
    ASSERT_EQ( "", e.agent.vdb_release );
    ASSERT_EQ( "", e.agent.vdb_phid_compute_env );
    ASSERT_EQ( "", e.agent.vdb_phid_guid );
    ASSERT_EQ( "", e.agent.vdb_phid_session_id );
    ASSERT_EQ( "", e.agent.vdb_libc );
}

TEST_F ( TestParseFixture, OnPremise_user_agent )
{
    const char * InputLine =
"159.226.149.175 - - [15/Aug/2018:10:31:47 -0400] \"sra-download.ncbi.nlm.nih.gov\" \"HEAD /?/srapub/SRR5385591.sra HTTP/1.1\" 404 0 0.000 \"-\" \"linux64 sra-toolkit test-sra.2.8.2\" \"-\" port=443 rl=164";

    SLogOPEvent e = parse_and_accept( InputLine );
    ASSERT_EQ( "linux64 sra-toolkit test-sra.2.8.2", e.agent.original );
    ASSERT_EQ( "linux64", e.agent.vdb_os );
    ASSERT_EQ( "test-sra", e.agent.vdb_tool );
    ASSERT_EQ( "2.8.2", e.agent.vdb_release );
    ASSERT_EQ( "", e.agent.vdb_phid_compute_env );
    ASSERT_EQ( "", e.agent.vdb_phid_guid );
    ASSERT_EQ( "", e.agent.vdb_phid_session_id );
    ASSERT_EQ( "", e.agent.vdb_libc );
}

TEST_F ( TestParseFixture, OnPremise_user_agent_with_phid_and_libc )
{
    const char * InputLine =
"159.226.149.175 - - [15/Aug/2018:10:31:47 -0400] \"sra-download.ncbi.nlm.nih.gov\" \"HEAD /?/srapub/SRR5385591.sra HTTP/1.1\" 404 0 0.000 \"-\" \"linux64 ncbi-vdb.2.10.3 (phid=noc7737000,libc=2.17)\" \"-\" port=443 rl=164";

    SLogOPEvent e = parse_and_accept( InputLine );
    ASSERT_EQ( "noc", e.agent.vdb_phid_compute_env );
    ASSERT_EQ( "7737", e.agent.vdb_phid_guid );
    ASSERT_EQ( "000", e.agent.vdb_phid_session_id );
    ASSERT_EQ( "2.17", e.agent.vdb_libc );
}

TEST_F ( TestParseFixture, OnPremise_agent_doubled )
{
    const char * InputLine =
"66.189.43.90 - - [15/Jul/2020:01:40:05 -0400] \"sra-download.ncbi.nlm.nih.gov\" \"HEAD /traces/refseq/NC_004330.1 HTTP/1.1\" 200 0 0.000 \"-\" \"linux64 sra-toolkit fasterq-dump.2.10.8 (phid=noc83f645a,libc=2.31)linux64 sra-toolkit fasterq-dump.2.10.8 (phid=noc83f645a,libc=2.31)\" \"-\" port=443 rl=288 tls=TLSv1.2";
    SLogOPEvent e = parse_and_accept( InputLine );
}

TEST_F ( TestParseFixture, OnPremise_time_is_not_float )
{
    const char * InputLine =
"130.14.25.251 - - [15/Jul/2020:00:15:54 -0400] \"gap-sview.ncbi.nlm.nih.gov\" \"GET / HTTP/1.1\" 403 202 0 \"-\" \"Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0)\" \"-\" -pct 1561 + \"NCBI-SID: -\" port=80 324 406 text/html";
    check_rejected( InputLine, true );
}

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}
