/*
 * $Header: commands.c,v 2.0 92/08/05 08:42:36 vak Exp $
 * $Log:	commands.c,v $
 * Revision 2.0  92/08/05  08:42:36  vak
 * 
 * 
 * Revision 1.2  92/06/19  22:09:32  usenet
 * Добавлен RFEED 100
 * 
 * Revision 1.2  92/06/19  22:07:50  usenet
 * News Server
 * 
 */
/*
 * Краткое описание команд:
 *
 * HELP                         - Выдать файл helpfile.
 * LIST                         - Выдать список групп.
 * LIST иерархия...             - Выдать список групп в иерархии.
 * GSTAT                        - Выдать инф. о подписке для групп.
 * USTAT                        - Выдать инф. о подписке для всех пользователей
 * USTAT имя                    - Выдать инф. о подписке для указанных польз.
 * GSTAT иерархия...            - Выдать инф. о подписке для групп в иерархии.
 * SUBSCRIBE группа...          - Подписаться на группу в режиме уведомления.
 * FEED группа...               - Подписаться на группу.
 * RFEED длина группа...        - Подписаться на группу в ограниченном режиме.
 * UNSUBSCRIBE группа...        - Отменить подписку на группу.
 * UNSUBSCRIBE all              - Отменить подписку на все группы.
 * FORGET                       - Исключить пользователя из базы данных.
 * CHECK                        - Выдать список подписанных групп.
 * GROUP группа                 - Перейти в указанную группу.
 * ARTICLE номер                - Выслать указанную статью.
 * INDEX [номер [номер]]        - Выдать список статей текущей группы.
 * LINDEX [номер [номер]]       - Выдать подробный список статей.
 * DEBUG                        - Включить режим отладки.
 * DEBUG OFF                    - Выключить режим отладки.
 * PACK                         - Установить режим упаковки.
 * PACK OFF                     - Выключить режим упаковки.
 * SEND пакет                   - Выслать указанный пакет.
 * SEND                         - Выдать список пакетов.
 * TIME                         - Выдать текущее время.
 * AGING                        - Включить старение
 * AGING OFF                    - выключить старение
 * QUIT                         - Конец потока команд.
 *
 * Copyright (C) 1991 Сергей Вакуленко, <vak@kiae.su>.
 */

# include <stdio.h>
# include "config.h"
# include "server.h"
# include "groups.h"
# include "header.h"
# include "lang.h"

# define TMPFNAME "/tmp/NScXXXXXX"

long group;

int command_count;
int debug = 0;

static pack = -1;
static FILE *tmpfd;
static char tmppattern [] = TMPFNAME;
static char *tmpname;
static char *gclass;

extern lang;
extern needreply;
extern char *sender;
extern char *strlower (), *time2a (), *time2fname (), *mktemp (), *ctime ();
extern char *strcpy (), *strcat (), *size2a (), *cut25 (), *groupclass ();
extern char *getsendername (), *canondate (), *strcopy (), *strchr();
extern long time (), filesize ();
extern FILE *popen (), *f2open();

int doquit (), dohelp (), dolist (), dosubscribe (), dounsubscribe ();
int dofeed (), docheck (), dogroup (), doarticle (), doindex (), dorfeed ();
int dodebug (), dopack (), dosend (), doforget (), dogstat (), doustat ();
int dotime (), doaging();

struct cmdtab cmdtable [] = {
	"article",      doarticle,
	"check",        docheck,
	"feed",         dofeed,
	"rfeed",        dorfeed,
	"forget",       doforget,
	"group",        dogroup,
	"help",         dohelp,
	"index",        doindex,
	"lindex",       doindex,
	"list",         dolist,
	"gstat",        dogstat,
	"ustat",        doustat,
	"quit",         doquit,
	"debug",        dodebug,
	"pack",         dopack,
	"send",         dosend,
	"subscribe",    dosubscribe,
	"unsubscribe",  dounsubscribe,
	"time",         dotime,
	"aging",        doaging,

