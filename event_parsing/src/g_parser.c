#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "g_parser.h"

static void parse_word( const t_str *src, size_t *src_idx, t_parsed * res, char term )
{
    size_t n = 0;
    res -> kind = epk_str;
    t_str * v = &( res -> str );
    v -> p = &( src -> p[ *src_idx ] );
    v -> n = 0;
    while ( *src_idx < src -> n )
    {
        if ( src -> p[ *src_idx ] == term ) break;
        n++;
        (*src_idx)++;
    }
    v -> n = n;
}

static bool parse_uint( const t_str *src, size_t *src_idx, t_parsed * res, char term )
{
    parse_word( src, src_idx, res, term );
    if ( res -> str . n < 1 ) return false;
    bool ok;
    uint64_t v = str_2_u64( &( res -> str ), &ok );
    if ( ok )
    {
        res -> kind = epk_uint;
        res -> u = v;
    }
    else
        res -> kind = epk_err;
    return ok;
}

static bool parse_int( const t_str *src, size_t *src_idx, t_parsed * res, char term )
{
    parse_word( src, src_idx, res, term );
    if ( res -> str . n < 1 ) return false;
    bool ok;
    int64_t v = str_2_i64( &( res -> str ), &ok );
    if ( ok )
    {
        res -> kind = epk_int;
        res -> i = v;
    }
    else
        res -> kind = epk_err;
    return ok;
}

static bool parse_float( const t_str *src, size_t *src_idx, t_parsed * res, char term )
{
    parse_word( src, src_idx, res, term );
    if ( res -> str . n < 1 ) return false;
    res -> kind = epk_float;
    res -> f = strtod( res -> str . p, NULL );
    return true;
}

typedef struct
{
    uint8_t day;
    uint8_t month;
    uint16_t year;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} t_time_ints;

static time_t get_unix_time( const t_time_ints * time_ints )
{
    struct tm t;
    memset( &t, 0, sizeof( t ) );
    t . tm_sec  = time_ints -> sec; /* 0-59 */
    t . tm_min  = time_ints -> min; /* 0-59 */
    t . tm_hour = time_ints -> hour;  /* 0-23 */
    t . tm_mday = time_ints -> day;   /* 1-31 */
    t . tm_mon  = time_ints -> month - 1; /* 0-11 */
    t . tm_year = time_ints -> year - 1900; /* years since 1900 */
    return mktime( &t );
}

static bool parse_time( const t_str *src, size_t *src_idx, t_parsed * res, char term )
{
    t_parsed dst[ 8 ];
    size_t num_values, errors;
    t_time_ints time_ints;
    
    parse_word( src, src_idx, res, term );
    if ( res -> str . n < 1 ) return false;
    /* recursivly calling g_parse! - it is save because we do not pass '%t' in format */
    errors = g_parse( &( res -> str ), "%u/%s/%u:%u:%u:%u %i", dst, 8, &num_values );
    if ( errors > 0 ) return false;
    if ( num_values < 6 ) return false;
    if ( dst[ 0 ] . kind != epk_uint ) return false;
    time_ints . day = dst[ 0 ] . u;
    if ( time_ints . day < 1 || time_ints . day > 31 ) return false;
    if ( dst[ 1 ] . kind != epk_str ) return false;
    time_ints . month = month_str_2_n( &( dst[ 1 ] . str ) );
    if ( dst[ 2 ] . kind != epk_uint ) return false;
    time_ints . year = dst[ 2 ] . u;
    if ( time_ints . year < 1900 || time_ints . year > 2099 ) return false;
    if ( dst[ 3 ] . kind != epk_uint ) return false;
    time_ints . hour = dst[ 3 ] . u;
    if ( time_ints . hour > 23 ) return false;
    if ( dst[ 4 ] . kind != epk_uint ) return false;
    time_ints . min = dst[ 4 ] . u;
    if ( time_ints . min > 59 ) return false;    
    if ( dst[ 5 ] . kind != epk_uint ) return false;
    time_ints . sec = dst[ 5 ] . u;
    if ( time_ints . sec > 59 ) return false;    

    res -> kind = epk_time;
    res -> u = get_unix_time( &time_ints );
    return true;
}

