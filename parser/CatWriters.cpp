#include "CatWriters.hpp"

using namespace NCBI :: Logging;
using namespace std;

// CatCounter

CatCounter::CatCounter()
: total ( 0 )
{
    cat_counts.resize( ReceiverInterface::cat_max );
    for ( auto& i : cat_counts )
    {
        i = 0;
    }
}

unsigned long int 
CatCounter::get_cat_count( ReceiverInterface::Category cat ) const
{
    return cat_counts [ cat ];
}

void 
CatCounter::count( ReceiverInterface::Category cat )
{
    ++ cat_counts.at( cat );
    ++ total;
}

string 
CatCounter::report( FormatterInterface & fmt ) const
{
    fmt.addNameValue("_total", get_total());
    fmt.addNameValue("good", get_cat_count( ReceiverInterface::cat_good ) );
    fmt.addNameValue("review", get_cat_count( ReceiverInterface::cat_review ) );
    fmt.addNameValue("bad", get_cat_count( ReceiverInterface::cat_bad ) );
    fmt.addNameValue("ignored", get_cat_count( ReceiverInterface::cat_ignored ) );
    fmt.addNameValue("unrecog", get_cat_count( ReceiverInterface::cat_ugly ) );
    return fmt.format();
}

// CatWriterInterface

CatWriterInterface :: ~CatWriterInterface() {}

// FileCatWriter

const std::string FileCatWriter :: extension = ".jsonl";

FileCatWriter :: FileCatWriter( const string & baseName ) :
    review  ( baseName+".review" + extension ),
    good    ( baseName+".good" + extension ),
    bad     ( baseName+".bad" + extension ), 
    ugly    ( baseName+".unrecog" + extension )
{
}

FileCatWriter :: ~FileCatWriter() 
{
}

void FileCatWriter :: write( ReceiverInterface::Category cat, const string & s )
{
    switch ( cat )    
    {
    case ReceiverInterface::cat_review :    review << s << endl; break;
    case ReceiverInterface::cat_good :      good.write( s.c_str(), s.size() ); good.put( '\n' ); break;
    case ReceiverInterface::cat_bad :       bad << s << endl; break;
    case ReceiverInterface::cat_ugly :      ugly << s << endl; break;
    case ReceiverInterface::cat_ignored :   break;
    default: throw logic_error( "Unknown Category" ); break;
    }
    ctr.count( cat ); 
}

// StringCatWriter

StringCatWriter :: ~StringCatWriter() 
{
}

void StringCatWriter :: write( ReceiverInterface::Category cat, const string & s )
{
    switch ( cat )    
    {
    case ReceiverInterface::cat_review :    review << s << endl; break;
    case ReceiverInterface::cat_good :      good << s << endl; break;
    case ReceiverInterface::cat_bad :       bad << s << endl; break;
    case ReceiverInterface::cat_ugly :      ugly << s << endl; break;
    case ReceiverInterface::cat_ignored :   break;
    default: throw logic_error( "Unknown Category" ); break;
    }
}
