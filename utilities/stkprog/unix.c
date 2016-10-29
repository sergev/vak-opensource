#ifndef WIN32
#   include <sys/time.h>
#else
#   include <time.h>
#endif

void *fix_time ()
{
	static struct timeval t0;

	gettimeofday (&t0, 0);
	return &t0;
}

unsigned long seconds_elapsed (void *arg)
{
	struct timeval t1, *t0 = arg;
	unsigned long seconds;

	gettimeofday (&t1, 0);
	seconds = t1.tv_sec - t0->tv_sec;
	if (seconds < 1)
		seconds = 1;
	return seconds;
}
