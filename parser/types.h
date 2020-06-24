#ifndef _h_types_
#define _h_types_

typedef struct t_str
{
    const char * p;
    int n;
    bool escaped;
} t_str;

#define EMPTY_TSTR(t) do { t.p = NULL; t.n = 0; t.escaped; } while (false)

#endif
