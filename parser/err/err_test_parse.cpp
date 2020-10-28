#include <gtest/gtest.h>

#include <sstream>
#include <vector>

#include "ERR_Interface.hpp"
#include "MSG_Interface.hpp"

#include "Formatters.hpp"
#include "CatWriters.hpp"
#include "parse_test_fixture.hpp"

using namespace std;
using namespace NCBI::Logging;

#define INIT_TSTR(t, s)     do { (t).p = s; (t).n = strlen(s); } while (false)

// MSG post-parsing

class MSGParseBlockFactory : public ParseBlockFactoryInterface
{
public:
    MSGParseBlockFactory() : m_receiver( std::make_shared<JsonLibFormatter>() ) {}
    virtual ~MSGParseBlockFactory() {}
    virtual std::unique_ptr<ParseBlockInterface> MakeParseBlock() const
    {
        return std::make_unique<MSGParseBlock>( m_receiver );
    }
    MSGReceiver mutable m_receiver;
};

class MSGParseTestFixture : public ParseTestFixture< MSGParseBlockFactory >
{
};

TEST_F( MSGParseTestFixture, empty )
{
    std::string res = try_to_parse_ugly( "\n" );
    ASSERT_EQ( "{\"_line_nr\":1,\"_unparsed\":\"\"}\n", res );
}

TEST_F( MSGParseTestFixture, client )
{
    std::string res = try_to_parse_good( "\"/netmnt/traces04//sra75/SRR/008255/SRR8453128\" failed (2: No such file or directory), client: 218.219.98.126, server: srafiles21.be-md.ncbi.nlm.nih.gov, request: \"GET /traces/sra75/SRR/008255/SRR8453128 HTTP/1.1\", host: \"sra-downloadb.be-md.ncbi.nlm.nih.gov\"\n" );
    ASSERT_EQ( "218.219.98.126", extract_value( res, "client" ) );
}

TEST_F( MSGParseTestFixture, server )
{
    std::string res = try_to_parse_good( "\"/netmnt/traces04//sra75/SRR/008255/SRR8453128\" failed (2: No such file or directory), client: 218.219.98.126, server: srafiles21.be-md.ncbi.nlm.nih.gov, request: \"GET /traces/sra75/SRR/008255/SRR8453128 HTTP/1.1\", host: \"sra-downloadb.be-md.ncbi.nlm.nih.gov\"\n" );
    ASSERT_EQ( "srafiles21.be-md.ncbi.nlm.nih.gov", extract_value( res, "server" ) );
}

TEST_F( MSGParseTestFixture, request )
{
    std::string res = try_to_parse_good( "\"/netmnt/traces04//sra75/SRR/008255/SRR8453128\" failed (2: No such file or directory), client: 218.219.98.126, server: srafiles21.be-md.ncbi.nlm.nih.gov, request: \"GET /traces/sra75/SRR/008255/SRR8453128 HTTP/1.1\", host: \"sra-downloadb.be-md.ncbi.nlm.nih.gov\"\n" );
    ASSERT_EQ( "GET", extract_value( res, "method" ) );
    ASSERT_EQ( "/traces/sra75/SRR/008255/SRR8453128", extract_value( res, "path" ) );
    ASSERT_EQ( "HTTP/1.1", extract_value( res, "vers" ) );
}

TEST_F( MSGParseTestFixture, request_vers_missing_1 )
{
    std::string res = try_to_parse_good( "\"/netmnt/traces04//sra75/SRR/008255/SRR8453128\" failed (2: No such file or directory), client: 218.219.98.126, server: srafiles21.be-md.ncbi.nlm.nih.gov, request: \"GET /traces/sra75/SRR/008255/SRR8453128 \", host: \"sra-downloadb.be-md.ncbi.nlm.nih.gov\"\n" );
    ASSERT_EQ( "GET", extract_value( res, "method" ) );
    ASSERT_EQ( "/traces/sra75/SRR/008255/SRR8453128", extract_value( res, "path" ) );
}

