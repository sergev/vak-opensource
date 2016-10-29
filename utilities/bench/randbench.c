#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define N 100000000

static unsigned long next = 1;

short
rand15 (void)
{
	next = next * 1103515245L + 12345;
	return (short) (next >> 16) & 0x7fff;
}

void
srand15 (unsigned short seed)
{
	next = seed;
}

long getutime ()
{
	struct timeval tv;

	gettimeofday (&tv, 0);
	return tv.tv_sec * 1000000 + tv.tv_usec;
}

int main ()
{
	long t0, t1;
	int i, n, cycles;

	printf ("Testing 10 seconds...\n");
	cycles = 0;
	t0 = getutime ();

	do {
		for (i=0; i<N; ++i)
			n = rand15 ();
		cycles += N;

		t1 = getutime ();
	} while (t1 < t0 + 10000000);

	printf ("Total %.3g nanoseconds per cycle\n",
		(double) (t1 - t0) * 1000 / cycles);

	return 0;
}
