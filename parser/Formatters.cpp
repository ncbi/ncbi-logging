#include "Formatters.hpp"

#include <sstream>

using namespace std;
using namespace NCBI::Logging;
using namespace ncbi;

FormatterInterface::~FormatterInterface() 
{
}

string & 
FormatterInterface::unEscape( const t_str & value, std::string & str ) const
{
    return str;
}

JsonLibFormatter::JsonLibFormatter()
: j ( JSON::makeObject() )
{
}

JsonLibFormatter::~JsonLibFormatter()
{
}

std::stringstream & 
JsonLibFormatter::format( std::stringstream & out )
{
    //TODO: j->addValue(...)
    out << j->toJSON().toSTLString();
    j = JSON::makeObject();
    return out; 
}

JSONValueRef ToJsonString( const t_str & in )
{
    if ( in . n > 0 )
    {
        return JSON::makeString( String( in . p, in . n) );
    }
    else
    {
        return JSON::makeString( String () );
    }
}

void 
JsonLibFormatter::addNameValue( const std::string & name, const t_str & value )
{
    j -> addValue( String ( name.c_str(), name.size() ), ToJsonString ( value ) );
}

void 
JsonLibFormatter::addNameValue( const std::string & name, int64_t value )
{
    j -> addValue( String ( name.c_str(), name.size() ), JSON::makeInteger( value ) );
}

void 
JsonLibFormatter::addNameValue( const std::string & name, const std::string & value )
{
    j -> addValue( String ( name.c_str(), name.size() ), JSON::makeString( String( value.c_str(), value.size() ) ) );
}

