#include <gtest/gtest.h>

#include "CL_Interface.hpp"
#include "Formatters.hpp"
#include "parse_test_fixture.hpp"

using namespace std;
using namespace NCBI::Logging;

#define INIT_TSTR(t, s)     do { (t).p = s; (t).n = strlen(s); } while (false)

TEST(CLReceiverTest, Create)
{
    CLReceiver e ( make_shared<JsonLibFormatter>() );
}

//TODO: setters

class CLTestFixture : public ParseTestFixture< CLParseBlockFactory >
{
};

TEST_F( CLTestFixture, lonely_eol )
{
    std::string res = try_to_parse_ugly( "\n" );
    ASSERT_EQ( "{\"_line_nr\":1,\"_unparsed\":\"\"}\n", res );
}


#if 0
TEST_F( CLTestFixture, line )
{
    std::string res = try_to_parse_good(
"Oct 20 15:27:38 cloudian-node-120.be-md.ncbi.nlm.nih.gov 1 2020-10-20T15:27:37.592-04:00 cloudian-node-120.be-md.ncbi.nlm.nih.gov S3REQ 4738 - [mdc@18060 REQID=\"45200f16-ad2e-1945-87cb-d8c49756ebf4\"] 2020-10-20 15:27:36,999|10.10.26.17|trace|getObject|sra-pub-run-5||252|0|256|20971520|20972028|593542|ERR527068%2FERR527068.1|206|45200f16-ad2e-1945-87cb-d8c49756ebf4|c89056a1918b4ea6534c912deee13f11-154|0|"
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