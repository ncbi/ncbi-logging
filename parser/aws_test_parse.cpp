#include <gtest/gtest.h>

#include <sstream>
#include <vector>

#include "log_lines.hpp"
#include "helper.hpp"
#include "test_helper.hpp"

using namespace std;
using namespace NCBI::Logging;

struct SLogAWSEvent
{
    string      ip;
    t_timepoint time;
    SRequest    request;
    string      res_code;
    string      res_len;
    string      referer;

    SAgent      agent;

    string      owner;
    string      bucket;
    string      requester;
    string      request_id;
    string      operation;
    string      key;
    string      error;
    string      obj_size;
    string      total_time;
    string      turnaround_time;
    string      version_id;
    string      host_id;
    string      sig_ver;
    string      cipher_suite;
    string      auth_type;
    string      host_header;
    string      tls_version;

    string      unparsed;

    SLogAWSEvent( const LogAWSEvent &ev )
    {
        ip          = ToString( ev . ip );
        time        = ev . time;
        request     = ev . request;
        res_code    = ToString( ev . res_code );
        res_len     = ToString( ev . res_len );
        referer     = ToString( ev . referer );
        agent       = ev . agent;

        owner       = ToString( ev . owner );
        bucket      = ToString( ev . bucket );
        requester   = ToString( ev . requester );
        request_id  = ToString( ev . request_id );
        operation   = ToString( ev . operation );
        key         = ToString( ev . key );
        error       = ToString( ev . error );
        obj_size    = ToString( ev . obj_size );
        total_time  = ToString( ev . total_time );
        turnaround_time = ToString( ev . turnaround_time );
        version_id  = ToString( ev . version_id );
        host_id     = ToString( ev . host_id );
        cipher_suite = ToString( ev . cipher_suite );
        sig_ver     = ToString( ev . sig_ver );
        auth_type   = ToString( ev . auth_type );
        host_header = ToString( ev . host_header );
        tls_version = ToString( ev . tls_version );

        unparsed = ToString( ev . unparsed );
    }
};

struct TestLogLines : public AWS_LogLines
{
    virtual void unrecognized( const t_str & text )
    {
        m_unrecognized . push_back( ToString( text ) );
    }

    virtual void acceptLine( const LogAWSEvent & event )
    {
        m_accepted . push_back ( SLogAWSEvent( event ) );
    }

    virtual void rejectLine( const LogAWSEvent & event )
    {
        m_rejected . push_back ( SLogAWSEvent( event ) );
    }

    vector< string >        m_unrecognized;
    vector < SLogAWSEvent > m_accepted;
    vector < SLogAWSEvent > m_rejected;

    virtual ~TestLogLines()
    {
    }
};

TEST ( TestParse, InitDestroy )
{
    TestLogLines lines;

    AWS_Parser p( lines, cin );
}

TEST ( TestParse, LineRejecting )
{
    const char * input = "line1\nline2\nline3\n";
    std::istringstream inputstream( input );
    TestLogLines lines;
    AWS_Parser p( lines, inputstream );
    p.parse();
    ASSERT_EQ( 3, lines.m_rejected.size() );
}

TEST ( TestParse, LineFilter )
{
    const char * input = "line1\nline2\nline3\n";
    std::istringstream inputstream( input );
    TestLogLines lines;
    AWS_Parser p( lines, inputstream );
    p . setLineFilter( 2 );
    p . parse();
    ASSERT_EQ( 1, lines . m_rejected.size() );
    ASSERT_EQ( "line2", lines . m_rejected . front() . unparsed );
}

class TestParseFixture : public ::testing::Test
{
public:
    virtual void SetUp() {}
    virtual void TearDown() {}

    SLogAWSEvent parse_accept( const char * input, bool p_debug = false )
    {
        std::istringstream inputstream( input );
        AWS_Parser p( m_lines, inputstream );
        p.setDebug( p_debug );
        p.parse();
        if ( m_lines.m_accepted.empty() ) throw logic_error( "last_m_accepted is null" );
        return m_lines . m_accepted.back();
    }

    SLogAWSEvent parse_reject( const char * input, bool p_debug = false )
    {
        std::istringstream inputstream( input );
        AWS_Parser p( m_lines, inputstream );
        p.setDebug( p_debug );
        p.parse();
        if ( m_lines.m_rejected.empty() ) throw logic_error( "m_rejected is null" );
        return m_lines . m_rejected.back();
    }

    TestLogLines m_lines;
};

TEST_F ( TestParseFixture, Empty )
{
    std::istringstream input ( "" );
    AWS_Parser( m_lines, input ).parse();
}

