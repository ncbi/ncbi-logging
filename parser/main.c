#include <stdio.h>
#include "parselib.h"

static void on_log_event( t_event * event, void * data )
{
    paselib_print_ev( event );
}

static int perform_parsing( parselib * lib )
{
    int res = 3;
    if ( NULL != lib )
    {
        parselib_register_callback( lib, on_log_event );
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
