/*
 * Utility functions.
 * Copyright (C) 1990-95 Serge Vakulenko
 */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ut.h"
#include "match.h"

#ifndef SIGTYPE
#   define SIGTYPE void
#endif
typedef SIGTYPE (*SIGFUN) (int);

/*
 * Get string from file.
 * Return 1 if ok, else 0.
 */
int getstr (FILE *fd, char *s)
{
	register c;

	while ((c = getc (fd)) >= 0 && c!='\n')
		*s++ = c;
	*s = 0;
	return (c >= 0);
}

/*
 * Check if the file contains pure text.
 */
int textfile (char *name)
{
	FILE *fd;
	register c;

	fd = fopen (name, "r");
	if (! fd)
		return (1);
	while ((c = getc (fd)) >= 0) {
		if (c >= ' ')
			continue;
		switch (c) {    /* Legal text characters: */
		case '\32':     /* ^Z */
		case '\r':      /* CR */
		case '\n':      /* NewLine */
		case '\b':      /* BackSpace */
		case '\t':      /* Tab */
		case '\f':      /* FormFeed */
			continue;
		}
		fclose (fd);
		return (0);
	}
	fclose (fd);
	return (1);
}

/*
 * Copy file to stdout, up to ^Z, deleting ^M.
 * Return 0 on success, -1 on error.
 */
int fcat (char *name, int maxlen)
{
	FILE *fd;
	register c, len;

	fd = fopen (name, "r");
	if (! fd)
		return (-1);
	len = 0;
	while (len<maxlen && (c = getc (fd)) >= 0 && c!='\32') {
		if (c == '\r')                  /* skip CR */
			continue;
		if (c == '\n')
			++len;
		putchar (c);
	}
	fclose (fd);
	return (0);
}

int f2cat (char *dir, char *file)
{
	char buf [256];

	strcpy (buf, dir);
	strcat (buf, "/");
	strcat (buf, file);
	return (fcat (buf, 9999));
}

/*
 * Copy file to file.
 * Return 0 on success,
 * -1 on read error,
 * -2 on write error.
 */
int ffcopy (FILE *from, FILE *to)
{
	char buf [512], *p;
	register n, k;

	for (;;) {
		n = fread (buf, 1, sizeof (buf), from);
		if (n < 0) {
			syslog (LOG_ERR, "read error");
			return (-1);
		}
		if (n == 0)
			break;
		p = buf;
		while (n > 0) {
			k = fwrite (p, 1, n, to);
			if (k <= 0) {
				syslog (LOG_ERR, "write error");
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
int ffncopy (FILE *from, FILE *to, long len)
{
	char buf [512], *p;
	register n, k;

	while (len > 0) {
		n = fread (buf, 1, sizeof (buf), from);
		if (n < 0) {
			syslog (LOG_ERR, "read error");
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
				syslog (LOG_ERR, "write error");
				return (-2);
			}
			n -= k;
			p += k;
		}
	}
	return (0);
}

/*
 * Wait until queue is unlocked,
 * then lock it by creating lock file.
 */
void lock (char *name)
{
	/* wait while queue is locked */
	while (! access (name, 0))
		sleep (1);

	/* lock queue */
	if (creat (name, 0) < 0) {
		syslog (LOG_ERR, "cannot create %s", name);
		exit (-1);
	}
}

/*
 * Return 1 if file is locked.
 */
int locked (char *name)
{
	return (! access (name, 0));
}

/*
 * Unlock queue by deleting lock file
 */
void unlock (char *name)
{
	unlink (name);
}

/*
 * Signal catched, unlock and exit.
 */
static SIGTYPE catcher (int n)
{
	syslog (LOG_INFO, "signal catched");
	exit (-1);
}

/*
 * Set cather of signal, if it is not ignored.
 */
static void catch (int sig, SIGFUN fun)
{
	if (signal (sig, SIG_IGN) != SIG_IGN)
		signal (sig, fun);
}

/*
 * Set up signal catcher.
 */
void sigcatch ()
{
	catch (SIGINT, catcher);
	catch (SIGQUIT, catcher);
	catch (SIGTERM, catcher);
	signal (SIGPIPE, SIG_IGN);
}

void sigign ()
{
	signal (SIGINT, SIG_IGN);
	signal (SIGQUIT, SIG_IGN);
	signal (SIGPIPE, SIG_IGN);
	signal (SIGTERM, catcher);
}

long filesize (char *name)
{
	struct stat x;

	if (stat (name, &x) < 0)
		return (0);
	return (x.st_size);
}

int isdir (char *name)
{
	struct stat x;

	if (stat (name, &x) < 0)
		return (0);
	return (S_ISDIR (x.st_mode));
}

int lower (int c)
{
	if (c>='A' && c<='Z')
		return (c + 'a' - 'A');
	if (c>=(char)0340 && c<=(char)0376)
		return (c - 040);
	return (c);
}

char *strlower (char *str)
{
	register char *p;

	for (p=str; *p; ++p)
		*p = lower (*p);
	return (str);
}

int strnccmp (char *str, char *pat, int len)
{
	int a, b;

	for (;;) {
		if (--len <= 0)
			return (0);
		if (! (a = *str++))
			return (*pat ? -1 : 0);
		if (! (b = *pat++))
			return (1);
		a = lower (a);
		b = lower (b);
		if (a < b)
			return (-1);
		if (a > b)
			return (1);
	}
}

char *strcopy (char *str)
{
	unsigned len = strlen (str);
	char *p = malloc (len+1);

	if (! p) {
		syslog (LOG_ERR, "out of memory in strcopy");
		exit (-1);
	}
	strcpy (p, str);
	return (p);
}

int checkpermissions (char *username, char *dirname, char *permfile)
{
	FILE *fd;
	char line [512];
	char *p;
	int permissions;

	fd = f2open (dirname, permfile, "r");
	if (! fd)
		return (0);
	permissions = 0;
	while (getstr (fd, line)) {
		p = line;
		while (*p==' ' || *p=='\t')
			++p;
		if (! *p || *p=='#')
			continue;
		if (line[0] == '^') {
			if (matchglob (username, line+1))
				break;
		} else if (matchglob (username, line)) {
			permissions = 1;
			break;
		}
	}
	fclose (fd);
	return (permissions);
}

FILE *f2open (char *dirname, char *filename, char *mode)
{
	char buf [512];

	sprintf (buf, "%s/%s", dirname, filename);
	return (fopen (buf, mode));
}

int f2access (char *dirname, char *filename, int mode)
{
	char buf [512];

	sprintf (buf, "%s/%s", dirname, filename);
	return (access (buf, mode));
}

void f2clean (char *dirname, char *filename)
{
	char buf [512];

	sprintf (buf, "%s/%s", dirname, filename);
	close (creat (buf, 0664));
}

void mkinfoname (char *name, char *buf)
{
	char *p = strrchr (name, '/');
	if (p) {
		strncpy (buf, name, p-name);
		strcpy (buf+(p-name), "/.info/");
		strcat (buf, p+1);
	} else {
		strcpy (buf, ".info/");
		strcat (buf, name);
	}
}

void fputse (char *p, FILE *fd)
{
	for (; *p; ++p) {
		if (*p != '\\') {
			putc (*p, fd);
			continue;
		}
		switch (*++p) {
		case 'n':
			putc ('\n', fd);
			break;
		case 'r':
			putc ('\r', fd);
			break;
		}
	}
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

		while (p > info && (p[-1] == '\n' || p[-1] == '\15' ||
		    p[-1] == ' ' || p[-1] == '\t')) {
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
