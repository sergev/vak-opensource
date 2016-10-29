/*
 * Cronyx archive search utility.
 *
 * Copyright (C) 1994-1997 Cronyx Ltd.
 * Author: Serge Vakulenko, <vak@cronyx.ru>
 * Date: Thu Mar 17 15:43:18 MSK 1994
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "reg.h"

#ifndef ROOTDIR
#define ROOTDIR         "/pub"          /* default root directory */
#endif
#define LINESZ          512             /* maximum input line length */

#define ISSPACE(c)	((c) == ' ' || (c) == '\t')
#define ISDIGIT(c)	((c) >= '0' && (c) <= '9')

char *rootdir = ROOTDIR;
char *progname;
int verbose;
int debug;
int rflag;              /* add leading '/' to file names */
int mflag;              /* print as mailserv commands */

const char version[] = "1.1";
const char copyright[] = "Copyright (C) 1994 Cronyx Ltd.";

extern char *optarg;
extern int optind;
extern int errno;

unsigned char lower (unsigned char c)
{
	if (c>='A' && c<='Z')
		return (c + 'a' - 'A');
	if (c>=0340 && c<=0377)
		return (c - 040);
	if (c==0263)
		return (0243);
	return (c);
}

char *strlower (char *str)
{ 
	char *p;

	for (p=str; *p; ++p)
		*p = lower (*p);
	return (str);
}

void putfmt (char *s, int n)
{
	int c;
	char *p;

	while (*s==' ' || *s=='\t')             /* skip leading blanks */
		++s;
	for (p=s; *p; ++p)
		if (*p == '\t')                 /* convert tabs to spaces */
			*p = ' ';
	for (--p; p>=s && *p==' '; --p)         /* remove trailing blanks */
		*p = 0;
	while (strlen (s) > n) {
		c = s [n];
		s [n] = 0;
		p = strrchr (s, ' ');
		s [n] = c;
		if (! p)
			p = s + n;

		c = *p;
		*p = 0;
		puts (s);
		*p = c;
		printf ("\t");
		n = 70;

		s = p;
		while (*s == ' ')
			++s;
	}
	puts (s);
}

/*
 * Perform the search in the directory index file.
 * The index file contains one line per each file.
 * The format is simple:
 *     <file path> description
 */
void find (char *pattern)
{
	char info [2*LINESZ], descr [LINESZ], file [LINESZ];
	char *path, *p, *f, *d;
	struct stat st;
	REGEXP reg;
	FILE *fd;
	long size;
	int i;
	struct tm *ptm;
	unsigned long mod;

	strlower (pattern);
	if (verbose)
		printf ("Searching `%s'...\n", pattern);

	reg = REGCOMP (pattern);
	if (! reg) {
		printf ("Invalid regular expression `%s'\n", pattern);
		exit (-1);
	}

	strcpy (info, rootdir);
	strcat (info, "/.index");
	fd = fopen (info, "r");
	if (! fd) {
		printf ("No search database available\n");
		exit (-1);
	}

	i = 0;
	while (fgets (info, sizeof (info), fd)) {
		p = info;
		if (! *p)
			continue;

		/* Strip trailing spaces */
		p += strlen (info);
		while (--p>info && (*p==' ' || *p=='\t' || *p=='\r' || *p=='\n'))
			*p = 0;

		p = info;
		while (ISSPACE (*p)) ++p;

		path = p;
		while (*p && ! ISSPACE (*p)) ++p;
		if (*p)
			*p++ = 0;
		while (ISSPACE (*p)) ++p;

		if (ISDIGIT (p[0]) && ISDIGIT (p[1]) &&
		    ISDIGIT (p[2]) && ISDIGIT (p[3]) &&
		    ISDIGIT (p[4]) && ISDIGIT (p[5])) {
			mod = atol (p);
			p += 6;
			while (ISSPACE (*p)) ++p;
		} else
			mod = 0;

		d = p;

		f = strrchr (path, '/');
		if (! f)                /* base file name */
			f = path;
		else if (! f[1]) {      /* directory */
			while (f>path && f[-1]=='/')
				*f-- = 0;
			while (f>path && f[-1]!='/')
				--f;
		} else                  /* file path */
			++f;
		
		strcpy (file, f);
		strcpy (descr, d);
		strlower (file);
		strlower (descr);
		if (!REGEXEC (reg, file) &&
		    (!*descr || !REGEXEC (reg, descr)))
			continue;

		strcpy (file, rootdir);
		strcat (file, "/");
		strcat (file, path);
		if (stat (file, &st) < 0)
			continue;
		if ((st.st_mode & S_IFMT) == S_IFREG)
			size = st.st_size;
		else
			size = -1;

		if (mflag)
			printf ("GET ");
		if (rflag)
			putchar ('/');
		printf ("%s", path);
		if (size >= 0) {
			if (! mod) {
				ptm = localtime (&st.st_mtime);
				mod = ptm->tm_year * 10000 +
					(ptm->tm_mon + 1) * 100 +
					ptm->tm_mday;
			}
			printf (" -- %ld bytes, %ld", size, mod);
		}
		printf ("\n");
		if (*d) {
			printf ("\t");
			putfmt (d, 70);
		}
		if (++i >= 500) {
			printf ("Too many matches -- search terminated\n");
			break;
		}
	}
	fclose (fd);

	if (! i)
		printf ("No matches found for `%s'\n", pattern);
}

void usage ()
{
	fprintf (stderr, "LFIND - file archive search utility, version %s\n",
		version);
	fprintf (stderr, "%s\n\n", copyright);
	fprintf (stderr, "Usage:\n\t%s [-v] [-r] [-D] [-m] [-d dir] pattern...\n",
		progname);
	fprintf (stderr, "Options:\n");
	fprintf (stderr, "\t-d dir\tdirectory to search\n");
	fprintf (stderr, "\t-v\tverbose mode\n");
	fprintf (stderr, "\t-r\tadd leading slash to file names\n");
	fprintf (stderr, "\t-m\tformat output as mailserv commands\n");
	fprintf (stderr, "\t-D\tdebug mode\n");
	exit (0);
}

void main (int argc, char **argv)
{
	progname = *argv;
	for (;;) {
		switch (getopt (argc, argv, "rmvDd:")) {
		case EOF:
			break;
		case 'r':
			++rflag;
			continue;
		case 'm':
			++mflag;
			continue;
		case 'v':
			++verbose;
			continue;
		case 'D':
			++debug;
			continue;
		case 'd':
			rootdir = optarg;
			continue;
		}
		break;
	}
	argc -= optind;
	argv += optind;

	if (! argc)
		usage ();

	while (--argc >= 0)
		find (*argv++);

	exit (0);
}
