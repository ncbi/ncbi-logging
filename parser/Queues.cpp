#include "Queues.hpp"

using namespace std;

NCBI::Logging::OneWriterManyReadersQueue::OneWriterManyReadersQueue( size_t limit ) 
:   m_limit( limit ),
    m_line_nr ( 0 ),
    m_max( 0 )
{
    m_open.store( true );
}


bool NCBI::Logging::OneWriterManyReadersQueue::enqueue( const std::string & s ) 
{
    lock_guard< std::mutex > guard( m_mutex ); 
    if ( m_queue.size() >= m_limit )
        return false;

    m_queue.push( s );  // makes a copy
    m_max = max( m_queue.size(), m_max );
    return true;
}

bool NCBI::Logging::OneWriterManyReadersQueue::dequeue( std::string & s, size_t & line_nr ) 
{ 
    lock_guard< std::mutex > guard( m_mutex ); 
    if ( m_queue.empty() )
        return false;

    s = m_queue.front(); // makes a copy ( maybe put shared ptr of string into q )
    ++ m_line_nr;
    line_nr = m_line_nr;
    m_queue.pop(); 
    return true;
}

NCBI::Logging::OutputQueue::OutputQueue( size_t limit )
:   m_limit( limit )
{
    m_open.store( true );
}

bool NCBI::Logging::OutputQueue::enqueue( const std::string & s, LogLinesInterface::Category cat )
{
    lock_guard< std::mutex > guard( m_mutex ); 
    if ( m_queue.size() >= m_limit )
        return false;

    m_queue.push( output_pair( s, cat ) );  // makes a copy
    return true;
}

bool NCBI::Logging::OutputQueue::dequeue( std::string & s, LogLinesInterface::Category &cat )
{
    lock_guard< std::mutex > guard( m_mutex ); 
    if ( m_queue.empty() )
        return false;

    auto p = m_queue.front(); // makes a copy ( maybe put shared ptr of string into q )
    s = p . first;
    cat = p . second;
    m_queue.pop(); 
    return true;
}
