#include <sys/time.h>

void get_time__ (double *sec)
{
    struct timeval tv;

    gettimeofday (&tv, 0);
    *sec = tv.tv_usec / 1000000.0 + tv.tv_sec;
}
