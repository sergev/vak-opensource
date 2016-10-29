/*
 * $Header: notify.c,v 2.0 92/08/05 08:42:55 vak Exp $
 * $Log:	notify.c,v $
 * Revision 2.0  92/08/05  08:42:55  vak
 * 
 * 
 * Revision 1.2  92/06/19  22:20:21  usenet
 * Fix errors. Rudnev
 * 
 */
/*
 * Рассылка списков статей в режиме notify.
 * Должен запускаться раз или два в сутки.
 *
 * Copyright (C) 1991 Сергей Вакуленко, <vak@kiae.su>.
 * All rights reserved.
 */

# include <stdio.h>
# include <sys/fcntl.h>
# include <sys/types.h>
# include "config.h"
# include "groups.h"
# include "lang.h"
# include "vdbm.h"
# include "lock.h"

# ifndef FILEMASK
# define FILEMASK 7
# endif

# define TMPFNAME "/tmp/NSnXXXXXX"

char tmpf [] = TMPFNAME;
VDBM *userdb;
int debug;
int dotimeoflife;
static del_user;

extern char *bsearch (), *strcopy (), *malloc (), *size2a (), *mktemp ();
extern char *strcpy (), *strchr (), *strrchr (), *cut25 (), *gendate ();
extern long filesize (), f2size (), atol ();
extern FILE *f2open (), *popen ();
extern char *time2a();
extern char *groupclass();

main (argc, argv)
char **argv;
{
	char cmd [100];
	int n;

	timeinit ();
	while (argc > 1 && argv[1][0] == '-') {
		register char *p;

		for (p= &argv[1][1]; *p; ++p) {
			switch (*p) {
			case 'n':
				dotimeoflife = -1 ;
				break;
			case 'd':
				++debug;
				break;
			default:
				goto usage;
			}
		}
		++argv;
		--argc;
	}
	if (argc > 2) {
usage:          fprintf (stderr, "usage: notify [-d] [-n] [config-file]\n");
		exit (-1);
	}
	if (! config (argv [1])) {
		fprintf (stderr, "cannot read config file\n");
		exit (-1);
	}

	/* catch signals */
	sigcatch ();

	messg ("notify started");
	locknotify ();
# ifdef REMOTEDB
	pinggroups ();
# endif

	/* Load /usr/spool/newsserv/groups - list of all known groups */
	if (! loadgroups (1))
		quit ();

	userdb = vdbm_open (0);
	if (! userdb) {
		error ("cannot create VDBM user database");
		quit ();
	}
	if ( dotimeoflife == 0 )
	       dotimeoflife = check_dotime(TIMEOFLIFE);

	mktemp (tmpf);

	new_lock();
	if (!f2size(SERVDIR,"newgroups+") && f2size (SERVDIR, "newgroups")) {
		run ("sort -u %s/newgroups > %s/newgroups~", SERVDIR, SERVDIR);
		f2rename (SERVDIR, "newgroups~", SERVDIR, "newgroups+");
		f2unlink (SERVDIR, "newgroups");
	}
	if (!f2size(SERVDIR,"oldgroups+") && f2size (SERVDIR, "oldgroups")) {
		run ("sort -u %s/oldgroups > %s/oldgroups~", SERVDIR, SERVDIR);
		f2rename (SERVDIR, "oldgroups~", SERVDIR, "oldgroups+");
		f2unlink (SERVDIR, "oldgroups");
	}
	new_unlock();
	fclose (stdin);
	ldclasstab("lclasses");
	for (n=0; n<=FILEMASK; ++n) {
		char artfname [16];

		/* Работаем _всегда_ с new+. Если он есть - был сбой, начнем с него */
		if ( f2size(SERVDIR, "new+") ) {
		       n--;
		} else {
		       strcpy (artfname, "newX");
		       artfname [3] = n + (n<10 ? '0' : ('A'-10));
		       new_lock();
		       f2rename(SERVDIR,artfname,SERVDIR,"new+");
		       new_unlock();
		};
		strcpy(artfname,"new+");
		if (f2size (SERVDIR, artfname)) {
			sprintf (cmd, "exec sort -u %s/%s", SERVDIR, artfname);
			if (popen (cmd, "r") != stdin) {
				error ("cannot read from pipe");
				quit ();
			}
			/* Post lists of new articles to users, subscribed in notify mode */
			sendnotify ();
			pclose (stdin);
		}
			f2unlink (SERVDIR, artfname);
	}
	if (f2size (SERVDIR, "newgroups+") ||
	    f2size (SERVDIR, "oldgroups+") || dotimeoflife > 0 )
		sendnewoldgroups ();
	if (! debug) {
		f2unlink (SERVDIR, "newgroups+");
		f2unlink (SERVDIR, "oldgroups+");
	}
	savegroups ();
	messg ("notify finished");
	return (0);
}

