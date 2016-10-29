/*
 * Сервер-шлюз из почтовых списков рассылки в телеконференции.
 *
 * Copyright (C) 1992-1995 Cronyx Ltd.
 * Автор: Сергей Вакуленко, vak@cronyx.ru
 * Wed Feb  8 18:29:31 MSK 1995
 */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "config.h"
#include "header.h"

#define TMPFNAME "/tmp/msXXXXXX"
#define MAXLISTS 100

char tmpf [] = TMPFNAME;
int debug;
unsigned long serial = 1;

struct listtab {
	char *list;
	char *group;
} listtab [MAXLISTS];

extern int error (char *s, ...);
extern void sigign (void);
extern void f2clean (char*, char*);
extern int config (char*);
extern int f2access (char*, char*, int);
extern int getstr (FILE*, char*);
extern int ffcopy (FILE*, FILE*);
extern int ffncopy (FILE*, FILE*, long);
extern long filesize (char*);
extern char *time2a (long);
extern FILE *f2open (char*, char*, char*);
extern char *rfcdate (char*);

/*
 * Abnormal termination of program.
 */
void quit ()
{
	exit (-1);
}

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

int artcopy (FILE *from, FILE *to, long len, char *group)
{
	long next = len + ftell (from);

	scanheader (from);

	if (! h_subject) {
		error ("no subject field");
		freeheader ();
		return (0);
	}
	if (! h_from) {
		error ("no from field");
		freeheader ();
		return (0);
	}
	if (! h_date) {
		error ("no date field");
		freeheader ();
		return (0);
	}
	if (! h_message_id) {
		error ("no message-id field");
		freeheader ();
		return (0);
	}

	fprintf (to, "Path: gateserv\n");
	fprintf (to, "Newsgroups: %s\n", group);
	fprintf (to, "Subject: %s\n", h_subject ? h_subject : "(no subject)");
	fprintf (to, "From: %s\n", h_from);
	if (h_to)
		fprintf (to, "To: %s\n", h_to);
	if (h_cc)
		fprintf (to, "Cc: %s\n", h_cc);
	if (h_reply_to)
		fprintf (to, "Reply-To: %s\n", h_reply_to);
	fprintf (to, "Approved: %s\n", APPROVED);
	fprintf (to, "Organization: %s\n",
		h_organization ? h_organization : "unknown");
	fprintf (to, "Date: %s\n", rfcdate (h_date));
	fprintf (to, "Message-ID: <%s/%s\n", group, h_message_id+1);
	fprintf (to, "X-Return-Path: %s\n", h_from_);
	if (h_mime_version)
		fprintf (to, "Mime-Version: %s\n", h_mime_version);
	if (h_content_type)
		fprintf (to, "Content-Type: %s\n", h_content_type);
	if (h_content_transfer_encoding)
		fprintf (to, "Content-Transfer-Encoding: %s\n", h_content_transfer_encoding);
	if (h_content_length)
		fprintf (to, "Content-Length: %s\n", h_content_length);
	fprintf (to, "\n");

	freeheader ();

	ffncopy (from, to, next - ftell (from));
	return (1);
}

void runqueuer (char *mailbox, char *list, char *group)
{
	FILE *fd, *tfd;
	long beg, next, t;
	char name [100];
	char from [256], *fromptr;

	fd = fopen (mailbox, "r+");
	if (! fd) {
		error ("cannot read %s", mailbox);
		return;
	}
	if (flock (fileno (fd), LOCK_EX) < 0) {
		/* The mailbox is locked, waiting. */
		fclose (fd);
		return;
	}
	beg = findfrom (fd, 0L, &fromptr);
	strcpy (from, fromptr);
	for (;;) {
		next = findfrom (fd, beg+1, &fromptr);
		if (next < 0)
			break;

		/* make queue file name */
		time (&t);
		sprintf (name, "%s/in.coming/%ld.%ld.t", NEWSSPOOLDIR,
			serial++, t);

		/* create queue file */
		tfd = fopen (name, "w");
		if (! tfd) {
			error ("cannot open %s", name);
			quit ();
		}

		/* save query into queue file */
		fseek (fd, beg, 0);
		if (artcopy (fd, tfd, next - beg, group))
			error ("queued %s to %s from %s", list, group, from);
		if (ferror (tfd)) {
			error ("error writing to %s", name);
			quit ();
		}
		fclose (tfd);

		beg = next;
		strcpy (from, fromptr);
	}
	if (ftruncate (fileno (fd), 0L) < 0) {
		error ("cannot rewrite %s", mailbox);
		quit ();
	}
	fclose (fd);
}

void parselists ()
{
	struct listtab *p = listtab;
	char *s;

	s = LISTS;
	for (; *s; ++p) {
		/* skip spaces */
		while (*s==' ' || *s=='\t')
			++s;

		/* parse list name */
		p->list = s;
		while (*s && *s!=' ' && *s!='\t')
			++s;
		if (! *s)
			break;
		*s++ = 0;

		/* skip spaces */
		while (*s==' ' || *s=='\t')
			++s;

		/* parse list name */
		p->group = s;
		while (*s && *s!=' ' && *s!='\t')
			++s;
		if (*s)
			*s++ = 0;
	}
	p->list = 0;
}

int main (int argc, char **argv)
{
	char *progname = argv[0];
	struct listtab *p;

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

	if (debug)
		fprintf (stderr, "lists = ``%s''\n", LISTS);
	parselists ();
	if (debug)
		for (p=listtab; p->list; ++p)
			fprintf (stderr, "list %s -> group %s\n",
				p->list, p->group);

	if (! debug) {
		/* let's fork */
		int pid = fork ();

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

	error ("daemon started");

	for (;;) {
		for (p=listtab; p->list; ++p) {
			char mailbox[256];

			strcpy (mailbox, MAILDIR);
			strcat (mailbox, "/");
			strcat (mailbox, p->list);
			if (filesize (mailbox))
				runqueuer (mailbox, p->list, p->group);
		}
		sleep (DAEMONDELAY);
	}
	/* NOTREACHED */
}

/*
 * Output error message to log file.
 * VARARGS1
 */
int error (char *fmt, ...)
{
	FILE *fd;
	va_list ap;

	fd = fopen (LOGFILE, "a");
	if (! fd)
		return (-1);
	fprintf (fd, "%s  ", time2a (time ((long *) 0)));
	va_start (ap, fmt);
	vfprintf (fd, fmt, ap);
	va_end (ap);
	fprintf (fd, "\n");
	fclose (fd);
	return (0);
}
