#include <gtest/gtest.h>

#include "Classifiers.hpp"

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

    ASSERT_EQ( 10, cc.get_total() );
    ASSERT_EQ( 1, cc.get_cat_count(LogLinesInterface::cat_review) );
    ASSERT_EQ( 2, cc.get_cat_count(LogLinesInterface::cat_good) );
    ASSERT_EQ( 3, cc.get_cat_count(LogLinesInterface::cat_bad) );
    ASSERT_EQ( 4, cc.get_cat_count(LogLinesInterface::cat_ugly) );
}

TEST(CatCounter, Count_BadCategory)
{
    CatCounter cc;
    ASSERT_THROW( cc.count(LogLinesInterface::cat_unknown), std::exception );
    ASSERT_EQ( 0, cc.get_total() );
}
// Classifiers

TEST(FileClassifier, Write)
{
    {
        FileClassifier f( "test" );
        f.write( LogLinesInterface::cat_review, "review");
        f.write( LogLinesInterface::cat_good, "good");
        f.write( LogLinesInterface::cat_bad, "bad");
        f.write( LogLinesInterface::cat_ugly, "ugly");
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

TEST(FileClassifier, Count)
{
    FileClassifier f( "actual/test" );
    f.write( LogLinesInterface::cat_review, "review");
    f.write( LogLinesInterface::cat_good, "good");
    f.write( LogLinesInterface::cat_bad, "bad");
    f.write( LogLinesInterface::cat_ugly, "ugly");

    ASSERT_EQ( 1, f.getCounter().get_cat_count( LogLinesInterface::cat_review ) );
    ASSERT_EQ( 1, f.getCounter().get_cat_count( LogLinesInterface::cat_good ) );
    ASSERT_EQ( 1, f.getCounter().get_cat_count( LogLinesInterface::cat_bad ) );
    ASSERT_EQ( 1, f.getCounter().get_cat_count( LogLinesInterface::cat_ugly ) );
}



