#include "Queues.hpp"

using namespace std;

NCBI::Logging::OneWriterManyReadersQueue::OneWriterManyReadersQueue( size_t limit ) 
:   m_limit( limit ),
    m_line_nr ( 0 ),
    m_max( 0 )
{
    m_open.store( true );
}


bool NCBI::Logging::OneWriterManyReadersQueue::enqueue( NCBI::Logging::OneWriterManyReadersQueue::value_type s ) 
{
    lock_guard< std::mutex > guard( m_mutex ); 
    if ( m_queue.size() >= m_limit )
        return false;

    m_queue.push( s );  // makes a copy
    m_max = max( m_queue.size(), m_max );
    return true;
}

NCBI::Logging::OneWriterManyReadersQueue::value_type NCBI::Logging::OneWriterManyReadersQueue::dequeue( size_t & line_nr ) 
{ 
    lock_guard< std::mutex > guard( m_mutex ); 
    if ( m_queue.empty() )
        return nullptr;

    NCBI::Logging::OneWriterManyReadersQueue::value_type s = m_queue.front();
    ++ m_line_nr;
    line_nr = m_line_nr;
    m_queue.pop(); 
    return s;
}

/* --------------------------------------------------------------------------- */

NCBI::Logging::OutputQueue::OutputQueue( size_t limit )
:   m_limit( limit )
{
    m_open.store( true );
}

bool NCBI::Logging::OutputQueue::enqueue( string_type s, LogLinesInterface::Category cat )
{
    lock_guard< std::mutex > guard( m_mutex ); 
    if ( m_queue.size() >= m_limit )
        return false;

    m_queue.push( output_pair( s, cat ) );
    return true;
}

NCBI::Logging::OutputQueue::string_type NCBI::Logging::OutputQueue::dequeue( LogLinesInterface::Category &cat )
{
    lock_guard< std::mutex > guard( m_mutex ); 
    if ( m_queue.empty() )
        return nullptr;

    auto p = m_queue.front();
    m_queue.pop(); 
    cat = p . second;    
    return p . first;
}