TEST_F ( TestParseFixture, AWS )
{
    const char * InputLine =
    "922194806485875312b252374a3644f1feecd16802a50d4729885c1d11e1fd37 s"
    "ra-pub-src-14 "
    "[09/May/2020:22:07:21 +0000] "
    "18.207.254.142 "
    "arn:aws:sts::783971887864:assumed-role/sra-developer-instance-profile-role/i-0d76b79326eb0165a "
    "B6301F55C2486C74 "
    "REST.PUT.PART "
    "SRR9612637/DRGHT.TC.307_interleaved.fq.1 "
    "\"PUT /SRR9612637/DRGHT.TC.307_interleaved.fq.1?partNumber=1&uploadId=rl6yL37lb4xUuIa9RvC0ON4KgDqJNvtwLoquo_cALj95v4njBOTUHpISyEjOaMG30lVYAo5eR_UEXo4dVJjUJA3SfjJtKjg30rvVEpg._Z9DZZo8S6oUjXHGDCW15EVzLZcJMgRG6N7J8d.42.lMAw-- HTTP/1.1\" "
    "200 "
    "- "
    "- "
    "8388608 "
    "557 "
    "12 "
    "\"-\" "
    "\"aws-cli/1.16.102 \\\"Python\\\"/2.7.16 Linux/4.14.171-105.231.amzn1.x86_64 botocore/1.12.92\" "
    "- "
    "fV92QmqOf5ZNYPIj7KZeQWiqAOFqdFtMlOn82aRYjwQHt8QfsWfS3TTOft1Be+bY01d9TObk5Qg= "
    "SigV4 ECDHE-RSA-AES128-GCM-SHA256 "
    "AuthHeader "
    "sra-pub-src-14.s3.amazonaws.com "
    "TLSv1.2";
    "\n";

    SLogAWSEvent e = parse_accept( InputLine );

    ASSERT_EQ( "922194806485875312b252374a3644f1feecd16802a50d4729885c1d11e1fd37", e.owner );
    ASSERT_EQ( "sra-pub-src-14", e.bucket );

    ASSERT_EQ( 9, e.time.day );
    ASSERT_EQ( 5, e.time.month );
    ASSERT_EQ( 2020, e.time.year );
    ASSERT_EQ( 22, e.time.hour );
    ASSERT_EQ( 7, e.time.minute );
    ASSERT_EQ( 21, e.time.second );
    ASSERT_EQ( 0, e.time.offset );

    ASSERT_EQ( "18.207.254.142", e.ip );

    ASSERT_EQ( "arn:aws:sts::783971887864:assumed-role/sra-developer-instance-profile-role/i-0d76b79326eb0165a", e.requester );
    ASSERT_EQ( "B6301F55C2486C74", e.request_id );
    ASSERT_EQ( "REST.PUT.PART", e.operation );
    ASSERT_EQ( "SRR9612637/DRGHT.TC.307_interleaved.fq.1", e.key );

    ASSERT_EQ( "PUT", e.request.method );
    ASSERT_EQ( "/SRR9612637/DRGHT.TC.307_interleaved.fq.1?partNumber=1&uploadId=rl6yL37lb4xUuIa9RvC0ON4KgDqJNvtwLoquo_cALj95v4njBOTUHpISyEjOaMG30lVYAo5eR_UEXo4dVJjUJA3SfjJtKjg30rvVEpg._Z9DZZo8S6oUjXHGDCW15EVzLZcJMgRG6N7J8d.42.lMAw--", 
                e.request.path );
    ASSERT_EQ( "SRR9612637", e.request.accession );
    ASSERT_EQ( "DRGHT", e.request.filename );
    ASSERT_EQ( ".TC.307_interleaved.fq.1", e.request.extension );
    ASSERT_EQ( "HTTP/1.1", e.request.vers );

    ASSERT_EQ( "200", e.res_code );
    ASSERT_EQ( "", e.error );
    ASSERT_EQ( "", e.res_len );
    ASSERT_EQ( "8388608", e.obj_size );
    ASSERT_EQ( "557", e.total_time );
    ASSERT_EQ( "12", e.turnaround_time );
    ASSERT_EQ( "", e.referer );
    ASSERT_EQ( "aws-cli/1.16.102 \\\"Python\\\"/2.7.16 Linux/4.14.171-105.231.amzn1.x86_64 botocore/1.12.92", e.agent . original );
    ASSERT_EQ( "", e.version_id );
    ASSERT_EQ( "fV92QmqOf5ZNYPIj7KZeQWiqAOFqdFtMlOn82aRYjwQHt8QfsWfS3TTOft1Be+bY01d9TObk5Qg=", e.host_id );
    ASSERT_EQ( "SigV4", e.sig_ver );
    ASSERT_EQ( "ECDHE-RSA-AES128-GCM-SHA256", e.cipher_suite );
    ASSERT_EQ( "AuthHeader", e.auth_type );
    ASSERT_EQ( "sra-pub-src-14.s3.amazonaws.com", e.host_header );
    ASSERT_EQ( "TLSv1.2", e.tls_version );
}

TEST_F ( TestParseFixture, AWS_turnaround_time_is_dash )
{
    const char * InputLine =
    "7dd4dcfe9b004fb7433c61af3e87972f2e9477fa7f0760a02827f771b41b3455 sra-pub-src-1 [25/May/2020:22:54:57 +0000] 52.54.203.43 arn:aws:sts::783971887864:assumed-role/sra-developer-instance-profile-role/i-04b64e15519efb678 EF87C0499CB7FDCA REST.HEAD.OBJECT ERR792423/m150101_223627_42225_c100719502550000001823155305141526_s1_p0.bas.h5.1 \"HEAD /ERR792423/m150101_223627_42225_c100719502550000001823155305141526_s1_p0.bas.h5.1 HTTP/1.1\" 200 - - 1318480 30 - \"-\" \"aws-cli/1.16.249 Python/2.7.16 Linux/4.14.138-89.102.amzn1.x86_64 botocore/1.12.239\" - Is1QS5IgOb006L7yAqIz7zKBtUIxbAZgzvM1aQbbSHXeKUDoSVfPXro2v1AN4gf7Ek5VTV2FeV8= SigV4 ECDHE-RSA-AES128-GCM-SHA256 AuthHeader sra-pub-src-1.s3.amazonaws.com TLSv1.2";
    
    SLogAWSEvent e = parse_accept( InputLine );

    ASSERT_EQ( "200", e.res_code );
    ASSERT_EQ( "", e.error );
    ASSERT_EQ( "", e.res_len );
    ASSERT_EQ( "1318480", e.obj_size );
    ASSERT_EQ( "30", e.total_time );
    ASSERT_EQ( "", e.turnaround_time );
    ASSERT_EQ( "", e.referer );
}

