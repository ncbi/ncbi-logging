#include <gtest/gtest.h>

#include "AGENT_Interface.hpp"
#include "Formatters.hpp"
#include "parse_test_fixture.hpp"

using namespace NCBI::Logging;
using namespace std;

TEST( AGENT_Parsing, Create )
{
    AGENTReceiver receiver( make_shared<JsonFastFormatter>() );
    AGENTParseBlock pb( receiver );
}

TEST( AGENT_Parsing, EmptyString )
{
    AGENTReceiver receiver( make_shared<JsonFastFormatter>() );
    AGENTParseBlock pb( receiver );
    pb.format_specific_parse( "", 0 );
    ASSERT_EQ( "{}", receiver.GetFormatter().format() );
}

class AGENTParseBlockFactory : public ParseBlockFactoryInterface
{
public:
    AGENTParseBlockFactory() : m_receiver( std::make_shared<JsonLibFormatter>() ) {}
    virtual ~AGENTParseBlockFactory() {}
    virtual std::unique_ptr<ParseBlockInterface> MakeParseBlock() const
    {
        return std::make_unique<AGENTParseBlock>( m_receiver );
    }
    AGENTReceiver mutable m_receiver;
};

class AGENTTestFixture : public ParseTestFixture< AGENTParseBlockFactory >
{
};

TEST_F( AGENTTestFixture, JustOS )
{
    const std::string res = try_to_parse_good( "linux64" );
    ASSERT_EQ( "linux64", extract_value( res, "os" ) );
}

TEST_F( AGENTTestFixture, OSandToolkit )
{
    const std::string res = try_to_parse_good( "linux64 sra-toolkit" );
    ASSERT_EQ( "linux64", extract_value( res, "os" ) );
}

TEST_F( AGENTTestFixture, phid_3part )
{
    const std::string ce="12a";
    const std::string guid="456b";
    const std::string ses="89c";
    const std::string res = try_to_parse_good( std::string( "phid=" ) + ce + guid + ses );
    ASSERT_EQ( ce, extract_value( res, "phid_compute_env" ) );
    ASSERT_EQ( guid, extract_value( res, "phid_guid" ) );
    ASSERT_EQ( ses, extract_value( res, "phid_session_id" ) );
}

TEST_F( AGENTTestFixture, phid_3part_no_values )
{
    const std::string ce="noc";
    const std::string guid="nog";
    const std::string ses="nos";
    const std::string res = try_to_parse_good( std::string( "phid=" ) + ce + guid + ses );
    ASSERT_EQ( ce, extract_value( res, "phid_compute_env" ) );
    ASSERT_EQ( guid, extract_value( res, "phid_guid" ) );
    ASSERT_EQ( ses, extract_value( res, "phid_session_id" ) );
}

TEST_F( AGENTTestFixture, phid_3part_2nd_invalid )
{
    const std::string ce="000";
    const std::string guid="XXXX";
    const std::string ses="111";
    const std::string res = try_to_parse_good( std::string( "phid=" ) + ce + guid + ses + " linux64" );
    ASSERT_EQ( ce, extract_value( res, "phid_compute_env" ) );
    ASSERT_THROW( extract_value( res, "phid_guid" ), ncbi::Exception );
    ASSERT_THROW( extract_value( res, "phid_session_id" ), ncbi::Exception );
    ASSERT_EQ( "linux64", extract_value( res, "os" ) );
}

TEST_F( AGENTTestFixture, tool_release )
{
    const std::string tool="fastq-dump";
    const std::string vers="2.8.3";
    const std::string res = try_to_parse_good( tool + "." + vers );
    ASSERT_EQ( tool, extract_value( res, "tool" ) );
    ASSERT_EQ( vers, extract_value( res, "release" ) );
}

TEST_F( AGENTTestFixture, tool_no_release )
{
    const std::string tool="fastq-dump";
    const std::string res = try_to_parse_good( tool + " linux64"  );
    ASSERT_EQ( tool, extract_value( res, "tool" ) );
    ASSERT_EQ( "linux64", extract_value( res, "os" ) );
    ASSERT_THROW( extract_value( res, "release" ), ncbi::Exception );
}

TEST_F( AGENTTestFixture, valid_libc )
{
    const std::string res = try_to_parse_good( "libc=2.8"  );
    ASSERT_EQ( "2.8", extract_value( res, "libc" ) );
}
