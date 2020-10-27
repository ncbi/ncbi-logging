#include <gtest/gtest.h>
#include "CL_Interface.cpp"

using namespace std;
using namespace NCBI::Logging;

TEST( CLReverseBlockTest, Create )
{
    CLReverseBlockFactory factory;
    auto pb = factory . MakeParseBlock();
    ASSERT_TRUE( pb );
}

TEST( CLReverseBlockTest, ReverseEmptyLine )
{
    // if an empty lines is given ( is in the json-source-file ) we want it to be in the unrecog-file
    CLReverseBlockFactory factory;
    auto pb = factory . MakeParseBlock();
    string line( "" );
    bool res = pb -> format_specific_parse( line.c_str(), line . size() );
    ASSERT_FALSE( res );
    ASSERT_EQ( ReceiverInterface::cat_ugly, pb->GetReceiver().GetCategory() );
}

TEST( CLReverseBlockTest, ReverseEmptyJson )
{
    // if an empty json is given ( is in the json-source-file ) we want it to be in the unrecog-file
    CLReverseBlockFactory factory;
    auto pb = factory . MakeParseBlock();
    string line( "{}" );
    bool res = pb -> format_specific_parse( line.c_str(), line . size() );
    ASSERT_FALSE( res );
}

TEST( CLReverseBlockTest, ReverseBadJson )
{
    // if an empty json is given ( is in the json-source-file ) we want it to be in the unrecog-file
    CLReverseBlockFactory factory;
    auto pb = factory . MakeParseBlock();
    string line( "{" );
    bool res = pb -> format_specific_parse( line.c_str(), line . size() );
    ASSERT_FALSE( res );
}

class CLReverseTestFixture : public ::testing::Test
{
public:
    string Parse( const string & line )
    {
        CLReverseBlockFactory factory;
        auto pb = factory . MakeParseBlock();
        if ( ! pb -> format_specific_parse( line.c_str(), line . size() ) )
        {
            return "CLReverseTestFixture: format_specific_parse() failed";
        }
        return pb -> GetReceiver() . GetFormatter() . format();
    }
};

TEST_F( CLReverseTestFixture, ReverseGoodJson )
{
    string line( "{\"accession\":\"ERR527068\",\"bucket\":\"sra-pub-run-5\",\"contentAccessorUserID\":\"\",\"copySource\":\"\",\"durationMsec\":\"593542\",\"eTag\":\"c89056a1918b4ea6534c912deee13f11-154\",\"errorCode\":\"0\",\"extension\":\".1\",\"filename\":\"ERR527068\",\"httpStatus\":\"206\",\"ip\":\"10.10.26.17\",\"method\":\"getObject\",\"owner\":\"trace\",\"path\":\"ERR527068%2FERR527068.1\",\"reqId\":\"45200f16-ad2e-1945-87cb-d8c49756ebf4\",\"requestBodySize\":\"0\",\"requestHdrSize\":\"252\",\"responseBodySize\":\"20971520\",\"responseHdrSize\":\"256\",\"syslog_prefix\":\"Oct 20 15:27:38 cloudian-node-120.be-md.ncbi.nlm.nih.gov 1 2020-10-20T15:27:37.592-04:00 cloudian-node-120.be-md.ncbi.nlm.nih.gov S3REQ 4738 - [mdc@18060 REQID=\\\"45200f16-ad2e-1945-87cb-d8c49756ebf4\\\"]\",\"timestamp\":\"2020-10-20 15:27:36,999\",\"totalSize\":\"20972028\"}" );

    ASSERT_EQ( "Oct 20 15:27:38 cloudian-node-120.be-md.ncbi.nlm.nih.gov 1 2020-10-20T15:27:37.592-04:00 cloudian-node-120.be-md.ncbi.nlm.nih.gov S3REQ 4738 - [mdc@18060 REQID=\"45200f16-ad2e-1945-87cb-d8c49756ebf4\"] 2020-10-20 15:27:36,999|10.10.26.17|trace|getObject|sra-pub-run-5||252|0|256|20971520|20972028|593542|ERR527068%2FERR527068.1|206|45200f16-ad2e-1945-87cb-d8c49756ebf4|c89056a1918b4ea6534c912deee13f11-154|0|", Parse(line) );
}

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}