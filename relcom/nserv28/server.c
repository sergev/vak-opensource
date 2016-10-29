/*
 * $Header: server.c,v 2.0 92/08/05 08:43:10 vak Exp $
 * $Log:	server.c,v $
 * Revision 2.0  92/08/05  08:43:10  vak
 * 
 * 
 * Revision 1.3  92/06/19  22:20:28  usenet
 * Fix errors. Rudnev
 * 
 * Revision 1.2  92/06/19  22:09:56  usenet
 * 
 * Revision 1.2  92/06/19  22:08:01  usenet
 * News Server
 * 
 */
/*
 * Почтовый сервер новостей.
 * Читает текст письма, подаваемый на стандартный ввод.
 * Выполняет содержащиеся в нем команды, протокол отсылает
 * обратно.
 * Обработка команд выполняется функцией mainloop(),
 * файл "reader.c".
 *
 * Copyright (C) 1991 Сергей Вакуленко, <vak@kiae.su>.
 * Модификации (C) - 1993, Руднев Алексей <alex@kiae.su>
 * All rights reserved.
 */

# include <stdio.h>
# include "config.h"
# include "lang.h"
# include "header.h"
# include "groups.h"

# define TMPFNAME "/tmp/NSsXXXXXX"

char tmpf [] = TMPFNAME;

char *sender;
int needreply;

extern char version [];
extern generation;

extern long filesize (), time ();
extern char *strlower (), *ctime (), *mktemp (), *strcpy (), *strcat ();
extern char *malloc (), *strcopy ();
extern FILE *f2open (), *popen ();
extern char *getsendername ();

main (argc, argv)
char **argv;
{
	FILE *fd;
	char *s;
	long replysz;

	timeinit ();
	if (argc > 2) {
		fprintf (stderr, "usage: %s [config]\n", argv [0]);
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

	/* open temp file on stdout */
	if (! freopen (tmpf, "w", stdout)) {
		error ("cannot reopen stdout on %s", tmpf);
		quit ();
	}

	/* scan header and find 'From' field */
	scanheader (stdin);
	if (h_reply_to)
		sender = getsendername (h_reply_to);
	else if (h_from)
		sender = getsendername (h_from);
	else if (h_resent_from)
		sender = getsendername (h_resent_from);
	else if (h_sender)
		sender = getsendername (h_sender);
	else {
		error ("cannot determine sender address");
		quit ();
	}

	for (s=sender; *s && *s != '@'; ++s);
	if (*s == '@')
		strlower (s + 1);

	/* check if this mail is from uucp */
	if (! strncmp (sender, "uucp@", 5) ||
	    ! strncmp (sender, "news@", 5) ||
	    ! strncmp (sender, "MAILER-DAEMON@", 14)) {
		fd = f2open (SERVDIR, "junk", "a");
		if (! fd) {
			error ("cannot open %s/junk", SERVDIR);
			quit ();
		}
		fprintf (fd, "\nFrom %s\n\n", sender);
		ffcopy (stdin, fd);
		fclose (fd);
		messg ("message from %s junked", sender);
		unlink (tmpf);
		return (0);
	}

	setlang (sudomain (sender) ? 'r' : 'l');

	printf (news_server, version, generation);
	dotime ();
	printf ("\n");

	needreply = 1;

	/* read and execute commands */
	if (checkpermissions (sender, SERVDIR, "users")) {
		if (! loadgroups (1)) {
			error ("cannot open groups file");
			quit ();
		} else if (h_newsgroups) {
			/* submit article to inews */
			needreply = 0;
			mark_user(sender);
			if (submit ())
				messg ("article from %s submitted to %s", sender, h_newsgroups);
			else
				error ("error submitting article from %s to %s", sender, h_newsgroups);
		} else {
			needreply = 0;
			mark_user(sender);
			mainloop ();
		}
	} else {
		printf (permission_denied);
		messg ("addressee %s rejected", sender);
	}
	fflush (stdout);

	if (needreply) {
		/* compute size of reply */
		replysz = filesize (tmpf);

		/* send reply to sender */
		fd = fopen (tmpf, "r");
		if (! fd) {
			error ("cannot reopen %s", tmpf);
			quit ();
		}
		replyto (fd, sender);

		messg ("send protocol to %s, %ld bytes", sender, replysz);
		outstat ("reply", sender, replysz);
	}
	unlink (tmpf);
	savegroups ();
	return (0);
}

/*
 * Abnormal termination of program.
 */

quit ()
{
	unlink (tmpf);
	savegroups ();
	exit (-1);
}

/*
 * Output error message to log file.
 * VARARGS1
 */

messg (s, a, b, c, d)
char *s, *a, *b, *c, *d;
{
	log ("s", s, a, b, c, d);
}

/*
 * Output error message to log file.
 * VARARGS1
 */
error (s, a, b, c, d)
char *s, *a, *b, *c, *d;
{
	log ("s +ERR+", s, a, b, c, d);
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
	int status;

	sprintf (buf, s, a, b, c, d, e, f);
	fflush (stdout);
	status = system (buf);
	fseek (stdout, 0L, 2);
	return (status);
}

replyto (from, name)
FILE *from;
char *name;
{
	char cmd [512], buf [513];
	FILE *fd;
	long cont = 0;

	buf [512] = 0;
	sprintf (cmd, "%s -r %s%s %s", MAILCMD, MYADDRESS,MYDOMAIN, name);
	while (fgets (buf, 512, from)) {
	       fd = popen (cmd, "w");
	       if (! fd) {
		       error ("cannot run '%s'", cmd);
		       quit ();
	       }
	       fprintf (fd, "From: %s%s (News Mailing Service)\n", MYADDRESS,
		       MYDOMAIN);
	       fprintf (fd, "To: %s\n", name);
	       if (cont)
		       fprintf (fd, "Subject: [Cont] %s\n", server_reply);
	       else
		       fprintf (fd, "Subject: %s\n", server_reply);
	       fprintf (fd, "Date: %s\n", gendate ());
	       fprintf (fd, "\n");
	       do {
		      fputs (buf, fd);
		      if (ftell (from) - cont > LISTSIZE * 1024L + 10240) {
			       fprintf (fd, to_be_continued);
			       break;
		      }
	       } while (fgets (buf, 512, from));
	       putc ('\n', fd);
	       pclose (fd);
	       cont = ftell (from);
	}
}

mark_user(user)
char *user;
{
       long utag;
       long flags;
       if ( !(utag = usertag(user)) )
	      return;
       flags = userflags(utag);
       if ( (flags & UFLAGAGE_M) == 0 || (flags & UFLAGAGE_M) == UFLAGAGE_N )
	      return;
       flags &= ~UFLAGAGE_M;
       setuserflags(utag,flags);
       messg("aging flushed: %s", user);
       return;
}