locknotify ()
{
	int fd;
	char name [512];

	sprintf (name, "%s/.notifylock", SERVDIR);

	if (access (name, 0) != 0)
		close (creat (name, 0600));

	fd = open (name, 1);
	if (fd < 0) {
		error ("cannot open lock file %s", name);
		quit ();
	}
	fcntl (fd, F_SETFD, 1);        /* close on exec */
	if (! LOCK (fd)) {
		messg ("locked, exiting");
		quit ();
	}
	/* don't close the lock file */

}

/*
 * Abnormal termination of program.
 */

quit ()
{
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
	log ("n", s, a, b, c, d);
}
/*
 * Output error message to log file.
 * VARARGS1
 */

error (s, a, b, c, d)
char *s, *a, *b, *c, *d;
{
	log ("n +ERR+", s, a, b, c, d);
}

/* VARARGS1 */

run (s, a, b, c, d, e, f)
char *s, *a, *b, *c, *d, *e, *f;
{
	char buf [512];
	int status;

	sprintf (buf, s, a, b, c, d, e, f);
	fflush (stdout);
	status = system (buf);
	fseek (stdout, 0L, 2);
	return (status);
}

sendnewoldgroups ()
{
	long u;

	for (u=firstuser(); u; u=nextuser(u)) {
		if (! marked (u)) {
			if ( prepareuser (u, NULL, 0,MYADDRESS))
			       postuser (u, NULL,MYADDRESS);
			else
			{
			       fflush(stdout);
			       unlink(tmpf);
			}
		}
	}
}

char *skip (p)
char **p;
{
	char *ret = *p;

	while (**p && **p!=' ' && **p!='\n')
		++(*p);
	while (**p==' ' || **p=='\n')
		*(*p)++ = 0;
	return (ret);
}

sendnotify ()
{
	char line [1024];
	char *p, *username, *groupname, *from, *subject;
	char *gclass, *ogclass;
	int issue;
	long size;
	char user [80];
	char group [80];
	char buf [512];
	int len;

	user [0] = 0;
	group [0] = 0;
	gclass = ogclass = "";
	while (! feof (stdin)) {
		if (! fgets (line, sizeof (line), stdin))
			continue;
		p = line;
		username = skip (&p);
		groupname = skip (&p);
		issue = atoi (skip (&p));
		size = atol (skip (&p));
		from = skip (&p);
		subject = p;
		if (*p) {
			p += strlen (p) - 1;
			if (*p == '\n')
				*p = 0;
		}
		if ( user[0] == 0 || strcmp(groupname,group ) )
		{
		       gclass = groupclass(groupname);
		       if ( user[0] == 0 ) ogclass = gclass;
		}
		if (user [0] == 0) {
			strcpy (user, username);
			prepareuser (0L, user, 1,gclass);
			group [0] = 0;
		}
		if (strcmp (user, username) || strcmp(gclass,ogclass) || ftell (stdout) > LISTSIZE * 1024L) {
			postuser (0L, user, ogclass);
			strcpy (user, username);
			prepareuser (0L, user, 1,gclass);
			group [0] = 0;
		}
		ogclass = gclass;
		if (strcmp (group, groupname)) {
			strcpy (group, groupname);
			printf ("\nGROUP %s\n", group);
		}

		len = strlen (from);
# ifdef notdef
		sprintf (buf,"-ART %6d %-4s %-25s ", issue, size2a (size),
			cut25 (from));
# endif
		sprintf (buf,"-ART %6d %-4s %-25s ", issue, size2a (size),
			from);
		fputs (buf, stdout);
		putsubj (subject, len<=25 ? 35 : len<60 ? 60-len : 0);
	}
	if (user [0] != 0)
		postuser (0L, user,gclass);
}

postuser (u, user,myname)
long u;
char *user;
char *myname;
{
	if (! user)
		user = username (u);
	if (! user) {
		error ("posting list to NULL user 0x%x?", u);
		unlink (tmpf);
		return;
	}
	if (debug) {
		printf ("POST %s\n", user);
		return;
	}
	fflush (stdout);
	run ("%s -r %s%s %s < %s", MAILCMD, myname, MYDOMAIN, user, tmpf);
	messg ("list posted to %s, %ld bytes", user, filesize (tmpf));
	outstat ("list", user, filesize (tmpf));
	unlink (tmpf);
	if ( del_user )
	{
	       del_user = 0;
	       deluser(user);
	       messg("User %s removed",user);
	}
}

