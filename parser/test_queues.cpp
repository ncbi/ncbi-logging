#include <gtest/gtest.h>
#include <thread>

#include "Queues.hpp"

using namespace std;
using namespace NCBI::Logging;

TEST ( TestOneWriterManyReadersQueue, Ctr )
{
    OneWriterManyReadersQueue Q( 10 );
}

class OneWriterManyReadersQueue_Fixture : public ::testing::Test
{
    public:
        OneWriterManyReadersQueue::value_type make_shared_str( const char * s )
        {
            return make_shared< string >( s );
        }

        void check_value_and_line_nr( OneWriterManyReadersQueue &Q, const char * value, size_t line_nr )
        {
            OneWriterManyReadersQueue::value_type s;
            size_t line_nr_q;
            s = Q.dequeue( line_nr_q );
            ASSERT_TRUE( s );
            ASSERT_EQ( value, *s );
            ASSERT_EQ( line_nr, line_nr_q );
        }

        void check_empty( OneWriterManyReadersQueue &Q )
        {
            OneWriterManyReadersQueue::value_type s;
            size_t line_nr_q;
            s = Q.dequeue( line_nr_q );
            ASSERT_FALSE( s );
        }
};

TEST_F ( OneWriterManyReadersQueue_Fixture, put_and_get )
{
    OneWriterManyReadersQueue Q( 10 );
    ASSERT_TRUE( Q.enqueue( make_shared_str( "1" ) ) );
    ASSERT_TRUE( Q.enqueue( make_shared_str( "2" ) ) );
    ASSERT_TRUE( Q.enqueue( make_shared_str( "3" ) ) );

    check_value_and_line_nr( Q, "1", 1 );
    check_value_and_line_nr( Q, "2", 2 );
    check_value_and_line_nr( Q, "3", 3 );
    check_empty( Q );
}

TEST ( TestOneWriterManyReadersQueue, close_the_q )
{
    OneWriterManyReadersQueue Q( 10 );
    ASSERT_TRUE( Q.is_open() );
    Q.close();
    ASSERT_FALSE( Q.is_open() );    
}

TEST_F ( OneWriterManyReadersQueue_Fixture, limit )
{
    OneWriterManyReadersQueue Q( 2 );
    ASSERT_TRUE( Q.enqueue( make_shared_str( "1" ) ) );
    ASSERT_TRUE( Q.enqueue( make_shared_str( "2" ) ) );
    ASSERT_FALSE( Q.enqueue( make_shared_str( "3" ) ) );

    check_value_and_line_nr( Q, "1", 1 );   // this dequeues, now we can enqueue!
    ASSERT_TRUE( Q.enqueue( make_shared_str( "3" ) ) );
}

void consumer( OneWriterManyReadersQueue * q, std::atomic< int > * cnt )
{
    while ( true )
    {
        size_t line_nr;
        OneWriterManyReadersQueue::value_type s = q -> dequeue( line_nr );

        if ( !s )
        {
            if ( !q -> is_open() )
                return;
            else
                std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
        }
        else
            ( *cnt )++;
    }
}

TEST ( TestOneWriterManyReadersQueue, with_threads )
{
    OneWriterManyReadersQueue Q( 100 );

    for ( int i = 0; i < 3; ++i )
    {
        std::stringstream ss;
        ss << "value #" << i;
        Q.enqueue( make_shared< string >( ss.str() ) );
    }

    std::atomic< int > count( 0 );
    std::thread worker_0( consumer, &Q, &count );
    std::thread worker_1( consumer, &Q, &count );
    std::thread worker_2( consumer, &Q, &count );

    int i = 3;
    while ( i < 20 )
    {
        std::stringstream ss;
        ss << "value #" << i;
        if ( Q.enqueue( make_shared< string >( ss.str() ) ) )
            i++;
        else
            std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
    }

    Q.close();
    worker_0.join();
    worker_1.join();
    worker_2.join();

    ASSERT_EQ( 20, count.load() );
}

