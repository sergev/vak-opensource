/*
 * Mail Server.
 * Copyright (C) 1990-95 Serge Vakulenko
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <syslog.h>
#include <sys/param.h>
#include "server.h"
#include "config.h"
#include "header.h"
#include "ut.h"

/* #ifdef RIS */
#   include "ris.h"
/* #endif */

#define TMPFNAME "/tmp/dmsXXXXXX"

char tmpf [] = TMPFNAME;
char home [MAXPATHLEN];
char *sender;
int giveup = 0;

extern long starttime;

/*
 * Abnormal termination of program.
 */
void clean ()
{
	unlink (tmpf);
}

/*
 * Output error message to log file.
 */
void error (char *s, ...)
{
	va_list ap;

	va_start (ap, s);
	vsyslog (LOG_INFO, s, ap);
	va_end (ap);
}

/*
 * Print error message.
 */
void message (char *s, ...)
{
	va_list ap;

	va_start (ap, s);
	vprintf (s, ap);
	va_end (ap);
	printf ("\n");
}

/*
 * Print fatal error message, copy rest of stdin to stdout,
 * then exit from program.
 */
void fatal (char *s, ...)
{
	char line [512];
	va_list ap;

	giveup = 1;
	printf ("! ");
	va_start (ap, s);
	vprintf (s, ap);
	va_end (ap);
	printf ("\n");
	while (fgets (line, sizeof (line), stdin)) {
		if (! strncmp (line, "Ignored: ", 9)) {
			error ("`ignored' message bounced");
			exit (-1);
		}
		printf ("Ignored: %s", line);
	}
	doquit ();
}

int run (char *s, ...)
{
	char buf [1024];
	va_list ap;

	va_start (ap, s);
	vsprintf (buf, s, ap);
	va_end (ap);
	return (runs (buf));
}

int runs (char *cmd)
{
	int status;

	fflush (stdout);
	status = system (cmd);
	if (status)
		message ("Error 0x%x executing %s", status, cmd);
	fseek (stdout, 0L, 2);
	return (status);
}

/*
 * Get sender address from 'From:' field
 * of mail header.
 */
char *getsendername (char *field)
{
	char *cp, *cp2, addr[256];
	int c, gotlt, lastsp, level;

	gotlt = 0;
	lastsp = 0;
	cp = field;
	cp2 = addr;
	while ((c = *cp++))
		switch (c) {
		case '(':
			level = 1;
			while (*cp != '\0' && level) {
				switch (*cp++) {
				case '(':
					level++;
					break;
				case ')':
					level--;
					break;
				}
			}
			if (*cp)
				cp++;
			lastsp = 0;
			break;
		case ' ':
			if (cp[0] == 'a' && cp[1] == 't' && cp[2] == ' ')
				cp += 3, *cp2++ = '@';
			else if (cp[0] == '@' && cp[1] == ' ')
				cp += 2, *cp2++ = '@';
			else
				lastsp = 1;
			break;
		case '<':
			cp2 = addr;
			gotlt++;
			lastsp = 0;
			break;
		case '>':
			if (gotlt)
				goto done;
			/* Fall into . . . */
		default:
			if (lastsp) {
				lastsp = 0;
				*cp2++ = ' ';
			}
			*cp2++ = c;
			break;
		}
done:
	*cp2 = 0;
	if (*addr == '@' && !strchr (addr, ':')) {
		char buf [512];

		strcpy (buf, addr);
		strcpy (addr, "root");
		strcat (addr, buf);
	}
	return (strcopy (addr));
}

void replyto (FILE *from, char *name)
{
	char cmd [512], buf [513];
	FILE *fd;
	long cont = 0;
	int part = 1;

	buf [512] = 0;
	if (giveup)
		sprintf (cmd, "%s %s %s", MAILTOCMD, name, ADMIN);
	else
		sprintf (cmd, "%s %s", MAILTOCMD, name);

	while (fgets (buf, 512, from)) {
		fd = popen (cmd, "w");
		if (! fd) {
			error ("cannot run '%s'", cmd);
			exit (-1);
		}
		fprintf (fd, "From: %s\n", MYADDRESS);
		fprintf (fd, "To: %s\n", name);
		if (giveup)
			fprintf (fd, "Cc: %s\n", ADMIN);
		if (cont)
			fprintf (fd, "Subject: Kiarchive Mail Server Reply, part %d\n", ++part);
		else
			fprintf (fd, "Subject: Kiarchive Mail Server Reply\n");
		fprintf (fd, "X-Complaints-To: mailserv-adm@relcom.ru\n");
		fprintf (fd, "X-Service-Address: mailserv@relcom.ru\n");
		if (HEADLINE) {
			fputse (HEADLINE, fd);
			fprintf (fd, "\n");
		}
		fprintf (fd, "\n");
		do {
		       fputs (buf, fd);
		       if (ftell (from) - cont > 64 * 1024L + 10240) {
				fprintf (fd, "\n---- to be continued ----\n");
				break;
		       }
		} while (fgets (buf, 512, from));
		putc ('\n', fd);
		pclose (fd);
		cont = ftell (from);
	}
}

