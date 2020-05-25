#include "helper.hpp"

uint8_t month_int( const t_str * s )
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