TEST ( TestOneWriterManyReadersQueue, with_limits )
{
    OneWriterManyReadersQueue Q( 3 );

    for ( int i = 0; i < 3; ++i )
    {
        std::stringstream ss;
        ss << "value #" << i;
        Q.enqueue( make_shared< string >( ss.str() ) );
    }

    std::atomic< int > count( 0 );
    std::thread worker_0( consumer, &Q, &count );
    std::thread worker_1( consumer, &Q, &count );
    std::thread worker_2( consumer, &Q, &count );

    int i = 3;
    int waits = 0;
    while ( i < 20 )
    {
        std::stringstream ss;
        ss << "value #" << i;
        if ( Q.enqueue( make_shared< string >( ss.str() ) ) )
            i++;
        else
        {
            std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
            waits++;
        }
    }

    Q.close();
    worker_0.join();
    worker_1.join();
    worker_2.join();

    ASSERT_EQ( 20, count.load() );
    ASSERT_LT( 0, waits );    
}

/* --------------------------------------------------------------------------- */

TEST ( TestOutputQueue, Ctr )
{
    OutputQueue Q( 10 );
}

class OutputQueue_Fixture : public ::testing::Test
{
    public:
        OutputQueue::string_type make_shared_str( const char * s )
        {
            return make_shared< string >( s );
        }

        void check_value_and_cat( OutputQueue &Q, const char * value, LogLinesInterface::Category cat )
        {
            OutputQueue::string_type s;
            LogLinesInterface::Category cat_q;
            s = Q.dequeue( cat_q );
            ASSERT_TRUE( s );
            ASSERT_EQ( value, *s );
            ASSERT_EQ( cat, cat_q );
        }

        void check_empty( OutputQueue &Q )
        {
            OutputQueue::string_type s;
            LogLinesInterface::Category cat_q;
            s = Q.dequeue( cat_q );
            ASSERT_FALSE( s );
        }
};

TEST_F ( OutputQueue_Fixture, put_and_get )
{
    OutputQueue Q( 10 );
    ASSERT_TRUE( Q.enqueue( make_shared_str( "1" ), LogLinesInterface::cat_good ) );
    ASSERT_TRUE( Q.enqueue( make_shared_str( "2" ), LogLinesInterface::cat_bad ) );
    ASSERT_TRUE( Q.enqueue( make_shared_str( "3" ), LogLinesInterface::cat_ugly ) );

    check_value_and_cat( Q, "1", LogLinesInterface::cat_good );
    check_value_and_cat( Q, "2", LogLinesInterface::cat_bad );
    check_value_and_cat( Q, "3", LogLinesInterface::cat_ugly );
}

TEST ( TestOutputQueue, close_the_q )
{
    OutputQueue Q( 10 );
    ASSERT_TRUE( Q.is_open() );
    Q.close();
    ASSERT_FALSE( Q.is_open() );    
}

TEST_F ( OutputQueue_Fixture, limit )
{
    OutputQueue Q( 2 );
    ASSERT_TRUE( Q.enqueue( make_shared_str( "1" ), LogLinesInterface::cat_good ) );
    ASSERT_TRUE( Q.enqueue( make_shared_str( "2" ), LogLinesInterface::cat_bad ) );
    ASSERT_FALSE( Q.enqueue( make_shared_str( "3" ), LogLinesInterface::cat_ugly ) );

    check_value_and_cat( Q, "1", LogLinesInterface::cat_good );
    ASSERT_TRUE( Q.enqueue( make_shared_str( "3" ), LogLinesInterface::cat_ugly ) );
}

void producer( OutputQueue * q, size_t cnt )
{
    while ( cnt > 0 )
    {
        if ( ! q -> enqueue( make_shared<string>( "" ), LogLinesInterface::cat_good ) )
        {
            if ( !q -> is_open() )
                return;
            else
                std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
        }
        --cnt;
    }
}


TEST ( TestOutputQueue, with_threads )
{
    OutputQueue Q( 100 );

    std::thread worker_0( producer, &Q, 20 );
    std::thread worker_1( producer, &Q, 30 );
    std::thread worker_2( producer, &Q, 40 );

    size_t cnt = 0;
    LogLinesInterface::Category cat;
    while ( cnt < 20+30+40 )
    {
        if ( Q.dequeue( cat ) )
        {
            ++ cnt;
        }
        else
        {
            std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
        }
    }

    Q.close();
    worker_0.join();
    worker_1.join();
    worker_2.join();
}
