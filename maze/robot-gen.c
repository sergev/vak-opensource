#include <sys/time.h>
#include "global.h"
#include "robot.h"

const char version[] = "1.0";
const char copyright[] = "Copyright (C) 2004 Serge Vakulenko";

char *progname;
int verbose;
int trace;
int debug;

extern char *optarg;
extern int optind;

void usage ()
{
	fprintf (stderr, "Robot Generator, Version %s\n%s\n", version, copyright);
	fprintf (stderr, "Usage:\n\t%s [-vtd] [-s seed] [-m max] neurons syns\n", progname);
	fprintf (stderr, "Options:\n");
	fprintf (stderr, "\t-v\tverbose mode\n");
	fprintf (stderr, "\t-t\ttrace mode\n");
	fprintf (stderr, "\t-d\tdebug\n");
	fprintf (stderr, "\t-s #\tseed for random generator (default random)\n");
	fprintf (stderr, "\t-m #\tmax factor (default 10)\n");
	fprintf (stderr, "\tneurons\tnumber of neurons\n");
	fprintf (stderr, "\tsyns\tnumber of synapses\n");
	exit (-1);
}

int main (int argc, char **argv)
{
	int nneurons, nsynapses, maxfactor = 10;
	unsigned int seed;
	struct timeval tv;
	robot_t *r;

	progname = "robot-gen";
	gettimeofday (&tv, 0);
	seed = tv.tv_sec + tv.tv_usec;
	for (;;) {
		switch (getopt (argc, argv, "vtds:m:")) {
		case EOF:
			break;
		case 'v':
			++verbose;
			continue;
		case 't':
			++trace;
			continue;
		case 'd':
			++debug;
			continue;
		case 's':
			seed = strtol (optarg, 0, 0);
			continue;
		case 'm':
			maxfactor = strtol (optarg, 0, 0);
			continue;
		default:
			usage ();
		}
		break;
	}
	argc -= optind;
	argv += optind;

	if (argc != 2)
		usage ();

	nneurons = strtol (argv[0], 0, 0);
	nsynapses = strtol (argv[1], 0, 0);
	srandom (seed);
	r = robot_init (nneurons, nsynapses, maxfactor);
	robot_save (r, stdout, 0);
	return (0);
}
