#include "LineSplitters.hpp"

#include <cstdio>

using namespace NCBI::Logging;
using namespace std;

StdLineSplitter::StdLineSplitter( std::istream &is )
: m_is( is )
{
}

StdLineSplitter::~StdLineSplitter()
{
}

bool StdLineSplitter::getLine( void )
{
    m_buffer.clear();
    getline( m_is, m_buffer );
    if ( m_buffer.empty() )
    {
        return m_is.good();
    }
    return true;
}

const char * StdLineSplitter::data( void ) const
{
    return m_buffer.c_str();
}

size_t StdLineSplitter::size( void ) const
{
    return m_buffer.size();
}

/// CLineSplitter

CLineSplitter::CLineSplitter( FILE * f )
:   m_f ( f ),
    m_line ( nullptr ),
    m_size ( 0 ),
    m_allocated( 0 )
{
}

CLineSplitter::~CLineSplitter()
{
    free( m_line );
}

bool CLineSplitter::getLine( void )
{
    if ( nullptr == m_f )
        return false;
    ssize_t read = getline( & m_line, & m_allocated, m_f );
    if ( read == -1 )
    {   // error or EOF
        m_size = 0;
        return false;
    }

    m_size = (ssize_t) read;

    if ( m_size > 0 && m_line[ m_size - 1 ] == 0x0A )
    {
        m_size--;
    }

    return true;
}

const char * CLineSplitter::data( void ) const
{
    return m_line;
}

size_t CLineSplitter::size( void ) const
{
    return m_size;
}

BufLineSplitter::BufLineSplitter( const char * buf, size_t size  )
:   m_f( fmemopen( ( void * )buf, size, "r" ) ), m_splitter( m_f )
{
}

BufLineSplitter::~BufLineSplitter()
{
    if ( nullptr != m_f )
        fclose( m_f );
}
