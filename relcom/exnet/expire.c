/*
 * Удаление лотов с истекшим сроком хранения.
 * Лоты, имя которых начинается с точки, хранятся дольше.
 *
 * Copyright (C) 1991 Сергей Вакуленко, <vak@kiae.su>.
 * All rights reserved.
 */

# include <stdio.h>
# include "config.h"
# include "lang.h"
# include "server.h"

# define TMPFNAME "/tmp/dmxXXXXXX"

char tmpf [] = TMPFNAME;

extern char *time2fname (), *mktemp (), *strcpy (), *strcat ();
extern long time ();

main (argc, argv)
char **argv;
{
	tzset ();
	if (argc > 2) {
		fprintf (stderr, "usage: %s [config-file]\n", argv [0]);
		exit (-1);
	}
	if (! config (argv [1])) {
		fprintf (stderr, "cannot read config file\n");
		exit (-1);
	}

	/* make temp file name */
	mktemp (tmpf);

	/* catch signals */
	sigcatch ();

	/* enter public directory */
	if (chdir (PUBLICDIR) < 0) {
		error ("cannot chdir to %s", PUBLICDIR);
		quit ();
	}

	expire ();

	return (0);
}

/*
 * Abnormal termination of program.
 */

quit ()
{
	unlink (tmpf);
	exit (-1);
}

/*
 * Output error message to log file.
 * VARARGS1
 */

error (s, a, b, c, d)
char *s, *a, *b, *c, *d;
{
	log (SERVER, s, a, b, c, d);
}

/*
 * Print error message.
 * VARARGS1
 */

message (s, a, b, c)
char *s, *a, *b, *c;
{
	printf (s, a, b, c);
	printf ("\n");
}

/*
 * Print fatal error message, copy rest of stdin to stdout,
 * then exit from program.
 * VARARGS1
 */

fatal (s, a, b, c)
char *s, *a, *b, *c;
{
	printf (s, a, b, c);
	printf ("\n");
	quit ();
}

/* VARARGS1 */

run (s, a, b, c, d, e, f)
char *s, *a, *b, *c, *d, *e, *f;
{
	char buf [1024];

	sprintf (buf, s, a, b, c, d, e, f);
	fflush (stdout);
	system (buf);
	fseek (stdout, 0L, 2);
}

sudomain (addr)
char *addr;
{
	int len;

	len = strlen (addr);
	if (len>=3 && !strcmp (addr+len-3, ".su"))
		return (1);
	return (0);
}

expire ()
{
	struct glist *glist;
	struct glistentry *p;
	struct lot *lot;
	char iname [20];
	long curtime;

	time (&curtime);
	checktime (curtime);
	glist = loadglist ();
	for (p=glist->list; p; p=p->next) {
		lot = loadlot (p->name);
		if (! lot)
			continue;
		if (p->name[0] == '.' && lot->t2 + DOTEXPIRETIME*60L*60 > curtime)
			continue;
		if (lot->t2 + EXPIRETIME*60L*60 > curtime)
			continue;

		run ("%s ../backup/%s %s.?", ARCHIVER,
			time2fname (time ((long *) 0)), p->name);

		strcpy (iname, p->name);
		strcat (iname, ".i");
		unlink (iname);
		strcpy (iname, p->name);
		strcat (iname, ".l");
		unlink (iname);
		strcpy (iname, p->name);
		strcat (iname, ".a");
		unlink (iname);
		strcpy (iname, p->name);
		strcat (iname, ".d");
		unlink (iname);
		notify (lot->host, p->name);

		error ("lot %s expired, host %s notified", p->name, lot->host);

		deleteglist (glist, p->name);
		freelot (lot);
	}
	saveglist (glist);
	freeglist (glist);
}

notify (address, lotname)
char *address, *lotname;
{
	FILE *fd;
	char buf [100];

	fd = fopen (tmpf, "w");
	if (! fd)
		fatal (cannot_create, tmpf);

	setlang (sudomain (address) ? 'r' : 'l');

	sprintf (buf, subject_lot_expired, lotname);

	fprintf (fd, "Subject: [Trade] %s\n", buf);
	fprintf (fd, "To: %s\n", address);
	fprintf (fd, "From: %s\n", MYADDRESS);
	fprintf (fd, "\n");
	fprintf (fd, your_lot_expired, lotname);
	fclose (fd);

	run ("%s %s < %s", MAILTOCMD, address, tmpf);
	sleep (1);

	unlink (tmpf);
}
