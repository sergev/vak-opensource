/*
 * Cronyx archive indexer.
 *
 * Copyright (C) 1994-1997 Cronyx Ltd.
 * Author: Serge Vakulenko, <vak@cronyx.ru>
 * Date: Thu Mar 17 18:57:24 MSK 1994
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>

#define LINESZ 512
#define MAXEXLEN 256
#define ISDIGIT(c)	((c) >= '0' && (c) <= '9')

char *progname;
char *prefix;
int verbose;
int debug;
int offset;

#ifdef ONEPASS
#include "vdbm.h"
VDBM *history;
#endif

char **stack;
int stack_len;
int stack_pos;

const char version[] = "1.0";
const char copyright[] = "Copyright (C) 1994 Cronyx Ltd.";

extern char *optarg;
extern int optind;

int kindex (char *dir);
void scan (char *dir);
void stack_free (void);
void stack_push (char *str, struct stat *st);
char *stack_pop (void);
int fgetinfo (char *info, int maxlen, FILE *fd);

char *exlist[MAXEXLEN];
int exlen;

void quit ()
{
	exit (-1);
}

void usage ()
{
	fprintf (stderr, "KINDEX - file archive index builder, version %s\n",
		version);
	fprintf (stderr, "%s\n\n", copyright);
	fprintf (stderr, "Usage:\n\t%s [-v] [-D] [-d prefix] [-x exclude] [dirname...]\n", progname);
	fprintf (stderr, "Options:\n");
	fprintf (stderr, "\t-v\tverbose mode\n");
	fprintf (stderr, "\t-D\tdebug mode\n");
	fprintf (stderr, "\t-d #\tthe directory name prefix to remove\n");
	fprintf (stderr, "\t-x #\tdon't follow the symlink given\n");
	quit ();
}

int main (int argc, char **argv)
{
	progname = strrchr (*argv, '/');
	if (! progname)
		progname = *argv;
	for (;;) {
		switch (getopt (argc, argv, "vDd:x:")) {
		case EOF:
			break;
		case 'v':
			++verbose;
			continue;
		case 'D':
			++debug;
			continue;
		case 'd':
			prefix = optarg;
			continue;
		case 'x':
			if (exlen >= MAXEXLEN) {
				fprintf (stderr, "%s: too many exclusions\n", progname);
				quit ();
			}
			exlist[exlen++] = optarg;
			continue;
		}
		break;
	}
	argc -= optind;
	argv += optind;

	if (!argc || **argv=='-')
		usage ();

	for (; argc>0; --argc, ++argv)
		kindex (argv[0]);

	return (0);
}

int runsort (char *dir)
{
	char idx [LINESZ];
	int pd[2], pid;

	strcpy (idx, dir);
	strcat (idx, "/.index");
	if (freopen (idx, "w", stdout) != stdout) {
		fprintf (stderr, "%s: cannot reopen stdout to %s\n",
			progname, idx);
		return (-1);
	}

	if (pipe (pd) < 0) {
		perror ("creating pipe");
		return (-1);
	}

	pid = vfork ();
	if (pid == -1) {
		perror ("creating process");
		return (-1);
	}

	if (pid == 0) {
		/* child */
		close (0);
		dup2 (pd[0], 0);
		close (pd[0]);
		close (pd[1]);
		execl ("/usr/bin/sort", "sort", "-d", "-f", (char*)0);
		execl ("/bin/sort", "sort", "-d", "-f", (char*)0);
		fprintf (stderr, "%s: cannot run sort\n", progname);
		_exit (-1);
	}

	close (1);
	dup2 (pd[1], 1);
	close (pd[0]);
	close (pd[1]);

	return (0);
}

int kindex (char *dir)
{
	if (runsort (dir) < 0)
		return (-1);

	stack_free ();
	stack_push (dir, 0);
	offset = 1 + strlen (dir);
	for (;;) {
		dir = stack_pop ();
		if (! dir)
			break;
		scan (dir);
		free (dir);
	}

	fclose (stdout);
	return (0);
}

int exclude (char *dir)
{
	char **p;

	for (p=exlist; p<exlist+exlen; ++p)
		if (strncmp (dir, *p, strlen (*p)) == 0)
			return (1);
	return (0);
}

