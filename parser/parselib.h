#ifndef __parse_lib_h
#define __parse_lib_h

#include <stdio.h>
#include <stdint.h>
#include "types.h"

typedef void ( *log_event_t )( t_event * event, void * data );

typedef struct
{
    FILE *src;
    log_event_t on_event;
    void *data;
} parselib;


parselib *parselib_create_from_stdin( void * data );
parselib *parselib_create_from_file( const char * filename, void * data );
void parselib_register_callback( parselib *lib, log_event_t on_event );
void parselib_destroy( parselib *lib );
void parselib_event( parselib *lib, t_event * event );
int parselib_run( parselib *lib );
void paselib_print_ev( t_event * ev );

#endif
