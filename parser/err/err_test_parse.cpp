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

TEST_F( MSGParseTestFixture, host )
{
    std::string res = try_to_parse_good( "\"/netmnt/traces04//sra75/SRR/008255/SRR8453128\" failed (2: No such file or directory), client: 218.219.98.126, server: srafiles21.be-md.ncbi.nlm.nih.gov, request: \"GET /traces/sra75/SRR/008255/SRR8453128 HTTP/1.1\", host: \"sra-downloadb.be-md.ncbi.nlm.nih.gov\"\n" );
    ASSERT_EQ( "sra-downloadb.be-md.ncbi.nlm.nih.gov", extract_value( res, "host" ) );
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


extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}
