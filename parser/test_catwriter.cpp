#include <gtest/gtest.h>

#include "CatWriters.hpp"

using namespace std;
using namespace NCBI::Logging;

// CatCounter

TEST(CatCounter, Create)
{
    CatCounter cc;
    ASSERT_EQ( 0, cc.get_total() );
    ASSERT_EQ( 0, cc.get_cat_count(LogLinesInterface::cat_review) );
    ASSERT_EQ( 0, cc.get_cat_count(LogLinesInterface::cat_good) );
    ASSERT_EQ( 0, cc.get_cat_count(LogLinesInterface::cat_bad) );
    ASSERT_EQ( 0, cc.get_cat_count(LogLinesInterface::cat_ugly) );
    ASSERT_EQ( 0, cc.get_cat_count(LogLinesInterface::cat_ignored) );
}

TEST(CatCounter, Count)
{
    CatCounter cc;
    cc.count(LogLinesInterface::cat_review);
    cc.count(LogLinesInterface::cat_good);
    cc.count(LogLinesInterface::cat_good);
    cc.count(LogLinesInterface::cat_bad);
    cc.count(LogLinesInterface::cat_bad);
    cc.count(LogLinesInterface::cat_bad);
    cc.count(LogLinesInterface::cat_ugly);
    cc.count(LogLinesInterface::cat_ugly);
    cc.count(LogLinesInterface::cat_ugly);
    cc.count(LogLinesInterface::cat_ugly);
    cc.count(LogLinesInterface::cat_ignored);

    ASSERT_EQ( 11, cc.get_total() );
    ASSERT_EQ( 1, cc.get_cat_count(LogLinesInterface::cat_review) );
    ASSERT_EQ( 2, cc.get_cat_count(LogLinesInterface::cat_good) );
    ASSERT_EQ( 3, cc.get_cat_count(LogLinesInterface::cat_bad) );
    ASSERT_EQ( 4, cc.get_cat_count(LogLinesInterface::cat_ugly) );
    ASSERT_EQ( 1, cc.get_cat_count(LogLinesInterface::cat_ignored) );
}

TEST(CatCounter, Report)
{
    CatCounter cc;
    cc.count(LogLinesInterface::cat_review);
    cc.count(LogLinesInterface::cat_good);
    cc.count(LogLinesInterface::cat_good);
    cc.count(LogLinesInterface::cat_bad);
    cc.count(LogLinesInterface::cat_bad);
    cc.count(LogLinesInterface::cat_bad);
    cc.count(LogLinesInterface::cat_ugly);
    cc.count(LogLinesInterface::cat_ugly);
    cc.count(LogLinesInterface::cat_ugly);
    cc.count(LogLinesInterface::cat_ugly);
    cc.count(LogLinesInterface::cat_ignored);
    cc.count(LogLinesInterface::cat_ignored);
    cc.count(LogLinesInterface::cat_ignored);
    cc.count(LogLinesInterface::cat_ignored);
    cc.count(LogLinesInterface::cat_ignored);

    JsonLibFormatter fmt; 
    ASSERT_EQ( 
        "{\"_total\":15,\"bad\":3,\"good\":2,\"ignored\":5,\"review\":1,\"ugly\":4}", 
        cc.report( fmt ) 
    );
}

TEST(CatCounter, Count_BadCategory)
{
    CatCounter cc;
    ASSERT_THROW( cc.count(LogLinesInterface::cat_max), std::exception );
    ASSERT_EQ( 0, cc.get_total() );
}
// CatWriters

TEST(FileCatWriter, Write)
{
    {
        FileCatWriter f( "test" );
        f.write( LogLinesInterface::cat_review, "review");
        f.write( LogLinesInterface::cat_good, "good");
        f.write( LogLinesInterface::cat_bad, "bad");
        f.write( LogLinesInterface::cat_ugly, "ugly");
        f.write( LogLinesInterface::cat_ignored, "ignored");
    }

    {
        ifstream in("test.review"); ASSERT_TRUE( in.good() );
        string s; in >> s;
        ASSERT_EQ( string("review"), s);
    }
    remove("test.review");

    {
        ifstream in("test.good"); ASSERT_TRUE( in.good() );
        string s; in >> s;
        ASSERT_EQ( string("good"), s);
    }
    remove("test.good");

    {
        ifstream in("test.bad"); ASSERT_TRUE( in.good() );
        string s; in >> s;
        ASSERT_EQ( string("bad"), s);
    }
    remove("test.bad");

    {
        ifstream in("test.unrecog"); ASSERT_TRUE( in.good() );
        string s; in >> s;
        ASSERT_EQ( string("ugly"), s);
    }

    remove("test.unrecog");
}

TEST(FileCatWriter, Count)
{
    FileCatWriter f( "actual/test" );
    f.write( LogLinesInterface::cat_review, "review");
    ASSERT_EQ( 1, f.getCounter().get_cat_count( LogLinesInterface::cat_review ) );
}



