#include <gtest/gtest.h>

#include "Classifiers.hpp"

using namespace std;
using namespace NCBI::Logging;

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

