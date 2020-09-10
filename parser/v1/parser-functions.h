#ifndef _h_parser_functions_
#define _h_parser_functions_

#include <stdint.h>
#include <time.h>

#include "types.h"

uint8_t month_int( const t_str * s );
time_t get_unix_time( uint64_t day, uint8_t month, uint64_t year, uint64_t hour, uint64_t minute, uint64_t second );

#endif