	0,              0,
};

doquit ()
{
	if (! command_count) {
		message (no_commands_entered);
		dohelp ();
	}
	if (tmpfd)
		flushbatch ();
	exitloop ();
}

dounknown ()
{
}

dotime ()
{
	long curtime;

	time (&curtime);
	printf (ctime (&curtime));
}

dodebug (ac, av)
char **av;
{
	++command_count;
	++needreply;
	if (ac <= 1)
		++debug;
	else
		debug = 0;
}

dopack (ac, av)
char **av;
{
	long u, flags;
	int oldpack;

	++command_count;
	if (ac <= 1)
		pack = 1;
	else
		pack = 0;
	printf (pack ? pack_turned_on : pack_turned_off);
	u = usertag (sender);
	if (! u)
		return;
	if (debug)
		printf (">>> Sender=%s\n>>> Username=%s\n",
			sender, username (u));
	flags = userflags (u);
	oldpack = (flags & UFLAGPACK) != 0;
	if (oldpack == pack)
		return;
	if (pack)
		flags |= UFLAGPACK;
	else
		flags &= ~UFLAGPACK;
	setuserflags (u, flags);
}

doaging (ac, av)
char **av;
{
	long u, flags;
	int oldflags;
	int aging;

	++command_count;
	if (ac <= 1)
		aging = 1;
	else
		aging = 0;
	printf (aging? aging_turned_on : aging_turned_off);
	u = usertag (sender);
	if (! u)
		return;
	if (debug)
		printf (">>> Sender=%s\n>>> Username=%s\n",
			sender, username (u));
	oldflags = flags = userflags (u);
	flags &= ~UFLAGAGE_M;
	if ( aging == 0 )  {
	       flags |= UFLAGAGE_N;
	}
	if (oldflags == flags)
		return;
	setuserflags (u, flags);
}

dohelp ()
{
	char *hfile = 0;
	FILE *fd;

	++command_count;
	++needreply;
	if (lang=='r' && !f2access (SERVDIR, "rhelp", 4))
		hfile = "rhelp";
	else if (! f2access (SERVDIR, "help", 4))
		hfile = "help";
	if (! hfile || ! (fd = fopen (hfile, "r")))
		fatal (no_help_available);
	help (fd);
	fclose (fd);
}

dosend (ac, av)
char **av;
{
	++command_count;
	++needreply;
	if (ac <= 1) {
		run ("/bin/ls -l %s/pub", SERVDIR);
		return;
	}
	while (*++av) {
		char packname [100];

		strlower (*av);
		strcpy (packname, "pub/");
		strcat (packname, *av);
		if (f2access (SERVDIR, packname, 4)) {
			printf (no_such_package, *av);
			continue;
		}
		run ("/bin/cat %s", packname);
	}
}

static matchtab (name, tab, n)
char *name;
char **tab;
{
	int i;

	for (i=0; i<n; ++i)
		if (match (name, tab[i]))
			return (1);
	return (0);
}

static prstat (fd, ns, nf, nr)
FILE *fd;
{
	fprintf (fd, ns ? "\t%d" : "\t-", ns);
	fprintf (fd, nf ? "\t%d" : "\t-", nf);
	fprintf (fd, nr ? "\t%d" : "\t-", nr);
}

static prgstat (fd, g, gname)
FILE *fd;
long g;
char *gname;
{
	int na, ns, nf, nr;
	struct activetab *a;

	if ( !g ) return;
	if (! gname)
		gname = groupname (g);
	gname = strcopy (gname);
	a = getactive (gname);
	na = a ? a->last - a->first + 1 : 0;
	groupstat (g, &ns, &nf, &nr);
	fprintf (fd, "%-39s", gname);
	fprintf (fd, na ? "\t%d" : "\t-", na);
	prstat (fd, ns, nf, nr);
	fputc ('\n', fd);
	free (gname);
}


