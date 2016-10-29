/*
 * Постановка заявки в очередь.  Запускается демоном сервера.
 * Заявка подается на стандартный ввод.
 * Переписывает ее в соответствующий каталог.
 *
 * Copyright (C) 1991 Сергей Вакуленко.
 * All rights reserved.
 */

# include <stdio.h>
# include "config.h"

extern FILE *f2open ();

main (argc, argv)
char **argv;
{
	int ident, rez;
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

	/* get next free queue ident */
	ident = getident ();

	/* make queue file name */
	sprintf (name, "%s/queue/q%d", SERVDIR, ident);

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
	if (f2access (SERVDIR, "queueident", 0))
		f2clean (SERVDIR, "queueident");

	/* open queue file */
	fd = f2open (SERVDIR, "queueident", "r+");
	if (! fd) {
		error ("cannot open %s/queueident", SERVDIR);
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
	char qactivefile [256];

	/* open active file for appending */
	sprintf (qactivefile, "%s/queueactive", SERVDIR);
	fd = fopen (qactivefile, "a");
	if (! fd) {
		error ("cannot open %s", qactivefile);
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
	exit (-1);
}

/*
 * Output error message to log file.
 * VARARGS1
 */

error (s, a, b, c, d)
char *s, *a, *b, *c, *d;
{
	log (ENQUEUE, s, a, b, c, d);
}
