#include "Classifiers.hpp"

using namespace NCBI :: Logging;
using namespace std;

ClassificationInterface :: ~ClassificationInterface() {}

FileClassifier :: FileClassifier( const string & baseName ) :
    review  ( baseName+".review" ),
    good    ( baseName+".good" ),
    bad     ( baseName+".bad" ), 
    ugly    ( baseName+".unrecog" )
{
}

FileClassifier :: ~FileClassifier() 
{
}

void FileClassifier :: write( LogLinesInterface::Category cat, const string & s )
{
    switch ( cat )    
    {
    case LogLinesInterface::cat_review :    review << s << endl; break;
    case LogLinesInterface::cat_good :      good << s << endl; break;
    case LogLinesInterface::cat_bad :       bad << s << endl; break;
    case LogLinesInterface::cat_ugly :      ugly << s << endl; break;
    default: throw logic_error( "Unknown Category" ); break;
    }
}

StringClassifier :: ~StringClassifier() 
{
}

void StringClassifier :: write( LogLinesInterface::Category cat, const string & s )
{
    switch ( cat )    
    {
    case LogLinesInterface::cat_review :    review << s << endl; break;
    case LogLinesInterface::cat_good :      good << s << endl; break;
    case LogLinesInterface::cat_bad :       bad << s << endl; break;
    case LogLinesInterface::cat_ugly :      ugly << s << endl; break;
    default: throw logic_error( "Unknown Category" ); break;
    }
}