static prgroup (fd, g, gname)
FILE *fd;
long g;
char *gname;
{
	char *info;

	if ( !g ) return;
	if (! gname)
		gname = groupname (g);
	gname = strcopy (gname);
	info = groupinfo (g);
	if ( !info || strlen(info) < 3 )
		fprintf (fd, "%s\n", gname);
	else
		fprintf (fd, "%-23s\t%s\n", gname, info);
	free (gname);
}

static FILE *open_ginfo()
{
	int len;
	long g;
	FILE *fd;
	register int i;
	int count = 0;
	char buf[256];

	fflush (stdout);
	fflush (stderr);
next:
	if ( fd = f2open(SERVDIR, GROUPINFO, "r" ))
	       return(fd);
	if ( count++ > 2 )
	       return(0);
	/* Тут есть потенциальная опасность, если работает несколько server-ов.
	 * В этом случае наш сервер может переименовать в info файл, который
	 * строится следующим сервером.
	 * и некоторое время этот файл будет кривым.
	 * Но пока ее проигнорируем
	 */
	f2unlink(SERVDIR, GROUPINFO_T);
	sprintf(buf,"exec sort > %s/%s",SERVDIR,GROUPINFO_T);
	fd = popen(buf,"w");
	if ( !fd ) goto next;
	for (g=firstgroup(); g; g=nextgroup(g))
	{
	       prgroup (fd, g, (char *)0);
	}
	pclose(fd);
	f2rename(SERVDIR,GROUPINFO_T,SERVDIR,GROUPINFO);
	goto next;
}

static listgroups (ac, av, prfun)
char **av;
int (*prfun) ();
{
	int len;
	long g;
	FILE *fd;
	char *gname;
	register int i;

	++command_count;
	++needreply;
	fflush (stdout);
	fflush (stderr);
	fd = popen ("exec sort", "w");
	if ( !fd ) return(0);
	if (ac <= 1) {
		for (g=firstgroup(); g; g=nextgroup(g))
		{
		       gname = groupname(g);
		       (*prfun) (fd, g, gname);
		}
		prfun(fd, (long)0, (char *)0);
		pclose(fd);
		return;
	}
	while (*++av) {
		len = strlen (*av);
		for (g=firstgroup(); g; g=nextgroup(g)) {
			gname = groupname (g);
			if (! gname)
				continue;
			if (! strncmp (*av, gname, len) &&
			    (gname[len]==0 || gname[len]=='.'))
				(*prfun) (fd, g, gname);
		}
	}
	prfun(fd, (long)0, (char *)0);
	pclose (fd);
}

