#include <stdio.h>
#include "parselib.h"

static void on_log_event( t_event * event, void * data )
{
    parselib_print_ev( event );
}

static void on_err_event( const char * msg, void * data )
{
    printf( "err: >%s<\n\n", msg );
}

static int perform_parsing( parselib * lib )
{
    int res = 3;
    if ( NULL != lib )
    {
        parselib_register_event_callback( lib, on_log_event );
        parselib_register_error_callback( lib, on_err_event );
        res = parselib_run( lib );
        parselib_destroy( lib );
    }
    return res;
}

int main( int argc, char **argv )
{
    int res = 0;
    
    if ( argc == 1 )
        res = perform_parsing( parselib_create_from_stdin( NULL ) );
    else
    {
        for ( int i = 1; 0 == res && i < argc; i++ )
            res = perform_parsing( parselib_create_from_file( argv[ i ], NULL ) );
    }
    return res;
}
