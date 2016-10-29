#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>

#define LIMIT 10000

const char version[] = "1.0";
const char copyright[] = "Copyright (C) 2005 Serge Vakulenko";

char *progname;
int verbose;

extern char *optarg;
extern int optind;

void pifagor ()
{
	long a, b, c;
	long long a2, b2, c2;

	for (a=3; a<LIMIT; ++a) {
		a2 = (long long) a * a;
		for (b=1; b<a; ++b) {
			b2 = (long long) b * b;
			c = lround (sqrt (a2 - b2));
			c2 = (long long) c * c;
			if (c2 + b2 == a2) {
				printf ("%ld\t%ld\t%ld\n", a, b, c);
				fflush (stdout);
			}
		}
	}
}

void usage ()
{
	fprintf (stderr, "Pifagor generator, Version %s, %s\n", version, copyright);
	fprintf (stderr, "Usage:\n\t%s [-vtd]\n", progname);
	fprintf (stderr, "Options:\n");
	fprintf (stderr, "\t-v\tverbose mode\n");
	exit (-1);
}

int main (int argc, char **argv)
{
	progname = *argv;
	for (;;) {
		switch (getopt (argc, argv, "v")) {
		case EOF:
			break;
		case 'v':
			++verbose;
			continue;
		default:
			usage ();
		}
		break;
	}
	argc -= optind;
	argv += optind;

	if (argc != 0)
		usage ();

	pifagor ();

	return (0);
}