TEST_F ( TestParseFixture, AWS_SpaceInReferrer )
{
    const char * InputLine =
    "7dd4dcfe9b004fb7433c61af3e87972f2e9477fa7f0760a02827f771b41b3455 sra-pub-src-1 [25/May/2020:22:54:57 +0000] 52.54.203.43 arn:aws:sts::783971887864:assumed-role/sra-developer-instance-profile-role/i-04b64e15519efb678 EF87C0499CB7FDCA REST.HEAD.OBJECT ERR792423/m150101_223627_42225_c100719502550000001823155305141526_s1_p0.bas.h5.1 \"HEAD /ERR792423/m150101_223627_42225_c100719502550000001823155305141526_s1_p0.bas.h5.1 HTTP/1.1\" 200 - - 1318480 30 - \"referrer with spaces\" \"aws-cli/1.16.249 Python/2.7.16 Linux/4.14.138-89.102.amzn1.x86_64 botocore/1.12.239\" - Is1QS5IgOb006L7yAqIz7zKBtUIxbAZgzvM1aQbbSHXeKUDoSVfPXro2v1AN4gf7Ek5VTV2FeV8= SigV4 ECDHE-RSA-AES128-GCM-SHA256 AuthHeader sra-pub-src-1.s3.amazonaws.com TLSv1.2";
    
    SLogAWSEvent e = parse_accept( InputLine, false );
    ASSERT_EQ( "referrer with spaces", e.referer );
}


TEST_F ( TestParseFixture, AWS_MultiLine )
{
    const char * InputLine =
    "7dd4dcfe9b004fb7433c61af3e87972f2e9477fa7f0760a02827f771b41b3455 sra-pub-src-1 [25/May/2020:22:54:57 +0000] 52.54.203.43 arn:aws:sts::783971887864:assumed-role/sra-developer-instance-profile-role/i-04b64e15519efb678 EF87C0499CB7FDCA REST.HEAD.OBJECT ERR792423/m150101_223627_42225_c100719502550000001823155305141526_s1_p0.bas.h5.1 \"HEAD /ERR792423/m150101_223627_42225_c100719502550000001823155305141526_s1_p0.bas.h5.1 HTTP/1.1\" 200 - - 1318480 30 - \"-\" \"aws-cli/1.16.249 Python/2.7.16 Linux/4.14.138-89.102.amzn1.x86_64 botocore/1.12.239\" - Is1QS5IgOb006L7yAqIz7zKBtUIxbAZgzvM1aQbbSHXeKUDoSVfPXro2v1AN4gf7Ek5VTV2FeV8= SigV4 ECDHE-RSA-AES128-GCM-SHA256 AuthHeader sra-pub-src-1.s3.amazonaws.com TLSv1.2\n"
    "8dd4dcfe9b004fb7433c61af3e87972f2e9477fa7f0760a02827f771b41b3455 sra-pub-src-1 [25/May/2020:22:54:57 +0000] 52.54.203.43 arn:aws:sts::783971887864:assumed-role/sra-developer-instance-profile-role/i-04b64e15519efb678 EF87C0499CB7FDCA REST.HEAD.OBJECT ERR792423/m150101_223627_42225_c100719502550000001823155305141526_s1_p0.bas.h5.1 \"HEAD /ERR792423/m150101_223627_42225_c100719502550000001823155305141526_s1_p0.bas.h5.1 HTTP/1.1\" 200 - - 1318480 30 - \"-\" \"aws-cli/1.16.249 Python/2.7.16 Linux/4.14.138-89.102.amzn1.x86_64 botocore/1.12.239\" - Is1QS5IgOb006L7yAqIz7zKBtUIxbAZgzvM1aQbbSHXeKUDoSVfPXro2v1AN4gf7Ek5VTV2FeV8= SigV4 ECDHE-RSA-AES128-GCM-SHA256 AuthHeader sra-pub-src-1.s3.amazonaws.com TLSv1.2\n";
    
    std::istringstream inputstream( InputLine );
    {
        AWS_Parser p( m_lines, inputstream );
        p.parse();
        ASSERT_EQ( 2, m_lines.m_accepted.size() );
    }
}