TEST_F( MSGParseTestFixture, request_vers_missing_2 )
{
    std::string res = try_to_parse_good( "\"/netmnt/traces04//sra75/SRR/008255/SRR8453128\" failed (2: No such file or directory), client: 218.219.98.126, server: srafiles21.be-md.ncbi.nlm.nih.gov, request: \"GET /traces/sra75/SRR/008255/SRR8453128\", host: \"sra-downloadb.be-md.ncbi.nlm.nih.gov\"\n" );
    ASSERT_EQ( "GET", extract_value( res, "method" ) );
    ASSERT_EQ( "/traces/sra75/SRR/008255/SRR8453128", extract_value( res, "path" ) );
}

TEST_F( MSGParseTestFixture, host )
{
    std::string res = try_to_parse_good( "\"/netmnt/traces04//sra75/SRR/008255/SRR8453128\" failed (2: No such file or directory), client: 218.219.98.126, server: srafiles21.be-md.ncbi.nlm.nih.gov, request: \"GET /traces/sra75/SRR/008255/SRR8453128 HTTP/1.1\", host: \"sra-downloadb.be-md.ncbi.nlm.nih.gov\"\n" );
    ASSERT_EQ( "sra-downloadb.be-md.ncbi.nlm.nih.gov", extract_value( res, "host" ) );
}

TEST_F( MSGParseTestFixture, trailing_space )
{
    std::string res = try_to_parse_good( "ModSecurity for nginx (STABLE)/2.9.3 (http://www.modsecurity.org/) configured.: ", true );
    ASSERT_NE( "", res );
}

// ERR parsing
TEST( LogERREventTest, Create )
{
    ERRReceiver e ( make_shared<JsonLibFormatter>() );
}

class ERRParseTestFixture : public ParseTestFixture< ERRParseBlockFactory >
{
};

TEST_F( ERRParseTestFixture, lonely_eol )
{
    std::string res = try_to_parse_ugly( "\n" );
    ASSERT_EQ( "{\"_line_nr\":1,\"_unparsed\":\"\"}\n", res );
}

TEST_F( ERRParseTestFixture, line )
{
    std::string res = try_to_parse_good( "2020/10/01 20:15:27 [alert] 61030#0: *650699 open() \"/netmnt/traces04//sra75/SRR/008255/SRR8453128\" failed (2: No such file or directory), client: 218.219.98.126, server: srafiles21.be-md.ncbi.nlm.nih.gov, request: \"GET /traces/sra75/SRR/008255/SRR8453128 HTTP/1.1\", host: \"sra-downloadb.be-md.ncbi.nlm.nih.gov\"" );

    ASSERT_EQ( "2020/10/01 20:15:27", extract_value( res, "datetime" ) );
    ASSERT_EQ( "[alert]", extract_value( res, "severity" ) );
    ASSERT_EQ( "61030#0:", extract_value( res, "pid" ) );
    ASSERT_EQ( "*650699 open() \"/netmnt/traces04//sra75/SRR/008255/SRR8453128\" failed (2: No such file or directory), client: 218.219.98.126, server: srafiles21.be-md.ncbi.nlm.nih.gov, request: \"GET /traces/sra75/SRR/008255/SRR8453128 HTTP/1.1\", host: \"sra-downloadb.be-md.ncbi.nlm.nih.gov\"", extract_value( res, "msg" ) );
}

TEST_F( ERRParseTestFixture, file_meta )
{
    const char * input = "file-meta ncbi_location=be-md&ncbi_role=production&applog_db_rotate_host=srafiles21";
    std::string res = try_to_parse_ignored( input );
    ASSERT_EQ( res, "{\"_line_nr\":1,\"_unparsed\":\"file-meta ncbi_location=be-md&ncbi_role=production&applog_db_rotate_host=srafiles21\"}\n" );
}

