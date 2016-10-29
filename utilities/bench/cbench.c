#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define N 50000000

long getutime ()
{
	struct timeval tv;

	gettimeofday (&tv, 0);
	return tv.tv_sec * 1000000 + tv.tv_usec;
}

int main ()
{
	long t0, t1;
	int i, n;

	printf ("Testing...\n");
	t0 = getutime ();

	for (i=0; i<N; ++i)
		n = rand ();

	t1 = getutime ();
	printf ("Time %ld msec (%.3g nsec per cycle)\n",
		(t1 - t0) / 1000, (double) (t1 - t0) * 1000 / N);
	printf ("Rate %.3g megacalls per second\n",
		N / (double) (t1 - t0));
	return 0;
}
