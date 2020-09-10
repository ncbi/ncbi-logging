#ifndef _h_types_
#define _h_types_

#include <stdint.h>
#include <stddef.h>

typedef struct t_str
{
    const char * p;
    size_t n;
    bool escaped;
} t_str;

#define EMPTY_TSTR(t) do { (t).p = NULL; (t).n = 0; (t).escaped = false; } while (false)

// only apply to consequitive t_str tokens!
#define MERGE_TSTR( t1, t2 ) do { (t1).n += (t2).n; (t1).escaped = (t1).escaped || (t2).escaped; } while (false)

typedef struct t_timepoint
{
    uint8_t day;
    uint8_t month;
    uint32_t year;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    int32_t offset;
} t_timepoint;

#endif