prepareuser (u, user, arts,sender)
long u;
char *user;
char *sender;
{
	FILE *fd;
	char line [80];
	int ko = 0;
	static long ou;

	if (! debug && ! freopen (tmpf, "w", stdout)) {
		error ("cannot create %s", tmpf);
		quit ();
	}

	if (! user)
		user = username (u);
	if (! u)
		u = usertag (user);
	mark (u);

	setlang (sudomain (user) ? 'r' : 'l');

	/* Print header to temporary file */
	printf ("To: %s\n", user);
	printf ("Subject: %s\n", arts ? list_of_articles : info_from_server);
	printf ("From: %s%s\n", MYSERVADDR, MYDOMAIN);
	printf ("Date: %s\n", gendate ());
	printf ("Sender: %s%s\n", sender, MYDOMAIN);
	if (HEADLINE) {
		printf (HEADLINE);
		printf ("\n");
	}
	printf ("\n");
	/*  Одна тонкость - если не запомнить ou, то при очень большой рассылке
	 *  происходит несколько обращений к prepareuser, и получается весьма
	 *  плохо
	 */
	if ( user && u && dotimeoflife && u != ou ) {
	       ko = time_plus_1(u);
	       /* Здесь ko != 0 - число в поле старения */
	       /* Если 100 - все, каюк подписчику        */
	       if ( ko && ko != 100  ) {
		       printf(you_send_nothing,ko * TIMEOFLIFE / 4 , TIMEOFLIFE - (ko * TIMEOFLIFE / 4));
		       messg("Send notification about silence to %s", user);
	       }
	       if ( ko == 100 ) {
		       printf(rest_in_pease,user);
		       del_user = 1;
	       }
	       ou = u;
	};

	if (f2size (SERVDIR, "newgroups+")) {
		fd = f2open (SERVDIR, "newgroups+", "r");
		if (fd) {
			ko += 1;
			printf (new_groups_created);
			while (fgets (line, sizeof (line), fd))
				printf ("\t%s", line);
			fclose (fd);
			printf ("\n");
		}
	}

	if (f2size (SERVDIR, "oldgroups+")) {
		fd = f2open (SERVDIR, "oldgroups+", "r");
		if (fd) {
			ko += 1;
			printf (groups_deleted);
			while (fgets (line, sizeof (line), fd))
				printf ("\t%s", line);
			fclose (fd);
			printf ("\n");
		}
	}
	if (! arts)
		return(ko);

	printf (uncomment_article_commands);
	printf (and_send_list_back, MYSERVADDR, MYDOMAIN);
	printf ("\n");

	if (! (userflags (u) & UFLAGPACK)) {
		printf (uncomment_this);
		printf ("-PACK\n");
	}
	return(ko);
}

mark (tag)
long tag;
{
	vdatum key, val;
	char c = 1;

	key.dptr = (char *) &tag;
	key.dsize = sizeof (tag);
	val.dptr = &c;
	val.dsize = 1;
	vdbm_store (userdb, key, val, 1);
}

marked (tag)
long tag;
{
	vdatum key, rez;

	key.dptr = (char *) &tag;
	key.dsize = sizeof (tag);
	rez = vdbm_fetch (userdb, key);
	return (rez.dptr != 0);
}

check_dotime(daysoflife)
int daysoflife;
{
    long delta = daysoflife * 3600 * 24 / 4;
    FILE *fd;
    long  ct, ot;
    extern long time();

       if ( daysoflife <= 0 )
	       return(0);
       ct = time((long *)0);
       ot = f2modt(SERVDIR,AGEFILE);
       if ( ot < 0 || ot < ct - delta )
       {
	   fd = f2open(SERVDIR,AGEFILE,"w");
	   if ( !fd ) {
		  error("Can not create AGING file %s", AGEFILE);
		  return(0);
	   }
	   fprintf(fd,time2a(ct));
	   fclose(fd);
	   messg("AGING allowed",0);
	   return(1);
       }
    return(0);
}

/*
 * Проверка, не постарел ли пользователь
 * Возвращаем счетчик _перед_ старением, 100, если мы его вычистили,
 * и 0, если он нестареющий
 */
time_plus_1(u)
long u; /* tag */
{
    long fl;
    int n;
    fl = userflags(u);
    if ( (fl&UFLAGAGE_M) == UFLAGAGE_N )
	   return(0);
    if ( (fl&UFLAGAGE_M) == UFLAGAGE_O )
	   return(100);
    n = UFLAG_NA(fl);
    fl += UFLAGAGE_1;
    setuserflags(u,fl);
    return(n);
}