TEST_F ( TestParseFixture, AWS_MultiLine_ErrorRecovery )
{
    const char * InputLine =
                                                                                   //   vvv bad month
    "7dd4dcfe9b004fb7433c61af3e87972f2e9477fa7f0760a02827f771b41b3455 sra-pub-src-1 [25/XXX/2020:22:54:57 +0000] 52.54.203.43 arn:aws:sts::783971887864:assumed-role/sra-developer-instance-profile-role/i-04b64e15519efb678 EF87C0499CB7FDCA REST.HEAD.OBJECT ERR792423/m150101_223627_42225_c100719502550000001823155305141526_s1_p0.bas.h5.1 \"HEAD /ERR792423/m150101_223627_42225_c100719502550000001823155305141526_s1_p0.bas.h5.1 HTTP/1.1\" 200 - - 1318480 30 - \"-\" \"aws-cli/1.16.249 Python/2.7.16 Linux/4.14.138-89.102.amzn1.x86_64 botocore/1.12.239\" - Is1QS5IgOb006L7yAqIz7zKBtUIxbAZgzvM1aQbbSHXeKUDoSVfPXro2v1AN4gf7Ek5VTV2FeV8= SigV4 ECDHE-RSA-AES128-GCM-SHA256 AuthHeader sra-pub-src-1.s3.amazonaws.com TLSv1.2\n"
    "8dd4dcfe9b004fb7433c61af3e87972f2e9477fa7f0760a02827f771b41b3455 sra-pub-src-1 [25/May/2020:22:54:57 +0000] 52.54.203.43 arn:aws:sts::783971887864:assumed-role/sra-developer-instance-profile-role/i-04b64e15519efb678 EF87C0499CB7FDCA REST.HEAD.OBJECT ERR792423/m150101_223627_42225_c100719502550000001823155305141526_s1_p0.bas.h5.1 \"HEAD /ERR792423/m150101_223627_42225_c100719502550000001823155305141526_s1_p0.bas.h5.1 HTTP/1.1\" 200 - - 1318480 30 - \"-\" \"aws-cli/1.16.249 Python/2.7.16 Linux/4.14.138-89.102.amzn1.x86_64 botocore/1.12.239\" - Is1QS5IgOb006L7yAqIz7zKBtUIxbAZgzvM1aQbbSHXeKUDoSVfPXro2v1AN4gf7Ek5VTV2FeV8= SigV4 ECDHE-RSA-AES128-GCM-SHA256 AuthHeader sra-pub-src-1.s3.amazonaws.com TLSv1.2\n";
    
    std::istringstream inputstream( InputLine );
    AWS_Parser p( m_lines, inputstream );
    p.parse();
    ASSERT_EQ ( 1, m_lines.m_rejected.size() ); // line 1
    ASSERT_EQ ( 1, m_lines.m_accepted.size() ); // line 2
    ASSERT_EQ ( 0, m_lines.m_unrecognized.size() );
}

TEST_F ( TestParseFixture, AWS_UnparsedInput_WhenRejected )
{
    const char * InputLine =
"7dd4dcfe9b004fb7433c61af3e87972f2e9477fa7f0760a02827f771b41b3455 sra-pub-src-1 [25/XXX/2020:22:54:57 +0000] 52.54.203.43 arn:aws:sts::783971887864:assumed-role/sra-developer-instance-profile-role/i-04b64e15519efb678 EF87C0499CB7FDCA REST.HEAD.OBJECT ERR792423/m150101_223627_42225_c100719502550000001823155305141526_s1_p0.bas.h5.1 \"HEAD /ERR792423/m150101_223627_42225_c100719502550000001823155305141526_s1_p0.bas.h5.1 HTTP/1.1\" 200 - - 1318480 30 - \"-\" \"aws-cli/1.16.249 Python/2.7.16 Linux/4.14.138-89.102.amzn1.x86_64 botocore/1.12.239\" - Is1QS5IgOb006L7yAqIz7zKBtUIxbAZgzvM1aQbbSHXeKUDoSVfPXro2v1AN4gf7Ek5VTV2FeV8= SigV4 ECDHE-RSA-AES128-GCM-SHA256 AuthHeader sra-pub-src-1.s3.amazonaws.com TLSv1.2";

    SLogAWSEvent e = parse_reject( InputLine );
    ASSERT_EQ ( string (InputLine), e.unparsed);
}

TEST_F ( TestParseFixture, AWS_StrayBackslashes )
{
    const char * InputLine =
"7dd4dcfe9b004fb7433c61af3e87972f2e9477fa7f0760a02827f771b41b3455 sra-pub-src-1 [25/May/2020:22:54:57 +0000] 52.54.203.43 arn:aws:sts::783971887864:assumed-role/sra-developer-instance-profile-role/i-04b64e15519efb678 EF87C0499CB7FDCA REST.HEAD.OBJECT ERR792423/m150101_223627_42225_c100719502550000001823155305141526_s1_p0.bas.h5.1 \"HEAD /ERR792423/m150101_223627_42225_c100719502550000001823155305141526_s1_p0.bas.h5.1 HTTP/1.1\" 200 - - 1318480 30 - \"referrer with spaces\" \"win64 sra-toolkit D:\\\\sratool\\\\sratoolkit.2.10.7 (phid=nocaeb9e77,li\" - Is1QS5IgOb006L7yAqIz7zKBtUIxbAZgzvM1aQbbSHXeKUDoSVfPXro2v1AN4gf7Ek5VTV2FeV8= SigV4 ECDHE-RSA-AES128-GCM-SHA256 AuthHeader sra-pub-src-1.s3.amazonaws.com TLSv1.2";

    std::istringstream inputstream( InputLine );
    SLogAWSEvent e = parse_accept( InputLine );
    ASSERT_EQ ( string ("win64 sra-toolkit D:\\\\sratool\\\\sratoolkit.2.10.7 (phid=nocaeb9e77,li"), e.agent.original);
}

