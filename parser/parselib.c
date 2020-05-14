#include "parselib.h"
#include "types.h"
#include "log1_parser.h"
#include "log1_scanner.h"

parselib * parselib_create_from_stdin( void * data )
{
    parselib * lib = (parselib *)malloc( sizeof( lib ) );
    if ( NULL != lib )
    {
        lib -> src = stdin;
        lib -> data = data;
        lib -> on_event = NULL;
    }
    return lib;
}

parselib * parselib_create_from_file( const char * filename, void * data )
{
    parselib * lib = (parselib *)malloc( sizeof( lib ) );
    if ( NULL != lib )
    {
        lib -> src = fopen( filename, "r" );
        if ( NULL == lib -> src )
        {
            free( lib );
            lib = NULL;
        }
        else
        {
            lib -> data = data;
            lib -> on_event = NULL;
        }
    }
    return lib;
}

void parselib_register_callback( parselib *lib, log_event_t on_event )
{
    if ( NULL != lib )
        lib -> on_event = on_event;
}

void parselib_destroy( parselib * lib )
{
    if ( NULL != lib )
    {
        if ( NULL != lib -> src )
            fclose( lib -> src );
        free( lib );
    }
}

int parselib_run( parselib * lib )
{
    int res = -1;
    if ( NULL != lib )
    {
        yyscan_t sc;

        log1_lex_init( &sc );
        log1_set_in( lib -> src, sc );
        res = log1_parse( sc, lib );
        log1_lex_destroy( sc );
    }
    return res;
}

void parselib_event( parselib *lib, t_event * event )
{
    if ( NULL != lib )
    {
        if ( NULL != lib -> on_event )
        {
            lib -> on_event( event, lib -> data );
        }
    }
}

static void print_tstr( const char * caption, const t_str * s )
{
    if ( s -> n > 0 )
        printf( "%s\t=%.*s\n", caption, s -> n, s -> p );
    else
        printf( "%s\t=none\n", caption );
}

void paselib_print_ev( t_event * ev )
{
    if ( NULL != ev )
    {
        print_tstr( "ip", &( ev -> ip ) );
        print_tstr( "user", &( ev -> user ) );

        printf( "time\t=[%.02u/%.02u/%.04u:%.02u:%.02u:%.02u]\n",
                ev -> t . day, ev -> t . month, ev -> t . year, ev -> t . hour, ev -> t . minute, ev -> t . second );

        print_tstr( "server", &( ev -> server ) );
        print_tstr( "req.method", &( ev -> req . method ) );
        print_tstr( "req.path", &( ev -> req . path ) );
        print_tstr( "req.params", &( ev -> req . params ) );
        print_tstr( "req.vers", &( ev -> req . vers ) );
        printf( "result\t=%d\n", ev -> res_code );
        printf( "body-len\t=%d\n", ev -> res_len );
        print_tstr( "req-time", &( ev -> req_time ) );
        print_tstr( "referer", &( ev -> referer ) );
        print_tstr( "agent", &( ev -> agent ) );
        printf( "port\t=%d\n", ev -> port );
        printf( "req-len\t=%d\n", ev -> req_len );
    }
}
