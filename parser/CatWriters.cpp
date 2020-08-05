#include "CatWriters.hpp"

using namespace NCBI :: Logging;
using namespace std;

// CatCounter

CatCounter::CatCounter()
: total ( 0 )
{
    cat_counts.resize( 4 );
    for ( auto& i : cat_counts )
    {
        i = 0;
    }
}

unsigned long int 
CatCounter::get_cat_count( LogLinesInterface::Category cat ) const
{
    return cat_counts [ cat ];
}

void 
CatCounter::count( LogLinesInterface::Category cat )
{
    ++ cat_counts.at( cat );
    ++ total;
}

// CatWriterInterface

CatWriterInterface :: ~CatWriterInterface() {}

// FileCatWriter

FileCatWriter :: FileCatWriter( const string & baseName ) :
    review  ( baseName+".review" ),
    good    ( baseName+".good" ),
    bad     ( baseName+".bad" ), 
    ugly    ( baseName+".unrecog" )
{
}

FileCatWriter :: ~FileCatWriter() 
{
}

void FileCatWriter :: write( LogLinesInterface::Category cat, const string & s )
{
    lock_guard<mutex> lock( mut );
    switch ( cat )    
    {
    case LogLinesInterface::cat_review :    review << s << endl; break;
    case LogLinesInterface::cat_good :      good << s << endl; break;
    case LogLinesInterface::cat_bad :       bad << s << endl; break;
    case LogLinesInterface::cat_ugly :      ugly << s << endl; break;
    default: throw logic_error( "Unknown Category" ); break;
    }
    ctr.count( cat ); 
}

// StringCatWriter

StringCatWriter :: ~StringCatWriter() 
{
}

void StringCatWriter :: write( LogLinesInterface::Category cat, const string & s )
{
    lock_guard<mutex> lock( mut );
    switch ( cat )    
    {
    case LogLinesInterface::cat_review :    review << s << endl; break;
    case LogLinesInterface::cat_good :      good << s << endl; break;
    case LogLinesInterface::cat_bad :       bad << s << endl; break;
    case LogLinesInterface::cat_ugly :      ugly << s << endl; break;
    default: throw logic_error( "Unknown Category" ); break;
    }
}
