/*
 * Обработчик очереди заявок.
 * Берет заявки из очереди и на каждую запускает собственно сервер.
 *
 * Copyright (C) 1991 Сергей Вакуленко.
 * All rights reserved.
 */

# include <stdio.h>
# include "config.h"

FILE *actf;

extern FILE *maketfile (), *f2open ();

main (argc, argv)
char **argv;
{
	char name [100];

	tzset ();
	umask (077);
	if (argc > 2) {
		fprintf (stderr, "usage: %s [config-file]\n", argv [0]);
		exit (-1);
	}
	if (! config (argv [1])) {
		fprintf (stderr, "cannot read config file\n");
		exit (-1);
	}

	/* catch signals */
	sigcatch ();

	/* make temp file */
	actf = maketfile ();
	if (! actf)
		quit ();

	/* get queue active file into temp file */
	if (! dequeue ())
		quit ();

	/* process batches */
	while (getstr (actf, name)) {
		if (! *name)
			continue;
		if (batch (name))
			error ("batch %s processed", name);
		unlink (name);
	}

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
	fd = f2open (SERVDIR, "queueactive", "r");
	if (! fd)
		return (1);     /* no active jobs in queue */

	/* copy active file to temp file */
	if (ffcopy (fd, actf) < 0) {
		fclose (fd);
		return (0);
	}
	fclose (fd);
	rewind (actf);

	/* clean active file */
	f2clean (SERVDIR, "queueactive");
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

	if (f2access (SERVDIR, "bin/server", 1)) {
		error ("cannot execute %s/bin/server", SERVDIR);
		return (0);
	}
	sprintf (buf, "%s/bin/server %s < %s", SERVDIR, CONFIGFILE, name);
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
	exit (-1);
}

/*
 * Output error message to log file.
 * VARARGS1
 */

error (s, a, b, c, d)
char *s, *a, *b, *c, *d;
{
	log (BATCHER, s, a, b, c, d);
}
