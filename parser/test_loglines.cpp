#include <gtest/gtest.h>

#include "log_lines.hpp"

using namespace std;
using namespace NCBI::Logging;

TEST ( TestLogLines, t_timepoint_ToString )
{
    t_timepoint t;
    t . day = 1;
    t . month = 2;
    t . year = 3;
    t . hour = 4;
    t . minute = 5;
    t . second = 6;
    t . offset = 7;    
    ASSERT_EQ( "[01/Feb/0003:04:05:06 +0007]", t.ToString() );
}

TEST ( TestLogLines, t_timepoint_ToString_NegOffset )
{
    t_timepoint t;
    t . day = 1;
    t . month = 2;
    t . year = 3;
    t . hour = 4;
    t . minute = 5;
    t . second = 6;
    t . offset = -500;    
    ASSERT_EQ( "[01/Feb/0003:04:05:06 -0500]", t.ToString() );
}

TEST ( TestLogLines, t_timepoint_ToString_BadMonth )
{
    t_timepoint t;
    t . day = 1;
    t . month = 22;
    t . year = 3;
    t . hour = 4;
    t . minute = 5;
    t . second = 6;
    t . offset = -500;    
    ASSERT_EQ( "[01/\?\?\?/0003:04:05:06 -0500]", t.ToString() );
}

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}
