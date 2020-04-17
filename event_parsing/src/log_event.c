#include <stdio.h>
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
    switch( p [ 0 ] )
    {
        case 'S' : return ( ( p[ 1 ] == 'R' ) && ( p[ 2 ] == 'R' ) ); break;
        case 'E' : return ( ( p[ 1 ] == 'R' ) && ( p[ 2 ] == 'R' ) ); break;
        case 'D' : return ( ( p[ 1 ] == 'R' ) && ( p[ 2 ] == 'R' ) ); break;
        case 'N' : return ( ( p[ 1 ] == 'C' ) && ( p[ 2 ] == '_' ) ); break;
    }
    return false;
}

typedef enum ev_status
{
    e_ip = 0,

    e_t_start,
    e_t_day,
    e_t_slash1,
    e_t_month,
    e_t_slash2,
    e_t_year,
    e_t_colon1,
    e_t_hour,
    e_t_colon2,
    e_t_minutes,
    e_t_colon3,
    e_t_seconds,
    e_t_space,
    e_t_stop,

    e_dom_start,
    e_dom1,
    e_dom,

    e_req_start,
    e_req_space1,
    e_req_acc,
    e_req_delim,
    e_req_stop,

    e_res1,
    e_res,

    e_bytes1,
    e_bytes,

    e_factor1,
    e_factor,

    e_dash3,
    e_dash3_stop,

    e_agent_start,
    e_agent1,
    e_agent,

    e_ignore,
    e_error
} ev_status;

static void print_status( ev_status status )
{
    switch( status )
    {
        case e_ip       : fprintf( stderr, "e_ip " ); break;

        case e_t_start  : fprintf( stderr, "e_t_start " ); break;
        case e_t_day    : fprintf( stderr, "e_t_day " ); break;
        case e_t_slash1 : fprintf( stderr, "e_t_slash1 " ); break;
        case e_t_month  : fprintf( stderr, "e_t_month " ); break;
        case e_t_slash2 : fprintf( stderr, "e_t_slash2 " ); break;
        case e_t_year   : fprintf( stderr, "e_t_year " ); break;
        case e_t_colon1 : fprintf( stderr, "e_t_colon1 " ); break;
        case e_t_hour   : fprintf( stderr, "e_t_hour " ); break;
        case e_t_colon2 : fprintf( stderr, "e_t_colon2 " ); break;
        case e_t_minutes: fprintf( stderr, "e_t_minutes " ); break;
        case e_t_colon3 : fprintf( stderr, "e_t_colon3 " ); break;
        case e_t_seconds: fprintf( stderr, "e_t_seconds " ); break;
        case e_t_space  : fprintf( stderr, "e_t_space " ); break;
        case e_t_stop   : fprintf( stderr, "e_t_stop " ); break;

        case e_dom_start: fprintf( stderr, "e_dom_start " ); break;
        case e_dom1     : fprintf( stderr, "e_dom1 " ); break;
        case e_dom      : fprintf( stderr, "e_dom " ); break;

        case e_req_start: fprintf( stderr, "e_req_start " ); break;
        case e_req_space1:fprintf( stderr, "e_req_space1 " ); break;
        case e_req_acc  : fprintf( stderr, "e_req_acc " ); break;
        case e_req_delim: fprintf( stderr, "e_req_delim " ); break;
        case e_req_stop : fprintf( stderr, "e_req_stop " ); break;

        case e_res1     : fprintf( stderr, "e_res1 " ); break;
        case e_res      : fprintf( stderr, "e_res " ); break;

        case e_bytes1   : fprintf( stderr, "e_bytes1 " ); break;
        case e_bytes    : fprintf( stderr, "e_bytes " ); break;

        case e_factor1  : fprintf( stderr, "e_factor1 " ); break;
        case e_factor   : fprintf( stderr, "e_factor " ); break;

        case e_dash3    : fprintf( stderr, "e_dash3 " ); break;
        case e_dash3_stop: fprintf( stderr, "e_dash3_stop " ); break;

        case e_agent_start: fprintf( stderr, "e_agent_start " ); break;
        case e_agent1   : fprintf( stderr, "e_agent1 " ); break;
        case e_agent    : fprintf( stderr, "e_agent " ); break;

        case e_ignore   : fprintf( stderr, "e_ignore " ); break;
        case e_error    : fprintf( stderr, "e_error " ); break;

        default : fprintf( stderr, "unknown %d ", status ); break;
    }
}

static void clear_event( t_log_event * event )
{
    clear_str( &( event -> ip ) );
    clear_str( &( event -> dom ) );
    clear_str( &( event -> acc ) );
    clear_str( &( event -> req ) );
    clear_str( &( event -> res ) );
    clear_str( &( event -> agnt ) );
    event -> unix_date = 0;
    event -> num_bytes = 0;
}

