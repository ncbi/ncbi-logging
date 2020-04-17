#include <stdio.h>
#include <string.h>

#include "g_parser.h"

typedef struct
{
    bool verbose;
    bool stop_on_error;
    struct g_parser * parser;
} ctx_t;

static void init_ctx( ctx_t * self )
{
    self -> verbose = false;
    self -> stop_on_error = false;
    self -> parser = NULL;
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
    if ( NULL != self -> parser )
    {
        destroy_g_parser( self -> parser );
        self -> parser = NULL;
    }
}

static bool prepare_ctx( ctx_t * self )
{
    self -> parser = make_g_parser();
    if ( NULL == self -> parser )
    {
        fprintf( stderr, "cannot create parser\n" );
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
    t_log_data event;

    while ( 0 == res && -1 != getline( (char **)&buffer, &n, stdin ) )
    {
        line_nr++;
        if ( self -> verbose )
            fprintf( stderr, "[%u] (len=%u) %s\n", line_nr, n, buffer );

        if ( g_parse_log_data( self -> parser, buffer, n, &event ) )
        {
            if ( self -> verbose )
                print_data_dbg( &event );
            else
                print_data_tsv( &event );
        }
        else
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
