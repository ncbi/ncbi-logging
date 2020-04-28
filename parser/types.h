#ifndef _h_types_
#define _h_types_

typedef struct t_str
{
    char * p;
    int n;
} t_str;

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