TEST_F( ERRParseTestFixture, not_file_meta )
{
    const char * input = "something ncbi_location=be-md&ncbi_role=production&applog_db_rotate_host=srafiles21";
    std::string res = try_to_parse_review( input );
    ASSERT_EQ( res, "{\"_line_nr\":1,\"_unparsed\":\"something ncbi_location=be-md&ncbi_role=production&applog_db_rotate_host=srafiles21\"}\n" );
}

TEST_F( ERRParseTestFixture, PostProcessing )
{
    std::string res = try_to_parse_good( "2020/10/01 20:15:27 [alert] 61030#0: *650699 open() \"/netmnt/traces04//sra75/SRR/008255/SRR8453128\" failed (2: No such file or directory), client: 218.219.98.126, server: srafiles21.be-md.ncbi.nlm.nih.gov, request: \"GET /traces/sra75/SRR/008255/SRR8453128/file.ext HTTP/1.1\", host: \"sra-downloadb.be-md.ncbi.nlm.nih.gov\"" );

    ASSERT_EQ( "218.219.98.126", extract_value( res, "client" ) );
    ASSERT_EQ( "srafiles21.be-md.ncbi.nlm.nih.gov", extract_value( res, "server" ) );
    ASSERT_EQ( "GET", extract_value( res, "method" ) );
    ASSERT_EQ( "/traces/sra75/SRR/008255/SRR8453128/file.ext", extract_value( res, "path" ) );
    ASSERT_EQ( "HTTP/1.1", extract_value( res, "vers" ) );
    ASSERT_EQ( "sra-downloadb.be-md.ncbi.nlm.nih.gov", extract_value( res, "host" ) );
    // URL post-parser
    ASSERT_EQ( "SRR8453128", extract_value( res, "accession" ) );
    ASSERT_EQ( "file", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext", extract_value( res, "extension" ) );
}

TEST_F( ERRParseTestFixture, Error_Open )
{
    std::string res = try_to_parse_good( "2020/10/01 20:15:27 [alert] 61030#0: *650699 open() \"/netmnt/traces04//sra75/SRR/008255/SRR8453128\" failed (2: No such file or directory), client: 218.219.98.126, server: srafiles21.be-md.ncbi.nlm.nih.gov, request: \"GET /traces/sra75/SRR/008255/SRR8453128/file.ext HTTP/1.1\", host: \"sra-downloadb.be-md.ncbi.nlm.nih.gov\"" );

    ASSERT_EQ( "openFailed", extract_value( res, "cat" ) );
}

TEST_F( ERRParseTestFixture, Error_Unlink )
{
    std::string res = try_to_parse_good( "2020/10/01 20:15:27 [alert] 61030#0: *650699 unlink() \"/export/nginx/proxy_cache/9/f4/0019a4a92b13d51bafaeeda76adf4f49\" failed (2: No such file or directory)" );
    ASSERT_EQ( "unlinkFailed", extract_value( res, "cat" ) );
}

TEST_F( ERRParseTestFixture, Error_Forbidden )
{
    std::string res = try_to_parse_good( "2020/10/15 00:03:10 [error] 61000#0: *643797 directory index of \"/home/dbtest/data/sracloud/\" is forbidden, client: 130.14.252.53, server: sra-download.ncbi.nlm.nih.gov, request: \"GET /\"" );
    ASSERT_EQ( "forbidden", extract_value( res, "cat" ) );
}

TEST_F( ERRParseTestFixture, Error_InactiveCache )
{
    std::string res = try_to_parse_good( "2020/10/15 00:12:16 [alert] 61030#0: ignore long locked inactive cache entry 3fc39b4cff6a53a2d675e621124f3c7a, count:1" );
    ASSERT_EQ( "inactive", extract_value( res, "cat" ) );
}

TEST_F( ERRParseTestFixture, Error_UpstreamClosed )
{
    std::string res = try_to_parse_good( "2020/10/15 00:19:51 [error] 60999#0: *648365 upstream prematurely closed connection while reading upstream, client: 112.17.92.191, server: srafiles21.be-md.ncbi.nlm.nih.gov, request: \"GET /sos2/sra-pub-run-13/ERR2239132/ERR2239132.1 HTTP/1.1\", upstream: \"http://10.154.190.39:80/sra-pub-run-13/ERR2239132/ERR2239132.1\", host: \"sra-downloadb.be-md.ncbi.nlm.nih.gov\"" );
    ASSERT_EQ( "upstream", extract_value( res, "cat" ) );
}

TEST_F( ERRParseTestFixture, Error_IsNotFound )
{
    std::string res = try_to_parse_good( "2020/10/26 04:47:45 [error] 43034#0: *14195760 \"/home/dbtest/data/sracloud/ref/index.html\" is not found (2: No such file or directory), client: 34.201.223.237, server: sra-download.ncbi.nlm.nih.gov, request: \"GET /ref/ HTTP/1.1\", host: \"srafiles11-1.be-md.ncbi.nlm.nih.gov\"" );
    ASSERT_EQ( "not-found", extract_value( res, "cat" ) );
}

TEST_F( ERRParseTestFixture, Error_couldNotFindLocation )
{
    std::string res = try_to_parse_good( "2020/10/26 19:12:10 [error] 43071#0: *14944615 could not find named location \"@405\", client: 10.154.26.23, server: srafiles11.be-md.ncbi.nlm.nih.gov, request: \"GBSTTN / HTTP/1.1\", host: \"srafiles11.be-md.ncbi.nlm.nih.gov\"" );
    ASSERT_EQ( "not-found", extract_value( res, "cat" ) );
}

TEST_F( ERRParseTestFixture, Error_failedSSLHandshake )
{

    std::string res = try_to_parse_good( "2020/10/26 19:22:53 [crit] 43025#0: *14959785 SSL_do_handshake() failed (SSL: error:1408A0D7:SSL routines:ssl3_get_client_hello:required cipher missing) while SSL handshaking, client: 10.154.26.23, server: 0.0.0.0:443" );
    ASSERT_EQ( "failedSSL", extract_value( res, "cat" ) );
}

TEST_F( ERRParseTestFixture, Error_Unknown )
{
    std::string res = try_to_parse_good( "2020/10/15 00:19:51 [error] 60999#0: *648365 client: 112.17.92.191, server: srafiles21.be-md.ncbi.nlm.nih.gov, request: \"GET /sos2/sra-pub-run-13/ERR2239132/ERR2239132.1 HTTP/1.1\", upstream: \"http://10.154.190.39:80/sra-pub-run-13/ERR2239132/ERR2239132.1\", host: \"sra-downloadb.be-md.ncbi.nlm.nih.gov\"" );
    ASSERT_EQ( "unknown", extract_value( res, "cat" ) );
}

TEST_F( ERRParseTestFixture, stat_failed )
{
    std::string res = try_to_parse_good( "2020/10/26 19:12:09 [error] 43057#0: *14944557 stat() \"/home/dbtest/data/sracloud/admin/login.jsp\" failed (2: No such file or directory), client: 10.154.26.23, server: srafiles11.be-md.ncbi.nlm.nih.gov, request: \"GET /admin/login.jsp HTTP/1.1\", host: \"srafiles11.be-md.ncbi.nlm.nih.gov\"" );
    ASSERT_EQ( "statFailed", extract_value( res, "cat" ) );
}

TEST_F( ERRParseTestFixture, pread_failed )
{
    std::string res = try_to_parse_good( "2020/10/26 18:58:27 [crit] 43038#0: *14926361 pread() \"/netmnt/traces04//sra13/SRR/012583/SRR12885188\" failed (5: Input/output error) while sending response to client, client: 3.236.230.138, server: sra-download.ncbi.nlm.nih.gov, request: \"GET /traces/sra13/SRR/012583/SRR12885188 HTTP/1.1\", host: \"sra-download.be-md.ncbi.nlm.nih.gov\"" );
    ASSERT_EQ( "preadFailed", extract_value( res, "cat" ) );
}

TEST_F( ERRParseTestFixture, readv_failed )
{
    std::string res = try_to_parse_good( "2020/10/26 15:23:35 [error] 60986#0: *15481986 readv() failed (104: Connection reset by peer) while reading upstream, client: 162.105.250.222, server: srafiles21.be-md.ncbi.nlm.nih.gov, request: \"GET /sos2/sra-pub-run-11/SRR6178394/SRR6178394.1 HTTP/1.1\", upstream: \"http://10.154.190.33:80/sra-pub-run-11/SRR6178394/SRR6178394.1\", host: \"sra-downloadb.be-md.ncbi.nlm.nih.gov\"" );
    ASSERT_EQ( "readvFailed", extract_value( res, "cat" ) );
}

TEST_F( ERRParseTestFixture, timed_out )
{
    std::string res = try_to_parse_good( "2020/10/26 14:28:32 [error] 60999#0: *15390740 upstream timed out (110: Connection timed out) while connecting to upstream, client: 34.203.236.120, server: srafiles21.be-md.ncbi.nlm.nih.gov, request: \"GET /sos2/sra-pub-run-11/SRR6191655/SRR6191655.1 HTTP/1.1\", upstream: \"http://10.154.190.39:80/sra-pub-run-11/SRR6191655/SRR6191655.1\", host: \"sra-downloadb.be-md.ncbi.nlm.nih.gov\"" );
    ASSERT_EQ( "timedOut", extract_value( res, "cat" ) );
}

TEST_F( ERRParseTestFixture, open_socket )
{
    std::string res = try_to_parse_good("2020/10/26 03:41:05 [alert] 10439#0: *3856127 open socket #64 left in connection 35" );
    ASSERT_EQ( "openSocket", extract_value( res, "cat" ) );
}

TEST_F( ERRParseTestFixture, host_not_found )
{
    std::string res = try_to_parse_good("2020/10/26 03:44:56 [emerg] 3476#0: host not found in upstream \"s3-stor31-dc1.st-va.ncbi.nlm.nih.gov\" in /etc/nginx/conf.d/cloudian_dc1-upstream.conf:6" );
    ASSERT_EQ( "hostNotFound", extract_value( res, "cat" ) );
}

TEST_F( ERRParseTestFixture, aborting )
{
    std::string res = try_to_parse_good( "2020/10/26 03:41:05 [alert] 10439#0: aborting" );
    ASSERT_EQ( "aborting", extract_value( res, "cat" ) );
}

TEST_F( ERRParseTestFixture, exiting )
{
    std::string res = try_to_parse_good( "2020/10/26 03:41:09 [alert] 10426#0: worker process 10445 exited on signal 9" );
    ASSERT_EQ( "exiting", extract_value( res, "cat" ) );
}

TEST_F( ERRParseTestFixture, ModSecurity )
{
    std::string res = try_to_parse_good( "2020/10/26 03:57:34 [notice] 10839#0: ModSecurity for nginx (STABLE)/2.9.3 (http://www.modsecurity.org/) configured.: " );
    ASSERT_EQ( "ModSecurity", extract_value( res, "cat" ) );
}

TEST_F( ERRParseTestFixture, ssl_deprecated )
{
    std::string res = try_to_parse_good( "2020/10/26 03:57:34 [warn] 10839#0: the \"ssl\" directive is deprecated, use the \"listen ... ssl\" directive instead in /etc/nginx/sites-enabled/sra-download.       ncbi.nlm.nih.gov.conf:57" );
    ASSERT_EQ( "directiveDeprecated", extract_value( res, "cat" ) );
}

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}
