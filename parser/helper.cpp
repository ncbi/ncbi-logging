#include "helper.hpp"

#include <sstream>
#include <iomanip>

using namespace std;

uint8_t NCBI::Logging::month_int( const t_str * s )
{
    char s0 = s -> p[ 0 ];
    char s1 = s -> p[ 1 ];
    char s2 = s -> p[ 2 ];

    if ( s -> n != 3 ) return 0;

    if ( s0 == 'A' ) /* Apr, Aug */
    {
        if ( s1 == 'p' && s2 == 'r' ) return 4;
        if ( s1 == 'u' && s2 == 'g' ) return 8;
    }
    else if ( s0 == 'D' ) /* Dec */
    {
        if ( s1 == 'e' && s2 == 'c' ) return 12;
    }
    else if ( s0 == 'F' ) /* Feb */
    {
        if ( s1 == 'e' && s2 == 'b' ) return 2;
    }
    else if ( s0 == 'J' ) /* Jan, Jun, Jul */
    {
        if ( s1 == 'a' && s2 == 'n' ) return 1;
        if ( s1 == 'u' )
        {
            if ( s2 == 'n' ) return 6;
            if ( s2 == 'l' ) return 7;
        }
    }
    else if ( s0 == 'M' ) /* Mar, May */
    {
        if ( s1 == 'a' )
        {
            if ( s2 == 'r' ) return 3;
            if ( s2 == 'y' ) return 5;
        }
    }
    else if ( s0 == 'N') /* Nov */
    {
        if ( s1 == 'o' && s2 == 'v' ) return 11;
    }
    else if ( s0 == 'O' ) /* Oct */
    {
        if ( s1 == 'c' && s2 == 't' ) return 10;
    }
    else if ( s0 == 'S' ) /* Sep */
    {
        if ( s1 == 'e' && s2 == 'p' ) return 9;
    }
    return 0;
}

string 
NCBI::Logging::ToString( const t_str & in )
{
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

static
string FormatMonth (uint8_t m)
{
    switch (m)
    {
    case 1: return "Jan";
    case 2: return "Feb";
    case 3: return "Mar";
    case 4: return "Apr";
    case 5: return "May";
    case 6: return "Jun";
    case 7: return "Jul";
    case 8: return "Aug";
    case 9: return "Sep";
    case 10: return "Oct";
    case 11: return "Nov";
    case 12: return "Dec";
    default: return "???";
    }
}

string 
NCBI::Logging::ToString( const t_timepoint & t ) 
{
    ostringstream out;
    out << setfill ('0'); 

    out << "[";
    out << setw (2) << (int)t.day << "/";

    out << FormatMonth( t.month )<<"/";

    out << setw (4) << (int)t.year <<":";
    out << setw (2) << (int)t.hour <<":";
    out << setw (2) << (int)t.minute <<":";
    out << setw (2) << (int)t.second <<" ";
    out << ( t.offset < 0 ? "-" : "+" ); 
    out << setw (4) << abs(t.offset);
    out << "]";

    return out.str();
}