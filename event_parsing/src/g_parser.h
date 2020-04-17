#ifndef _h_g_parser_
#define _h_g_parser_

#include <stdbool.h>
#include <stdint.h>
#include "t_str.h"

typedef enum e_parsed_kind
{
    epk_err,
    epk_str,
    epk_uint,
    epk_int,
    epk_float,
    epk_time,
    epk_req
} e_parsed_kind;

typedef struct t_request
{
    t_str method, path, vers;
} t_request;

typedef struct t_parsed
{
    e_parsed_kind kind;
    t_str str;
    uint64_t u;
    int64_t i;
    double f;
    t_request req;
} t_parsed;

/* =========================================================
 * src ... pointer to string to be parsed
 * fmt ... format-string for parsing
 * dst ... array of t_parsed structs for parse-results
 * dst_size ... number of t_parsed structs in dst
 * num_values ... how many values in dst have been parsed
 * 
 * fmt:
 * "%s - - [%t] \"%s\" %r %n %n %f \"%s\" \"%s\" \"%s\" %s %s"
 * 
 * %s ... a word ( terminated by following character )
 * %t ... a time-stamp ( human-readable: [01/Jan/2020:02:53:24 -0500] )
 * %u ... a unsigned int
 * %i ... a signed int
 * %f ... a float
 * %r ... a http(s) request
 * %% ... the '%' sign
 * 
 * returns the number of errors
  ========================================================= */
size_t g_parse( const t_str * src, const char * fmt, t_parsed * dst, size_t dst_size, size_t * num_values );

void print_parsed( const t_parsed * parsed, size_t num_parsed );

#endif
