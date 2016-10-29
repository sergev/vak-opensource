# include <stdio.h>
# include <signal.h>
# include <sys/types.h>
# include <sys/stat.h>
# include "config.h"

# define TFILENAME      "/tmp/msbXXXXXX"
# define BSZ            512

extern char *ctime ();

/*
 * Open temp file on read/write.
 */

FILE *maketfile ()
{
	static char tmpfname [] = TFILENAME;
	static char *tmp;
	FILE *fd;

	if (! tmp)
		mktemp (tmp = tmpfname);
	close (creat (tmp, 0664));
	fd = fopen (tmp, "r+");
	if (! fd) {
		error ("cannot create %s", tmp);
		return (0);
	}
	unlink (tmp);
	return (fd);
}

/*
 * Get string from file.
 * Return 1 if ok, else 0.
 */

getstr (fd, s)
FILE *fd;
register char *s;
{
	register c;

	while ((c = getc (fd)) >= 0 && c!='\n')
		*s++ = c;
	*s = 0;
	return (c >= 0);
}

/*
 * Copy file to file.
 * Return 0 on success,
 * -1 on read error,
 * -2 on write error.
 */

ffcopy (from, to)
FILE *from, *to;
{
	char buf [BSZ], *p;
	register n, k;

	for (;;) {
		n = fread (buf, 1, BSZ, from);
		if (n < 0) {
			error ("read error");
			return (-1);
		}
		if (n == 0)
			break;
		p = buf;
		while (n > 0) {
			k = fwrite (p, 1, n, to);
			if (k <= 0) {
				error ("write error");
				return (-2);
			}
			n -= k;
			p += k;
		}
	}
	return (0);
}

/*
 * Copy len bytes from file to file.
 * Return 0 on success,
 * -1 on read error,
 * -2 on write error.
 */

ffncopy (from, to, len)
FILE *from, *to;
long len;
{
	char buf [BSZ], *p;
	register n, k;

	while (len > 0) {
		n = fread (buf, 1, BSZ, from);
		if (n < 0) {
			error ("read error");
			return (-1);
		}
		if (n == 0)
			break;
		if (n > len)
			n = len;
		len -= n;
		p = buf;
		while (n > 0) {
			k = fwrite (p, 1, n, to);
			if (k <= 0) {
				error ("write error");
				return (-2);
			}
			n -= k;
			p += k;
		}
	}
	return (0);
}

log (progname, s, a, b, c, d)
char *progname, *s, *a, *b, *c, *d;
{
	FILE *fd;
	char *p;
	long t;

	/* lock log file */
	lock (LLOCKFILE);

	/* open log file for appending */
	fd = fopen (LOGFILE, "a");
	if (! fd) {
		unlock (LLOCKFILE);
		return (-1);
	}
	/* get time of day string */
	time (&t);
	p = ctime (&t);
	p [24] = 0;            /* remove trailing '\n' */
	fprintf (fd, "%s  %-9s", p, progname);
	fprintf (fd, s, a, b, c, d);
	fprintf (fd, "\n");
	fclose (fd);

	/* lock log file */
	unlock (LLOCKFILE);
}

/*
 * Wait until queue is unlocked,
 * then lock it by creating lock file.
 */

lock (name)
char *name;
{
	/* wait while queue is locked */
	while (! access (name, 0))
		sleep (1);

	/* lock queue */
	if (creat (name, 0) < 0) {
		error ("cannot create %s", name);
		exit (-1);
	}
}

/*
 * Return 1 if file is locked.
 */

locked (name)
char *name;
{
	return (! access (name, 0));
}

/*
 * Unlock queue by deleting lock file
 */

unlock (name)
char *name;
{
	unlink (name);
}

/*
 * Signal catched, unlock and exit.
 */

catcher ()
{
	error ("signal catched");
	quit ();
}

/*
 * Set cather of signal, if it is not ignored.
 */

catch (sig, fun)
int (*fun) ();
{
	if (signal (sig, SIG_IGN) != SIG_IGN)
		signal (sig, fun);
}

/*
 * Set up signal catcher.
 */

sigcatch ()
{
	catch (SIGINT, catcher);
	catch (SIGQUIT, catcher);
	catch (SIGTERM, catcher);
}

/*
 * Ignore signals.
 */

sigign ()
{
	signal (SIGINT, SIG_IGN);
	signal (SIGQUIT, SIG_IGN);
	signal (SIGTERM, SIG_IGN);
	signal (SIGTSTP, SIG_IGN);
}

long filesize (name)
char *name;
{
	struct stat x;

	if (stat (name, &x) < 0)
		return (0);
	return (x.st_size);
}

char *strlower (str)
char *str;
{
	register char *p;

	for (p=str; *p; ++p) {
		if (*p>='A' && *p<='Z')
			*p += 'a' - 'A';
		if (*p>=(char)0340 && *p<=(char)0376)
			*p -= 040;
	}
	return (str);
}
