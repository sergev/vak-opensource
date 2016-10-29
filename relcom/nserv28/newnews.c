/*
 * $Header: newnews.c,v 2.1 92/08/05 12:21:42 vak Exp $
 * $Log:	newnews.c,v $
 * Revision 2.1  92/08/05  12:21:42  vak
 * Сделан перенос в Xenix 386.  В Xenix 286 не работает
 * по причине ляпов в GDBM из-за короткого int.
 * 
 * Revision 2.0  92/08/05  08:42:53  vak
 * 
 * 
 * Revision 1.2  92/06/19  22:20:14  usenet
 * Fix errors. Rudnev
 * 
 */
/*
 * 1) обновление файла групп
 * 2) создание списка новых групп
 * 3) создание списка удаленных групп
 * 4) создание списка пришедших статей
 * 5) рассылка пришедших статей по списку feed
 * Должен запускаться каждый час.
 *
 * Copyright (C) 1991 Сергей Вакуленко, <vak@kiae.su>.
 * All rights reserved.
 */

# include <stdio.h>
# include <sys/fcntl.h>
# include "config.h"
# include "header.h"
# include "groups.h"
# include "lang.h"
# include "lock.h"
# include "vdbm.h"

# define NADDRPERCMD 30

# define TMPFNAME "/tmp/NSfXXXXXX"

# ifndef FILEMASK
# define FILEMASK 7
# endif

# define MSUBS  1
# define MFEED  2
# define MPACK  3
# define MGPACK 4

struct feedtab {
	char *msgid;
	long user;
	long group;
	long issue;
	short mode;
};

struct feedtab *arttab;         /* Общая таблица рассылки */
int nart;
int arttablen;

struct feedtab *feedtab;        /* Таблица неупакованной рассылки */
int nfeed;
int feedtablen;

struct feedtab *packtab;        /* Таблица упакованной рассылки */
int npack;
int packtablen;

struct feedtab *gpacktab;       /* Таблица групповой упак. рассылки */
int ngpack;
int gpacktablen;

int debug;
char tmpname[] = TMPFNAME;

VDBM *userdb;

extern char *bsearch (), *strcopy (), *malloc (), *realloc (), *strcpy ();
extern char *getsendername (), *ctime (), *groupclass (), *groupiclass ();
extern char *mktemp (), *strncpy (), *strchr ();
extern long filesize (), time ();
extern FILE *f2open ();

static cmpmode (a, b)
register struct feedtab *a, *b;
{
	if (a->mode != b->mode)
		return (a->mode > b->mode ? 1 : -1);
	if (a->group != b->group)
		return (a->group > b->group ? 1 : -1);
	if (a->issue != b->issue)
		return (a->issue > b->issue ? 1 : -1);
	return (0);
}

static cmpfeedtab (a, b)
register struct feedtab *a, *b;
{
	if (a->group != b->group)
		return (a->group > b->group ? 1 : -1);
	if (a->issue != b->issue)
		return (a->issue > b->issue ? 1 : -1);
	if (a->user != b->user)
		return (a->user > b->user ? 1 : -1);
	return (0);
}

static cmplong (a, b)
register long *a, *b;
{
	if (*a != *b)
		return (*a > *b ? 1 : -1);
	return (0);
}

static cmpmsgid (a, b)
register struct feedtab *a, *b;
{
	int rez;

	rez = strcmp (a->msgid, b->msgid);
	if (rez)
		return (rez);
	if (a->user != b->user)
		return (a->user > b->user ? 1 : -1);
	if (a->mode != b->mode)
		return (a->mode < b->mode ? 1 : -1); /* reverse order! */
	return (0);
}

static cmppacktab (a, b)
register struct feedtab *a, *b;
{
	if (a->user != b->user)
		return (a->user > b->user ? 1 : -1);
	if (a->mode != b->mode)
		return (a->mode > b->mode ? 1 : -1);
	if (a->group != b->group)
		return (a->group > b->group ? 1 : -1);
	if (a->issue != b->issue)
		return (a->issue > b->issue ? 1 : -1);
	return (0);
}

