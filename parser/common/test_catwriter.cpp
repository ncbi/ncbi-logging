#include <gtest/gtest.h>

#include "CatWriters.hpp"

using namespace std;
using namespace NCBI::Logging;

// CatCounter

TEST(CatCounter, Create)
{
    CatCounter cc;
    ASSERT_EQ( 0, cc.get_total() );
    ASSERT_EQ( 0, cc.get_cat_count(ReceiverInterface::cat_review) );
    ASSERT_EQ( 0, cc.get_cat_count(ReceiverInterface::cat_good) );
    ASSERT_EQ( 0, cc.get_cat_count(ReceiverInterface::cat_bad) );
    ASSERT_EQ( 0, cc.get_cat_count(ReceiverInterface::cat_ugly) );
    ASSERT_EQ( 0, cc.get_cat_count(ReceiverInterface::cat_ignored) );
}

TEST(CatCounter, Count)
{
    CatCounter cc;
    cc.count(ReceiverInterface::cat_review);
    cc.count(ReceiverInterface::cat_good);
    cc.count(ReceiverInterface::cat_good);
    cc.count(ReceiverInterface::cat_bad);
    cc.count(ReceiverInterface::cat_bad);
    cc.count(ReceiverInterface::cat_bad);
    cc.count(ReceiverInterface::cat_ugly);
    cc.count(ReceiverInterface::cat_ugly);
    cc.count(ReceiverInterface::cat_ugly);
    cc.count(ReceiverInterface::cat_ugly);
    cc.count(ReceiverInterface::cat_ignored);

    ASSERT_EQ( 11, cc.get_total() );
    ASSERT_EQ( 1, cc.get_cat_count(ReceiverInterface::cat_review) );
    ASSERT_EQ( 2, cc.get_cat_count(ReceiverInterface::cat_good) );
    ASSERT_EQ( 3, cc.get_cat_count(ReceiverInterface::cat_bad) );
    ASSERT_EQ( 4, cc.get_cat_count(ReceiverInterface::cat_ugly) );
    ASSERT_EQ( 1, cc.get_cat_count(ReceiverInterface::cat_ignored) );
}

TEST(CatCounter, Report)
{
    CatCounter cc;
    cc.count(ReceiverInterface::cat_review);
    cc.count(ReceiverInterface::cat_good);
    cc.count(ReceiverInterface::cat_good);
    cc.count(ReceiverInterface::cat_bad);
    cc.count(ReceiverInterface::cat_bad);
    cc.count(ReceiverInterface::cat_bad);
    cc.count(ReceiverInterface::cat_ugly);
    cc.count(ReceiverInterface::cat_ugly);
    cc.count(ReceiverInterface::cat_ugly);
    cc.count(ReceiverInterface::cat_ugly);
    cc.count(ReceiverInterface::cat_ignored);
    cc.count(ReceiverInterface::cat_ignored);
    cc.count(ReceiverInterface::cat_ignored);
    cc.count(ReceiverInterface::cat_ignored);
    cc.count(ReceiverInterface::cat_ignored);

    JsonLibFormatter fmt; 
    ASSERT_EQ( 
        "{\"_total\":15,\"bad\":3,\"good\":2,\"ignored\":5,\"review\":1,\"unrecog\":4}", 
        cc.report( fmt ) 
    );
}

TEST(CatCounter, Count_BadCategory)
{
    CatCounter cc;
    ASSERT_THROW( cc.count(ReceiverInterface::cat_max), std::exception );
    ASSERT_EQ( 0, cc.get_total() );
}

// CatWriters

void verify_file( const std::string & filename, const std::string & content )
{
    ifstream in( filename );
    ASSERT_TRUE( in.good() );
    string s; in >> s;
    ASSERT_EQ( content, s );
    remove( filename.c_str() );
}

TEST(FileCatWriter, Write)
{
    {
        FileCatWriter f( "test", ".jsonl" );
        f.write( ReceiverInterface::cat_review, "review" );
        f.write( ReceiverInterface::cat_good, "good" );
        f.write( ReceiverInterface::cat_bad, "bad" );
        f.write( ReceiverInterface::cat_ugly, "unrecog" );
        f.write( ReceiverInterface::cat_ignored, "ignored" );
    }

    verify_file( "test.review.jsonl", "review" );
    verify_file( "test.good.jsonl", "good" );
    verify_file( "test.bad.jsonl", "bad" );
    verify_file( "test.unrecog.jsonl", "unrecog" );

    /* verify that the ignored-file does not exist */
    ASSERT_FALSE( ifstream( "test.ignored.jsonl" ).good() );
}

TEST(FileCatWriter, Count)
{
    FileCatWriter f( "actual/test", ".jsonl" );
    f.write( ReceiverInterface::cat_review, "review");
    ASSERT_EQ( 1, f.getCounter().get_cat_count( ReceiverInterface::cat_review ) );
}



