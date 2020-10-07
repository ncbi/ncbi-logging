#include <gtest/gtest.h>

#include "URL_Interface.hpp"
#include "Formatters.hpp"
#include "parse_test_fixture.hpp"

using namespace NCBI::Logging;
using namespace std;

TEST( URL_Parsing, Create )
{
    URLReceiver receiver( make_shared<JsonFastFormatter>() );
    URLParseBlock pb( receiver );
}

TEST( URL_Parsing, EmptyString )
{
    URLReceiver receiver( make_shared<JsonFastFormatter>() );
    URLParseBlock pb( receiver );
    pb.format_specific_parse( "", 0 );
    receiver . finalize();
    ASSERT_EQ( "{\"accession\":\"\",\"extension\":\"\",\"filename\":\"\"}", receiver.GetFormatter().format() );
}

class TestURLParseBlock : public URLParseBlock
{
public:
    TestURLParseBlock( URLReceiver & receiver ) : URLParseBlock( receiver ) {}

    bool format_specific_parse( const char * line, size_t line_size )
    {
        bool res = URLParseBlock::format_specific_parse( line, line_size );
        URLReceiver & receiver = static_cast< URLReceiver & > ( GetReceiver() );
        receiver.finalize();
        return res;
    }
};

class TestURLParseBlockFactory : public ParseBlockFactoryInterface
{
public:
    TestURLParseBlockFactory() : m_receiver( std::make_shared<JsonLibFormatter>() ) {}
    virtual ~TestURLParseBlockFactory() {}
    virtual std::unique_ptr<ParseBlockInterface> MakeParseBlock() const
    {
        return std::make_unique<TestURLParseBlock>( m_receiver );
    }
    URLReceiver mutable m_receiver;
};

class URLTestFixture : public ParseTestFixture< TestURLParseBlockFactory >
{

};

TEST_F( URLTestFixture, PathOnly )      { ASSERT_NE( "", try_to_parse_good( "path" ) ); }
TEST_F( URLTestFixture, LongerPath )    { ASSERT_NE( "", try_to_parse_good( "/path/a/b.ext/c" ) ); }

TEST_F( URLTestFixture, PathQuery )             { ASSERT_NE( "", try_to_parse_good( "/path/a/b/c?key=v1" ) ); }
TEST_F( URLTestFixture, PathLongerQuery )       { ASSERT_NE( "", try_to_parse_good( "/path/a/b/c?k1=v1&k2=v2" ) ); }
TEST_F( URLTestFixture, PathQueryNoEqual )      { ASSERT_NE( "", try_to_parse_good( "/path/a/b/c?k1v1&k2v2" ) ); }
TEST_F( URLTestFixture, PathQuerySemicolon )    { ASSERT_NE( "", try_to_parse_good( "/path/a/b/c?k1v1;k2v2" ) ); }
TEST_F( URLTestFixture, QueryOnly )             { ASSERT_NE( "", try_to_parse_good( "?k1v1;k2v2" ) ); }

TEST_F( URLTestFixture, PathQueryFragment ) { ASSERT_NE( "", try_to_parse_good( "/path/a/b/c?query#fragment" ) ); }
TEST_F( URLTestFixture, EmptyFragment )     { ASSERT_NE( "", try_to_parse_good( "/path/a/b/c?query#" ) ); }

TEST_F( URLTestFixture, JustAccession )
{
    const std::string res = try_to_parse_good( "SRR000123" );
    std::cout << res << std::endl;
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
}

TEST_F( URLTestFixture, MultipleAccessions )
{
    const std::string res = try_to_parse_good( "SRR000123/SRR000456/SRR000789" );
    ASSERT_EQ( "SRR000789", extract_value( res, "accession" ) );
}

