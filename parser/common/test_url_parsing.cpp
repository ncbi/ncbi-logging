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
    ASSERT_EQ( "{\"accession\":\"\",\"extension\":\"\",\"filename\":\"\"}", receiver.GetFormatter().format() );
}

class URLParseBlockFactory : public ParseBlockFactoryInterface
{
public:
    URLParseBlockFactory() : m_receiver( std::make_shared<JsonLibFormatter>() ) {}
    virtual ~URLParseBlockFactory() {}
    virtual std::unique_ptr<ParseBlockInterface> MakeParseBlock() const
    {
        return std::make_unique<URLParseBlock>( m_receiver );
    }
    URLReceiver mutable m_receiver;
};

class URLTestFixture : public ParseTestFixture< URLParseBlockFactory >
{
};

//
// If there is no '?':
//  input               accession          filename        extension
//  A                   A                   A                           OK
//  A1/A2/A3            A3                  A3                          OK
//  A/f.ext             A                   f               .ext        OK
//  A.ext               A                   A               .ext        OK
//  A/.ext              A                                   .ext        OK
//  A/f1.ext1/f2.ext2   A                   f2              .ext        OK
//  A1/A2.ext           A2                  A2              .ext        OK
//  A/f                 A                   f                           OK
//  f1/A/f2.ext         A                   f2              .ext        OK
//  A/f1/f2.ext         A                   f2              .ext        OK
//  A1/A2/A3/f.ext      A3                  f               .ext        OK

// If there is a '?':
// ?A                   A                   A                           OK
// f1/f2?A              A                   f2                          OK
// f1/f2.ext?A          A                   f2              .ext        OK
// f1.ext/f2?A          A                   f2              .ext        OK  !!! weird !!!
// A1?A2                A1                  A1                          OK
// A1?f2                A1                  f2                          OK
// A1?f2.ext            A1                  f2              .ext        OK
// /?k1=A1&k2=f2.ext    A1                  f2              .ext

//  If thereis a '?' and no accession to the left of it,
//  same rules apply to the substring following the '?'
//  ?a=A1&b=A2          A2                                  .ext

#if 0
TEST_F( URLTestFixture, JustAccession )
{
    const std::string res = try_to_parse_good( "SRR000123" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR000123", extract_value( res, "filename" ) ); // when filename+extension are missing, use the accession
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, MultipleAccessionsNoFile )
{
    const std::string res = try_to_parse_good( "SRR000123/SRR000456/SRR000789" );
    ASSERT_EQ( "SRR000789", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR000789", extract_value( res, "filename" ) ); // when filename+extension are missing, use the last accession
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, AccessionFilenameExtension )
{
    const std::string res = try_to_parse_good( "SRR000123/filename.ext" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, AccessionAsFilename )
{
    const std::string res = try_to_parse_good( "SRR000123.ext1.ext2.ext3" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR000123", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext1.ext2.ext3", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, AccessionExtension )
{
    const std::string res = try_to_parse_good( "SRR000123/.ext" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, MultipleFilenames )
{
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

TEST_F( URLTestFixture, AccessionFilename )
{
    const std::string res = try_to_parse_good( "SRR000123/filename" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
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

TEST_F( URLTestFixture, MultilpeAccessions )
{
    const std::string res = try_to_parse_good( "/SRR000123/SRR1755353/filename.fastq.gz" );
    ASSERT_EQ( "SRR1755353", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( ".fastq.gz", extract_value( res, "extension" ) );
}

//---------------------------------------------------------------------

TEST_F( URLTestFixture, QMJustAccession )
{
    const std::string res = try_to_parse_good( "?SRR000123" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR000123", extract_value( res, "filename" ) ); // when filename+extension are missing, use the accession
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, PathQMAccession )
{
    const std::string res = try_to_parse_good( "/p1/p2?SRR000123" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "p2", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, PathExtQMAccession )
{
    const std::string res = try_to_parse_good( "/p1/p2.ext?SRR000123" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "p2", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, PathExtPathQMAccession )
{
    const std::string res = try_to_parse_good( "/p1.ext/p2?SRR000123" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "p2", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, AccessionQMAccession )
{
    const std::string res = try_to_parse_good( "SRR000123?SRR000456" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR000123", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, AccessionQMFilename )
{
    const std::string res = try_to_parse_good( "SRR000123?filename" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( "", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, AccessionQMFilenameExt )
{
    const std::string res = try_to_parse_good( "SRR000123?filename.ext" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, QMAccesionFilenameExt )
{
    const std::string res = try_to_parse_good( "/?k1=SRR000123&k2=filename.ext" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, QMAccesionFilenameExtInParams )
{
    std::string res = try_to_parse_good( "?list-type=2&delimiter=%2F&prefix=SRR11060177%2FSRR99999999/filename.1&morefilenames.moreextensions.1&name=SRR000123&encoding-type=url" );
    ASSERT_EQ( "SRR11060177", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( ".1", extract_value( res, "extension" ) );
}

// #if 0
TEST_F( URLTestFixture, QMAccesion1FilenameExtAccession2 )
{
    const std::string res = try_to_parse_good( "/?k1=SRR000123&k2=filename.ext&k3=SRR000456" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "filename", extract_value( res, "filename" ) );
    ASSERT_EQ( ".ext", extract_value( res, "extension" ) );
}

TEST_F( URLTestFixture, SecondAccessionAsFilename )
{
    const std::string res = try_to_parse_good( "/SRR000123/o?prefix=SRR1755353.fastq.gz" );
    ASSERT_EQ( "SRR000123", extract_value( res, "accession" ) );
    ASSERT_EQ( "SRR1755353", extract_value( res, "filename" ) );
    ASSERT_EQ( ".fastq.gz", extract_value( res, "extension" ) );
}
#endif

