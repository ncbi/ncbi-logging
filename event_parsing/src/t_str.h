#ifndef _h_t_str_
#define _h_t_str_

#include <stdlib.h>

typedef struct
{
    const char * p;
    size_t n;
} t_str;

void clear_str( t_str * s );
unsigned int str_2_uint( const t_str * s );
unsigned long str_2_ulong( const t_str * s );
unsigned int month_str_2_n( const t_str * s );

#endif