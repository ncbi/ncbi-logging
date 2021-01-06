#include <gtest/gtest.h>

#include "CL_PATH_Interface.hpp"
#include "Formatters.hpp"
#include "parse_test_fixture.hpp"

using namespace NCBI::Logging;
using namespace std;

TEST( CL_PATH_Parsing, Create )
{
    CLPATHReceiver receiver( make_shared<JsonFastFormatter>() );
    CLPATHParseBlock pb( receiver );
}

TEST( CL_PATH_Parsing, EmptyString )
{
    CLPATHReceiver receiver( make_shared<JsonFastFormatter>() );
    CLPATHParseBlock pb( receiver );
    pb.format_specific_parse( "", 0 );
    receiver . finalize();
    ASSERT_EQ( "{\"accession\":\"\",\"extension\":\"\",\"filename\":\"\"}", receiver.GetFormatter().format() );
}

class TestCLPATHParseBlock : public CLPATHParseBlock
{
public:
    TestCLPATHParseBlock( CLPATHReceiver & receiver )
    :   CLPATHParseBlock( receiver ),
        found_accession ( false )
    {}

    bool format_specific_parse( const char * line, size_t line_size )
    {
        CLPATHParseBlock::format_specific_parse( line, line_size );
        CLPATHReceiver & receiver = static_cast< CLPATHReceiver & > ( GetReceiver() );
        found_accession = ! receiver . m_accession . empty();
        receiver.finalize(); // this punches a cat of good unconditionally into the receiver
        return true;
    }

    // The CLPATHReceiver does not call a format-setter during parsing, it instead
    // records accession/filename/extension in internal string-members.
    bool found_accession;
};

class TestCLPATHParseBlockFactory : public ParseBlockFactoryInterface
{
public:
    TestCLPATHParseBlockFactory() : m_receiver( std::make_shared<JsonLibFormatter>() ) {}
    virtual ~TestCLPATHParseBlockFactory() {}
    virtual std::unique_ptr<ParseBlockInterface> MakeParseBlock() const
    {
        return std::make_unique<TestCLPATHParseBlock>( m_receiver );
    }
    CLPATHReceiver mutable m_receiver;
};

class CLPATHTestFixture : public ParseTestFixture< TestCLPATHParseBlockFactory >
{
    public :
        const TestCLPATHParseBlock& get_test_url_parse_block( void ) const
        {
            return *( static_cast<const TestCLPATHParseBlock * >( pb . get() ) );
        }
};

TEST_F( CLPATHTestFixture, PathOnly )      { ASSERT_NE( "", try_to_parse_good( "path" ) ); }
TEST_F( CLPATHTestFixture, LongerPath )    { ASSERT_NE( "", try_to_parse_good( "/path/a/b.ext/c" ) ); }

TEST_F( CLPATHTestFixture, PathQuery )             { ASSERT_NE( "", try_to_parse_good( "/path/a/b/c?key=v1" ) ); }
TEST_F( CLPATHTestFixture, PathLongerQuery )       { ASSERT_NE( "", try_to_parse_good( "/path/a/b/c?k1=v1&k2=v2" ) ); }
TEST_F( CLPATHTestFixture, PathQueryNoEqual )      { ASSERT_NE( "", try_to_parse_good( "/path/a/b/c?k1v1&k2v2" ) ); }
TEST_F( CLPATHTestFixture, PathQuerySemicolon )    { ASSERT_NE( "", try_to_parse_good( "/path/a/b/c?k1v1;k2v2" ) ); }
TEST_F( CLPATHTestFixture, QueryOnly )             { ASSERT_NE( "", try_to_parse_good( "?k1v1;k2v2" ) ); }

TEST_F( CLPATHTestFixture, PathQueryFragment ) { ASSERT_NE( "", try_to_parse_good( "/path/a/b/c?query#fragment" ) ); }
TEST_F( CLPATHTestFixture, EmptyFragment )     { ASSERT_NE( "", try_to_parse_good( "/path/a/b/c?query#" ) ); }

