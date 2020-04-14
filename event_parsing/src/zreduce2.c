#include <stdio.h>
#include <string.h>

#include "log_event.h"
#include "log_session.h"

typedef struct
{
    const char * f_in_filename;
    const char * f_out_filename;
    
    int delete_loops;
    int report_interval;
    int min_gap;
    
    bool verbose;
    bool print_options;
    bool tail;
    bool cache_time;
    
    FILE * f_in;
    FILE * f_out;
    struct event_extractor * ev_extractor;
    struct sessionizer * sessionizer;
} ctx_t;

#define DFLT_DELETE_LOOPS 4
#define DFLT_REPORT_INTERVAL 120
#define DFLT_MIN_GAP 30

static void init_ctx( ctx_t * self )
{
    self -> f_in_filename = NULL;
    self -> f_out_filename = NULL;

    self -> delete_loops = DFLT_DELETE_LOOPS;
    self -> report_interval = DFLT_REPORT_INTERVAL;
    self -> min_gap = DFLT_MIN_GAP * 60;

    self -> verbose = false;
    self -> print_options = false;
    self -> tail = false;
    self -> cache_time = false;

    self -> f_in = NULL;
    self -> f_out = NULL;
    self -> ev_extractor = NULL;
    self -> sessionizer = NULL;
}

static bool scan_args( int argc, char * argv[], ctx_t * self )
{
    int i;    
    if ( argc < 2 )
    {
        fprintf( stderr, "usage: zreduce2 file.txt\n" );
        return false;
    }

    for ( i = 1; i < argc; i++ )
    {
        const char * arg = argv[ i ];
        if ( arg[ 0 ] == '-' )
        {
            if ( 0 == strcmp( arg, "--print" ) )
                self -> print_options = true;
            else if ( 0 == strcmp( arg, "--verbose" ) )
                self -> verbose = true;
            else if ( 0 == strcmp( arg, "--tail" ) )
                self -> tail = true;
            else if ( 0 == strcmp( arg, "--cache_time" ) )
                self -> cache_time = true;
            else if ( 0 == strncmp( arg, "--min_gap=", 10 ) )
                self -> min_gap = atoi( &( arg[ 10 ] ) ) * 60;
            else
                fprintf( stderr, "invalid option '%s'\n", arg );
        }
        else
        {
            if ( NULL == self -> f_in_filename )
                self -> f_in_filename = arg;
            else if ( NULL == self -> f_out_filename )
                self -> f_out_filename = arg;
        }
    }
    return true;
}

static void print_ctx( ctx_t * self )
{
    if ( NULL != self -> f_in_filename )
        fprintf( stderr, "input file  : '%s'\n", self -> f_in_filename );
    else
        fprintf( stderr, "input file  : STDIN\n" );
    
    if ( NULL != self -> f_out_filename )
        fprintf( stderr, "output file : '%s'\n", self -> f_out_filename );
    else
        fprintf( stderr, "output file : STDOUT\n" );

    fprintf( stderr, "tail        : %s\n", self -> tail ? "YES" : "NO" );
    fprintf( stderr, "verbose     : %s\n", self -> verbose ? "YES" : "NO" );
    fprintf( stderr, "cache time  : %s\n", self -> cache_time ? "YES" : "NO" );    
    fprintf( stderr, "del-loops   : %d\n", self -> delete_loops );
    fprintf( stderr, "min-gap     : %d sec.\n", self -> min_gap );
}

static bool is_NULL_or( const char * s, const char * other )
{
    return ( ( NULL == s ) || ( 0 == strcmp( s, other ) ) );
}

static void close_ctx( ctx_t * self )
{
    if ( !is_NULL_or( self -> f_in_filename, "stdin" ) )
    {
        if ( NULL != self -> f_in )
            fclose( self -> f_in );
    }
    if ( !is_NULL_or( self -> f_out_filename, "stdout" ) )
    {
        if ( NULL != self -> f_out )
            fclose( self -> f_out );
    }
    if ( NULL != self -> ev_extractor )
    {
        destroy_event_extractor( self -> ev_extractor );
        self -> ev_extractor = NULL;
    }
    if ( NULL != self -> sessionizer )
    {
        destroy_sessionizer( self -> sessionizer );
        self -> sessionizer = NULL;
    }
}

