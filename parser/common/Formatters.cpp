#include "Formatters.hpp"

#include "Queues.hpp"
#include <utf8proc.h>

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
    try
    {
        j -> addValue( String ( name.c_str(), name.size() ), ToJsonString ( value ) );
    }
    catch( ncbi::JSONUniqueConstraintViolation & )
    {
    }
}

void
JsonLibFormatter::addNameValue( const std::string & name, int64_t value )
{
    try
    {
        j -> addValue( String ( name.c_str(), name.size() ), JSON::makeInteger( value ) );
    }
    catch( ncbi::JSONUniqueConstraintViolation & )
    {
    }
}

void
JsonLibFormatter::addNameValue( const std::string & name, const std::string & value )
{
    try
    {
        j -> addValue( String ( name.c_str(), name.size() ), JSON::makeString( String( value.c_str(), value.size() ) ) );
    }
    catch( ncbi::JSONUniqueConstraintViolation & )
    {
    }
}

void
JsonLibFormatter::addArray( const std::string & name )
{
    arrayName = name;
    openArray = ncbi::JSON::makeArray();
}

void
JsonLibFormatter::addArrayValue( const t_str & value )
{
    openArray -> appendValue( JSON::makeString( String( value.p, value.n ) ) );
}

void
JsonLibFormatter::closeArray()
{
    try
    {
        j -> addValue( String ( arrayName.c_str(), arrayName.size() ), openArray.release() );
    }
    catch( ncbi::JSONUniqueConstraintViolation & )
    {
    }
}

/* ------------------------------------------------------------------------------------------ */

std::ostream& operator<< (std::ostream& os, const t_str & s)
{
    os . put ( '"' );

    const utf8proc_uint8_t * str = ( const utf8proc_uint8_t * ) s.p;
    utf8proc_int32_t ch;
    utf8proc_ssize_t len;
    utf8proc_ssize_t remain = s.n;
    while ( remain > 0 && ( len = utf8proc_iterate( str, remain, & ch ) ) )
    {
        if ( utf8proc_codepoint_valid ( ch ) )
        {
            switch ( ch )
            {
            case '\\':
            case '\"':
                os . put( '\\' );
                os . put( ch );
                break;
            case '\b':
                os . write( "\\b", 2 );
                break;
            case '\t':
                os . write( "\\t", 2 );
                break;
            case '\n':
                os . write( "\\n", 2 );
                break;
            case '\f':
                os . write( "\\f", 2 );
                break;
            case '\r':
                os . write( "\\r", 2 );
                break;
            default:
                if ( ch >= 0 && ch < 32 )
                {
                    stringstream tmp;
                    tmp << "\\u";
                    tmp << std::hex << std::setfill('0') << std::setw(4);
                    tmp << ch;
                    os . write( tmp.str().c_str(), tmp.str().size() );
                }
                else
                {
                    utf8proc_uint8_t out[4];
                    utf8proc_ssize_t size = utf8proc_encode_char( ch, out );
                    os . write( (const char*)out, size );
                }

                break;
            }
        }
        else
        {
            throw InvalidUTF8String ( XP ( XLOC ) << "badly formed UTF-8 character" );
        }

        str += len;
        remain -= len;
    }
    os . put ( '"' );
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
    seen.clear();
    s . put ( '}' );
    return s.str();
}

void JsonFastFormatter::addNameValue( const std::string & name, const t_str & value )
{
    if ( seen . find( name ) == seen.end() )
    {
        ss.str( "" );
        ss . put( '"' );
        ss . write( name.c_str(), name.size() );
        ss . put( '"' );
        ss . put( ':' );
        ss << value;    /* here exception can happen */
        kv.push_back( ss.str() );
        seen . insert( name );
    }
}

void JsonFastFormatter::addNameValue( const std::string & name, int64_t value )
{
    if ( seen . find( name ) == seen.end() )
    {
        ss . str( "" );
        ss . put( '"' );
        ss . write( name.c_str(), name.size() );
        ss . put( '"' );
        ss . put( ':' );
        ss << value;
        kv.push_back( ss.str() );
        seen . insert( name );
    }
}

void JsonFastFormatter::addNameValue( const std::string & name, const std::string & value )
{
    t_str tmp { value.c_str(), value.size(), false };
    addNameValue( name, tmp );
}

void
JsonFastFormatter::addArray( const std::string & name )
{
    ss . str( "" );
    ss . put( '"' );
    ss . write( name.c_str(), name.size() );
    ss . put( '"' );
    ss . put( ':' );
    ss . put( '[' );
    m_array_name = name;
    first_in_array = true;
}

void
JsonFastFormatter::addArrayValue( const t_str & value )
{
    if ( ! first_in_array )
    {
        ss . put( ',' );
    }
    else
    {
        first_in_array = false;
    }
    ss . put( '"' );
    ss . write( value.p, value.n );
    ss . put( '"' );
}

void
JsonFastFormatter::closeArray()
{
    if ( seen . find( m_array_name ) == seen.end() )
    {
        ss . put( ']' );
        kv.push_back( ss . str() );
    }
    m_array_name.clear();
}

/* ----------- ReverseFormatter ----------- */
ReverseFormatter::~ReverseFormatter()
{
}

string ReverseFormatter::format()
{
    string tmp = ss.str();
    ss.str( "" );
    return tmp;
}

void ReverseFormatter::addNameValue( const std::string & name, const t_str & value )
{
    if ( ( nullptr != value . p ) && ( 0 != value . n ) )
    {
        if ( ! ss . str() . empty() )
            ss . put( separator );
        ss .write( value . p, value . n );
    }
}

void ReverseFormatter::addNameValue( const std::string & name, int64_t value )
{
    if ( ! ss . str() . empty() )
        ss . put( separator );
    ss << value;
}

void ReverseFormatter::addNameValue( const std::string & name, const std::string & value )
{
    if ( ! ss . str() . empty() )
        ss . put( separator );
    ss .write( value . c_str(), value . size() );
}