TEST_F( CLPATHTestFixture, NoAccession )
{
    const std::string res = try_to_parse_good( "WRR000123" );
    ASSERT_EQ( "", extract_value( res, "accession" ) );

    ASSERT_FALSE( get_test_url_parse_block() . found_accession );
}
TEST_F( CLPATHTestFixture, JustAccession )
{
    const std::string res = try_to_parse_good( "SRR000123" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );

    ASSERT_TRUE( get_test_url_parse_block() . found_accession );
}

TEST_F( CLPATHTestFixture, MultipleAccessions )
{
    const std::string res = try_to_parse_good( "SRR000123/SRR000456/SRR000789" );
    ASSERT_EQ( "SRR000789", extract_value( res, "accession" ) );
}

TEST_F( CLPATHTestFixture, AccessionFilename )
{
    const std::string res = try_to_parse_good( "SRR000123/filename" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( CLPATHTestFixture, AccessionFilenameExtension )
{
    const std::string res = try_to_parse_good( "SRR000123/filename.ext" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext", extract_value( res, "extension" ) );
}

TEST_F( CLPATHTestFixture, AccessionIsTheFilenameIfMissing )
{   // when filename+extension are missing, use the accession as filename
    const std::string res = try_to_parse_good( "SRR000123" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR000123", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( CLPATHTestFixture, AccessionExtension )
{   // filename is missing but there is an extension, leave the filename empty
    const std::string res = try_to_parse_good( "SRR000123/.ext" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext", extract_value( res, "extension" ) );
}

TEST_F( CLPATHTestFixture, PathDotOnly )
{
    const std::string res = try_to_parse_good( "/." );
    ASSERT_EQ( "", extract_value( res, "accession" ) );
    ASSERT_EQ( "", extract_value( res, "filename" ) );
    ASSERT_EQ( ".", extract_value( res, "extension" ) );
}

TEST_F( CLPATHTestFixture, AccessionAsFilename )
{
    const std::string res = try_to_parse_good( "SRR000123.ext1.ext2.ext3" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR000123", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext1.ext2.ext3", extract_value( res, "extension" ) );
}

TEST_F( CLPATHTestFixture, MultipleFilenames )
{   // capture the leaf filename/ext
    const std::string res = try_to_parse_good( "SRR000123/f1.ext1/f2.ext2" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "f2", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext2", extract_value( res, "extension" ) );
}

TEST_F( CLPATHTestFixture, TwoAccessions_Second_will_be_filename )
{
    const std::string res = try_to_parse_good( "SRR000123/SRR000456.ext2" );
    ASSERT_EQ( "SRR000456", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR000456", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext2", extract_value( res, "extension" ) );
}

TEST_F( CLPATHTestFixture, FilenameBeforeAccession )
{
    const std::string res = try_to_parse_good( "/storage/SRR000123/filename.ext" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext", extract_value( res, "extension" ) );
}

TEST_F( CLPATHTestFixture, Accession_Something_Filename )
{
    const std::string res = try_to_parse_good( "/SRR000123/storage/filename.ext" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext", extract_value( res, "extension" ) );
}

TEST_F( CLPATHTestFixture, NoAccession_But_Filename )
{
    const std::string res = try_to_parse_good( "/storage/filename.ext" );
    ASSERT_EQ( "", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext", extract_value( res, "extension" ) );
}

TEST_F( CLPATHTestFixture, PathWithPct )
{
    const std::string res = try_to_parse_good( "ERR792423/5141526_%s1_p0.bas.h5.1%ab" );
    ASSERT_EQ( "ERR792423", extract_value( res, "accession" ) );
    ASSERT_EQ( "5141526_%s1_p0", extract_value( res, "filename" ) );
    ASSERT_EQ( ".bas.h5.1%ab", extract_value( res, "extension" ) );
}

TEST_F( CLPATHTestFixture, PathWithSlashURLEnc )
{
    const std::string res = try_to_parse_good( "ERR792423%2F5141526_%s1_p0.bas.h5.1%ab" );
    ASSERT_EQ( "ERR792423", extract_value( res, "accession" ) );
    ASSERT_EQ( "5141526_%s1_p0", extract_value( res, "filename" ) );
    ASSERT_EQ( ".bas.h5.1%ab", extract_value( res, "extension" ) );
}

TEST_F( CLPATHTestFixture, KeepFilenameAndExtensionTogether1 )
{
    const std::string res = try_to_parse_good( "/somewhere.1/filename.2" );
    ASSERT_EQ( "", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( ".2", extract_value( res, "extension" ) );
}

TEST_F( CLPATHTestFixture, KeepFilenameAndExtensionTogether2 )
{
    const std::string res = try_to_parse_good( "/somewhere.1/filename" );
    ASSERT_EQ( "", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( CLPATHTestFixture, TakeFilenameOnlyAfterAccession )
{
    const std::string res = try_to_parse_good( "/somewhere.1/SRR123456" );
    ASSERT_EQ( "SRR123456", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR123456", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( CLPATHTestFixture, DoNotMixFilenamAndExtensionFormDifferentPathSegments1 )
{
    const std::string res = try_to_parse_good( "/somewhere.1/.2" );
    ASSERT_EQ( "", extract_value( res, "accession" ) );
    ASSERT_EQ( "", extract_value( res, "filename" ) );
    ASSERT_EQ( ".2", extract_value( res, "extension" ) );
}

TEST_F( CLPATHTestFixture, ThereIsAEscapedQuoteInThePath )
{
    const std::string res = try_to_parse_good( "a\\\"" );
    ASSERT_EQ( "", extract_value( res, "accession" ) );
    ASSERT_EQ( "a\\\"", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( CLPATHTestFixture, EncodedSlashInPath )
{
    const std::string res = try_to_parse_good( "sos1%2Fsra-pub-run-2%2FSRR8422826%2FSRR8422826.1" );
    ASSERT_EQ( "SRR8422826", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR8422826", extract_value( res, "filename" ) );
    ASSERT_EQ( ".1", extract_value( res, "extension" ) );
}


TEST_F( CLPATHTestFixture, Investigate )
{
    const std::string res = try_to_parse_good( "/cgi-bin/d6rw1r5y.asp?<script>document.cookie=%22testtifh=7678;%22</script>" );
    ASSERT_EQ( "", extract_value( res, "accession" ) );
    ASSERT_EQ( "d6rw1r5y", extract_value( res, "filename" ) );
    ASSERT_EQ( ".asp", extract_value( res, "extension" ) );
}

TEST_F( CLPATHTestFixture, FishingForWinIni )
{
    const std::string res = try_to_parse_good( "..\\x5C..\\x5C..\\x5C..\\x5C..\\x5C..\\x5C..\\x5C..\\x5C..\\x5C..\\x5Cwindows\\x5Cwin.ini" );
    ASSERT_EQ( "", extract_value( res, "accession" ) );
    ASSERT_EQ( "", extract_value( res, "filename" ) );
    ASSERT_EQ( "..\\x5C..\\x5C..\\x5C..\\x5C..\\x5C..\\x5C..\\x5C..\\x5C..\\x5C..\\x5Cwindows\\x5Cwin.ini", extract_value( res, "extension" ) );
}

//---------------------------------------------------------------------

TEST_F( CLPATHTestFixture, QMJustAccession )
{   // ? is the end of input
    const std::string res = try_to_parse_good( "?SRR000123" );
    ASSERT_EQ( "", extract_value( res, "accession" ) );
    ASSERT_EQ( "", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( CLPATHTestFixture, DoubleQmarks )
{
    ASSERT_NE( "", try_to_parse_good( "SRR7121804%2FSRR7121804.3?versionId=null?partNumber=2807" ) );
}

TEST_F( CLPATHTestFixture, Experiment )
{
    ASSERT_NE( "", try_to_parse_good( "SRR7121804?a=b=c" ) );
}