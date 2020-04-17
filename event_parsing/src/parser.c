#include <stdio.h>
#include <string.h>

#include "log_event.h"
#include "g_parser.h"

typedef struct
{
    bool verbose;
    bool stop_on_error;
    struct event_extractor * ev_extractor;
} ctx_t;

static void init_ctx( ctx_t * self )
{
    self -> verbose = false;
    self -> stop_on_error = false;
    self -> ev_extractor = NULL;
}

static bool scan_args( int argc, char * argv[], ctx_t * self )
{
    int i;
    for ( i = 1; i < argc; i++ )
    {
        const char * arg = argv[ i ];
        if ( arg[ 0 ] == '-' )
        {
            if ( 0 == strcmp( arg, "--verbose" ) )
                self -> verbose = true;
            else if ( 0 == strcmp( arg, "--stop-on-error" ) )
                self -> stop_on_error = true;
            else
                fprintf( stderr, "invalid option '%s'\n", arg );
        }
    }
    return true;
}

static void close_ctx( ctx_t * self )
{
    if ( NULL != self -> ev_extractor )
    {
        destroy_event_extractor( self -> ev_extractor );
        self -> ev_extractor = NULL;
    }
}

static bool prepare_ctx( ctx_t * self )
{
    self -> ev_extractor = make_event_extractor();
    if ( NULL == self -> ev_extractor )
    {
        fprintf( stderr, "cannot create event-extractor\n" );
        close_ctx( self );
        return false;
    }
    return true;
}

static int run( ctx_t * self )
{
    int res = 0;
    char * buffer = NULL;
    size_t n = 0, line_nr = 0;
    t_log_event event;

    while ( 0 == res && -1 != getline( (char **)&buffer, &n, stdin ) )
    {
        line_nr++;
        if ( self -> verbose )
            fprintf( stderr, "[%u] %s", line_nr, buffer );

        if ( extract_event( self -> ev_extractor, buffer, n, &event, self -> verbose ) )
        {
            if ( self -> verbose )
                print_event_dbg( &event );
            else
                print_event_tsv( &event );
        }
        else if ( self -> stop_on_error )
        {
            fprintf( stderr, "[%u] error\n", line_nr );
            res = 3;
        }
    }

    return res;
}

static int run2( const char *line, const char * fmt )
{
    t_parsed dst[ 16 ];
    t_str src;
    size_t num_values, errors;

    printf( "LINE: '%s'\n", line );
    printf( "FMT:  '%s'\n", fmt );

    fill_str( &src, line, 1000 );
    errors = g_parse( &src, fmt, dst, 16, &num_values );
    printf( "%u error(s)\n", errors );
    print_parsed( dst, num_values );

    return errors == 0 ? 0 : 3;
}

int main( int argc, char * argv[] )
{
    int res = 3;

    res = run2( "a-\"0200\" [01/Jan/2020:02:53:24 -0500] -12 c 10.07 \"GET /somewhere/somewhat HTTP1.1\"",
                "%s-\"%u\" [%t] %i %s %f \"%r\"" );
/*
    ctx_t ctx;
    
    init_ctx( &ctx );
    if ( scan_args( argc, argv, &ctx ) )
    {
        if ( prepare_ctx( &ctx ) )
        {
            res = run( &ctx );
            close_ctx( &ctx );
        }
    }
*/
    return res;
}
