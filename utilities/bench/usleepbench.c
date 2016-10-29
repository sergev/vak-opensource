#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N	10

int t0, t1;

int getusec ()
{
	struct timeval tv;

	gettimeofday (&tv, 0);
	return tv.tv_sec * 1000000 + tv.tv_usec;
}

int main (int argc, char **argv)
{
	int msec, i;

	if (argc != 2) {
		fprintf (stderr, "Usage:\n");
		fprintf (stderr, "        timerbench msec\n");
		exit (1);
	}
	msec = strtol (argv[1], 0, 0);

	t0 = getusec ();
	for (i=0; i<N; i++)
		usleep (msec * 1000);
	t1 = getusec ();

	printf ("Measured interval is %.2f msec.\n",
		(t1 - t0) / 1000.0 / N);
	return 0;
}
