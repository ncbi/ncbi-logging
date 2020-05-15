#ifndef __parse_lib_h
#define __parse_lib_h

#include <stdio.h>
#include <stdint.h>
#include "types.h"

typedef void ( *log_event_t )( t_event * event, void * data );
typedef void ( *err_event_t )( const char* msg, void * data );

typedef struct
{
    FILE * src;
    log_event_t on_event;
    err_event_t on_error;
    void * data;
} parselib;


parselib *parselib_create_from_stdin( void * data );
parselib *parselib_create_from_file( const char * filename, void * data );
void parselib_destroy( parselib * self );

void parselib_register_event_callback( parselib * self, log_event_t on_event );
void parselib_register_error_callback( parselib * self, err_event_t on_event );

void parselib_event( parselib * self, t_event * event );
void parselib_error( parselib * self, const char * msg );

int parselib_run( parselib * self );

void parselib_print_ev( t_event * ev );

#endif
