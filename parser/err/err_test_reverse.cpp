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

#if 0
TEST_F( ERRReverseTestFixture, ReverseGoodJson )
{
    string line( "{\"accepted\":\"true\",\"event\":\"extra\",\"id1\":\"77619/000/0000/R\",\"id2\":\"CC952F33EE2FDBD1\",\"id3\":\"0008/0008\",\"ip\":\"185.151.196.174\",\"msg\":\"issued_suhit=m_1\",\"server\":\"traceweb22\",\"service\":\"sra\",\"sid\":\"CC952F33EE2FDBD1_0000SID\",\"time\":\"2020-06-11T23:59:58.079949\"}" );

    ASSERT_EQ( "77619/000/0000/R CC952F33EE2FDBD1 0008/0008 2020-06-11T23:59:58.079949 traceweb22 185.151.196.174 CC952F33EE2FDBD1_0000SID sra extra issued_suhit=m_1", Parse(line) );
}
#endif

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}
