#ifndef _h_log_event_
#define _h_log_event_

#include <time.h>
#include <stdbool.h>
#include "t_str.h"

typedef struct
{
    t_str ip, dom, acc, res, agnt;
    time_t unix_date;
    unsigned int num_bytes;
} t_log_event;

struct event_extractor;

struct event_extractor * make_event_extractor( void );
void destroy_event_extractor( struct event_extractor * self );

bool extract_event( struct event_extractor * self,
                    const char * buffer,
                    size_t buffer_len,
                    t_log_event * event );

#endif
