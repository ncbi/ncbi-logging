#ifndef _h_types_
#define _h_types_

typedef struct t_str
{
    const char * p;
    int n;
    bool escaped;
} t_str;

#define EMPTY_TSTR(t) do { (t).p = NULL; (t).n = 0; (t).escaped; } while (false)

// only apply to consequitive t_str tokens!
#define MERGE_TSTR( t1, t2 ) do { (t1).n += (t2).n; (t1).escaped = (t1).escaped || (t2).escaped; } while (false)

#endif