static storeuser (u, uname)
long u;
char *uname;
{
	vdatum key, val;

	key.dptr = (char *) &u;
	key.dsize = sizeof (u);
	val.dptr = uname;
	val.dsize = strlen (uname) + 1;
	vdbm_store (userdb, key, val, 1);
}

static char *fetchuser (u)
long u;
{
	vdatum key, rez;

	key.dptr = (char *) &u;
	key.dsize = sizeof (u);
	rez = vdbm_fetch (userdb, key);
	return (rez.dptr);
}

main (argc, argv)
char **argv;
{
	long u;

	while (argc > 1 && argv[1][0] == '-') {
		register char *p;

		for (p= &argv[1][1]; *p; ++p) {
			switch (*p) {
			case 'd':
				setbuf(stdout, (char *)0);
				++debug;
				break;
			default:
				goto usage;
			}
		}
		++argv;
		--argc;
	}

	timeinit ();
	if (argc > 2) {
usage:          fprintf (stderr, "usage: %s [config]\n", argv [0]);
		exit (-1);
	}
	if (! config (argv [1])) {
		fprintf (stderr, "cannot read config file\n");
		exit (-1);
	}
	mktemp (tmpname);

	setlang (sudomain (MYDOMAIN) ? 'r' : 'l');

	/* catch signals */
	sigcatch ();

	messg ("newnews started");
	locknewnews ();
# ifdef REMOTEDB
	pinggroups ();
# endif

	/* Load /usr/spool/newsserv/groups - list of all known groups */
	if (debug)
		printf ("Loading groups file\n");
	if (! loadgroups (1)) {
		if (debug)
			printf ("Cannot open groups file\n");
		else
			error ("cannot open groups database, halted");
		quit ();
	}

	/* Build in-core table of user names */
	userdb = vdbm_open (0);
	if (! userdb) {
		error ("cannot create VDBM user database");
		quit ();
	}
	for (u=firstuser(); u; u=nextuser(u)) {
		char *uname = username (u);
		if (! uname)
			continue;
		storeuser (u, uname);
	}

	/* Make /usr/spool/newsserv/newgroups - new groups */
	if (debug)
		printf ("Making list of new groups\n");
	mknewgroups ();

	/* Make /usr/spool/newsserv/oldgroups - deleted groups */
	if (debug)
		printf ("Making list of deleted groups\n");
	mkoldgroups ();

	/* Update list of known groups */
	if (debug)
		printf ("Updating list of groups\n");
	updategroups ();

	arttab = (struct feedtab *) malloc (sizeof (struct feedtab));
	feedtab = (struct feedtab *) malloc (sizeof (struct feedtab));
	packtab = (struct feedtab *) malloc (sizeof (struct feedtab));
	gpacktab = (struct feedtab *) malloc (sizeof (struct feedtab));
	if (!arttab || !feedtab || !packtab || !gpacktab) {
		if (debug)
			printf ("Out of memory in main()\n");
		error ("out of memory in main");
		savegroups ();
		return (0);
	}
	arttablen = 1;
	feedtablen = 1;
	packtablen = 1;
	gpacktablen = 1;
	nart = 0;
	nfeed = 0;
	npack = 0;
	ngpack = 0;

	/* Make global table of new articles */
	if (debug)
		printf ("Creating table of new articles\n");
	mknewarticles ();

	/* Remove cross-posted articles from list */
	nocrosspost ();
	messg ("detected %d unique message-ids", nart);

	/* Sort articles by (1) subscription mode, (2) group, (3) issue. */
	if (nart)
		qsort ((char *) arttab, nart, sizeof (struct feedtab), cmpmode);

	/* Update /usr/spool/newsserv/newarticles - list of arrived articles */
	if (debug)
		printf ("Appending list of new articles\n");
	updatenewarts ();
	messg ("list of new articles updated");

	/* Split arttab into feedtab, packtab, gpacktab */
	splittab ();
	free ((char *) arttab);
	arttab = 0;
	arttablen = 0;
	nart = 0;

	/* Sort articles by (1) group, (2) issue, (3) user. */
	if (ngpack)
		qsort ((char *) gpacktab, ngpack, sizeof (struct feedtab), cmpfeedtab);
	if (debug)
		printf ("Sending %d gpacked articles to subscribers\n", ngpack);
	sendgpack ();   /* упакованная погрупповая рассылка */

	if (debug)
		printf ("Sending %d packed articles to subscribers\n", npack);
	sendpack ();    /* упакованная постатейная рассылка */

	/* Sort articles by (1) group, (2) issue, (3) user. */
	if (nfeed)
		qsort ((char *) feedtab, nfeed, sizeof (struct feedtab), cmpfeedtab);
	/* Post new articles to users, subscribed in feed mode */
	if (debug)
		printf ("Sending %d articles to subscribers\n", nfeed);
	sendfeed ();    /* неупакованная рассылка */

	if (debug)
		printf ("Saving groups file\n");
	savegroups ();

	messg ("newnews finished");
	return (0);
}