TEST_F ( TestParseFixture, AWS_NoIP )
{
    const char * InputLine =
"7dd4dcfe9b004fb7433c61af3e87972f2e9477fa7f0760a02827f771b41b3455 sra-pub-src-1 [25/May/2020:22:54:57 +0000] - arn:aws:sts::783971887864:assumed-role/sra-developer-instance-profile-role/i-04b64e15519efb678 EF87C0499CB7FDCA REST.HEAD.OBJECT ERR792423/m150101_223627_42225_c100719502550000001823155305141526_s1_p0.bas.h5.1 \"HEAD /ERR792423/m150101_223627_42225_c100719502550000001823155305141526_s1_p0.bas.h5.1 HTTP/1.1\" 200 - - 1318480 30 - \"referrer with spaces\" \"win64 sra-toolkit D:\\\\sratool\\\\sratoolkit.2.10.7 (phid=nocaeb9e77,li\" - Is1QS5IgOb006L7yAqIz7zKBtUIxbAZgzvM1aQbbSHXeKUDoSVfPXro2v1AN4gf7Ek5VTV2FeV8= SigV4 ECDHE-RSA-AES128-GCM-SHA256 AuthHeader sra-pub-src-1.s3.amazonaws.com TLSv1.2";

    SLogAWSEvent e = parse_accept( InputLine );
    ASSERT_EQ ( string (), e.ip);
}

TEST_F ( TestParseFixture, AWS_Request_QuotedDash )
{
    const char * InputLine =
"7dd4dcfe9b004fb7433c61af3e87972f2e9477fa7f0760a02827f771b41b3455 sra-pub-src-1 [25/May/2020:22:54:57 +0000] - arn:aws:sts::783971887864:assumed-role/sra-developer-instance-profile-role/i-04b64e15519efb678 EF87C0499CB7FDCA REST.HEAD.OBJECT ERR792423/m150101_223627_42225_c100719502550000001823155305141526_s1_p0.bas.h5.1 \"-\" 200 - - 1318480 30 - \"referrer with spaces\" \"win64 sra-toolkit D:\\\\sratool\\\\sratoolkit.2.10.7 (phid=nocaeb9e77,li\" - Is1QS5IgOb006L7yAqIz7zKBtUIxbAZgzvM1aQbbSHXeKUDoSVfPXro2v1AN4gf7Ek5VTV2FeV8= SigV4 ECDHE-RSA-AES128-GCM-SHA256 AuthHeader sra-pub-src-1.s3.amazonaws.com TLSv1.2";

    SLogAWSEvent e = parse_accept( InputLine );
    ASSERT_EQ ( string (), e.request.path);
    ASSERT_EQ ( string (), e.request.method);
    ASSERT_EQ ( string (), e.request.vers);
}

TEST_F ( TestParseFixture, AWS_multiple_valid_accessions_in_key )
{
    const char * InputLine =
"7dd4dcfe9 sra-pub-src-1 [25/May/2020:22:54:57 +0000] - arn:aws:sts - REST.HEAD.OBJECT SRX123456/ERR792423/5141526_s1_p0.bas.h5.1 \"-\" 200 - - 1318480 30 - \"referrer with spaces\" \"win64 sra-toolkit D:\\\\sratool\\\\sratoolkit.2.10.7 (phid=nocaeb9e77,li\" - Is1QS= SigV4 ECDHE-RSA-AES128-GCM-SHA256 AuthHeader sra-pub-src-1.s3.amazonaws.com TLSv1.2";

    SLogAWSEvent e = parse_accept( InputLine );
    ASSERT_EQ ( string ("SRX123456/ERR792423/5141526_s1_p0.bas.h5.1"), e.key );
    ASSERT_EQ ( string ("ERR792423"), e.request.accession );
    ASSERT_EQ ( string ("5141526_s1_p0"), e.request.filename );
    ASSERT_EQ ( string (".bas.h5.1"), e.request.extension );
}

TEST_F ( TestParseFixture, AWS_no_valid_accessions_in_key )
{
    const char * InputLine =
"7dd4dcfe9 sra-pub-src-1 [25/May/2020:22:54:57 +0000] - arn:aws:sts - REST.HEAD.OBJECT abc.12/5141526_s1_p0.bas.h5.1 \"-\" 200 - - 1318480 30 - \"referrer with spaces\" \"win64 sra-toolkit D:\\\\sratool\\\\sratoolkit.2.10.7 (phid=nocaeb9e77,li\" - Is1QS= SigV4 ECDHE-RSA-AES128-GCM-SHA256 AuthHeader sra-pub-src-1.s3.amazonaws.com TLSv1.2";

    SLogAWSEvent e = parse_accept( InputLine );
    ASSERT_EQ ( string ("abc.12/5141526_s1_p0.bas.h5.1"), e.key );
    ASSERT_EQ ( string (), e.request.accession );
    ASSERT_EQ ( string (), e.request.filename );
    ASSERT_EQ ( string (), e.request.extension );
}

TEST_F ( TestParseFixture, AWS_valid_accessions_with_multiple_path_segments_in_key )
{
    const char * InputLine =
"7dd4dcfe9 sra-pub-src-1 [25/May/2020:22:54:57 +0000] - arn:aws:sts - REST.HEAD.OBJECT SRR123456/abc.12/5141526_s1_p0.bas.h5.1 \"-\" 200 - - 1318480 30 - \"referrer with spaces\" \"win64 sra-toolkit D:\\\\sratool\\\\sratoolkit.2.10.7 (phid=nocaeb9e77,li\" - Is1QS= SigV4 ECDHE-RSA-AES128-GCM-SHA256 AuthHeader sra-pub-src-1.s3.amazonaws.com TLSv1.2";

    SLogAWSEvent e = parse_accept( InputLine );
    ASSERT_EQ ( string ("SRR123456/abc.12/5141526_s1_p0.bas.h5.1"), e.key );
    ASSERT_EQ ( string ("SRR123456"), e.request.accession );
    ASSERT_EQ ( string ("5141526_s1_p0"), e.request.filename );
    ASSERT_EQ ( string (".bas.h5.1"), e.request.extension );
}