static bool parse_request( const t_str *src, size_t *src_idx, t_parsed * res, char term )
{
    t_parsed dst[ 4 ];
    size_t num_values, errors;

    parse_word( src, src_idx, res, term );
    if ( res -> str . n < 1 ) return false;
    /* recursivly calling g_parse! - it is save because we do not pass '%r' in format */
    errors = g_parse( &( res -> str ), "%s %s %s", dst, 4, &num_values );
    if ( errors > 0 ) return false;
    if ( num_values < 3 ) return false;
    res -> req . method = dst[ 0 ] . str;
    res -> req . path = dst[ 1 ] . str;
    res -> req . vers = dst[ 2 ] . str;
    res -> kind = epk_req;
    return true;
}

size_t g_parse( const t_str * src, const char * fmt, t_parsed * dst, size_t dst_size, size_t * num_values )
{
    size_t errors = 0;
    t_str fmtstr;
    size_t src_idx = 0;
    size_t fmt_idx = 0;
    size_t dst_idx = 0;

    if ( NULL == src ) errors++;
    if ( NULL == fmt ) errors++;
    if ( NULL == dst ) errors++;
    if ( 0 == dst_size ) errors++;
    if ( errors > 0 ) return errors;
    if ( 0 == fmt[ 0 ] ) errors++;
    if ( !fill_str( &fmtstr, fmt, 1000 ) ) errors++;
    if ( errors > 0 ) return errors;
    
    if ( NULL != num_values ) *num_values = 0;
    while( src_idx < src -> n && fmt_idx < fmtstr . n )
    {
        char fmt_char = fmtstr . p[ fmt_idx++ ];
        if ( fmt_char == '%' )
        {
            /* special format */
            if ( ( fmt_idx < fmtstr . n ) && ( dst_idx < dst_size ) )
            {
                t_parsed * res = &( dst[ dst_idx ++ ] );
                char term = ' ';
                fmt_char = fmtstr . p[ fmt_idx++ ];
                if ( fmt_idx < fmtstr . n ) term = fmtstr . p[ fmt_idx ];
                switch( fmt_char )
                {
                    case 's' : parse_word( src, &src_idx, res, term ); break;
                    case 'u' : if ( !parse_uint( src, &src_idx, res, term ) ) errors++; break;
                    case 'i' : if ( !parse_int( src, &src_idx, res, term ) ) errors++; break;
                    case 'f' : if ( !parse_float( src, &src_idx, res, term ) ) errors++; break;
                    case 't' : if ( !parse_time( src, &src_idx, res, term ) ) errors++; break;
                    case 'r' : if ( !parse_request( src, &src_idx, res, term ) ) errors++; break;
                    case '%' : if ( src -> p[ src_idx++ ] != fmt_char ) errors++; break;
                }
                if ( NULL != num_values ) (*num_values)++;
            }
            else
            {
                errors++;
                return errors;
            }
        }
        else
        {
            /* direct match against src */
            if ( src -> p[ src_idx++ ] != fmt_char ) errors++;
        }
    }
    return errors;
}

void print_parsed( const t_parsed * parsed, size_t num_parsed )
{
    if ( NULL == parsed ) return;
    if ( 0 == num_parsed ) return;
    for ( size_t i = 0; i < num_parsed; ++i )
    {
        const t_parsed * p = &( parsed[ i ] );
        switch( p -> kind )
        {
            case epk_err   : printf( "[%d] ERROR\n", i ); break;
            case epk_str   : printf( "[%d] <str> : '%.*s'\n", i, p -> str . n, p -> str . p ); break;
            case epk_uint  : printf( "[%d] <uint> : %lu\n", i, p -> u ); break;
            case epk_int   : printf( "[%d] <int> : %ld\n", i, p -> i ); break;
            case epk_float : printf( "[%d] <float> : %f\n", i, p -> f ); break;
            case epk_time  : printf( "[%d] <time> : %lu\n", i, p -> u ); break;
            case epk_req   : printf( "[%d] <req> : '%.*s' '%.*s' '%.*s'\n", i,
                                    p -> req . method . n, p -> req . method . p,
                                    p -> req . path . n, p -> req . path . p,
                                    p -> req . vers . n, p -> req . vers . p );
                                    break;
        }
    }
}