locknewnews ()
{
	int fd;
	char name [512];

	sprintf (name, "%s/.feedlock", SERVDIR);

	if (access (name, 0) != 0)
		close (creat (name, 0600));

	fd = open (name, 1);
	if (fd < 0) {
		error ("cannot open lock file %s", name);
		quit ();
	}
	fcntl (fd, F_SETFD, 1);        /* close on exec */
	if (! LOCK (fd)) {
		error ("locked, exiting");
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
	log ("f", s, a, b, c, d);
}
/*
 * Output error message to log file.
 * VARARGS1
 */

error (s, a, b, c, d)
char *s, *a, *b, *c, *d;
{
	log ("f +ERR+", s, a, b, c, d);
}

/* VARARGS1 */

run (s, a, b, c, d, e, f)
char *s, *a, *b, *c, *d, *e, *f;
{
	static char *buf;
	int status;

	if (! buf) {
		buf = malloc (3*1024);
		if (! buf) {
			error ("out of memory in run");
			quit ();
		}
	}
	sprintf (buf, s, a, b, c, d, e, f);
	fflush (stdout);
	status = system (buf);
	fseek (stdout, 0L, 2);
	return (status);
}

mknewgroups ()
{
	FILE *fd;
	char *name;
	int newgr;

	new_lock();
	fd = f2open (SERVDIR, "newgroups", "a");
	if (! fd) {
		error ("cannot append to %s/newgroups", SERVDIR);
		quit ();
	}
	newgr = 0;
	for (name=firstactive(); name; name=nextactive(name)) {
		if (grouptag (name))
			continue;
		if (!checkglist (name, "@" ))
			continue;
		fprintf (fd, "%s\n", name);
		++newgr;
		if (newgr >= MAXNEWGROUPS) {
			error ("too many new groups: %d, all articles skipped",
				newgr);
			fclose (fd);
			f2unlink (SERVDIR, "newgroups");
			new_unlock();
			return;
		}
	}
	fclose (fd);
	new_unlock();
}

mkoldgroups ()
{
	FILE *fd;
	long g;
	char *name;
	int count;

	new_lock();
	fd = f2open (SERVDIR, "oldgroups", "a");
	if (! fd) {
		error ("cannot append to %s/oldgroups", SERVDIR);
		quit ();
	}
	count = 0;
	for (g=firstgroup(); g; g=nextgroup(g)) {
		name = groupname (g);
		if (! name)
			continue;
		if (! getactive (name) && count++ < MAXNEWGROUPS)
			fprintf (fd, "%s\n", name);
	}
	fclose (fd);
	new_unlock();
}

updategroups ()
{
	register long g;
	char *name;
	long last;
	struct activetab *a;
	int new_info = 0;

	for (name=firstactive(); name; name=nextactive(name)) {
		if (grouptag (name))
			continue;
		a = getactive (name);
		if (! a)
			continue;
		last = a->last;
		addgroup (name, last);
		new_info = 1;
	}
	for (g=firstgroup(); g; g=nextgroup(g)) {
		name = groupname (g);
		if (! getactive (name))
			delgroup (name);
		new_info = 1;
	}
	if ( new_info )
	       f2unlink(SERVDIR,GROUPINFO);
}

mknewarticles ()
{
	long g, last, olast, totalnew, n;
	int ns, nused;
	struct activetab *a;
	char *name;
	struct subscrtab *s, *ss;

	/*
	 * Подсчитываем общее число новых статей.
	 */
	totalnew = 0;
	for (g=firstgroup(); g; g=nextgroup(g)) {
		name = groupname (g);
		if (! name)
			continue;
		a = getactive (name);
		if (! a )
			continue;
		last = a->last;
		olast = grouplast (g);
		if ( last < olast ) continue;
		totalnew += last - olast;
	}
	/*
	 * Если новых статей слишком много, видимо, был сбой.
	 * Статьи не рассылаем, синхронизируем groups с active.
	 */
	if (totalnew > MAXNEWARTS) {
		error ("too many new articles: %ld, all articles skipped",
			totalnew);
		for (g=firstgroup(); g; g=nextgroup(g)) {
			a = getactive (groupname (g));
			if (! a)
				continue;
			last = a->last;
			setgrouplast (g, last);
		}
		return;
	}
	messg ("total %ld new articles", totalnew);
	nused = 0;
	for (g=firstgroup(); g; g=nextgroup(g)) {
		name = groupname (g);
		if (! name)
			continue;
		a = getactive (name);
		if (! a)
			continue;
		last = a->last;
		olast = grouplast (g);
		if (olast < 0)
			olast = 0;
		if (last <= olast)
			continue;
		s = groupsubscr (g, &ns);
		if (! ns) {
			if (last != olast)
				setgrouplast (g, last);
			continue;
		}
		ss = (struct subscrtab *) malloc (ns *
			sizeof (struct subscrtab));
		if (! ss) {
			error ("no memory for subscrtab for group %s",
				groupname (g));
			continue;
		}
		memcpy (ss, s, ns * sizeof (struct subscrtab));
		s = ss;
		if (debug)
			printf ("Group %s %d..%d x%d\n", groupname (g),
				olast+1, last, ns);
		for (n=olast+1; n<=last && nart<MAXARTS; ++n, ++nused)
			storeinfo (g, n, s, ns);
		setgrouplast (g, n-1);
		free ((char *) s);
	}
	messg ("%d articles used", nused);
	messg ("total %d subscriptions", nart);
}

nocrosspost ()
{
	register struct feedtab *fp, *fq;

	if (! nart)
		return;

	/* Sort articles by (1) message ID, (2) user, (3) rev. mode. */
	qsort ((char *) arttab, nart, sizeof (struct feedtab), cmpmsgid);

	/* Delete articles with the same msgID and user (cross-postings). */
	for (fq=arttab, fp=fq+1; fp<arttab+nart; ++fp) {
		if (fp->user==fq->user && !strcmp (fp->msgid, fq->msgid))
			continue;
		++fq;
		if (fp == fq)
			continue;
		/* DON'T free message-id, it can be cross-referenced
		 * free (fq->msgid);
		 */
		*fq = *fp;
	}
	nart = fq - arttab + 1;
	if (debug)
		printf ("Detected %d unique message-ids\n", nart);
}

updatenewarts ()
{
	FILE *fd [FILEMASK+1];
	char artfname [16];
	register struct feedtab *p, *q;
	int n;

	if (! nart)
		return;
	new_lock();
	for (n=0; n<=FILEMASK; ++n) {
		strcpy (artfname, "newX");
		artfname [3] = n + (n<10 ? '0' : ('A'-10));
		fd [n] = f2open (SERVDIR, artfname, "a");
		if (! fd [n]) {
			error ("cannot append to %s/%s", SERVDIR, artfname);
			quit ();
		}
	}
	for (p=arttab; p<arttab+nart && p->mode==MSUBS; p=q) {
		for (q=p+1; q<arttab+nart && q->mode==MSUBS &&
			p->group==q->group && p->issue==q->issue; ++q);
		storeart (p->group, p->issue, p, q, fd);
	}
	for (n=0; n<=FILEMASK; ++n) {
		if (ftell (fd [n]) == 0) {
			/* file is empty -- delete it */
			strcpy (artfname, "newX");
			artfname [3] = n + (n<10 ? '0' : ('A'-10));
			f2unlink (SERVDIR, artfname);
		}
		fclose (fd [n]);
	}
	new_unlock();
}

addtab (p, mode)
struct feedtab *p;
{
	switch (mode) {
	case MFEED:
		if (nfeed >= feedtablen) {
			feedtablen += 64;
			feedtab = (struct feedtab *) realloc ((char *) feedtab,
				(unsigned) (feedtablen * sizeof (struct feedtab)));
			if (! feedtab) {
				error ("out of memory in splittab (feed)");
				return;
			}
		}
		feedtab[nfeed] = *p;
		feedtab[nfeed++].mode = mode;
		break;
	case MPACK:
		if (npack >= packtablen) {
			packtablen += 64;
			packtab = (struct feedtab *) realloc ((char *) packtab,
				(unsigned) (packtablen * sizeof (struct feedtab)));
			if (! packtab) {
				error ("out of memory in splittab (pack)");
				return;
			}
		}
		packtab[npack] = *p;
		packtab[npack++].mode = mode;
		break;
	case MGPACK:
		if (ngpack >= gpacktablen) {
			gpacktablen += 64;
			gpacktab = (struct feedtab *) realloc ((char *) gpacktab,
				(unsigned) (gpacktablen * sizeof (struct feedtab)));
			if (! gpacktab) {
				error ("out of memory in splittab (gpack)");
				return;
			}
		}
		gpacktab[ngpack] = *p;
		gpacktab[ngpack++].mode = mode;
		break;
	}
}

splittab ()
{
	register struct feedtab *p;

	for (p=arttab; p<arttab+nart; ++p)
		switch (p->mode) {
		case MFEED:
		case MPACK:
		case MGPACK:
			addtab (p, p->mode);
			break;
		}
}

storeart (g, n, p, q, fd)
long g, n;
struct feedtab *p, *q;
FILE **fd;
{
	char from [256];
	char subject [512];
	char *fromaddr, *gname;
	long size;

	if (! scanarticle (g, n, from, subject, &size, (char *) 0))
		return;
	gname = groupname (g);
	if (! gname)
		return;
	gname = strcopy (gname);
	fromaddr = getsendername (from);
	/*
	 * Режим уведомления:
	 * Кому
	 * Группа
	 * Номер статьи
	 * Обьем статьи
	 * ИД
	 * От кого
	 * Тема
	 */
	for (; p<q; ++p) {
		char *unam = fetchuser (p->user);
		char *up;
		int findex;

		if (! unam) {
			error ("null user name 0x%x (group %s, art %d)",
				p->user, gname, n);
			continue;
		}
		/*
		 * Так как распределение по именам очень неоднородно,
		 * попытаемся раскидать по именам машин дополнительно
		 */
		findex = unam[0];
		up = strchr (unam, '@');
		if (up && up [1])
			findex += up [1];
		fprintf (fd [findex & FILEMASK],
			"%s %s %ld %ld %s %s\n", unam,
			gname, n, size, fromaddr, subject);
	}
	free (gname);
	free (fromaddr);
}

sendgpack ()
{
	register struct feedtab *p, *q;

	/* Articles are already sorted by (1) group, (2) issue, (3) user. */
	for (p=gpacktab; p<gpacktab+ngpack; p=q) {
		for (q=p+1; q<gpacktab+ngpack && p->group==q->group; ++q);
		/*
		 * Если статей много, шлем в групповом режиме
		 * (один пакет на группу).  Если мало, переходим
		 * в негрупповой режим.
		 */
		if (q-p > 10)
			send1gpack (p, q);
		else while (p < q)
			addtab (p++, MPACK);
	}
}

send1gpack (p, q)
register struct feedtab *p, *q;
{
	struct feedtab *r;
	long *utab, *fp, *fq;
	int nutab, utablen;
	long size, curtime;
	FILE *fd;
	char *gname, *gclass;
	char timestr [30];

	utab = (long *) malloc (sizeof (long));
	if (! utab) {
		error ("out of memory in send1gpack");
		return;
	}
	utablen = 1;
	nutab = 0;
	gname = groupname (p->group);
	gname = strcopy (gname);
	gclass = groupclass (gname);
	time (&curtime);
	strcpy (timestr, ctime (&curtime));

	/* Collect users */
	for (r=p; r<q; ++r) {
		if (nutab >= utablen) {
			utablen += 64;
			utab = (long *) realloc ((char *) utab,
				(unsigned) (utablen * sizeof (long)));
			if (! utab) {
				error ("out of memory in send1gpack.");
				free (gname);
				return;
			}
		}
		utab[nutab] = r->user;
		++nutab;
	}

	/* Сортируем пользователей по номеру */
	qsort ((char *) utab, nutab, sizeof (long), cmplong);

	/* Убираем повторяющиеся адреса */
	for (fq=utab, fp=fq+1; fp<utab+nutab; ++fp) {
		if (*fp == *fq)
			continue;
		++fq;
		if (fp == fq)
			continue;
		*fq = *fp;
	}
	nutab = fq - utab + 1;

	if (debug)
		printf ("Send pack %s (%s) to %d subscribers\n",
			gname, gclass, nutab);
	while (p < q) {
		fd = fopen (tmpname, "w+");
		if (! fd) {
			error ("cannot create %s", tmpname);
			break;
		}
		unlink (tmpname);
		size = 0;
		for (; p<q; ++p) {
			if (size >= BATCHSIZE * 1024L)
				break;
			if (p<q-1 && p->issue==p[1].issue)
				continue;
			if (! existarticle (p->group, p->issue))
				continue;
			if (size)
				fprintf (fd, "\n");
			fprintf (fd, "From_%s %s", MYADDRESS, timestr);
			batcharticle (p->group, p->issue, fd, gclass);
			size = ftell (fd);
		}
		if (! size) {
			fclose (fd);
			break;
		}
		fflush (fd);
		rewind (fd);
		sendbatch (fd, utab, nutab, gname, gclass);
		fclose (fd);
	}
	free ((char *) utab);
	free (gname);
}

sendpack ()
{
	register struct feedtab *p, *q;

	if (! npack)
		return;
	for (p=packtab; p<packtab+npack; ++p)
		p->mode = groupcindex (groupname (p->group));

	/* Sort articles by (1) user, (2) class, (3) group, (4) issue. */
	qsort ((char *) packtab, npack, sizeof (struct feedtab), cmppacktab);

	for (p=packtab; p<packtab+npack; p=q) {
		for (q=p+1; q<packtab+npack && p->user==q->user &&
			p->mode==q->mode; ++q);
		/* Если всего одна статья - шлем в режиме FEED */
		if (q-p > 1)
			send1pack (p->user, p, q, groupiclass (p->mode));
		else
			addtab (p, MFEED);
	}
}

send1pack (u, p, q, gclass)
long u;
register struct feedtab *p, *q;
char *gclass;
{
	long size, curtime;
	FILE *fd;
	char timestr [30];

	time (&curtime);
	strcpy (timestr, ctime (&curtime));

	if (debug)
		printf ("Send pack %s (%d) to %s\n",
			gclass, q-p, fetchuser (u));
	while (p < q) {
		fd = fopen (tmpname, "w+");
		if (! fd) {
			error ("cannot create %s", tmpname);
			break;
		}
		unlink (tmpname);
		size = 0;
		for (; p<q; ++p) {
			if (size >= BATCHSIZE * 1024L)
				break;
			if (! existarticle (p->group, p->issue))
				continue;
			if (size)
				fprintf (fd, "\n");
			fprintf (fd, "From_%s %s", MYADDRESS, timestr);
			batcharticle (p->group, p->issue, fd, gclass);
			size = ftell (fd);
		}
		if (! size) {
			fclose (fd);
			break;
		}
		fflush (fd);
		rewind (fd);
		send1batch (fd, fetchuser (u), gclass);
		fclose (fd);
	}
}

sendfeed ()
{
	register struct feedtab *p, *q, *r;
	char *buf, *b;

	if (! nfeed)
		return;
	buf = malloc (NADDRPERCMD * 50);
	if (! buf) {
		error ("out of memory in sendfeed");
		quit ();
	}

	/*
	 * Send articles as one letter to as many users as possible:
	 *    sendmail user1 user2...userN < article
	 * Number of addresses in command line is limited,
	 * so we will use no more than, say, 30 addreses per
	 * one call.
	 */
	for (p=feedtab; p<feedtab+nfeed; p=q) {
		for (q=p+1; q<feedtab+nfeed && p->group==q->group &&
			q<p+NADDRPERCMD && p->issue==q->issue; ++q);
		b = buf;
		for (r=p; r<q; ++r) {
			strcpy (b, fetchuser (r->user));
			b += strlen (b);
			*b++ = ' ';
		}
		*--b = 0;
		sendarticle (p->group, p->issue, q-p, buf);
	}
	free (buf);
}

storeinfo (g, artnum, s, ns)
long g, artnum;
struct subscrtab *s;
int ns;
{
	int feedlimit, n;
	char msgid [256], *msgidptr;
	long size;

	if (! ns)
		return;
	*msgid = 0;
	if (! scanarticle (g, artnum, (char *) 0, (char *) 0, &size, msgid))
		return;
	if (*msgid != '<')
	{
		if (*msgid)
			error ("bad message-id '%s' in %s/%ld", msgid,
				groupname (g), artnum);
		else
			error ("article without message-id: %s/%ld",
				groupname (g), artnum);
		return;
	}
	msgidptr = strcopy (msgid);
	for (n=0; n<ns; ++n, ++s) {
		switch (s->mode) {
		default:        continue;
		case 'A':       feedlimit = 2;          break;
		case 'B':       feedlimit = 3;          break;
		case 'C':       feedlimit = 5;          break;
		case 'D':       feedlimit = 8;          break;
		case 'E':       feedlimit = 10;         break;
		case 'F':       feedlimit = 15;         break;
		case 'G':       feedlimit = 20;         break;
		case 'H':       feedlimit = 30;         break;
		case 'I':       feedlimit = 50;         break;
		case 'J':       feedlimit = 80;         break;
		case 'K':       feedlimit = 200;        break;
		case 'f':       feedlimit = FEEDLIMIT;  break;
		case 's':       feedlimit = 0;          break;
		}
		if (nart >= arttablen) {
			arttablen += 64;
			arttab = (struct feedtab *) realloc ((char *) arttab,
				(unsigned) (arttablen * sizeof (struct feedtab)));
			if (! arttab) {
				error ("out of memory in storeinfo");
				return;
			}
		}
		/*
		 * 1) ИД
		 * 2) Номер пользователя
		 * 3) Номер группы
		 * 4) Номер статьи
		 * 5) Режим подписки
		 */
		arttab[nart].msgid = msgidptr;
		arttab[nart].user = s->tag;
		arttab[nart].group = g;
		arttab[nart].issue = artnum;
		if (size >= feedlimit*1024)
			arttab[nart].mode = MSUBS;
		else if (! (userflags (s->tag) & UFLAGPACK))
			arttab[nart].mode = MFEED;
		else if (s->mode == 'f')
			arttab[nart].mode = MGPACK;
		else
			arttab[nart].mode = MPACK;
		++nart;
	}
}

scanarticle (g, n, from, subj, size, msgid)
long g, n;
char *from, *subj, *msgid;
long *size;
{
	char filename [512];
	register char *p, *s;
	FILE *fd;

	strcpy (filename, NEWSSPOOLDIR);
	p = filename + strlen (filename);
	*p++ = '/';
	for (s=groupname(g); *s; ++s)
		*p++ = *s=='.' ? '/' : *s;
	sprintf (p, "/%ld", n);

	fd = fopen (filename, "r");
	if (! fd)
		return (0);     /* article is already expired */

	if (from)
		*from = 0;
	if (subj)
		*subj = 0;
	scanheader (fd);

	if (from) {
		if (h_from)
			strncpy (from, h_from, 255), from [255] = 0;
		else if (h_reply_to)
			strncpy (from, h_reply_to, 255), from [255] = 0;
		else if (h_resent_from)
			strncpy (from, h_resent_from, 255), from [255] = 0;
		else if (h_sender)
			strncpy (from, h_sender, 255), from [255] = 0;
	}
	if (subj && h_subject)
		strncpy (subj, h_subject, 255), subj [255] = 0;

	if (msgid)
		*msgid = 0;
	if (h_message_id && msgid)
		strncpy (msgid, h_message_id, 255), msgid [255] = 0;

	*size = filesize (filename);
	freeheader ();
	fclose (fd);
	return (1);
}
