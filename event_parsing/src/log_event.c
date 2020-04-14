
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "log_event.h"

typedef struct event_extractor
{
    time_t cached_t;
    int cached_year;
    int cached_month;
} event_extractor;

event_extractor * make_event_extractor( void )
{
    event_extractor * obj = malloc( sizeof *obj );
    if ( obj != NULL )
    {
        obj -> cached_t = 0;
        obj -> cached_year = 0;
        obj -> cached_month = 0;
    }
    return obj;
}

void destroy_event_extractor( event_extractor * self )
{
    if ( NULL != self )
    {
        free( ( void * ) self );
    }
}

typedef struct
{
    unsigned int day;
    unsigned int month;
    unsigned int year;
    unsigned int hour;
    unsigned int min;
    unsigned int sec;
} t_time_ints;

/*
static double to_julian( unsigned int day, unsigned int month, unsigned int year,
					     unsigned int hour, unsigned int minute, unsigned int second )
{
	double a = floor( ( 14.0 - month ) / 12.0 );
	double y = year + 4800.0 - a;
	double m = month + ( 12.0 * a ) - 3.0;
	double jdn = day + floor( ( 153 * m + 2 ) / 5 ) + 365 * y + floor( y / 4 ) - floor( y / 100 ) + floor( y / 400 ) - 32045;
    return jdn + ( ( hour - 12.0 ) / 24.0 ) + ( minute / 1440.0 ) + ( second / 86400.0 );
}
*/

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

static time_t calc_unix_time( event_extractor * self, const t_time_ints * time_ints )
{
    time_t day_ofs  = ( time_ints -> day - 1 ) * ( 24 * 60 * 60 );
    time_t hour_ofs = time_ints -> hour * 60 * 60;
    time_t min_ofs = time_ints -> min * 60;
    return self -> cached_t + day_ofs + hour_ofs + min_ofs + time_ints -> sec;
}

static time_t to_unix_time( event_extractor * self, const t_time_ints * time_ints )
{

    if ( NULL == self )
        return get_unix_time( time_ints );

    if ( self -> cached_year != time_ints -> year ||
         self -> cached_month != time_ints -> month )
    {
        t_time_ints t_ym;
        memset( &t_ym, 0, sizeof( t_ym ) );            
        t_ym . year  = time_ints -> year;
        t_ym . month = time_ints -> month;
        t_ym . day   = 1;
        self -> cached_t = get_unix_time( &t_ym );
        self -> cached_year = time_ints -> year;
        self -> cached_month = time_ints -> month;
    }
    return calc_unix_time( self, time_ints );
}

static bool valid_SRA( const t_str * acc )
{
    const char * p = acc -> p;
    if ( p [ 0 ] == 'S' )
    {
        if ( ( p[ 1 ] != 'R' ) || ( p[ 2 ] != 'R' ) ) return false;
    }
    else if ( p[ 0 ] == 'D' )
    {
        if ( ( p[ 1 ] != 'R' ) || ( p[ 2 ] != 'R' ) ) return false;
    }
    else if ( p[ 0 ] == 'E' )
    {
        if ( ( p[ 1 ] != 'R' ) || ( p[ 2 ] != 'R' ) ) return false;
    }
    else if ( p[ 0 ] == 'N' )
    {
        if ( ( p[ 1 ] != 'C' ) || ( p[ 2 ] != '_' ) ) return false;
    }
    else
        return false;

    return true;
}