TEST_F ( TestParseFixture, AWS_valid_accessions_no_extension_in_key )
{
    const char * InputLine =
"7dd4dcfe9 sra-pub-src-1 [25/May/2020:22:54:57 +0000] - arn:aws:sts - REST.HEAD.OBJECT SRR123456/abc.12/5141526_s1_p0 \"-\" 200 - - 1318480 30 - \"referrer with spaces\" \"win64 sra-toolkit D:\\\\sratool\\\\sratoolkit.2.10.7 (phid=nocaeb9e77,li\" - Is1QS= SigV4 ECDHE-RSA-AES128-GCM-SHA256 AuthHeader sra-pub-src-1.s3.amazonaws.com TLSv1.2";

    SLogAWSEvent e = parse_accept( InputLine );
    ASSERT_EQ ( string ("SRR123456/abc.12/5141526_s1_p0"), e.key );
    ASSERT_EQ ( string ("SRR123456"), e.request.accession );
    ASSERT_EQ ( string ("5141526_s1_p0"), e.request.filename );
    ASSERT_EQ ( string (""), e.request.extension );
}

TEST_F ( TestParseFixture, AWS_valid_accessions_only_extension_in_key )
{
    const char * InputLine =
"7dd4dcfe9 sra-pub-src-1 [25/May/2020:22:54:57 +0000] - arn:aws:sts - REST.HEAD.OBJECT SRR123456/abc.12/.bas.h5.1 \"-\" 200 - - 1318480 30 - \"referrer with spaces\" \"win64 sra-toolkit D:\\\\sratool\\\\sratoolkit.2.10.7 (phid=nocaeb9e77,li\" - Is1QS= SigV4 ECDHE-RSA-AES128-GCM-SHA256 AuthHeader sra-pub-src-1.s3.amazonaws.com TLSv1.2";

    SLogAWSEvent e = parse_accept( InputLine );
    ASSERT_EQ ( string ("SRR123456/abc.12/.bas.h5.1"), e.key );
    ASSERT_EQ ( string ("SRR123456"), e.request.accession );
    ASSERT_EQ ( string (""), e.request.filename );
    ASSERT_EQ ( string (".bas.h5.1"), e.request.extension );
}

TEST_F ( TestParseFixture, AWS_accessions_is_also_filename )
{
    const char * InputLine =
"7dd4dcfe9 sra-pub-src-1 [25/May/2020:22:54:57 +0000] - arn:aws:sts - REST.HEAD.OBJECT SRR123456.1 \"-\" 200 - - 1318480 30 - \"referrer with spaces\" \"win64 sra-toolkit D:\\\\sratool\\\\sratoolkit.2.10.7 (phid=nocaeb9e77,li\" - Is1QS= SigV4 ECDHE-RSA-AES128-GCM-SHA256 AuthHeader sra-pub-src-1.s3.amazonaws.com TLSv1.2";

    SLogAWSEvent e = parse_accept( InputLine );
    ASSERT_EQ ( string ("SRR123456.1"), e.key );
    ASSERT_EQ ( string ("SRR123456"), e.request.accession );
    ASSERT_EQ ( string ("SRR123456"), e.request.filename );
    ASSERT_EQ ( string (".1"), e.request.extension );
}

TEST_F ( TestParseFixture, AWS_empty_key )
{   // when the key is empty, parse the request field
    const char * InputLine =
"922194806485875312b252374a3644f1feecd16802a50d4729885c1d11e1fd37 sra-pub-run-5 [09/Mar/2020:22:53:57 +0000] 35.172.121.21 arn:aws:sts::783971887864:assumed-role/sra-developer-instance-profile-role/i-04a6132b8172af805 479B09DC662E4B67 REST.GET.BUCKET - \"GET ?list-type=2&delimiter=%2F&prefix=SRR11060177%2FSRR99999999/filename.1&morefilenames.moreextensions.1&name=SRR000123&encoding-type=url HTTP/1.1\" 200 - 325 - 14 14 \"-\" \"aws-cli/1.16.102 Python/2.7.16 Linux/4.14.154-99.181.amzn1.x86_64 botocore/1.12.92\" - 4588JL1XJI30m/MURh3Xoz4qVakHYt/u1JwJ/u4BvxAUCOFUfvPJAG/utO0+cBgipDArBig9kL4= SigV4 ECDHE-RSA-AES128-GCM-SHA256 AuthHeader sra-pub-run-5.s3.amazonaws.com TLSv1.2";

    SLogAWSEvent e = parse_accept( InputLine );
    ASSERT_EQ ( string (""), e.key );
    ASSERT_EQ ( string ("SRR11060177"), e.request.accession );
    ASSERT_EQ ( string ("filename"), e.request.filename );
    ASSERT_EQ ( string (".1"), e.request.extension );
}