static bool on_session_line( const char * line, size_t len, void * ctx )
{
    ctx_t * self = ctx;
    fwrite( line, 1, len, self -> f_out );
    return true;
}

static bool prepare_ctx( ctx_t * self )
{
    if ( is_NULL_or( self -> f_in_filename, "stdin" ) )
        self -> f_in = stdin;
    else
    {
        self -> f_in = fopen( self -> f_in_filename, "rb" );
        if ( NULL == self -> f_in )
        {
            fprintf( stderr, "cannot open '%s' for read\n", self -> f_in_filename );
            return false;
        }
    }
    
    if ( is_NULL_or( self -> f_out_filename, "stdout" ) )
        self -> f_out = stdout;
    else
    {
        self -> f_out = fopen( self -> f_out_filename, "wb" );
        if ( NULL == self -> f_out )
        {
            fprintf( stderr, "cannot open '%s' for write\n", self -> f_out_filename );
            close_ctx( self );
            return false;
        }
    }
    
    if ( self -> cache_time )
    {
        self -> ev_extractor = make_event_extractor();
        if ( NULL == self -> ev_extractor )
        {
            fprintf( stderr, "cannot create event-extractor\n" );
            close_ctx( self );
            return false;
        }
    }

    self -> sessionizer = make_sessionizer( self -> delete_loops,
                                            self -> min_gap,
                                            on_session_line,
                                            self );
    if ( self -> sessionizer == NULL )
    {
        fprintf( stderr, "cannot create sessionizer\n" );
        close_ctx( self );
        return false;
    }
    return true;
}

/*
static t_print_res sessions_to_file( ctx_t * self, const char * filename )
{
    t_print_res res = { 0, 0, 0 };
    
    FILE * f = fopen( filename, "w" );
    if ( f )
    {
        res = print_sessions( self -> sessionizer );

        fclose( f );
    }
    return res;
}
*/

int main_tail( ctx_t * self )
{
    int res = 0;
/*
    char * buffer;
    size_t n = 0, n_events = 0, n_updates = 0;
    time_t t_last;

    time ( &t_last );        
    while ( -1 != getline( (char **)&buffer, &n, self -> f_in ) )
    {
        t_log_event event;
        if ( extract_event( self -> ev_extractor, buffer, &event ) )
        {
            n_events++;
            enter_event( self -> sessionizer, &event );

            if ( ( n_events % 100 ) == 0 )
            {
                time_t t_now;
                
                time ( &t_now );    
                if ( t_now > ( t_last + self -> report_interval ) )
                {
                    t_print_res pr = sessions_to_file( self, "block.txt" );
                    
                    printf( "STOP #%u : %u of %u ( del %u, events %u )\n",
                            n_updates++, pr . printed, pr . total, pr . deleted, n_events );
                            
                    t_last = t_now;
                    n_events = 0;
                }
            }
        }
    }
*/
    return res;
}

int main_all( ctx_t * self )
{
    int res = 0;
    char * buffer = NULL;
    size_t n = 0, n_lines = 0, n_events = 0;
    t_print_res pr;

    while ( -1 != getline( (char **)&buffer, &n, self -> f_in ) )
    {
        t_log_event event;
        n_lines++;
        if ( self -> verbose )
            fprintf( stderr, "[%u] %s", n_lines, buffer );
        
        if ( extract_event( self -> ev_extractor, buffer, n, &event ) )
        {
            enter_event( self -> sessionizer, &event );
            n_events++;
        }
    }
    
    pr = print_sessions( self -> sessionizer );
    unsigned int n_sessions = get_num_sessions( self -> sessionizer );
    fprintf( stderr,
             "processed %lu lines, %lu events, %u sessions\n",
             n_lines, n_events, n_sessions );
    
    return res;
}

int main( int argc, char * argv[] )
{
    int res = 3;
    ctx_t ctx;
    
    init_ctx( &ctx );
    if ( scan_args( argc, argv, &ctx ) )
    {
        if ( ctx . print_options )
            print_ctx( &ctx );
        
        if ( prepare_ctx( &ctx ) )
        {
            if ( ctx . tail )
                res = main_tail( &ctx );
            else
                res = main_all( &ctx );
            close_ctx( &ctx );
        }
    }
    return res;
}