#define MAX1LIST  50
dolist (ac, av)
char **av;
{
	FILE *fd, *pd;
	char buf[256];
	register char *p;
	char old_g[256];
	int g_s, g_i, g_d, len;

	++command_count;
	++needreply;
	fd = open_ginfo();
	if ( !fd ) {
	       error("Can not open/create ginfo file %s",GROUPINFO);
	       printf("Sorry, repeat this command later\n");
	       return;
	};
	if ( ac <= 1 )
	{
	       printf(short_list_of_news_groups);
	       printf ("%-23s\t%s\n", "Group name", "Description");
	       printf ("-----------------------------------\n");
	       g_s = g_i = g_d = 0;
	       len = 1;
	       strcpy(old_g," ");
	       while ( fgets(buf, sizeof buf, fd) )
	       {
		       len = 0;
		       for(p = buf; *p && *p != ' ' && *p != '\n' && *p != '\t'; p++)
			       if ( *p == '.' /* && len++ == 1 */ ) break;
		       len = p - buf;
		       if ( len > (sizeof old_g)-1) len = (sizeof old_g) - 1;
		       if ( strncmp(buf,old_g,len) != 0 )
		       {
			       if ( g_s )
				       printf(" ****** %d groups (%s.*) SKIPPED...\n",g_s,old_g);
			       g_s = g_i = g_d = 0;
			       strncpy(old_g,buf,len);
			       old_g[len] = 0;
		       }
		       p = strchr(buf,'\t');
		       if ( p && g_i++ < MAX1LIST )
			       fputs(buf,stdout);
		       else if (!p && g_d < MAX1LIST )
			       fputs(buf,stdout);
		       else g_s++;
		       g_d++;
	       }
	       if ( g_s )
		       printf(" **** %d groups (%s.*) SKIPPED\n",g_s,old_g);
	       fclose(fd);
	       return;
	};
	printf ("%-23s\t%s\n", "Group name", "Description");
	printf ("-----------------------------------\n");
	while ( ac-- > 1 )
	{
	       av++;
	       for(p = *av; *p; p++ )
	       {
		     if ( *p != '.' && *p != '-' && ( *p < 'a' || *p > 'z' ))
		       *p = 0;
		       break;
	       }
	       sprintf(buf,"grep '^%.64s'", *av);
	       fflush(stdout);
	       pd = popen(buf,"w");
	       if ( !pd ) {
		       error("Can not open pipe with %s",buf);
		       printf("System error: can't make list for %s\n", *av);
		       av++;
		       continue;
	       }
	       fseek(fd, 0l, 0);
	       while ( fgets(buf, sizeof buf, fd) )
		     fputs(buf,pd);
	       pclose(pd);
       }
       fclose(fd);
}

dogstat (ac, av)
char **av;
{

	printf ("%-39s\t%s\t%s\t%s\t%s\n", "Group name", "Arts", "Subscr", "Feed", "Rfeed");
	printf ("---------------------------------------------------------------------\n");
	listgroups (ac, av, prgstat);
}

doustat (ac, av)
char **av;
{
	long *tab;
	struct stattab *st;
	int n, i, k;
	FILE *fd;
	char *name;

	++command_count;
	++needreply;
	--ac;
	++av;
	printf ("%-39s\t%s\t%s\t%s\t%s\n", "User address", "Total", "Subscr", "Feed", "Rfeed");
	printf ("---------------------------------------------------------------------\n");
	tab = userlist (&n);
	if (! tab)
		return;
	if (ac > 0) {
		for (i=0; i<n; ++i) {
			name = username (tab [i]);
			if (! name || ! matchtab (name, av, ac))
				tab [i] = 0;
		}
		for (k=i=0; i<n; ++i) {
			if (! tab [i])
				continue;
			if (i != k)
				tab [k] = tab [i];
			++k;
		}
		n = k;
	}
	st = userstat (tab, n);
	if (! st) {
		free ((char *) tab);
		return;
	}
	fflush (stdout);
	fflush (stderr);
	fd = popen ("exec sort -n +1", "w");
	for (i=0; i<n; ++i) {
		if (! tab [i])
			continue;
		name = username (tab [i]);
		fprintf (fd, "%-39s", name);
		k = st[i].subs + st[i].feed + st[i].rfeed;
		fprintf (fd, k ? "\t%d" : "\t-", k);
		prstat (fd, st[i].subs, st[i].feed, st[i].rfeed);
		fputc ('\n', fd);
	}
	pclose (fd);
	free ((char *) tab);
	free ((char *) st);
}