TEST_F ( TestParseFixture, AWS_every_field_is_quoted_dash )
{   // when the key is empty, parse the request field
    const char * InputLine =
"\"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\" \"-\"";

    SLogAWSEvent e = parse_accept( InputLine );

    ASSERT_EQ( "", e.owner );
    ASSERT_EQ( "", e.bucket );

    ASSERT_EQ( 0, e.time.day );
    ASSERT_EQ( 0, e.time.month );
    ASSERT_EQ( 0, e.time.year );
    ASSERT_EQ( 0, e.time.hour );
    ASSERT_EQ( 0, e.time.minute );
    ASSERT_EQ( 0, e.time.second );
    ASSERT_EQ( 0, e.time.offset );

    ASSERT_EQ( "", e.ip );
    ASSERT_EQ( "", e.requester );
    ASSERT_EQ( "", e.request_id );
    ASSERT_EQ( "", e.operation );
    ASSERT_EQ( "", e.key );
    ASSERT_EQ( "", e.request.method );
    ASSERT_EQ( "", e.request.path );
    ASSERT_EQ( "", e.request.accession );
    ASSERT_EQ( "", e.request.filename );
    ASSERT_EQ( "", e.request.extension );
    ASSERT_EQ( "", e.request.vers );

    ASSERT_EQ( "", e.res_code );
    ASSERT_EQ( "", e.error );
    ASSERT_EQ( "", e.res_len );
    ASSERT_EQ( "", e.obj_size );
    ASSERT_EQ( "", e.total_time );
    ASSERT_EQ( "", e.turnaround_time );
    ASSERT_EQ( "", e.referer );
    ASSERT_EQ( "", e.agent .original );
    ASSERT_EQ( "", e.version_id );
    ASSERT_EQ( "", e.host_id );
    ASSERT_EQ( "", e.sig_ver );
    ASSERT_EQ( "", e.cipher_suite );
    ASSERT_EQ( "", e.auth_type );
    ASSERT_EQ( "", e.host_header );
    ASSERT_EQ( "", e.tls_version );
}

TEST_F ( TestParseFixture, AWS_every_field_is_justdash )
{   // when the key is empty, parse the request field
    const char * InputLine =
"- - - - - - - - - - - - - - - - - - - - - - - -";

    SLogAWSEvent e = parse_accept( InputLine );

    ASSERT_EQ( "", e.owner );
    ASSERT_EQ( "", e.bucket );

    ASSERT_EQ( 0, e.time.day );
    ASSERT_EQ( 0, e.time.month );
    ASSERT_EQ( 0, e.time.year );
    ASSERT_EQ( 0, e.time.hour );
    ASSERT_EQ( 0, e.time.minute );
    ASSERT_EQ( 0, e.time.second );
    ASSERT_EQ( 0, e.time.offset );

    ASSERT_EQ( "", e.ip );
    ASSERT_EQ( "", e.requester );
    ASSERT_EQ( "", e.request_id );
    ASSERT_EQ( "", e.operation );
    ASSERT_EQ( "", e.key );
    ASSERT_EQ( "", e.request.method );
    ASSERT_EQ( "", e.request.path );
    ASSERT_EQ( "", e.request.accession );
    ASSERT_EQ( "", e.request.filename );
    ASSERT_EQ( "", e.request.extension );
    ASSERT_EQ( "", e.request.vers );

    ASSERT_EQ( "", e.res_code );
    ASSERT_EQ( "", e.error );
    ASSERT_EQ( "", e.res_len );
    ASSERT_EQ( "", e.obj_size );
    ASSERT_EQ( "", e.total_time );
    ASSERT_EQ( "", e.turnaround_time );
    ASSERT_EQ( "", e.referer );
    ASSERT_EQ( "", e.agent . original );
    ASSERT_EQ( "", e.version_id );
    ASSERT_EQ( "", e.host_id );
    ASSERT_EQ( "", e.sig_ver );
    ASSERT_EQ( "", e.cipher_suite );
    ASSERT_EQ( "", e.auth_type );
    ASSERT_EQ( "", e.host_header );
    ASSERT_EQ( "", e.tls_version );
}

// TEST_F ( TestParseFixture, AWS_extra_tokens )
// {   // when extra tokens are detected, fail the line
//     const char * InputLine =
// "922194806485875312b252374a3644f1feecd16802a50d4729885c1d11e1fd37 sra-pub-run-1 [20/Mar/2019:19:44:01 +0000] 130.14.20.103 arn:aws:iam::783971887864:user/ignatovi 92D3B218463A63EA REST.GET.TAGGING - \"GET /sra-pub-run-1?tagging= HTTP/1.1\" 404 NoSuchTagSet 293 - 59 - \"-\" \"S3Console/0.4, aws-internal/3 aws-sdk-java/1.11.509 Linux/4.9.137-0.1.ac.218.74.329.metal1.x86_64 OpenJDK_64-Bit_Server_VM/25.202-b08 java/1.8.0_202\" - AIDAISBTTLPGXGH6YFFAY LzYGhqEwXn5Xiuil9tI6JtK2PiIo+SC6Ute3Isq2qEmt/t0Z7qFkyD0mp1ZIc43bm0qSX4tBbbc= SigV4 ECDHE-RSA-AES128-SHA AuthHeader s3.amazonaws.com TLSv1.2";

//     std::istringstream inputstream( InputLine );
//     AWS_Parser p( m_lines, inputstream );
//     p.setDebug( false );
//     p.parse();
//     ASSERT_EQ ( 1, m_lines.m_unrecognized.size() );
// }

