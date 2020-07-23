#include "Formatters.hpp"

#include <sstream>
#include "LogLinesInterface.hpp"
#include "helper.hpp"

using namespace std;
using namespace NCBI::Logging;

string 
JsonFormatter::format( LogLinesInterface & line ) const
{
    stringstream ret;
    ret << "{";
    LogLinesInterface::Member m;
    bool first = true;
    while ( line.nextMember( m ) )
    {
        if ( ! first ) 
        {
            ret << ",";
        }
        else
        {
            first = false;
        }

        ret << "\"" << ToString( m.name ) << "\":";
        switch ( m. type )
        {
        case LogLinesInterface::Member::t_string: 
            ret << "\"" << Escape( m.u.s ) << "\""; break;
        case LogLinesInterface::Member::t_int: 
            ret << m.u.i; break;
        }
    }
    ret << "}";
    return ret.str();
}

string 
JsonFormatter::format( const string & name, const t_str & value ) const
{   
    stringstream ret;
    ret << "{";
    ret << "\"" << name << "\":\"" << Escape( value ) << "\""; 
    ret << "}";
    return ret.str();
}

string 
JsonFormatter::Escape( const t_str & in ) const
{   //TODO: reduce the amount of string copy and allocation (convert to operator<<(ostream,t_str) )
    if ( in.n == 0 )
    {
        return string();
    }   
    if ( ! in.escaped ) 
    {
        return string ( in.p, in.n );
    }

    ostringstream s;
    // this has to correspond to what is going on in the .l files
    // at this point, only " are escaped
    bool escaped = false;
    for (auto i = 0; i < in.n; ++i )
    {
        char ch = in.p[i];
        if ( escaped )
        {
            if ( ch == '"' )
            {
                s << ch;
            }
            else    // not a character we escape, so put '\' back in
            {
                s << "\\" << ch;
            }
            escaped = false;
        }
        else
        {
            if ( ch == '\\' )
            {
                escaped = true;
            }
            else
            {
                s << ch;
            }
        } 
    }

    if (escaped)
    {   // trailing '\'
        s << "\\";
    }

    return s.str();    
}
