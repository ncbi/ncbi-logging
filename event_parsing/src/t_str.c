
#include "t_str.h"

void clear_str( t_str * s )
{
    s -> p = NULL;
    s -> n = 0;
}

unsigned int str_2_uint( const t_str * s )
{
	unsigned int res = 0;
	int i;
	for ( i = 0; i < s -> n; ++i )
	{
		int c = s -> p[ i ];
		if ( ( c < '0' ) || ( c > '9' ) ) return 0;
		res = ( res * 10 ) + ( c - '0' );
	}
	return res;
}

unsigned long str_2_ulong( const t_str * s )
{
	unsigned long res = 0;
	int i;
	for ( i = 0; i < s -> n; ++i )
	{
		int c = s -> p[ i ];
		if ( c < '0' || c > '9' ) return 0;
		res = ( res * 10 ) + ( c - '0' );
	}
	return res;
}

unsigned int month_str_2_n( const t_str * s )
{
	unsigned int res = 0;
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
