#include <gtest/gtest.h>

#include "AWSToJsonImpl.hpp"

using namespace std;
using namespace NCBI::Logging;

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}

