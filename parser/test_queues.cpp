#include <gtest/gtest.h>
#include <thread>

#include "Queues.hpp"

using namespace std;
using namespace NCBI::Logging;

TEST ( TestOneWriterManyReadersQueue, Ctr )
{
    OneWriterManyReadersQueue Q( 10 );
}

TEST ( TestOneWriterManyReadersQueue, put_and_get )
{
    OneWriterManyReadersQueue Q( 10 );
    ASSERT_TRUE( Q.enqueue( "1" ) );
    ASSERT_TRUE( Q.enqueue( "2" ) );
    ASSERT_TRUE( Q.enqueue( "3" ) );

    std::string s;
    size_t line_nr;
    ASSERT_TRUE( Q.dequeue( s, line_nr ) );
    ASSERT_EQ( "1", s );
    ASSERT_EQ( 1, line_nr );
    ASSERT_TRUE( Q.dequeue( s, line_nr ) );
    ASSERT_EQ( "2", s );
    ASSERT_EQ( 2, line_nr );
    ASSERT_TRUE( Q.dequeue( s, line_nr ) );
    ASSERT_EQ( "3", s );
    ASSERT_EQ( 3, line_nr );
    ASSERT_FALSE( Q.dequeue( s, line_nr ) );
}

TEST ( TestOneWriterManyReadersQueue, close_the_q )
{
    OneWriterManyReadersQueue Q( 10 );
    ASSERT_TRUE( Q.is_open() );
    Q.close();
    ASSERT_FALSE( Q.is_open() );    
}

TEST ( TestOneWriterManyReadersQueue, limit )
{
    OneWriterManyReadersQueue Q( 2 );
    ASSERT_TRUE( Q.enqueue( "1" ) );
    ASSERT_TRUE( Q.enqueue( "2" ) );
    ASSERT_FALSE( Q.enqueue( "3" ) );
    std::string s;
    size_t line_nr;
    ASSERT_TRUE( Q.dequeue( s, line_nr ) );
    ASSERT_TRUE( Q.enqueue( "3" ) );
}

void consumer( OneWriterManyReadersQueue * q, std::atomic< int > * cnt )
{
    while ( true )
    {
        std::string s;
        size_t line_nr;
        if ( !q -> dequeue( s, line_nr ) )
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
        Q.enqueue( ss.str() );
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
        if ( Q.enqueue( ss.str() ) )
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
        Q.enqueue( ss.str() );
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
        if ( Q.enqueue( ss.str() ) )
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

TEST ( TestOutputQueue, Ctr )
{
    OutputQueue Q( 10 );
}

TEST ( TestOutputQueue, put_and_get )
{
    OutputQueue Q( 10 );
    ASSERT_TRUE( Q.enqueue( "1", LogLinesInterface::cat_good ) );
    ASSERT_TRUE( Q.enqueue( "2", LogLinesInterface::cat_bad ) );
    ASSERT_TRUE( Q.enqueue( "3", LogLinesInterface::cat_ugly ) );

    std::string s;
    LogLinesInterface::Category cat;
    ASSERT_TRUE( Q.dequeue( s, cat ) );
    ASSERT_EQ( "1", s );
    ASSERT_EQ( LogLinesInterface::cat_good, cat );
    ASSERT_TRUE( Q.dequeue( s, cat ) );
    ASSERT_EQ( "2", s );
    ASSERT_EQ( LogLinesInterface::cat_bad, cat );
    ASSERT_TRUE( Q.dequeue( s, cat ) );
    ASSERT_EQ( "3", s );
    ASSERT_EQ( LogLinesInterface::cat_ugly, cat );
}

TEST ( TestOutputQueue, close_the_q )
{
    OutputQueue Q( 10 );
    ASSERT_TRUE( Q.is_open() );
    Q.close();
    ASSERT_FALSE( Q.is_open() );    
}

TEST ( TestOutputQueue, limit )
{
    OutputQueue Q( 2 );
    ASSERT_TRUE( Q.enqueue( "1", LogLinesInterface::cat_good ) );
    ASSERT_TRUE( Q.enqueue( "2", LogLinesInterface::cat_good ) );
    ASSERT_FALSE( Q.enqueue( "3", LogLinesInterface::cat_good ) );
    std::string s;
    LogLinesInterface::Category cat;
    ASSERT_TRUE( Q.dequeue( s, cat ) );
    ASSERT_TRUE( Q.enqueue( "3", LogLinesInterface::cat_good ) );
}

void producer( OutputQueue * q, size_t cnt )
{
    while ( cnt > 0 )
    {
        if ( !q -> enqueue( string(), LogLinesInterface::cat_good ) )
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
    std::string s;
    LogLinesInterface::Category cat;
    while ( cnt < 20+30+40 )
    {
        if ( Q.dequeue( s, cat ) )
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

