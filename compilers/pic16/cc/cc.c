/*
 * C Compiler front end for PIC16Cxx processors.
 * Based on Rithie's sources from Unix Seventh Edition.
 *
 * Copyright (C) 2006 Serge Vakulenko <vak@cronyx.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#define CCOM		"cc1"
#define CPP		"cpp"
#define AS		"as"

#define MAXARGC		512
#define MAXNAMLEN	32

char *stdinc		= "-I/usr/local/lib/pic16/include";
char *ccom		= "/usr/local/lib/pic16/cc1";
char *cpp		= "/usr/bin/cpp";
char *as		= "/usr/local/bin/as16";
char templi []		= "/tmp/cciXXXXXX";

char *outfile, *tmpi;
char *av [MAXARGC], *clist [MAXARGC], *llist [MAXARGC], *plist [MAXARGC];
int cflag, errflag, Pflag, Sflag, Eflag, pflag, gflag, Oflag, vflag;
int nc, nl, np, nxs, na;

void error (char *s, ...)
{
	va_list ap;

	va_start (ap, s);
	fprintf (stderr, "cc: ");
	vfprintf (stderr, s, ap);
	va_end (ap);
	fprintf (stderr, "\n");
	cflag++;
	errflag++;
}

int getsuf (char *as)
{
	int c, t;
	char *s;

	s = as;
	c = 0;
	while ((t = *s++) != 0)
		if (t == '/')
			c = 0;
		else
			c++;
	s -= 3;
	if (c<=MAXNAMLEN && c>2 && *s++ == '.')
		return (*s);
	return (0);
}

char *savestr (char *cp)
{
	int len;
	char *new;

	len = strlen (cp) + 1;
	new = malloc (len);
	if (! new) {
		error ("out of memory");
		exit (1);
	}
	return (strncpy (new, cp, len));
}

char *setsuf (char *as, int ch)
{
	char *s, *s1;

	s = s1 = savestr (as);
	while (*s)
		if (*s++ == '/')
			s1 = s;
	s[-1] = ch;
	return (s1);
}

int nodup (char **l, char *os)
{
	char *t, *s;
	int c;

	s = os;
	if (getsuf (s) != 's')
		return (1);
	while ((t = *l++) != 0) {
		while ((c = *s++) != 0)
			if (c != *t++)
				break;
		if (!*t && !c)
			return (0);
		s = os;
	}
	return (1);
}

void cleanup ()
{
	if (! Pflag) {
		unlink (tmpi);
	}
}

void killed (int signum)
{
	cleanup ();
	exit (1);
}

int callsys (char *f, char **v)
{
	int t, status;

	if (vflag) {
		char **p;

		putchar (' ');
		for (p=v; *p; ++p)
			printf ("%s ", *p);
		putchar ('\n');
	}
	t = fork ();
	if (t == -1) {
		error ("cannot create process");
		return (1);
	}
	if (t == 0) {
		/* порожденный процесс */
		execv (f, v);
		error ("cannot find %s", f);
		exit (1);
	}
	while (t != wait (&status))
		continue;
	t = status & 0377;
	if (t != 0) {
		if (t != SIGINT)
			error ("fatal error in %s", f);
		cleanup ();
		exit (1);
	}
	return ((status >> 8) & 0377);
}

int main (int argc, char **argv)
{
	char *t;
	int i, j, c;

	if (argc > MAXARGC-10) {
		error ("too many arguments");
		exit (1);
	}
	for (i=1; i<argc; i++) {
		if (*argv[i] == '-') {
			switch (argv[i][1]) {
			case 'v':       /* print passes */
				vflag++;
				continue;
			case 'S':       /* compile to assembler only */
				Sflag++;
				cflag++;
				continue;
			case 'O':       /* optimize */
				Oflag++;
				continue;
			case 'o':       /* output file name */
				if (++i < argc) {
					outfile = argv[i];
					switch (getsuf (outfile)) {
					case 'c':
					case 'o':
						error ("-o would overwrite %s",
							outfile);
						exit (1);
					}
				}
				continue;
			case 'p':       /* profile */
				pflag++;
				continue;
			case 'g':       /* save debugger info */
				gflag++;
				continue;
			case 'E':       /* preprocess to stdout */
				Eflag++;
			case 'P':       /* preprocess to .i */
				Pflag++;
				plist[np++] = argv[i];
			case 'c':       /* compile to object */
				cflag++;
				continue;
			case 'D':       /* cpp flags */
			case 'I':
			case 'U':
			case 'C':
				plist[np++] = argv[i];
				continue;
			}
		}
		t = argv[i];
		c = getsuf (t);
		if (c == 'c' || Eflag) {
			clist[nc++] = t;
			t = setsuf (t, 's');
		}
		if (nodup (llist, t)) {
			llist[nl++] = t;
			if (getsuf (t) == 's')
				nxs++;
		}
	}
	if (! nc)
		goto load;
	if (signal (SIGINT, SIG_IGN) != SIG_IGN)
		signal (SIGINT, killed);
	if (signal (SIGTERM, SIG_IGN) != SIG_IGN)
		signal (SIGTERM, killed);
	if (! Pflag)
		close (mkstemp (tmpi = templi));
	for (i=0; i<nc; i++) {
		if (nc > 1)
			printf ("%s:\n", clist[i]);
		if (Pflag)
			tmpi = setsuf (clist[i], 'i');

		na = 0;
		av [na++] = CPP;
		for (j=0; j<np; j++)
			av [na++] = plist[j];
		av [na++] = stdinc;
		av [na++] = clist[i];
		av [na++] = Eflag ? "-" : tmpi;
		av [na++] = 0;
		if (callsys (cpp, av)) {
			errflag++;
			cflag++;
		}
		if (Pflag || Eflag)
			continue;

		na = 0;
		av [na++] = CCOM;
		av [na++] = "-dumpbase";
		av [na++] = clist[i];
		av [na++] = tmpi;
		av [na++] = "-o";
		av [na++] = setsuf (clist[i], 's');
		av [na] = 0;
		if (callsys (ccom, av)) {
			cflag++;
			errflag++;
			continue;
		}
	}
load:
	if (! cflag && nl) {
		na = 0;
		av [na++] = AS;
		if (Oflag)
			av [na++] = "-O";
		if (outfile) {
			av [na++] = "-o";
			av [na++] = outfile;
		}
		i = 0;
		while (i < nl)
			av [na++] = llist[i++];
		av [na++] = 0;
		if (callsys (as, av))
			errflag++;

		if (nc==1 && nxs==1 && !errflag)
			unlink (setsuf (clist[0], 's'));
	}
	cleanup ();
	exit (errflag != 0);
}
