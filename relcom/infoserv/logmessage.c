# include <stdio.h>
# include "config.h"

main (argc, argv)
char **argv;
{
	if (argc != 3) {
		fprintf (stderr, "Usage: logmessage progname message\n");
		exit (1);
	}

	/* catch signals */
	sigcatch ();
	log (argv [1], argv [2]);
	return (0);
}

/*
 * Abnormal termination of program.
 */

quit ()
{
	unlock (LLOCKFILE);
	exit (-1);
}

/*
 * Output error message to log file.
 */

error (s, a, b, c, d)
char *s, *a, *b, *c, *d;
{
	log (LOGMSG, s, a, b, c, d);
}
