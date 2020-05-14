#ifndef _h_types_
#define _h_types_

#include <time.h>

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

typedef struct t_request
{
    t_str method;
    t_str path;
    t_str params;
    t_str vers;
} t_request;

typedef struct t_event
{
    t_str ip, user, server, req_time, referer, agent, forwarded;
    t_timepoint t;
    t_request req;
    int64_t res_code, res_len, port, req_len;
} t_event;

#endif
