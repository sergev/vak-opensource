#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "config.h"
#include "ut.h"

#define TMPFNAME "/tmp/msXXXXXX"

char tmpf [] = TMPFNAME;
int debug;

/*
 * Scan line from specefied offset to find
 * string, beginning with "From ".
 * Return offset.
 */
long findfrom (FILE *fd, long off, char **fromptr)
{
	register c, boln, bof;
	register long ret;
	static char from [256];
	char *p;

	fseek (fd, off, 0);
	bof = 1;
	boln = 1;
	ret = off;
	*fromptr = "";
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
			    getc (fd) == 'm' && getc (fd) == ' ') {
				fgets (from, sizeof (from), fd);
				*fromptr = from;
				for (p=from; *p && *p!=' ' && *p!='\n' &&
					*p!='\r'; ++p);
				*p = 0;
				return (ret);
			}
			off = ret + 1;
			fseek (fd, off, 0);
		}
		boln = 0;
	}
}

/*
 * Get next free queue ident from ident file.
 * Increment ident in file.
 */
int getident ()
{
	FILE *fd;
	int ident;

	/* if queue file does not exist, create it */
	if (f2access (SERVDIR, "queueident", 0))
		f2clean (SERVDIR, "queueident");

	/* open queue file */
	fd = f2open (SERVDIR, "queueident", "r+");
	if (! fd) {
		syslog (LOG_ERR, "cannot open %s/queueident, aborted", SERVDIR);
		exit (-1);
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
void enqueue (char *name)
{
	FILE *fd;

	/* open active file for appending */
	fd = f2open (SERVDIR, "queueactive", "a");
	if (! fd) {
		syslog (LOG_ERR, "cannot open %s/queueactive, aborted",
			SERVDIR);
		exit (-1);
	}
	fprintf (fd, "%s\n", name);
	fclose (fd);
}

void runqueuer ()
{
	FILE *fd, *tfd;
	register long beg, next;
	int ident;
	char name [100];
	char from [256], *fromptr;

	fd = fopen (MAILBOX, "r");
	if (! fd) {
		syslog (LOG_ERR, "cannot read %s", MAILBOX);
		return;
	}
	beg = findfrom (fd, 0L, &fromptr);
	strcpy (from, fromptr);
	for (;;) {
		next = findfrom (fd, beg+1, &fromptr);
		if (next < 0)
			break;

		/* get next free queue ident */
		ident = getident ();

		/* make queue file name */
		sprintf (name, "%s/queue/q%d", SERVDIR, ident);

		/* create queue file */
		tfd = fopen (name, "w");
		if (! tfd) {
			syslog (LOG_ERR, "cannot open %s, aborted", name);
			exit (-1);
		}

		/* save query into queue file */
		fseek (fd, beg, 0);
		ffncopy (fd, tfd, next - beg);
		if (ferror (tfd)) {
			syslog (LOG_ERR, "error writing to %s, aborted", name);
			exit (-1);
		}
		fclose (tfd);

		/* append name of query to queue active file */
		enqueue (name);

		syslog (LOG_INFO, "queued %d from %s", ident, from);
		beg = next;
		strcpy (from, fromptr);
	}
	fclose (fd);
	if (close (creat (MAILBOX, 0600)) < 0) {
		syslog (LOG_ERR, "cannot rewrite %s, aborted", MAILBOX);
		exit (-1);
	}
}

/*
 * Get content of active file into temp file,
 * then clean active file.
 * Leave temp file rewinded.
 */
int dequeue (FILE *actfd)
{
	FILE *fd;

	/* open active file for reading */
	fd = f2open (SERVDIR, "queueactive", "r");
	if (! fd)
		return (1);     /* no active jobs in queue */

	/* copy active file to temp file */
	if (ffcopy (fd, actfd) < 0) {
		fclose (fd);
		return (0);
	}
	fclose (fd);
	rewind (actfd);

	/* clean active file */
	f2clean (SERVDIR, "queueactive");
	return (1);
}

/*
 * Process batch job.
 */
int batch (char *name)
{
	char buf [256];
	FILE *pd, *fd;

	if (f2access (SERVDIR, "bin/server", 1)) {
		syslog (LOG_ERR, "cannot execute %s/bin/server", SERVDIR);
		return (0);
	}
	sprintf (buf, "%s/bin/server", SERVDIR);
	pd = popen (buf, "w");
	if (! pd) {
		syslog (LOG_ERR, "cannot run '%s'", buf);
		return (0);
	}
	fd = fopen (name, "r");
	if (! fd) {
		syslog (LOG_ERR, "cannot read '%s'", name);
		pclose (pd);
		return (0);
	}
	ffcopy (fd, pd);
	fclose (fd);
	pclose (pd);
	sleep (1);
	return (1);
}

void runbatcher ()
{
	FILE *actf;
	char name [100], histname [100], *basename;

	/* open temp file on read/write */
	close (creat (tmpf, 0664));
	actf = fopen (tmpf, "r+");
	if (! actf) {
		syslog (LOG_ERR, "cannot create %s, aborted", tmpf);
		exit (-1);
	}
	unlink (tmpf);

	/* get queue active file into temp file */
	if (! dequeue (actf))
		exit (-1);

	/* process batches */
	while (getstr (actf, name)) {
		if (! *name)
			continue;
		if (! batch (name))
			continue;
		syslog (LOG_INFO, "processed %s", name);

		if ((basename = strrchr (name, '/')))
			++basename;
		else
			basename = name;
		strcpy (histname, SERVDIR);
		strcat (histname, "/history/");
		strcat (histname, basename);

		if (link (name, histname) < 0)
			syslog (LOG_ERR, "cannot link %s to %s",
				name, histname);
		unlink (name);
	}
	fclose (actf);
}

int main (int argc, char **argv)
{
	int pid;
	char qactivefile [256];
	char *progname = argv[0];

	openlog ("mdmn", 0, LOG_MAIL);
	umask (002);
	if (argc > 1 && !strcmp (argv[1], "-d")) {
		++debug;
		++argv;
		--argc;
	}
	if (argc > 2) {
		fprintf (stderr, "usage: %s [-d] [config-file]\n", progname);
		exit (-1);
	}
	if (! config (argv [1])) {
		fprintf (stderr, "cannot read config file\n");
		exit (-1);
	}

	if (! debug) {
		/* let's fork */
		pid = fork ();
		if (pid < 0) {
			fprintf (stderr, "%s: cannot fork\n", progname);
			exit (-1);
		}
		if (pid)
			exit (0);
		/* now we are a child */

		/* catch signals */
		sigign ();
	}
	/* make temp file name */
	mktemp (tmpf);

	syslog (LOG_INFO, "daemon started");

	sprintf (qactivefile, "%s/queueactive", SERVDIR);
	for (;;) {
		if (filesize (MAILBOX))
			runqueuer ();
		if (filesize (qactivefile))
			runbatcher ();
		sleep (DAEMONDELAY);
	}

	/* NOTREACHED */
}
