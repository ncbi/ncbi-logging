#include "Formatters.hpp"

using namespace std;
using namespace NCBI::Logging;

FormatterInterface::~FormatterInterface() 
{
}

string & 
FormatterInterface::unEscape( const t_str & value, std::string & str ) const
{
    return str;
}

JsonLibFormatter::~JsonLibFormatter()
{
}

std::stringstream & 
JsonLibFormatter::format( std::stringstream & out )
{
    return out; // TODO
}

void 
JsonLibFormatter::addNameValue( const std::string & name, const t_str & value )
{
    //TODO
}

void 
JsonLibFormatter::addNameValue( const std::string & name, int64_t value )
{
    //TODO
}

