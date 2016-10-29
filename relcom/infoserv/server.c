/*
 *      Demos Info Server.
 *      Copyright (C) 1990 Demos/*, Moscow
 *
 *      Author: Serge Vakulenko, vak@kiae.su
 */

# include <stdio.h>
# include <sys/types.h>
# include <sys/param.h>
# include <sys/times.h>
# include "messages.h"
# include "config.h"

# define TMPFNAME "/tmp/dmsXXXXXX"

char tmpf [] = TMPFNAME;

char sender [512];

long starttime;
long endtime;

extern long filesize ();

main ()
{
	FILE *fd;
	long replysz;

	/* catch signals */
	sigcatch ();

	/* make temp file name */
	mktemp (tmpf);

	/* lock server */
	lock (SLOCKFILE);

	/* enter public directory */
	if (chdir (INFODIR) < 0) {
		error ("cannot chdir to %s", INFODIR);
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
	if (! strncmp (sender, "uucp@", 5)) {
		fd = fopen (JUNKFILE, "a");
		if (! fd) {
			error ("cannot open %s", JUNKFILE);
			quit ();
		}
		fprintf (fd, "\nFrom %s\n\n", sender);
		ffcopy (stdin, fd);
		fclose (fd);
		error ("uucp message junked");
		/* unlock SLOCKFILE */
		unlock (SLOCKFILE);
		return (0);
	}

	/* print version etc. */
	message (VERSION);
	message ("Sender %s", sender);
	time (&starttime);
	printf (MSGMESSAGE);
	printf (ctime (&starttime));

	/* read and execute commands */
	runinfo ();
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

	/* unlock SLOCKFILE */
	unlock (SLOCKFILE);
	return (0);
}

/*
 * Abnormal termination of program.
 */

quit ()
{
	unlock (SLOCKFILE);
	unlock (LLOCKFILE);
	unlink (tmpf);
	exit (-1);
}

/*
 * Output error message to log file.
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
	register c, i;
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
}

/*
 * Print error message.
 */

message (s, a, b, c)
char *s, *a, *b, *c;
{
	printf (MSGMESSAGE);
	printf (s, a, b, c);
	printf ("\n");
}

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
{
	char buf [512];
	FILE *fd;

	sprintf (buf, MAILTOCMD, name);
	fflush (stdout);
	fd = popen (buf, "w");
	if (! fd) {
		error ("cannot run '%s'", buf);
		quit ();
	}
	ffcopy (from, fd);
	pclose (fd);
}

runinfo ()
{
	char buf [512];
	FILE *fd;

	fflush (stdout);
	fd = popen (INFOCMD, "w");
	if (! fd) {
		error ("cannot run '%s'", INFOCMD);
		quit ();
	}
	ffcopy (stdin, fd);
	pclose (fd);
}