bool extract_event( struct event_extractor * self,
                    const char * buffer,
                    size_t buffer_len,
                    t_log_event * event,
                    bool verbose
                  )
{
    if ( NULL == buffer || NULL == event ) return false;

    const char * p = buffer;
    const char * p_end = p + buffer_len;

    ev_status status = e_ip;
    t_time_ints time_ints;

    t_str day     = { NULL, 0 };
    t_str month   = { NULL, 0 };
    t_str year    = { NULL, 0 };
    t_str hour    = { NULL, 0 };
    t_str min     = { NULL, 0 };
    t_str sec     = { NULL, 0 };
    t_str cnt     = { NULL, 0 };

    clear_event( event );
    event -> ip . p = p;

    while ( p < p_end )
    {
        char c = *p;
        if ( verbose ) print_status( status );
        switch( status )
        {
            /* IP-address >139.80.16.229< */
            case e_ip           : if ( c != ' ' ) event -> ip . n++; else status = e_t_start; break;

            /* date-time > - - [01/Jan/2020:02:50:24 -0500]< */
            case e_t_start      : if ( c == '[' ) status = e_t_day; break;
            case e_t_day        : day . p = p; day . n = 1; status = e_t_slash1; break;
            case e_t_slash1     : if ( c == '/' ) status = e_t_month; else day . n++; break;
            case e_t_month      : month . p = p; month . n = 1; status = e_t_slash2; break;
            case e_t_slash2     : if ( c == '/' ) status = e_t_year; else month . n++; break;
            case e_t_year       : year . p = p; year . n = 1; status = e_t_colon1; break;
            case e_t_colon1     : if ( c == ':' ) status = e_t_hour; else year . n++; break;
            case e_t_hour       : hour . p = p; hour . n = 1; status = e_t_colon2; break;
            case e_t_colon2     : if ( c == ':' ) status = e_t_minutes; else hour . n++; break;
            case e_t_minutes    : min . p = p; min . n = 1; status = e_t_colon3; break;
            case e_t_colon3     : if ( c == ':' ) status = e_t_seconds; else min . n++; break;
            case e_t_seconds    : sec . p = p; sec . n = 1; status = e_t_space; break;
            case e_t_space      : if ( c == ' ' ) status = e_t_stop; else sec . n++; break;
            case e_t_stop       : if ( c == ']' ) status = e_dom_start; break;

            /* domain > "sra-download.ncbi.nlm.nih.gov"< !!! can also be not quoted !!! */
            case e_dom_start    : switch ( c )
                                  {
                                    case '"' : status = e_dom1; break;
                                    case ' ' : break;
                                    default  : event -> dom . p = p; event -> dom . n = 1; status = e_dom; break;
                                  }
                                  break;
            case e_dom1         : event -> dom . p = p; event -> dom . n = 1; status = e_dom; break;
            case e_dom          : switch( c )
                                  {
                                    case ' ' : ;/* fall through intented!!! */
                                    case '"' : status = e_req_start; break;
                                    default  : event -> dom . n++; break;
                                  }
                                  break;

            /* url -> accession > "GET /traces/sra32/SRR/005807/SRR5946882 HTTP/1.1"< */
            case e_req_start    : if ( c == '"' ) { status = e_req_space1; event -> req . p = p; } break;
            case e_req_space1   : if ( c == ' ' ) status = e_req_acc;
                                  event -> req . n++;
                                  break;
            case e_req_acc      : event -> acc . p = p;
                                  event -> acc . n = 1;
                                  event -> req . n ++;
                                  status = e_req_delim;
                                  break;
            case e_req_delim    : switch( c )
                                  {
                                    case ' ' : ; /* fall through intented!!! */
                                    case '?' : status = e_req_stop; break;
                                    case '/' : status = e_req_acc; break; /* loop-back intented!!! */
                                    default  : event -> acc . n++; break;
                                  }
                                  event -> req . n ++;
                                  break;
            case e_req_stop     : if ( c == '"' ) status = e_res1; event -> req . n ++; break;

            /* result-code > 206< */
            case e_res1         : if ( c >= '0' && c <= '9' ) {
                                    event -> res . p = p; event -> res . n = 1; status = e_res;
                                  } break;

            case e_res          : if ( c == ' ' )
                                    status = e_bytes1;
                                  else if ( c >= '0' && c <= '9' )
                                    event -> res . n++;
                                  else
                                    status = e_error;
                                  break;

            /* byte-count > 32768< */
            case e_bytes1       : if ( c >= '0' && c <= '9' ) {
                                    cnt.p = p; cnt.n = 1; status = e_bytes;
                                  } break;
            case e_bytes        : if ( c == ' ' )
                                    status = e_factor1;
                                  else if ( c >= '0' && c <= '9' )
                                    cnt . n++;
                                  else
                                    status = e_error;
                                  break;

            /* factor > 0.000< */
            case e_factor1      : if ( c != ' ' ) status = e_factor; break;
            case e_factor       : if ( c == ' ' ) status = e_dash3; break;

            /* dash #3 > "-"< */
            case e_dash3        : if ( c == '"' ) status = e_dash3_stop; break;
            case e_dash3_stop   : if ( c == '"' ) status = e_agent_start; break;

            /* agent > "linux64 sra-toolkit fastq-dump.2.9.1"< */
            case e_agent_start  : if ( c == '"' ) status = e_agent1; break;
            case e_agent1       : event -> agnt . p = p; event -> agnt . n = 1; status = e_agent; break;
            case e_agent        : if ( c == '"' ) status = e_ignore; else event -> agnt . n++; break;

            /* ignore remaining fields > "-" port=443 rl=293< */
            case e_ignore       : p = ( p_end - 1 ); break; /* this will end the loop! */
        }
        p++;
    }

    if ( verbose ) fprintf( stderr, "\n" );

    /* filter out NULL-ptr's if a event-parts are not present */
    if ( ( event -> ip . p == NULL ) ||
         ( day . p == NULL ) || ( month . p == NULL ) || ( year . p == NULL ) ||
         ( hour . p == NULL ) || ( min . p == NULL ) || ( sec . p == NULL ) ||
         ( event -> dom . p == NULL ) || ( event -> acc . p == NULL ) ||
         ( event -> res . p == NULL ) || ( cnt . p == NULL ) || ( event -> agnt . p == NULL ) )
    {
        if ( verbose ) fprintf(  stderr, "event-part not present\n" );
        return false;
    }
    
    /* filter out invalid lengths of event-parts */
    if ( ( event -> ip . n < 7 ) || ( day . n == 0 ) || ( month . n == 0 ) || ( year . n == 0 ) ||
         ( hour . n == 0 ) || ( min . n == 0 ) || ( sec . n == 0 ) || ( event -> dom . n == 0 ) ||
         ( event -> acc . n < 9 ) || ( event -> res . n < 3 ) || ( cnt . n < 1 ) ||
         ( event -> agnt . n < 1 ) )
    {
        if ( verbose ) fprintf( stderr, "invalid length of event-part\n" );
        return false;
    }
    
    /* filter accessions not starting with SRR | DRR | ERR | NC_ */
    if ( !valid_SRA( &( event -> acc ) ) )
    {
        if ( verbose ) fprintf( stderr, "invalid accession '%.*s'\n", event -> acc . n, event -> acc . p );
        return false;
    }

    time_ints . day  = str_2_u64( &day, NULL );
    if ( time_ints . day < 1 || time_ints . day > 31 )
    {
        if ( verbose ) fprintf( stderr, "invalid day: %d\n", time_ints . day );
        return false;
    }

    time_ints . month = month_str_2_n( &month );
    if ( time_ints . month < 1 || time_ints . month > 12 )
    {
        if ( verbose ) fprintf( stderr, "invalid month: %d\n", time_ints . month );
        return false;
    }
    
    time_ints . year = str_2_u64( &year, NULL );
    if ( time_ints . year < 1900 || time_ints . year > 2099 )
    {
        if ( verbose ) fprintf( stderr, "invalid year: %d\n", time_ints . year );
        return false;
    }

    time_ints . hour = str_2_u64( &hour, NULL );
    if ( time_ints. hour > 23 )
    {
        if ( verbose ) fprintf( stderr, "invalid hour: %d\n", time_ints . hour );
        return false;
    }

    time_ints . min  = str_2_u64( &min, NULL );
    if ( time_ints . min > 59 )
    {
        if ( verbose ) fprintf( stderr, "invalid minutes: %d\n", time_ints . min );
        return false;
    }

    time_ints . sec = str_2_u64( &sec, NULL );
    if ( time_ints . sec > 59 )
    {
        if ( verbose ) fprintf( stderr, "invalid seconds: %d\n", time_ints . sec );
        return false;
    }

    event -> unix_date = to_unix_time( self, &time_ints );
    event -> num_bytes = str_2_u64( &cnt, NULL );

    if ( verbose ) fprintf( stderr, "success!\n" );
    return true;
}

void print_event_tsv( const t_log_event * event )
{
    printf( "%.*s\t%.*s\t%.*s\t%.*s\t%u\t%u\n",
        event -> ip . n, event -> ip . p,
        event -> acc . n, event -> acc . p,
        event -> res . n, event -> res . p,
        event -> agnt . n, event -> agnt . p,
        event -> unix_date,
        event -> num_bytes
        );
}

void print_event_dbg( const t_log_event * event )
{
    printf( "ip:\t%.*s\n", event -> ip . n, event -> ip . p );
    printf( "dom:\t%.*s\n", event -> dom . n, event -> dom . p );
    printf( "req:\t%.*s\n", event -> req . n, event -> req . p );
    printf( "acc:\t%.*s\n", event -> acc . n, event -> acc . p );
    printf( "res:\t%.*s\n", event -> res . n, event -> res . p );
    printf( "agnt:\t%.*s\n", event -> agnt . n, event -> agnt . p );
    printf( "tt:\t%d\n", event -> unix_date );
    printf( "cnt:\t%d\n\n", event -> num_bytes );
}
