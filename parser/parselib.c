#include "parselib.h"
#include "log1_parser.h"
#include "log1_scanner.h"

static void parselib_initialize( parselib * self, void * data )
{
    self -> data = data;
    self -> on_event = NULL;
    self -> on_error = NULL;
}

parselib * parselib_create_from_stdin( void * data )
{
    parselib * self = (parselib *)malloc( sizeof( * self ) );
    if ( NULL != self )
    {
        self -> src = stdin;
        parselib_initialize( self, data );
    }
    return self;
}

parselib * parselib_create_from_file( const char * filename, void * data )
{
    parselib * self = (parselib *)malloc( sizeof( * self ) );
    if ( NULL != self )
    {
        self -> src = fopen( filename, "r" );
        if ( NULL == self -> src )
        {
            free( self );
            self = NULL;
        }
        else
            parselib_initialize( self, data );
    }
    return self;
}

void parselib_register_event_callback( parselib * self, log_event_t on_event )
{
    if ( NULL != self )
        self -> on_event = on_event;
}

void parselib_register_error_callback( parselib * self, err_event_t on_error )
{
    if ( NULL != self )
        self -> on_error = on_error;
}

void parselib_destroy( parselib * self )
{
    if ( NULL != self )
    {
        if ( NULL != self -> src )
            fclose( self -> src );
        free( self );
    }
}

int parselib_run( parselib * self )
{
    int res = -1;
    if ( NULL != self )
    {
        yyscan_t sc;

        log1_lex_init( &sc );
        log1_set_in( self -> src, sc );
        res = log1_parse( sc, self );
        log1_lex_destroy( sc );
    }
    return res;
}

void parselib_event( parselib * self, t_event * event )
{
    if ( NULL != self )
    {
        if ( NULL != self -> on_event )
            self -> on_event( event, self -> data );
    }
}

void parselib_error( parselib *self, const char * msg )
{
    if ( NULL != self )
    {
        if ( NULL != self -> on_error )
            self -> on_error( msg, self -> data );
    }
}

static void print_tstr( const char * caption, const t_str * s )
{
    if ( s -> n > 0 )
        printf( "%s\t=%.*s\n", caption, s -> n, s -> p );
    else
        printf( "%s\t=none\n", caption );
}

void parselib_print_ev( t_event * ev )
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
        printf( "req-len\t=%d\n\n", ev -> req_len );
    }
}
