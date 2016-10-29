# include <stdio.h>
# include "config.h"

main ()
{
	int ident, rez;
	char name [100];

	/* catch signals */
	sigcatch ();

	/* lock queue */
	lock (QLOCKFILE);

	/* get next free queue ident */
	ident = getident ();

	/* make queue file name */
	sprintf (name, QFILENAME, ident);

	/* create queue file */
	if (! freopen (name, "w", stdout)) {
		error ("cannot create %s", name);
		quit ();
	}

	/* save query into queue file */
	rez = ffcopy (stdin, stdout);
	if (rez < 0)
		quit ();

	/* append name of query to queue active file */
	enqueue (name);

	error ("batch %d queued", ident);
	unlock (QLOCKFILE);
	return (0);
}

/*
 * Get next free queue ident from ident file.
 * Increment ident in file.
 */

getident ()
{
	FILE *fd;
	int ident;

	/* if queue file does not exist, create it */
	if (access (QIDENTFILE, 0))
		close (creat (QIDENTFILE, 0664));

	/* open queue file */
	fd = fopen (QIDENTFILE, "r+");
	if (! fd) {
		error ("cannot open %s", QIDENTFILE);
		quit ();
	}

	/* get next free ident from ident file */
	ident = 0;
	fscanf (fd, "%d", &ident);

	/* increment ident */
	rewind (fd);
	fprintf (fd, "%d          \n", ident + 1);
	fclose (fd);

	return (ident);
}

/*
 * Append batch file name to atcive queue file.
 */

enqueue (name)
char *name;
{
	FILE *fd;

	/* open active file for appending */
	fd = fopen (QACTIVEFILE, "a");
	if (! fd) {
		error ("cannot open %s", QACTIVEFILE);
		quit ();
	}
	fprintf (fd, "%s\n", name);
	fclose (fd);
}

/*
 * Abnormal termination of program.
 */

quit ()
{
	unlock (QLOCKFILE);
	unlock (LLOCKFILE);
	exit (-1);
}

/*
 * Output error message to log file.
 */

error (s, a, b, c, d)
char *s, *a, *b, *c, *d;
{
	log (ENQUEUE, s, a, b, c, d);
}
