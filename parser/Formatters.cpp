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

std::ostream& operator<< (std::ostream& os, const t_str & s)
{
    os << "\"";
    for ( auto i = 0; i < s.n; ++i )
    {
        switch ( s.p[i] )
        {
        case '\\':
        case '\"':
            os << '\\';
            os << s.p[i];
            break;
        default:
            if ( s.p[i] < 0 )
            {
                throw ncbi::InvalidUTF8String( ncbi::XP(XLOC) << "badly formed UTF-8 character" );
            }
            else if ( s.p[i] < 0x20 )
            {
                std::ostringstream temp;
                temp << "\\u";
                temp << std::hex << std::setfill('0') << std::setw(4);
                temp << (int)s.p[i];
                os << temp.str();
            }
            else
            {
                os << s.p[i];
            }
            break;
        }
    }
    os << "\"";
    return os;
}

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