docheck (ac, av)
char **av;
{
	long u, g;
	char *gname, *feedlimit;
	struct subscrtab *s;
	int ns, n;
	int l;

	++command_count;
	++needreply;
	u = usertag (sender);
	if (! u)
	{
		printf(you_are_unknown, sender);
		return;
	}
	if (debug)
		printf (">>> Sender=%s\n>>> Username=%s\n",
			sender, username (u));
	printf(">>> Check list, user: %s\n",sender);
	l = userflags(u);
	if ( l & UFLAGPACK )
	       printf("> PACK\n");
	else
	       printf("> PACK OFF\n");
	if ( (l&UFLAGAGE_M) == UFLAGAGE_N)
	       printf("> AGING OFF\n");
	else
	       printf("> AGING\n");
	--ac;
	++av;

/* Чтоб. его! Ошибочка вышла у vak-а   */
/*        s = usersubscr (u, av, &ns); */
	  s = usersubscr (u, (char **)0, &ns);
	if (! s)
		return;

	for (n=0; n<ns; ++n, ++s) {
		gname = groupname (s->tag);
		switch (s->mode) {
		default:        continue;
		case 'A':       feedlimit = "2";        break;
		case 'B':       feedlimit = "3";        break;
		case 'C':       feedlimit = "5";        break;
		case 'D':       feedlimit = "8";        break;
		case 'E':       feedlimit = "10";       break;
		case 'F':       feedlimit = "15";       break;
		case 'G':       feedlimit = "20";       break;
		case 'H':       feedlimit = "30";       break;
		case 'I':       feedlimit = "50";       break;
		case 'J':       feedlimit = "80";       break;
		case 'K':       feedlimit = "200";      break;
		case 'f':
			printf ("> FEED %s\n",gname);
			continue;
		case 's':
			printf ("> SUBSCRIBE %-27s\n", gname);
			continue;
		}
		printf ("> RFEED %s %s\n", feedlimit, gname);
	}
}

dogroup (ac, av)
char **av;
{
	++command_count;
	if (ac < 2)
		return;
	if ( !checkglist(av[1], sender) )
	{
	       messg("group %s don't allowed for user %s",av[1], sender);
	       printf(group_dont_allow,av[1]);
	       return;
	}
	group = grouptag (av[1]);
	if (! group)
		printf (group_not_found, av[1]);
}

doarticle (ac, av)
char **av;
{
	int artnum;
	long size, curtime;
	static char timestr [30];
	char *gname = 0;
	char *newgclass;

	++command_count;
	if (ac < 2)
		return;
	if (av[1][0] == '<') {
		sendmsgid (av[1], sender);
		return;
	}
	if (! group) {
		printf (no_group_selected);
		return;
	}
	artnum = atoi (av[1]);
	if (! existarticle (group, artnum)) {
		printf (no_such_article);
		return;
	}
	if (pack < 0) {
		long u;

		u = usertag (sender);
		if (u)
			pack = (userflags (u) & UFLAGPACK) != 0;
		else
			pack = 0;
	}
	gname = strcopy (groupname (group));
	if (! pack) {
justsend:       sendarticle (group, artnum, 1, sender);
		if (gname)
			free (gname);
		return;
	}
	if (! tmpfd) {
		if (! initbatch ())
			goto justsend;
		gclass = groupclass (gname);
	}
	if (! *timestr) {
		time (&curtime);
		strcpy (timestr, ctime (&curtime));
	}
	size = ftell (tmpfd);
	if (size > BATCHSIZE * 1024L) {
		flushbatch ();
		initbatch ();
		size = 0;
	}
	newgclass = groupclass (gname);
	if (strcmp (gclass, newgclass)) {
		flushbatch ();
		if (! initbatch ())
			goto justsend;
		gclass = newgclass;
	}
	if (size)
		fprintf (tmpfd, "\n");
	fprintf (tmpfd, "From_%s %s", MYADDRESS, timestr);
	batcharticle (group, artnum, tmpfd, gclass);
	if (gname)
		free (gname);
}

flushbatch ()
{
	if (ftell (tmpfd)) {
		fflush (tmpfd);
		rewind (tmpfd);
		send1batch (tmpfd, sender, gclass);
	}
	fclose (tmpfd);
	tmpfd = 0;
}

initbatch ()
{
	if (! tmpname)
		tmpname = mktemp (tmppattern);
	tmpfd = fopen (tmpname, "w+");
	if (! tmpfd)
		return (0);
	unlink (tmpname);
	return (1);
}

