# include <stdio.h>
# include "config.h"

FILE *actf;

extern FILE *maketfile ();

main ()
{
	char name [100];

	/* if batch is locked, do nothing */
	if (locked (BLOCKFILE))
		return (0);

	/* catch signals */
	sigcatch ();

	/* lock batch processing */
	lock (BLOCKFILE);

	/* lock queue */
	lock (QLOCKFILE);

	/* make temp file */
	actf = maketfile ();
	if (! actf)
		quit ();

	/* get queue active file into temp file */
	if (! dequeue ())
		quit ();

	/* unlock queue */
	unlock (QLOCKFILE);

	/* process batches */
	while (getstr (actf, name)) {
		if (! *name)
			continue;
		if (batch (name))
			error ("batch %s processed", name);
		unlink (name);
	}

	unlock (BLOCKFILE);
	return (0);
}

/*
 * Get content of active file into temp file,
 * then clean active file.
 * Leave temp file rewinded.
 */

dequeue ()
{
	FILE *fd;

	/* open active file for reading */
	fd = fopen (QACTIVEFILE, "r");
	if (! fd)
		return (1);     /* no active jobs in queue */

	/* copy active file to temp file */
	if (ffcopy (fd, actf) < 0) {
		fclose (fd);
		return (0);
	}
	fclose (fd);
	rewind (actf);

	/* clean activve file */
	close (creat (QACTIVEFILE, 0664));
	return (1);
}

/*
 * Process batch job.
 */

batch (name)
char *name;
{
	char buf [256];
	int rez;

	if (access (SERVERNAME, 1)) {
		error ("cannot execute %s", SERVERNAME);
		return (0);
	}
	sprintf (buf, "%s < %s", SERVERNAME, name);
	rez = system (buf);
	if (rez) {
		error ("error %d while executing '%s'", rez, buf);
		return (0);
	}
	return (1);
}

/*
 * Abnormal termination of program.
 */

quit ()
{
	unlock (QLOCKFILE);
	unlock (BLOCKFILE);
	unlock (LLOCKFILE);
	exit (-1);
}

/*
 * Output error message to log file.
 */

error (s, a, b, c, d)
char *s, *a, *b, *c, *d;
{
	log (BATCHER, s, a, b, c, d);
}
