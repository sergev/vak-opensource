# include <stdio.h>
# include "config.h"

# define TMPFNAME "/tmp/dmsXXXXXX"

char tmpf [] = TMPFNAME;

extern FILE *maketfile ();
extern long filesize ();

main ()
{
	int pid;

	/* check that we have infoserv uid */
	if (access (MAILSERVDIR, 7)) {
		fprintf (stderr, "infoservd: cannot access %s\n", MAILSERVDIR);
		exit (-1);
	}

	/* let's fork */
	pid = fork ();
	if (pid < 0) {
		fprintf (stderr, "infoservd: cannot fork\n");
		exit (-1);
	}
	if (pid)
		exit (0);

	/* now we are a child */
	error ("info server daemon started");

	/* ignore signals */
	sigign ();

	/* make temp file name */
	mktemp (tmpf);

	for (;;) {
		if (filesize (MAILBOX))
			runqueuer ();
		if (filesize (QACTIVEFILE))
			system (BATCHERNAME);
		sleep (SERVDELAY);
	}

	/* actually never returns */
	return (0);
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
 */

error (s, a, b, c, d)
char *s, *a, *b, *c, *d;
{
	log (DAEMON, s, a, b, c, d);
}

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

		run ("%s < %s", QUEUERNAME, tmpf);
		beg = next;
	}
	fclose (fd);
}