doindex (ac, av)
char **av;
{
	struct activetab *a;
	int ndeleted, n, nmin=-1, nmax=99999999;
	int longflag;

	++command_count;
	++needreply;
	if (! group) {
		printf (no_group_selected);
		return;
	}
	a = getactive (groupname (group));
	if (! a) {
		printf (no_such_group, groupname (group));
		return;
	}
	longflag = av[0][0] == 'l';
	if (ac >= 3)
		nmax = atoi (av [2]);
	if ( nmax < 0 )
	       nmax = a->last + nmax;
	if (nmax > a->last)
		nmax = a->last;
	if (ac >= 2)
		nmin = atoi (av [1]);
	if ( nmin < 0 )
	       nmin = a->last + nmin;
	if (nmin < a->first)
		nmin = a->first;
	if (debug)
		printf ("min=%d (%d)  max=%d (%d)\n", nmin, a->first, 
			nmax, a->last);
	for (n=nmax, ndeleted=0; n>=nmin && ndeleted<100; --n)
		if (! indexarticle (group, n, longflag))
			++ndeleted;
}

dosubscribe (ac, av)
char **av;
{
	++command_count;
	++needreply;
	if (ac < 2)
		return;
	dosub1 (++av, 's');
}

dofeed (ac, av)
char **av;
{
	++command_count;
	++needreply;
	if (ac < 2)
		return;
	dosub1 (++av, 'f');
}

dorfeed (ac, av)
char **av;
{
	int feedlimit, feedmode;

	++command_count;
	++needreply;
	if (ac < 3)
		return;
	feedlimit = atoi (*++av);
	if (feedlimit < 2)
		feedmode = 'f';
	else if (feedlimit < 3)
		feedmode = 'A';
	else if (feedlimit < 5)
		feedmode = 'B';
	else if (feedlimit < 8)
		feedmode = 'C';
	else if (feedlimit < 10)
		feedmode = 'D';
	else if (feedlimit < 15)
		feedmode = 'E';
	else if (feedlimit < 20)
		feedmode = 'F';
	else if (feedlimit < 30)
		feedmode = 'G';
	else if (feedlimit < 50)
		feedmode = 'H';
	else if (feedlimit < 80)
		feedmode = 'I';
	else if ( feedlimit > 100 )
		feedmode = 'K';
	else
		feedmode = 'J';
	dosub1 (++av, feedmode);
}

dosub1 (av, smode)
char **av;
{
	long u, g;

	u = usertag (sender);
	if (! u)
		u = adduser (sender);
	if (! u)
		return;
	if (debug)
		printf (">>> Sender=%s\n>>> Username=%s\n",
			sender, username (u));
	for (; *av; ++av) {
		if (!checkglist (*av, sender ) )
		{
		       messg("group %s don't allowed for user %s",*av, sender);
		       printf(group_dont_allow,*av);
		       continue;
		}
		g = grouptag (*av);
		if (! g) {
			printf (group_not_found, *av);
			continue;
		}
		subscribe (u, g, smode);
		messg ("user %s subscribed to %s, mode %s", sender, *av,
			smode=='f' ? "feed" : smode=='s' ? "notify" : "rfeed");
		printf (user_subscribed_to_group,
			sender, *av);
		printf (smode=='f' ? in_feed_mode :
			smode=='s' ? in_notify_mode : in_rfeed_mode);
	}
}

dounsubscribe (ac, av)
char **av;
{
	long u, g;

	++command_count;
	++needreply;
	if (ac < 2)
		return;
	u = usertag (sender);
	if (! u) {
		printf (you_are_not_subscribed);
		return;
	}
	if (debug)
		printf (">>> Sender=%s\n>>> Username=%s\n",
			sender, username (u));
	++av;
	strlower (*av);
	if (! strcmp (*av, "all")) {   /* Just as 'remove' */
		if (deluser (sender)) {
			messg ("user %s removed", sender);
			printf (user_removed_from_list, sender);
		} else
		       printf (you_are_not_registered);
		return;
	}
	for (; *av; ++av) {
		g = grouptag (*av);
		if (! g) {
			printf (group_not_found, *av);
			continue;
		}
		if (! unsubscribe (u, g)) {
			printf (you_are_not_subscribed_to_group, *av);
			continue;
		}
		messg ("user %s unsubscribed from %s", sender, *av);
		printf (user_unsubscribed_from_group, sender, *av);
	}
}