int main (int argc, char **argv)
{
	FILE *fd;
	char *s;
	long replysz;
/* #ifdef RIS */
	int k;
/* #endif */

	openlog ("msrv", 0, LOG_MAIL);
	if (argc > 2) {
		fprintf (stderr, "usage: %s [config-file]\n", argv [0]);
		exit (-1);
	}
	if (! config (argv [1])) {
		fprintf (stderr, "cannot read config file\n");
		exit (-1);
	}

	/* catch signals */
	sigcatch ();

	/* make temp file name */
	mktemp (tmpf);
	atexit (clean);

	/* enter public directory */
	if (chdir (PUBLICDIR) < 0) {
		error ("cannot chdir to %s", PUBLICDIR);
		exit (-1);
	}
	if (! getwd (home)) {
		error ("cannot get %s directory name", PUBLICDIR);
		exit (-1);
	}

	/* open temp file on stdout */
	if (! freopen (tmpf, "w", stdout)) {
		error ("cannot reopen stdout on %s", tmpf);
		exit (-1);
	}
	fclose (stderr);        /* no stderr */

	/* scan header and find 'From' field */
	scanheader (stdin);
	if (h_reply_to)
		sender = getsendername (h_reply_to);
	else if (h_resent_from)
		sender = getsendername (h_resent_from);
	else if (h_from)
		sender = getsendername (h_from);
	else if (h_sender)
		sender = getsendername (h_sender);
	else {
		error ("cannot determine sender address");
		exit (-1);
	}
	for (s=sender; *s && *s != '@'; ++s);
	if (*s == '@')
		strlower (s + 1);

	/* check if this mail is from uucp */
	if (! strncmp (sender, "uucp@", 5) ||
	    ! strncmp (sender, "mailserv",8) ||
	    ! strncmp (sender, "MAILER-DAEMON", 13)) {
		fd = f2open (SERVDIR, "junkfile", "a");
		if (! fd) {
			error ("cannot open %s/junkfile", SERVDIR);
			exit (-1);
		}
		fprintf (fd, "\nFrom %s\n\n", sender);
		ffcopy (stdin, fd);
		fclose (fd);
		error ("message from %s junked", sender);
		return (0);
	}

	/* print version etc. */
	message (VERSION);
	message (COPYRIGHT);
	time (&starttime);
	printf ("%s\n", ctime (&starttime));

	f2cat (SERVDIR, "motd");

	/* read and execute commands */
	if (! checkpermissions (sender, SERVDIR, "users")) {
		printf ("Permission denied\n");
		error ("addressee %s rejected", sender);
	} else if (! checkpermissions (h_from_, SERVDIR, "paths")) {
		printf ("Permission denied\n");
		error ("path %s rejected", h_from_);
/* #ifdef RIS */
	} else if (strchr (sender, '@') && (k = rischeck (sender)) != 0) {
		printf ("Relcom access control system message for user `%s':\n\n\t",
			sender);
		if (k == 1)
			printf ("Access to mailserv denied.\n");
		else if (risreply[0])
			printf ("Error code %d: %s\n", k, risreply);
		else
			printf ("Error code %d, reason unknown\n", k);
		printf ("\nPlease, send e-mail to node-adm@relcom.ru for help.\n\n");
		error ("ris access denied (k=%d, %s)", k,
			risreply[0] ? risreply : "no reply");
		giveup = 1;     /* e-mail to arcman */
/* #endif */
	} else
		mainloop ();
	fclose (stdout);

	/* compute size of reply */
	replysz = filesize (tmpf);

	/* send reply to sender */
	fd = fopen (tmpf, "r");
	unlink (tmpf);
	if (! fd) {
		error ("cannot reopen %s", tmpf);
		exit (-1);
	}
	replyto (fd, sender);
	error ("send protocol to %s, %ld bytes", sender, replysz);

	return (0);
}

char *strip (char *wd)
{
	int len = strlen (home);
	char *p = wd;

	if (strncmp (wd, home, len) == 0)
		p += len;
	if (p[0]=='/' && p[1]=='.' && p[2]>='0' && p[2]<='9' && p[3]=='/')
		p += 3;
	return (p);
}

#if 0
char *cwd ()
{
	static char wd[MAXPATHLEN];
	int len = strlen (home);
	char *p = wd;

	if (! getwd (wd))
		return ("/???");
	if (strncmp (wd, home, len) == 0)
		p += len;
	if (p[0]=='/' && p[1]=='.' && p[2]>='0' && p[2]<='9' && p[3]=='/')
		p += 3;
	return (p);
}
#endif
