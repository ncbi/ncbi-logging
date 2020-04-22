#ifndef _h_g_parser_
#define _h_g_parser_

#include <stdbool.h>
#include <stdint.h>
#include "t_str.h"

struct g_parser;

struct g_parser * make_g_parser( void );
void destroy_g_parser( struct g_parser * self );

typedef struct
{
    t_str ip, dom, req, method, path, vers, acc, agnt;
    time_t unix_date;
    unsigned int res, num_bytes, port, req_len;
    double factor;
} t_log_data;

bool g_parse_log_data( struct g_parser * self, const char * buff, size_t buff_len, t_log_data * data );

void print_data_dbg( t_log_data * data );
void print_data_tsv( t_log_data * data );

#endif
