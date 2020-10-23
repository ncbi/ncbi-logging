#include <gtest/gtest.h>

#include "ERR_Interface.cpp"

using namespace std;
using namespace NCBI::Logging;

TEST( ERRReverseBlockTest, Create )
{
    ERRReverseBlockFactory factory;
    auto pb = factory . MakeParseBlock();
    ASSERT_TRUE( pb );
}

TEST( ERRReverseBlockTest, ReverseEmptyLine )
{
    // if an empty lines is given ( is in the json-source-file ) we want it to be in the unrecog-file
    ERRReverseBlockFactory factory;
    auto pb = factory . MakeParseBlock();
    string line( "" );
    bool res = pb -> format_specific_parse( line.c_str(), line . size() );
    ASSERT_FALSE( res );
    ASSERT_EQ( ReceiverInterface::cat_ugly, pb->GetReceiver().GetCategory() );
}

TEST( ERRReverseBlockTest, ReverseEmptyJson )
{
    // if an empty json is given ( is in the json-source-file ) we want it to be in the unrecog-file
    ERRReverseBlockFactory factory;
    auto pb = factory . MakeParseBlock();
    string line( "{}" );
    bool res = pb -> format_specific_parse( line.c_str(), line . size() );
    ASSERT_FALSE( res );
}

TEST( ERRReverseBlockTest, ReverseBadJson )
{
    // if an empty json is given ( is in the json-source-file ) we want it to be in the unrecog-file
    ERRReverseBlockFactory factory;
    auto pb = factory . MakeParseBlock();
    string line( "{" );
    bool res = pb -> format_specific_parse( line.c_str(), line . size() );
    ASSERT_FALSE( res );
}

class ERRReverseTestFixture : public ::testing::Test
{
public:
    string Parse( const string & line )
    {
        ERRReverseBlockFactory factory;
        auto pb = factory . MakeParseBlock();
        if ( ! pb -> format_specific_parse( line.c_str(), line . size() ) )
        {
            return "ERRReverseTestFixture: format_specific_parse() failed";
        }
        return pb -> GetReceiver() . GetFormatter() . format();
    }
};

TEST_F( ERRReverseTestFixture, ReverseGoodJson )
{
    string line( "{\"accession\":\"SRR8453128\",\"client\":\"218.219.98.126\",\"datetime\":\"2020/10/15 00:12:17\",\"extension\":\"\",\"filename\":\"SRR8453128\",\"host\":\"sra-downloadb.be-md.ncbi.nlm.nih.gov\",\"method\":\"GET\",\"msg\":\"*650699 open() \\\"/netmnt/traces04//sra75/SRR/008255/SRR8453128\\\" failed (2: No such file or directory), client: 218.219.98.126, server: srafiles21.be-md.ncbi.nlm.nih.gov, request: \\\"GET /traces/sra75/SRR/008255/SRR8453128 HTTP/1.1\\\", host: \\\"sra-downloadb.be-md.ncbi.nlm.nih.gov\\\"\",\"path\":\"/traces/sra75/SRR/008255/SRR8453128\",\"pid\":\"60987#0:\",\"server\":\"srafiles21.be-md.ncbi.nlm.nih.gov\",\"severity\":\"[error]\",\"vers\":\"HTTP/1.1\"}" );

    ASSERT_EQ( "2020/10/15 00:12:17 [error] 60987#0: *650699 open() \"/netmnt/traces04//sra75/SRR/008255/SRR8453128\" failed (2: No such file or directory), client: 218.219.98.126, server: srafiles21.be-md.ncbi.nlm.nih.gov, request: \"GET /traces/sra75/SRR/008255/SRR8453128 HTTP/1.1\", host: \"sra-downloadb.be-md.ncbi.nlm.nih.gov\"",
        Parse(line) );
}

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}