void scan (char *dir)
{
	DIR *d;
	FILE *fd;
	struct dirent *e;
	char path [LINESZ], info [LINESZ], *iptr;
	struct stat st;
	int dirflag;
	struct tm *ptm;
	unsigned long mod;

	d = opendir (dir);
	if (! d) {
		perror (dir);
		return;
	}
	while ((e = readdir (d)) != 0) {
		if (e->d_name[0] == '.')
			continue;
		strcpy (path, dir);
		strcat (path, "/");
		strcat (path, e->d_name);
		if (lstat (path, &st) < 0)
			continue;
		dirflag = 0;
		switch (st.st_mode & S_IFMT) {
		case S_IFDIR:
			if (exclude (dir))
				continue;
			stack_push (path, &st);
			dirflag = 1;
			break;
		case S_IFLNK:
			if (exclude (dir))
				continue;
			if (stat (path, &st) < 0)
				continue;
			if ((st.st_mode & S_IFMT) == S_IFDIR) {
				stack_push (path, &st);
				dirflag = 2;
			}
			break;
		case S_IFREG:
			break;
		default:
			continue;
		}
		sprintf (info, "%s/.info/%s", dir, e->d_name);
		fd = fopen (info, "r");
		iptr = info;
		*info = 0;
		ptm = localtime (&st.st_mtime);
		mod = ptm->tm_year * 10000 + (ptm->tm_mon + 1) * 100 +
			ptm->tm_mday;
		if (fd) {
			if (fgetinfo (info, sizeof (info), fd)) {
				if (ISDIGIT (info[0]) && ISDIGIT (info[1]) &&
				    ISDIGIT (info[2]) && ISDIGIT (info[3]) &&
				    ISDIGIT (info[4]) && ISDIGIT (info[5])) {
					mod = atol (info);
					iptr = info + 6;
					while (*iptr==' ' || *iptr=='\t')
						++iptr;
				}
			} else
				*info = 0;
			fclose (fd);
		}

		if (debug) {
			fputs (path+offset, stderr);
			if (dirflag) {
				putc ('/', stderr);
				if (dirflag > 1)
					putc ('/', stderr);
			} else
				fprintf (stderr, " %06ld", mod);
			if (*iptr) {
				putc (' ', stderr);
				fputs (iptr, stderr);
			}
			putc ('\n', stderr);
		}
		if (prefix)
			fputs (prefix, stdout);
		fputs (path+offset, stdout);
		if (dirflag) {
			putchar ('/');
			if (dirflag > 1)
				putchar ('/');
		} else
			printf (" %06ld", mod);
		if (*iptr) {
			putchar (' ');
			fputs (iptr, stdout);
		}
		putchar ('\n');
		if (dirflag && verbose) {
			fputs (path+offset, stderr);
			if (dirflag)
				putc ('/', stderr);
			if (*iptr) {
				fputs (" -- ", stderr);
				fputs (iptr, stderr);
			}
			putc ('\n', stderr);
		}
	}
	closedir (d);
}

void stack_free ()
{
	int i;

#ifdef ONEPASS
	if (history)
		vdbm_close (history);
	history = vdbm_open (32);
#endif
	for (i=0; i<stack_pos; ++i)
		free (stack[i]);
	stack_pos = 0;
}

void stack_push (char *str, struct stat *st)
{
#ifdef ONEPASS
	vdatum key, data;
	struct stat stbuf;
	unsigned long buf[2];

	if (! st) {
		stat (str, &stbuf);
		st = &stbuf;
	}

	/*
	 * Try to find the directory in the history.
	 * Ignore it if found.
	 */
	buf[0] = st->st_dev;
	buf[1] = st->st_ino;
	key.dptr = (void*) buf;
	key.dsize = sizeof (buf);
	if ((st->st_mode & S_IFMT) == S_IFDIR) {
		data = vdbm_fetch (history, key);
		if (data.dsize)
			return;
	}

	/*
	 * Add new directory name to the history.
	 */
	data.dptr = "";
	data.dsize = 1;
	vdbm_store (history, key, data, 0);
#endif
	if (stack_len <= stack_pos) {
		if (stack_len == 0) {
			stack_len = 128;
			stack = malloc (stack_len * sizeof (char*));
		} else {
			stack_len += 128;
			stack = realloc (stack, stack_len * sizeof (char*));
		}
		if (! stack) {
			fprintf (stderr, "%s: out of memory\n", progname);
			quit ();
		}
	}
	stack[stack_pos++] = strdup (str);
}

char *stack_pop ()
{
	if (stack_pos <= 0)
		return (0);
	return (stack[--stack_pos]);
}

/*
 * Get multistring from the file.
 * The continuation lines are indented by spaces.
 */
int fgetinfo (char *info, int maxlen, FILE *fd)
{
	char *p = info;
	int n;

	while (maxlen > 0) {
		if (! fgets (p, maxlen, fd))
			break;
		n = strlen (p);
		maxlen -= n;
		p += n;

		while (p > info && (p[-1] == '\n' || p[-1] == '\15')) {
			*--p = 0;
			++maxlen;
		}

		n = getc (fd);
		if (n != ' ' && n != '\t') {
			ungetc (n, fd);
			break;
		}
		*p++ = ' ';
		--maxlen;
		do {
			n = getc (fd);
		} while (n == ' ' || n == '\t');
		ungetc (n, fd);
	}
	while (p > info && (p[-1] == '\n' || p[-1] == '\15')) {
		*--p = 0;
		++maxlen;
	}
	return (p - info);
}