TEST_F( URLTestFixture, AccessionFilename )
{
    const std::string res = try_to_parse_good( "SRR000123/filename" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, AccessionFilenameExtension )
{
    const std::string res = try_to_parse_good( "SRR000123/filename.ext" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, AccessionIsTheFilenameIfMissing )
{   // when filename+extension are missing, use the accession as filename
    const std::string res = try_to_parse_good( "SRR000123" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR000123", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, AccessionExtension )
{   // filename is missing but there is an extension, leave the filename empty
    const std::string res = try_to_parse_good( "SRR000123/.ext" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, AccessionAsFilename )
{
    const std::string res = try_to_parse_good( "SRR000123.ext1.ext2.ext3" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR000123", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext1.ext2.ext3", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, MultipleFilenames )
{   // capture the leaf filename/ext
    const std::string res = try_to_parse_good( "SRR000123/f1.ext1/f2.ext2" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "f2", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext2", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, TwoAccessions_Second_will_be_filename )
{
    const std::string res = try_to_parse_good( "SRR000123/SRR000456.ext2" );
    ASSERT_EQ( "SRR000456", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR000456", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext2", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, FilenameBeforeAccession )
{
    const std::string res = try_to_parse_good( "/storage/SRR000123/filename.ext" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, Accession_Something_Filename )
{
    const std::string res = try_to_parse_good( "/SRR000123/storage/filename.ext" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, NoAccession_But_Filename )
{
    const std::string res = try_to_parse_good( "/storage/filename.ext" );
    ASSERT_EQ( "", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, PathWithPct )
{
    const std::string res = try_to_parse_good( "ERR792423/5141526_%s1_p0.bas.h5.1%ab" );
    ASSERT_EQ( "ERR792423", extract_value( res, "accession" ) );
    ASSERT_EQ( "5141526_%s1_p0", extract_value( res, "filename" ) );
    ASSERT_EQ( ".bas.h5.1%ab", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, PathWithSlashURLEnc )
{
    const std::string res = try_to_parse_good( "ERR792423%2F5141526_%s1_p0.bas.h5.1%ab" );
    ASSERT_EQ( "", extract_value( res, "accession" ) );
    ASSERT_EQ( "ERR792423%2F5141526_%s1_p0", extract_value( res, "filename" ) );
    ASSERT_EQ( ".bas.h5.1%ab", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, KeepFilenameAndExtensionTogether1 )
{
    const std::string res = try_to_parse_good( "/somewhere.1/filename.2" );
    ASSERT_EQ( "", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( ".2", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, KeepFilenameAndExtensionTogether2 )
{
    const std::string res = try_to_parse_good( "/somewhere.1/filename" );
    ASSERT_EQ( "", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, TakeFilenameOnlyAfterAccession )
{
    const std::string res = try_to_parse_good( "/somewhere.1/SRR123456" );
    ASSERT_EQ( "SRR123456", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR123456", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, DoNotMixFilenamAndExtensionFormDifferentPathSegments1 )
{
    const std::string res = try_to_parse_good( "/somewhere.1/.2" );
    ASSERT_EQ( "", extract_value( res, "accession" ) );
    ASSERT_EQ( "", extract_value( res, "filename" ) );
    ASSERT_EQ( ".2", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, ThereIsAEscapedQuoteInThePath )
{
    const std::string res = try_to_parse_good( "a\\\"" );
    ASSERT_EQ( "", extract_value( res, "accession" ) );
    ASSERT_EQ( "", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

//---------------------------------------------------------------------

TEST_F( URLTestFixture, QMJustAccession )
{
    const std::string res = try_to_parse_good( "?SRR000123" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR000123", extract_value( res, "filename" ) ); // when filename+extension are missing, use the accession
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, QMAccessionFilenameExtension )
{
    const std::string res = try_to_parse_good( "?SRR000123%2Ffile.ext" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "file", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, QMAccessionExtension )
{
    const std::string res = try_to_parse_good( "?SRR000123.ext" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR000123", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext", extract_value( res, "extension" ) );
}

// Of multiple name=value pairs with reconized accessions, pick the one that has more
// elements (accession/filename/extension); if equal pick the first.
// Inside name=value strings, pick the leaf accession.
TEST_F( URLTestFixture, QMAccesionFilenameExtInParams_1 )
{   // the first pair (prefix=SRR11060177%2FSRR99999999/filename.1) has more elements
    std::string res = try_to_parse_good( "?list-type=2&delimiter=%2F&prefix=SRR11060177%2FSRR99999999/filename.1&morefilenames.moreextensions.1&name=SRR000123&encoding-type=url" );
    ASSERT_EQ( "SRR99999999", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( ".1", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, QMAccesionFilenameExtInParams_2 )
{   // the following pair (prefix=SRR11060177%2FSRR99999999/filename.1) has more elements
    std::string res = try_to_parse_good( "?list-type=2&name=SRR000123&delimiter=%2F&prefix=SRR11060177%2FSRR99999999/filename.1&morefilenames.moreextensions.1&encoding-type=url" );
    ASSERT_EQ( "SRR99999999", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( ".1", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, PathQMAccession )
{   // if an accession apears in the query and not in the path, ignore path for filename/ext
    const std::string res = try_to_parse_good( "/p1/p2.ext?SRR000123" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR000123", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, AccessionQMAccession )
{   // if naked accessions are on both sides, use the one from the path
    const std::string res = try_to_parse_good( "SRR000123?SRR000456" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR000123", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, AccessionQMFilename )
{   // if accession in the path, ignore filename from the query
    const std::string res = try_to_parse_good( "SRR000123?filename.ext" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR000123", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, QMAccesionFilenameExt )
{   // in the query, do not pick up filenames from different k-v pairs than
    // the one the acession is in
    const std::string res = try_to_parse_good( "/?k1=SRR000123&k2=filename.ext" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR000123", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, QMNoAccession_But_Filename )
{
    const std::string res = try_to_parse_good( "/?key=storage/filename.ext" );
    ASSERT_EQ( "", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, QMNoAccession_prefer_Filename_from_path )
{
    const std::string res = try_to_parse_good( "/filename1.ext1?key=storage/filename2.ext2" );
    ASSERT_EQ( "", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename1", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext1", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, UseExtensionFromQueryIfNotInPath )
{
    const std::string res = try_to_parse_good( "/?key=SRR000123/.ext2" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext2", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, QMKeepFilenameAndExtensionTogether1 )
{
    const std::string res = try_to_parse_good( "/?p=somewhere.1/filename.2" );
    ASSERT_EQ( "", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( ".2", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, QMKeepFilenameAndExtensionTogether2 )
{
    const std::string res = try_to_parse_good( "/?p=somewhere.1/filename" );
    ASSERT_EQ( "", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, QMTakeFilenameOnlyAfterAccession )
{
    const std::string res = try_to_parse_good( "/?ex=somewhere.1/SRR123456" );
    ASSERT_EQ( "SRR123456", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR123456", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, QMDoNotMixFilenamAndExtensionFormDifferentPathSegments1 )
{
    const std::string res = try_to_parse_good( "/?p=somewhere.1/.2" );
    ASSERT_EQ( "", extract_value( res, "accession" ) );
    ASSERT_EQ( "", extract_value( res, "filename" ) );
    ASSERT_EQ( ".2", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, QMDoNotMixFilenamAndExtensionFormDifferentPathSegments2 )
{
    const std::string res = try_to_parse_good( "/?p=somewhere.1%2Ffilename.2" );
    ASSERT_EQ( "", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( ".2", extract_value( res, "extension" ) );
}