bool extract_event( struct event_extractor * self,
                    const char * buffer,
                    size_t buffer_len,
                    t_log_event * event )
{
    if ( NULL == buffer || NULL == event )
        return false;

    const char * p = buffer;
    const char * p_end = p + buffer_len;
    
    
    unsigned int status = 0;
    t_time_ints time_ints;

    t_str day     = { NULL, 0 };
    t_str month   = { NULL, 0 };
    t_str year    = { NULL, 0 };
    t_str hour    = { NULL, 0 };
    t_str min     = { NULL, 0 };
    t_str sec     = { NULL, 0 };
    t_str cnt     = { NULL, 0 };
    
    event -> ip . p = p;
    event -> ip . n = 0;
    clear_str( &( event -> dom ) );
    clear_str( &( event -> acc ) );
    clear_str( &( event -> res ) );
    clear_str( &( event -> agnt ) );
    event -> unix_date = 0;
    event -> num_bytes = 0;
    
    while ( p < p_end )
    {
        switch( status )
        {
            /* IP-address */
            case  0 :	if ( *p != ' ' ) event -> ip . n++; else status++; break;

            /* date-time */
            case  1 :	if ( *p == '[' ) status++; break;
            case  2 :	day . p = p; day . n = 1; status++; break;
            case  3 :   if ( *p == '/' ) status++; else day . n++; break;
            case  4 :	month . p = p; month . n = 1; status++; break;
            case  5 :   if ( *p == '/' ) status++; else month.n++; break;
            case  6 :	year . p = p; year . n = 1; status++; break;
            case  7 :   if ( *p == ':' ) status++; else year.n++; break;
            case  8 :	hour . p = p; hour . n = 1; status++; break;
            case  9 :	if ( *p == ':' ) status++; else hour.n++; break;
            case 10 :	min . p = p; min . n = 1; status++; break;
            case 11 :	if ( *p == ':' ) status++; else min.n++; break;
            case 12 :	sec . p = p; sec . n = 1; status++; break;
            case 13 :	if ( *p == ' ' ) status++; else sec . n++; break;
            case 14 :	if ( *p == ']' ) status++; break;
            
            /* domain */
            case 15 :	if ( *p == '"' ) status++; break;
            case 16 :	event -> dom . p = p; event -> dom . n = 1; status++; break;
            case 17 :	if ( *p != '"' ) event -> dom . n++; else status++; break;

            /* url -> accession */
            case 18 :	if ( *p == '"' ) status++; break;
            case 19 :	if ( *p == ' ' ) status++; break;
            case 20 :	event -> acc . p = p; event -> acc . n = 1; status++; break;
            case 21 :	if ( *p == ' ' || *p == '?' )
                            status = 23;
                        else if ( *p == '/' )
                            status = 22;
                        else 
                            event -> acc . n++;
                        break;
            case 22 :	event -> acc . p = p; event -> acc . n = 1; status = 21; break;
            case 23 :	if ( *p == '"' ) status++; break;
            
            /* result-code */
            case 24 :	if ( *p != ' ' ) { event -> res . p = p; event -> res . n = 1; status++; } break;
            case 25 :	if ( *p == ' ' ) status++; else event -> res . n++; break;
            
            /* byte-count */
            case 26 :	if ( *p != ' ' ) { cnt.p = p; cnt.n = 1; status++; } break;
            case 27 :	if ( *p == ' ' ) status++; else cnt.n++; break;
            
            /* skip empty fields */
            case 28 :	if ( *p != ' ' ) status++; break;
            case 29 :	if ( *p == ' ' ) status++; break;
            case 30 :	if ( *p == '"' ) status++; break;
            case 31 :	if ( *p == '"' ) status++; break;
            case 32 :	if ( *p == '"' ) status++; break;
            
            /* agent */
            case 33 :	event -> agnt.p = p; event -> agnt.n = 1; status++; break;
            case 34 :	if ( *p == '"' ) status++; else event -> agnt.n++; break;
            
            /* ignore remaining fields */
            case 35 :	p = ( p_end - 1 ); break;
        }
        p++;
    }

    /* filter out NULL-ptr's if a event-parts are not present */
    if ( ( event -> ip . p == NULL ) ||
         ( day . p == NULL ) || ( month . p == NULL ) || ( year . p == NULL ) ||
         ( hour . p == NULL ) || ( min . p == NULL ) || ( sec . p == NULL ) ||
         ( event -> dom . p == NULL ) || ( event -> acc . p == NULL ) ||
         ( event -> res . p == NULL ) || ( cnt . p == NULL ) || ( event -> agnt . p == NULL ) )
        return false;

    /* filter out invalid lengths of event-parts */
    if ( ( event -> ip . n < 7 ) || ( day . n == 0 ) || ( month . n == 0 ) || ( year . n == 0 ) ||
         ( hour . n == 0 ) || ( min . n == 0 ) || ( sec . n == 0 ) || ( event -> dom . n == 0 ) ||
         ( event -> acc . n < 9 ) || ( event -> res . n < 3 ) || ( cnt . n < 1 ) ||
         ( event -> agnt . n < 1 ) )
        return false;

    /* filter accessions not starting with SRR | DRR | ERR | NC_ */
    if ( !valid_SRA( &( event -> acc ) ) )
        return false;
    
    time_ints . day  = str_2_uint( &day );
    if ( time_ints . day < 1 || time_ints . day > 31 ) return false;

    time_ints . month = month_str_2_n( &month );
    if ( time_ints . month < 1 || time_ints . month > 12 ) return false;
    
    time_ints . year = str_2_uint( &year );
    if ( time_ints . year < 1900 || time_ints . year > 2035 ) return false;

    time_ints . hour = str_2_uint( &hour );
    if ( time_ints. hour > 23 ) return false;

    time_ints . min  = str_2_uint( &min );
    if ( time_ints . min > 59 ) return false;
    
    time_ints . sec = str_2_uint( &sec );
    if ( time_ints . sec > 59 ) return false;

    event -> unix_date = to_unix_time( self, &time_ints );
    event -> num_bytes = str_2_ulong( &cnt );
    
    return true;
}
