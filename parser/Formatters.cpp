#include "Formatters.hpp"

#include "Queues.hpp"
#include <sstream>
#include <algorithm>

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

string
JsonLibFormatter::format()
{
    //TODO: j->addValue(...)
    stringstream out;
    out << j->toJSON().toSTLString();
    j = JSON::makeObject();
    return out.str();
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

/* ------------------------------------------------------------------------------------------ */

JsonFastFormatter::~JsonFastFormatter()
{

}

string
JsonFastFormatter::format()
{
    stringstream s;
    s . put ( '{' );
    std::sort( kv.begin(), kv.end() );
    bool first = true;
    for( auto& item : kv )
    {
        if ( first )
            first = false;
        else
            s . put( ',' );
        s . write( item.c_str(), item.size() );
    }
    kv.clear();
    s . put ( '}' );
    return s.str();
}

void JsonFastFormatter::addNameValue( const std::string & name, const t_str & value )
{
    ss.str( "" );
    ss . put( '"' );
    ss . write( name.c_str(), name.size() );
    ss . put( '"' );
    ss . put( ':' );
    ss << value;    /* here exception can happen */
    kv.push_back( ss.str() );
}

void JsonFastFormatter::addNameValue( const std::string & name, int64_t value )
{
    ss.str( "" );
    ss . put( '"' );
    ss . write( name.c_str(), name.size() );
    ss . put( '"' );
    ss . put( ':' );
    ss << value;
    kv.push_back( ss.str() );
}

void JsonFastFormatter::addNameValue( const std::string & name, const std::string & value )
{
    t_str tmp { value.c_str(), value.size(), false };
    addNameValue( name, tmp );
}

