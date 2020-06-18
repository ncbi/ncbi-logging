#include <gtest/gtest.h>

#include "log_lines.hpp"
#include "helper.hpp"

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
    ASSERT_EQ( "[01/Feb/0003:04:05:06 +0007]", ToString(t) );
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
    ASSERT_EQ( "[01/Feb/0003:04:05:06 -0500]", ToString(t) );
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
    ASSERT_EQ( "[01/\?\?\?/0003:04:05:06 -0500]", ToString(t) );
}

class ACC_EXT_TestFixture : public ::testing::Test
{
public:
    virtual void SetUp()
    {
        InitRequest( r );
    }

    virtual void TearDown()
    {
    }

    void init_path( const char * path )
    {
        r . path . p = path;
        r . path . n = strlen( path );
    }

    void check_results( const char * acc, const char * ext )
    {
        ASSERT_EQ( acc, ToString( r . accession ) );
        ASSERT_EQ( ext, ToString( r . extension ) );    
    }

    void check( const char * path, const char * acc, const char * ext )
    {
        init_path( path );
        extract_acc_ext( r );
        check_results( acc, ext );
    }

    t_request r;
};

TEST_F ( ACC_EXT_TestFixture, extract_acc_ext_empty )
{
    ASSERT_EQ( 0, r . path . n );
    extract_acc_ext( r );
    ASSERT_EQ( 0, r . accession . n );
    ASSERT_EQ( 0, r . extension . n );    
}

TEST_F ( ACC_EXT_TestFixture, extract_acc_ext_pure_path )               { check( "/some/path",      "path", "" ); }
TEST_F ( ACC_EXT_TestFixture, extract_acc_ext_path_and_ext )            { check( "/some/path.txt",  "path", "txt" ); }
TEST_F ( ACC_EXT_TestFixture, extract_acc_ext_path_and_ext_empty )      { check( "/some/path.",     "path", "" ); }
TEST_F ( ACC_EXT_TestFixture, extract_acc_ext_path_and_multi_dot )      { check( "/some/path.t.t",  "path", "t.t" ); }
TEST_F ( ACC_EXT_TestFixture, extract_acc_ext_one_slash )               { check( "/path",           "path", "" ); }
TEST_F ( ACC_EXT_TestFixture, extract_acc_ext_no_slash )                { check( "path",            "path", "" ); }
TEST_F ( ACC_EXT_TestFixture, extract_acc_ext_no_slash_but_dot )        { check( "path.txt",        "path", "txt" ); }
TEST_F ( ACC_EXT_TestFixture, extract_acc_ext_no_slash_but_only_dot )   { check( "path.",           "path", "" ); }
TEST_F ( ACC_EXT_TestFixture, extract_acc_ext_no_slash_but_multi_dot )  { check( "path.t.t",        "path", "t.t" ); }

TEST_F ( ACC_EXT_TestFixture, extract_acc_ext_pure_path_qm )            { check( "/some/path?",         "path", "" ); }
TEST_F ( ACC_EXT_TestFixture, extract_acc_ext_pure_path_multi_qm )      { check( "/some/path?xx?x?",    "path", "" ); }
TEST_F ( ACC_EXT_TestFixture, extract_acc_ext_qm_path_and_ext )         { check( "/some/path.txt?a",    "path", "txt" ); }
TEST_F ( ACC_EXT_TestFixture, extract_acc_ext_qm_path_and_dot )         { check( "/some/path.?b",       "path", "" ); }
TEST_F ( ACC_EXT_TestFixture, extract_acc_ext_qm_path_and_multi_dot )   { check( "/some/path.t.t?b",    "path", "t.t" ); }
TEST_F ( ACC_EXT_TestFixture, extract_acc_ext_qm_one_slash )            { check( "/path?b",             "path", "" ); }
TEST_F ( ACC_EXT_TestFixture, extract_acc_ext_qm_no_slash )             { check( "path?b",              "path", "" ); }
TEST_F ( ACC_EXT_TestFixture, extract_acc_ext_qm_no_slash_but_dot )     { check( "path.txt?b",          "path", "txt" ); }
TEST_F ( ACC_EXT_TestFixture, extract_acc_ext_qm_no_slash_only_dot )    { check( "path.?b",             "path", "" ); }
TEST_F ( ACC_EXT_TestFixture, extract_acc_ext_qm_no_slash_multi_dot )   { check( "path.t.t?b",          "path", "t.t" ); }

extern "C"
{
    int main ( int argc, const char * argv [], const char * envp []  )
    {
        testing :: InitGoogleTest ( & argc, ( char ** ) argv );
        return RUN_ALL_TESTS ();
    }
}