doforget ()
{
	++command_count;
	++needreply;
	if (deluser (sender)) {
		messg ("user %s removed", sender);
		printf (user_removed_from_list, sender);
	} else
		printf (you_are_not_registered);
}

indexarticle (g, n, longflag)
long g;
{
	char filename [512];
	register char *p, *s;
	long size;
	FILE *fd;

	strcpy (filename, NEWSSPOOLDIR);
	p = filename + strlen (filename);
	*p++ = '/';
	s = groupname (g);
	if (! s)
		return (0);
	for (; *s; ++s)
		*p++ = *s=='.' ? '/' : *s;
	sprintf (p, "/%d", n);

	fd = fopen (filename, "r");
	if (! fd)
		return (0);
	scanheader (fd);
	fclose (fd);
	size = filesize (filename);

	if (longflag) {
		printf ("Issue: %d\n", n);
		if (h_subject)
			printf ("Subject: %s\n", h_subject);
		if (h_from)
			printf ("From: %s\n", h_from);
		printf ("Date: %s\n", canondate (h_date));
		if (h_message_id)
			printf ("Message-ID: %s\n", h_message_id);
		printf ("Size: %s\n", size2a (size));
		printf ("\n");
	} else {
		char *sendername = getsendername (h_from);
		int len = strlen (sendername);

		printf ("-ART %6d %-4s %-25s ", n, size2a (size),
			sendername);
		if (h_subject)
			putsubj (h_subject, len<=25 ? 35 : len<60 ? 60-len : 0);
		else
			printf ("<none>\n");
	}
	freeheader ();
	return (1);
}

sendmsgid (msgid, user)
char *msgid, *user;
{
	FILE *pd;
	char buf [256], gname [120];
	register c;
	register char *p;
	long g;
	int artnum;

	/*
	 * Find message id in history file.
	 * Use dbz utility to do this.
	 */
	if ( strlen(msgid) > 60 || strchr(msgid,'\'') )
	{
		printf (group_not_found, gname);
		return (0);
	}
	sprintf (buf, "echo '%s' | %s -ix %s/history", msgid, DBZNAME,
		NEWSLIBDIR);
	pd = popen (buf, "r");
	if (! pd) {
		error ("cannot run '%s'", buf);
		return (0);
	}

	/* Skip message-id field */
	while ((c = getc (pd)) >= 0 && c != '\t')
		continue;

	/* Skip dates field */
	while ((c = getc (pd)) >= 0 && c != '\t')
		continue;

	/* Get group name and article number */
	p = gname;
	for (;;) {
		c = getc (pd);
		if (c<0 || c=='/')
			break;
		if (p < gname + sizeof (gname) - 1)
			*p++ = c;
	}
	*p = 0;

	fscanf (pd, "%d", &artnum);
	pclose (pd);

	if (gname[0] == 0) {
		printf (no_such_article);
		return (0);
	}

	g = grouptag (gname);
	if (! g) {
		printf (group_not_found, gname);
		return (0);
	}

	printf ("Group %s, article %d\n", gname, artnum);
	sendarticle (g, artnum, 1, user);

	return (1);
}

/*
 * Print help file
 */
help (fd)
FILE *fd;
{
	register c;

	while ((c = getc (fd)) >= 0) {
		if (c == '$') {
			if ((c = getc (fd)) < 0)
				break;
			if (c == '$') {
				printf ("%s%s", MYSERVADDR, MYDOMAIN);
				continue;
			}
			putchar ('$');
		}
		putchar (c);
	}
}
