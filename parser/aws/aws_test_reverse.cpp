#include <gtest/gtest.h>

#include "AWS_Interface.cpp"

using namespace std;
using namespace NCBI::Logging;

TEST( AWSFormatterTest, Create )
{
    AWSFormatter fmt;
}

TEST( AWSFormatterTest, FormatEmptyObj )
{
    AWSFormatter fmt;
    ASSERT_EQ( "", fmt.format() );
}

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}
