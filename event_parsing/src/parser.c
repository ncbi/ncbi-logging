#include <stdio.h>
#include <string.h>

#include "log_event.h"

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
            print_event( &event );
        }
        else if ( self -> stop_on_error )
        {
            fprintf( stderr, "[%u] error\n", line_nr );
            res = 3;
        }
    }

    return res;
}

int main( int argc, char * argv[] )
{
    int res = 3;
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
    return res;
}
