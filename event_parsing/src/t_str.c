#include "t_str.h"

void clear_str( t_str * s )
{
    s -> p = NULL;
    s -> n = 0;
}

uint64_t str_2_u64( const t_str * s, bool * ok )
{
	uint64_t res = 0;
	int i;
    if ( NULL != ok ) *ok = true;
	for ( i = 0; i < s -> n; ++i )
	{
		int c = s -> p[ i ];
		if ( ( c < '0' ) || ( c > '9' ) )
        {
            if ( NULL != ok ) *ok = false;
            return 0;
        }
		res = ( res * 10 ) + ( c - '0' );
	}
	return res;
}

int64_t str_2_i64( const t_str * s, bool * ok )
{
    if ( s -> p [ 0 ] != '-' )
        return ( int64_t )str_2_u64( s, ok );
    else
    {
        uint64_t value;
        t_str s1 = { s -> p, s -> n };
        s1 . p ++;
        s1 . n --;
        value = str_2_u64( &s1, ok );
        return ( -value );
    }
}

uint8_t month_str_2_n( const t_str * s )
{
	uint8_t res = 0;
	switch( s -> p[ 0 ] )
	{
		case 'A' :	if ( s -> p[ 1 ] == 'P' || s -> p[ 1 ] == 'p' )
                        res = 3;
                    else if ( s -> p[ 1 ] == 'U' || s -> p[ 1 ] == 'u' )
                        res = 8;
                    break;
                    
		case 'D' :	res = 12; break;
        
		case 'F' :	res = 2; break;
        
		case 'J' :	if ( s -> p[ 1 ] == 'A' || s -> p[ 1 ] == 'a' )
						res = 1;
					else if ( s -> p[ 1 ] == 'U' || s -> p[ 1 ] == 'u' )
					{
						if ( s -> p[ 2 ] == 'N' || s -> p[ 2 ] == 'n' )
							res = 6;
						else if ( s -> p[ 2 ] == 'L' || s -> p[ 2 ] == 'l' )
							res = 7;
					}
					break;
                    
		case 'M' :	if ( s -> p[ 1 ] == 'A' || s -> p[ 1 ] == 'a' )
					{
						if ( s -> p[ 2 ] == 'R' || s -> p[ 2 ] == 'r' )
                            res = 4;
                        else if ( s -> p[ 2 ] == 'Y' || s -> p[ 2 ] == 'y' )
                            res = 5;
					}
					break;
                    
		case 'N' :	res = 11; break;
        
		case 'O' :	res = 10; break;
        
		case 'S' :	res = 9; break;		
	}
	return res;
}

bool fill_str( t_str * dst, const char * src, size_t max_len )
{
    size_t i = 0;
    if ( NULL == dst ) return false;
    if ( NULL == src ) return false;
    if ( 0 == src[ 0 ] ) return false;
    dst -> p = src;
    while ( i < max_len && 0 != src[ i ] ) { i++; }
    dst -> n = i;
    return i < max_len;
}
