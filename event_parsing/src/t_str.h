#ifndef _h_t_str_
#define _h_t_str_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    const char * p;
    size_t n;
} t_str;

void clear_str( t_str * s );
uint64_t str_2_u64( const t_str * s, bool *ok );
int64_t str_2_i64( const t_str * s, bool *ok );
uint8_t month_str_2_n( const t_str * s );
bool fill_str( t_str * dst, const char * src, size_t max_len );

#endif