TEST_F ( TestParseFixture, AWS_empty_user_agent )
{   
    const char * InputLine =
"922194806485875312b252374a3644f1feecd16802a50d4729885c1d11e1fd37 sra-pub-run-5 [09/Mar/2020:22:53:57 +0000] 35.172.121.21 arn:aws:sts::783971887864:assumed-role/sra-developer-instance-profile-role/i-04a6132b8172af805 479B09DC662E4B67 REST.GET.BUCKET - \"GET ?list-type=2&delimiter=%2F&prefix=SRR11060177%2FSRR99999999/filename.1&morefilenames.moreextensions.1&name=SRR000123&encoding-type=url HTTP/1.1\" 200 - 325 - 14 14 \"-\" - - 4588JL1XJI30m/MURh3Xoz4qVakHYt/u1JwJ/u4BvxAUCOFUfvPJAG/utO0+cBgipDArBig9kL4= SigV4 ECDHE-RSA-AES128-GCM-SHA256 AuthHeader sra-pub-run-5.s3.amazonaws.com TLSv1.2";

    SLogAWSEvent e = parse_accept( InputLine );
    ASSERT_EQ( "", e.agent.original );
    ASSERT_EQ( "", e.agent.vdb_os );
    ASSERT_EQ( "", e.agent.vdb_tool );
    ASSERT_EQ( "", e.agent.vdb_release );
    ASSERT_EQ( "", e.agent.vdb_phid_compute_env );
    ASSERT_EQ( "", e.agent.vdb_phid_guid );
    ASSERT_EQ( "", e.agent.vdb_phid_session_id );
    ASSERT_EQ( "", e.agent.vdb_libc );
}

TEST_F ( TestParseFixture, AWS_user_agent )
{   
    const char * InputLine =
"922194806485875312b252374a3644f1feecd16802a50d4729885c1d11e1fd37 sra-pub-run-5 [09/Mar/2020:22:53:57 +0000] 35.172.121.21 arn:aws:sts::783971887864:assumed-role/sra-developer-instance-profile-role/i-04a6132b8172af805 479B09DC662E4B67 REST.GET.BUCKET - \"GET ?list-type=2&delimiter=%2F&prefix=SRR11060177%2FSRR99999999/filename.1&morefilenames.moreextensions.1&name=SRR000123&encoding-type=url HTTP/1.1\" 200 - 325 - 14 14 \"-\" \"linux64 sra-toolkit test-sra.2.8.2 (phid=noc7737000,libc=2.17)\" - 4588JL1XJI30m/MURh3Xoz4qVakHYt/u1JwJ/u4BvxAUCOFUfvPJAG/utO0+cBgipDArBig9kL4= SigV4 ECDHE-RSA-AES128-GCM-SHA256 AuthHeader sra-pub-run-5.s3.amazonaws.com TLSv1.2";

    SLogAWSEvent e = parse_accept( InputLine );
    ASSERT_EQ( "linux64 sra-toolkit test-sra.2.8.2 (phid=noc7737000,libc=2.17)", e.agent.original );
    ASSERT_EQ( "linux64", e.agent.vdb_os );
    ASSERT_EQ( "test-sra", e.agent.vdb_tool );
    ASSERT_EQ( "2.8.2", e.agent.vdb_release );
    ASSERT_EQ( "noc", e.agent.vdb_phid_compute_env );
    ASSERT_EQ( "7737", e.agent.vdb_phid_guid );
    ASSERT_EQ( "000", e.agent.vdb_phid_session_id );
    ASSERT_EQ( "2.17", e.agent.vdb_libc );
}

TEST_F ( TestParseFixture, AWS_2_part_hostId )
{   
    const char * InputLine =
"922194806485875312b252374a3644f1feecd16802a50d4729885c1d11e1fd37 sra-pub-run-1 [20/Mar/2019:19:44:01 +0000] 130.14.20.103 arn:aws:iam::783971887864:user/ignatovi 92D3B218463A63EA REST.GET.TAGGING - \"GET /sra-pub-run-1?tagging= HTTP/1.1\" 404 NoSuchTagSet 293 - 59 - \"-\" \"S3Console/0.4, aws-internal/3 aws-sdk-java/1.11.509 Linux/4.9.137-0.1.ac.218.74.329.metal1.x86_64 OpenJDK_64-Bit_Server_VM/25.202-b08 java/1.8.0_202\" - AIDAISBTTLPGXGH6YFFAY LzYGhqEwXn5Xiuil9tI6JtK2PiIo+SC6Ute3Isq2qEmt/t0Z7qFkyD0mp1ZIc43bm0qSX4tBbbc= SigV4 ECDHE-RSA-AES128-SHA AuthHeader s3.amazonaws.com TLSv1.2";

    SLogAWSEvent e = parse_accept( InputLine );
    ASSERT_EQ( "AIDAISBTTLPGXGH6YFFAY LzYGhqEwXn5Xiuil9tI6JtK2PiIo+SC6Ute3Isq2qEmt/t0Z7qFkyD0mp1ZIc43bm0qSX4tBbbc=", e.host_id );
}

TEST_F ( TestParseFixture, AWS_1_part_hostId_a )
{   
    const char * InputLine = "- - - - - - - - - - - - - - - - - - AIDAISBTTLPGXGH6YFFAY - - - - TLSv1.2";
    SLogAWSEvent e = parse_accept( InputLine );
    ASSERT_EQ( "AIDAISBTTLPGXGH6YFFAY", e.host_id );
}

TEST_F ( TestParseFixture, AWS_1_part_hostId_b )
{   
    const char * InputLine = "- - - - - - - - - - - - - - - - - - LzYGhqEwXn5Xiuil9tI6JtK2PiIo+SC6Ute3Isq2qEmt/t0Z7qFkyD0mp1ZIc43bm0qSX4tBbbc= - - - - TLSv1.2";
    SLogAWSEvent e = parse_accept( InputLine );
    ASSERT_EQ( "LzYGhqEwXn5Xiuil9tI6JtK2PiIo+SC6Ute3Isq2qEmt/t0Z7qFkyD0mp1ZIc43bm0qSX4tBbbc=", e.host_id );
}

//TODO: rejected lines with more than IP recognized

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}
