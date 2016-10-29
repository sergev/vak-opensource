/*
 * Демон почтового сервера.
 * Постоянно крутится и читает почтовый ящик, принадлежащий серверу.
 * Приходящая почта раздраконивается на отдельные письма,
 * и на каждое письмо вызывается программа enqueue, которая ставит
 * их в очередь.
 * Затем, если очередь непуста, вызывает обработчик заявок batcher.
 * Таким образом, обработка заявок происходит последовательно,
 * друг за другом. Это облегчает синхронизацию и уменьшает
 * пиковую нагрузку на процессор.
 *
 * Copyright (C) 1991 Сергей Вакуленко.
 * All rights reserved.
 */

# include <stdio.h>
# include "config.h"

# define TMPFNAME "/tmp/xmsXXXXXX"

char tmpf [] = TMPFNAME;

extern FILE *maketfile ();
extern long filesize (), time ();
extern char *mktemp ();

main (argc, argv)
char **argv;
{
	int pid;
	long lastexpire = 0;
	long curtime;
	char qactivefile [256];

	tzset ();
	if (argc > 2) {
		fprintf (stderr, "usage: %s [config-file]\n", argv [0]);
		exit (-1);
	}
	if (! config (argv [1])) {
		fprintf (stderr, "cannot read config file\n");
		exit (-1);
	}

	/* check that we have trade uid */
	if (access (SERVDIR, 7)) {
		fprintf (stderr, "%s: cannot access %s\n", argv [0], SERVDIR);
		exit (-1);
	}

	/* let's fork */
	pid = fork ();
	if (pid < 0) {
		fprintf (stderr, "%s: cannot fork\n", argv [0]);
		exit (-1);
	}
	if (pid)
		exit (0);
	/* now we are a child */

	/* set file creation mode mask,
	/* so that all created files could not
	/* be read by others */
	umask (077);

	/* make temp file name */
	mktemp (tmpf);

	/* catch signals */
	sigcatch ();

	error ("daemon started");
	sprintf (qactivefile, "%s/queueactive", SERVDIR);
	for (;;) {
		if (filesize (MAILBOX))
			runqueuer ();
		if (filesize (qactivefile))
			run ("%s/bin/batcher %s", SERVDIR, CONFIGFILE);
		curtime = time ((long *) 0);
		checktime (curtime);
		if (! lastexpire || lastexpire + EXPIRETIME*60L*60 < curtime) {
			run ("%s/bin/expire %s", SERVDIR, CONFIGFILE);
			time (&lastexpire);
		}
		sleep (DAEMONDELAY);
	}

	/* NOTREACHED */
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
	log (DAEMON, s, a, b, c, d);
}

/* VARARGS1 */

run (s, a, b, c, d)
char *s, *a, *b, *c, *d;
{
	char buf [1024];

	sprintf (buf, s, a, b, c, d);
	system (buf);
}

/*
 * Scan line from specefied offset to find
 * string, beginning with "From ".
 * Return offset.
 */

long findfrom (fd, off)
FILE *fd;
long off;
{
	register c, boln, bof;
	register long ret;

	fseek (fd, off, 0);
	bof = 1;
	boln = 1;
	ret = off;
	for (;;) {
		c = getc (fd);
		if (c < 0)
			return (bof ? -1 : off);
		bof = 0;
		++off;
		if (c == '\n') {
			boln = 1;
			ret = off;
			continue;
		}
		if (boln && c=='F') {
			if (getc (fd) == 'r' && getc (fd) == 'o' &&
			    getc (fd) == 'm' && getc (fd) == ' ')
				return (ret);
			off = ret + 1;
			fseek (fd, off, 0);
		}
		boln = 0;
	}
}

runqueuer ()
{
	FILE *fd, *tfd;
	register long beg, next;

	fd = fopen (MAILBOX, "r");
	if (! fd) {
		error ("cannot read %s", MAILBOX);
		return;
	}
	if (unlink (MAILBOX) < 0) {
		error ("cannot unlink %s", MAILBOX);
		return;
	}
	beg = findfrom (fd, 0L);
	for (;;) {
		next = findfrom (fd, beg+1);
		if (next < 0)
			break;

		tfd = fopen (tmpf, "w");
		if (! tfd) {
			error ("cannot open %s", tmpf);
			quit ();
		}

		fseek (fd, beg, 0);
		ffncopy (fd, tfd, next - beg);
		fclose (tfd);

		run ("%s/bin/enqueue %s < %s", SERVDIR, CONFIGFILE, tmpf);
		beg = next;
	}
	fclose (fd);
}
