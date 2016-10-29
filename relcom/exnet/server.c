/*
 * Почтовый сервер электронной биржи EXNET.
 * Читает текст письма, подавыемый на стандартный ввод.
 * Выполняет содержащиеся в нем команды, протокол отсылает
 * обратно.
 * Обработка команд выполняется функцией mainloop(),
 * файл "reader.c".
 *
 * Copyright (C) 1991 Сергей Вакуленко, <vak@kiae.su>.
 * All rights reserved.
 */

# include <stdio.h>
# include "config.h"
# include "lang.h"

# define TMPFNAME "/tmp/dmsXXXXXX"

char tmpf [] = TMPFNAME;

char sender [512];
int adminmode;

extern long starttime;

extern long filesize (), time ();
extern char *strlower (), *ctime (), *mktemp (), *strcpy (), *strcat ();
extern FILE *f2open ();

main (argc, argv)
char **argv;
{
	FILE *fd;
	long replysz;

	tzset ();
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

	/* enter public directory */
	if (chdir (PUBLICDIR) < 0) {
		error ("cannot chdir to %s", PUBLICDIR);
		quit ();
	}

	/* open temp file on stdout */
	if (! freopen (tmpf, "w", stdout)) {
		error ("cannot reopen stdout on %s", tmpf);
		quit ();
	}
	fclose (stderr);        /* no stderr */

	/* scan header and find 'From' field */
	scanheader ();

	/* check if this mail is from uucp */
	if (! strncmp (sender, "uucp@", 5) || ! strncmp (sender, "MAILER-DAEMON@", 5)) {
		fd = f2open (SERVDIR, "junkfile", "a");
		if (! fd) {
			error ("cannot open %s/junkfile", SERVDIR);
			quit ();
		}
		fprintf (fd, "\nFrom %s\n\n", sender);
		ffcopy (stdin, fd);
		fclose (fd);
		error ("uucp message junked");
		return (0);
	}

	setlang (sudomain (sender) ? 'r' : 'l');

	/* print version etc. */
	message (server_version);
	message (server_copyright);
	time (&starttime);
	checktime (starttime);
	printf (ctime (&starttime));
	printf ("\n");

	/* read and execute commands */
	if (checkpermissions (sender, SERVDIR, "users")) {
		adminmode = checkpermissions (sender, SERVDIR, "admins");
		mainloop ();
	} else {
		printf (permission_denied);
		error ("addressee %s rejected", sender);
	}
	fclose (stdout);

	/* compute size of reply */
	replysz = filesize (tmpf);

	/* send reply to sender */
	fd = fopen (tmpf, "r");
	unlink (tmpf);
	if (! fd) {
		error ("cannot reopen %s", tmpf);
		quit ();
	}
	replyto (fd, sender);

	error ("send protocol to %s, %ld bytes", sender, replysz);

	return (0);
}

/*
 * Abnormal termination of program.
 */

quit ()
{
	unlink (tmpf);
	exit (-1);
}

/*
 * Output error message to log file.
 * VARARGS1
 */

error (s, a, b, c, d)
char *s, *a, *b, *c, *d;
{
	log (SERVER, s, a, b, c, d);
}

/*
 * Scan mail header.
 */

scanheader ()
{
	register char *s;
	register c;
	char line [512];

	for (;;) {
		/* get line from stdin */
		c = getchar ();
		if (c < 0) {
			error ("cannot read mail header");
			quit ();
		}
		for (s=line; c>=0 && c!='\n'; c=getchar()) {
			*s++ = c;
			if (s-line >= sizeof (line)) {
				error ("too long header line");
				quit ();
			}
		}
		*s = 0;
		for (--s; s>=line && (*s==' ' || *s=='\t'); *s-- = 0);

		/* end of header */
		if (*line == 0)
			break;

		if (! *sender && !strncmp (line, "From: ", 6))
			getsendername (line + 6);
	}
	if (! *sender) {
		error ("no 'From' field");
		quit ();
	}
	for (s=sender; *s && *s != '@'; ++s);
	if (*s == '@')
		strlower (s + 1);

}

/*
 * Print error message.
 * VARARGS1
 */

message (s, a, b, c)
char *s, *a, *b, *c;
{
	printf (s, a, b, c);
	printf ("\n");
}

/*
 * Print fatal error message, copy rest of stdin to stdout,
 * then exit from program.
 * VARARGS1
 */

fatal (s, a, b, c)
char *s, *a, *b, *c;
{
	printf (s, a, b, c);
	printf ("\n");
	doquit ();
}

/* VARARGS1 */

run (s, a, b, c, d, e, f)
char *s, *a, *b, *c, *d, *e, *f;
{
	char buf [1024];

	sprintf (buf, s, a, b, c, d, e, f);
	fflush (stdout);
	system (buf);
	fseek (stdout, 0L, 2);
}

/*
 * Get sender address from 'From:' field
 * of mail header.
 */

getsendername (name)
char *name;
{
	register c;
	register char *cp, *cp2;
	int gotlt, lastsp, level;

	gotlt = 0;
	lastsp = 0;
	cp = name;
	cp2 = sender;
	while (c = *cp++)
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
			cp2 = sender;
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
	if (*sender == '@') {
		char buf [512];

		strcpy (buf, sender);
		strcpy (sender, "root");
		strcat (sender, buf);
	}
}

replyto (from, name)
FILE *from;
char *name;
{
	char buf [512];
	FILE *fd;

	sprintf (buf, "%s %s", MAILTOCMD, name);
	fd = popen (buf, "w");
	if (! fd) {
		error ("cannot run '%s'", buf);
		quit ();
	}
	fprintf (fd, "Subject: [Trade] %s\n", server_reply);
	fprintf (fd, "To: %s\n", name);
	fprintf (fd, "From: %s\n", MYADDRESS);
	fprintf (fd, "\n");
	ffcopy (from, fd);
	pclose (fd);
	sleep (1);
}

sudomain (addr)
char *addr;
{
	int len;
	char *domain;

	len = strlen (addr);
	if (len < 3 || addr [len-3] != '.')
		return (0);
	domain = addr + len - 3;
	if (!strcmp (domain, ".su"))    /* U.S.S.R. */
		return (1);
	if (!strcmp (domain, ".ua"))    /* Ukraine */
		return (1);
	if (!strcmp (domain, ".by"))    /* Bielorussia */
		return (1);
	return (0);
}
