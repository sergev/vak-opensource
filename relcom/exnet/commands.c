/*
 * Краткое описание команд:
 *
 * HELP                         - Выдать файл helpfile.
 * INDEX [шаблон...]            - Выдать список лотов.
 * LONGINDEX [шаблон...]        - Выдать список лотов с описанием.
 * NEWINDEX время [шаблон...]   - Выдать список новых лотов.
 * LONGNEWINDEX время [шаблон...] - Выдать список новых лотов с описанием.
 * STATUS [шаблон...]           - Выдать инф. о подписке на лоты.
 * NEWSTATUS время [шаблон...]  - Выдать инф. о подписке на лоты.
 * ADD лот                      - Подписаться на лот.
 * ADD адрес лот                - Подписать указанный адрес.
 * DELETE лот                   - Отменить подписку на лот.
 * CREATE лот вр1 вр2 описание  - Создать лот.
 * DESTROY лот...               - Уничтожить лот.
 * SEND лот                     - Послать сообщение по лоту.
 * GET лот                      - Взять архив переписки по лоту.
 * QUIT                         - Конец потока команд.
 *
 * Ограничения:
 * 1) Доступ к серверу имеют только те, чьи адреса (шаблоны)
 * содержатся в файле users.
 * 2) В файле admins находятся адреса администраторов.
 * 3) Команды GET и DESTROY может выполнять только тот, кто создал этот лот.
 * Или администратор.
 * 4) Команда SEND для подписчиков работает только в период активности лота
 * (от T1 до T2).  Для создателя лота и администратора работает всегда.
 * 5) Команды STATUS и NEWSTATUS доступны только администратору.
 *
 * За командами CREATE и SEND следует текст письма, заканчивающийся строкой,
 * содерщащей символ '%' в первой позиции.
 *
 * Copyright (C) 1991 Сергей Вакуленко, <vak@kiae.su>.
 * All rights reserved.
 */

/*
 * Лот представляет собой группу из 4-х файлов:
 * 1) name.i - информация по лоту.  Адрес хозяина,
 * краткое описание лота, времена начала и конца торгов.
 * 2) name.d - описание лота, задаваемое владельцем.
 * 3) name.l - список адресов подписчиков.
 * 4) name.a - архив переписки по лоту.
 * Кроме этого, файл listfile содержит перечень имен всех
 * лотов и время их создания.
 * Лот считается существующим, если есть файлы ".i", ".d" и
 * запись в файле listfile.
 */

# include <stdio.h>
# include "config.h"
# include "server.h"
# include "lang.h"

long starttime;
long endtime;

static char tmpfpattern [] = "/tmp/smsXXXXXX";
static char *tmpf;

extern char sender [];
extern adminmode;
extern char *strlower (), *time2a (), *time2fname (), *mktemp (), *ctime ();
extern char *strcpy (), *strcat ();
extern long a2time (), time ();

int dohelp (), doindex (), doadd (), dodelete (), docreate (), doquit ();
int dodestroy (), donewindex (), dosend (), doget ();
int dostatus (), donewstatus ();

struct cmdtab cmdtable [] = {
	"quit",         doquit,
	"help",         dohelp,
	"create",       docreate,
	"destroy",      dodestroy,
	"index",        doindex,
	"longindex",    doindex,
	"status",       dostatus,
	"newindex",     donewindex,
	"longnewindex", donewindex,
	"newstatus",    donewstatus,
	"add",          doadd,
	"delete",       dodelete,
	"send",         dosend,
	"get",          doget,

	0,              0,
};

doquit ()
{
	printf ("\n");
	time (&endtime);
	printf (ctime (&endtime));
	exitloop ();
}

dohelp ()
{
	if (f2access (SERVDIR, "helpfile", 4))
		fatal (no_help_available);
	run ("cat %s/helpfile", SERVDIR);
	doquit ();
}

docreate (ac, av)
char **av;
{
	char *lotname;
	long t1, t2;
	char info [512];
	char line [512];
	char iname [20];
	int i;
	FILE *fd;

	if (ac < 5)
		fatal (bad_create_command);
	lotname = strlower (av [1]);
	if (strlen (lotname) > 12)
		lotname [12] = 0;

	strcpy (iname, lotname);
	strcat (iname, ".i");
	if (! access (iname, 0))
		fatal (lot_already_exist, lotname);

	t1 = a2time (av [2]);
	t2 = a2time (av [3]);
	info [0] = 0;
	for (i=4; av [i]; ++i) {
		if (info [0])
			strcat (info, " ");
		strcat (info, av [i]);
	}

	fd = fopen (iname, "w");
	if (! fd)
		fatal (cannot_create, iname);
	fprintf (fd, "%s\n%s\n", sender, info);
	fprintf (fd, "%s\n", time2a (t1));
	fprintf (fd, "%s\n", time2a (t2));
	fclose (fd);

	strcpy (iname, lotname);
	strcat (iname, ".d");
	fd = fopen (iname, "w");
	if (! fd)
		fatal (cannot_create, iname);

	while (gets (line)) {
		if (! strcmp (line, "%"))
			break;
		fprintf (fd, "%s\n", line);
	}
	fclose (fd);

	appendglist (lotname, starttime);

	printf (lot_created, lotname);
	printf (time_range_is, time2a (t1));
	printf (time_range_to, time2a (t2));

	error ("lot %s created by %s", lotname, sender);
}

dodestroy (ac, av)
char **av;
{
	char *lotname;
	struct glist *glist;
	char iname [20];
	struct lot *lot;

	if (ac <= 1)
		return;
	glist = loadglist ();
	while (*++av) {
		lotname = strlower (*av);
		if (strlen (lotname) > 12)
			lotname [12] = 0;

		lot = loadlot (lotname);
		if (! lot) {
			printf (lot_does_not_exist, lotname);
			continue;
		}
		if (! adminmode && strcmp (lot->host, sender)) {
			printf (not_allowed_to_destroy, lotname);
			continue;
		}

		run ("%s ../backup/%s %s.?", ARCHIVER,
			time2fname (starttime), lotname);

		strcpy (iname, lotname);
		strcat (iname, ".i");
		unlink (iname);
		strcpy (iname, lotname);
		strcat (iname, ".l");
		unlink (iname);
		strcpy (iname, lotname);
		strcat (iname, ".a");
		unlink (iname);
		strcpy (iname, lotname);
		strcat (iname, ".d");
		unlink (iname);
		printf (lot_destroyed, lotname);
		error ("lot %s destroyed by %s", lotname, sender);

		deleteglist (glist, lotname);
		freelot (lot);
	}
	saveglist (glist);
	freeglist (glist);
}

doindex (ac, av)
char **av;
{
	char *lotname;
	struct glist *glist;
	struct glistentry *p;
	int longflag;

	longflag = !strcmp (av[0], "longindex");
	glist = loadglist ();
	if (ac <= 1) {
		for (p=glist->list; p; p=p->next)
			doindex1 (p->name, p->t0, longflag);
	} else while (*++av) {
		lotname = strlower (*av);
		if (strlen (lotname) > 12)
			lotname [12] = 0;
		for (p=glist->list; p; p=p->next)
			if (match (p->name, lotname))
				doindex1 (p->name, p->t0, longflag);
	}
	freeglist (glist);
}

donewindex (ac, av)
char **av;
{
	char *lotname;
	struct glist *glist;
	struct glistentry *p;
	int longflag;
	long t0;

	if (ac <= 1)
		fatal (bad_newindex_command);
	longflag = !strcmp (av[0], "longnewindex");

	t0 = a2time (av [1]);
	--ac;
	++av;
	glist = loadglist ();
	if (ac <= 1) {
		for (p=glist->list; p; p=p->next)
			if (p->t0 >= t0)
				doindex1 (p->name, p->t0, longflag);
	} else while (*++av) {
		lotname = strlower (*av);
		if (strlen (lotname) > 12)
			lotname [12] = 0;
		for (p=glist->list; p; p=p->next)
			if (p->t0 >= t0 && match (p->name, lotname))
				doindex1 (p->name, p->t0, longflag);
	}
	freeglist (glist);
}

doindex1 (lotname, t0, longflag)
char *lotname;
long t0;
{
	struct lot *lot;

	lot = loadlot (lotname);
	if (! lot)
		return;
	if (! strcmp (sender, lot->host)) {
		freelot (lot);
		return;
	}
	printf ("%-12s  %-18s %s\n", lot->name, lot->host, lot->info);
	printf ("%-12s  ", "");
	printf ("%-17s  ", time2a (t0));
	printf ("%-17s  ", time2a (lot->t1));
	printf ("%-17s\n", time2a (lot->t2));
	if (longflag) {
		char dname [20];
		char line [512];
		FILE *fd;

		strcpy (dname, lot->name);
		strcat (dname, ".d");
		fd = fopen (dname, "r");
		if (fd) {
			while (getstr (fd, line))
				printf (": %s\n", line);
			fclose (fd);
		} else
			printf (no_information_available);
	}
	freelot (lot);
}

doadd (ac, av)
char **av;
{
	struct lot *lot;
	char *lotname, *subscriber, *p;
	char iname [20], line [100];
	FILE *fd;

	if (ac>3 || ac<2)
		fatal (bad_add_command);
	if (ac == 3) {
		subscriber = av [1];
		--ac;
		++av;

		for (p=subscriber; *p && *p != '@'; ++p);
		if (*p == '@')
			strlower (p + 1);
		if (! checkpermissions (subscriber, SERVDIR, "users")) {
			error ("subscriber %s rejected", subscriber);
			fatal (no_access_to_server, subscriber);
		}
	} else
		subscriber = sender;

	lotname = strlower (av [1]);
	if (strlen (lotname) > 12)
		lotname [12] = 0;

	lot = loadlot (lotname);
	if (! lot) {
		printf (lot_does_not_exist, lotname);
		return;
	}
	if (! strcmp (lot->host, subscriber)) {
		printf (you_owner_of_lot, lotname);
		return;
	}
	freelot (lot);

	strcpy (iname, lotname);
	strcat (iname, ".l");
	fd = fopen (iname, "r");
	if (fd) {
		int flag = 0;

		while (getstr (fd, line)) {
			if (! strcmp (line, subscriber)) {
				printf (already_subscribed,
					subscriber, lotname);
				flag = 1;
			}
		}
		fclose (fd);
		if (flag)
			return;
	}
	fd = fopen (iname, "a");
	if (! fd) {
		printf (cannot_write, iname);
		return;
	}
	fprintf (fd, "%s\n", subscriber);
	fclose (fd);
	printf (subscribed_to, subscriber, lotname);

	error ("addressee %s subscribed to %s", subscriber, lotname);
}

dodelete (ac, av)
char **av;
{
	struct lot *lot;
	char *lotname, *subscriber, *p;
	char iname [20], iname2 [20], line [100];
	FILE *fd, *fd2;

	if (ac != 2)
		fatal (bad_delete_command);
	subscriber = sender;

	for (p=subscriber; *p && *p != '@'; ++p);
	if (*p == '@')
		strlower (p + 1);

	lotname = strlower (av [1]);
	if (strlen (lotname) > 12)
		lotname [12] = 0;

	lot = loadlot (lotname);
	if (! lot) {
		printf (lot_does_not_exist, lotname);
		return;
	}
	if (! strcmp (lot->host, subscriber)) {
		printf (you_owner_of_lot, lotname);
		return;
	}
	freelot (lot);

	strcpy (iname2, lotname);
	strcat (iname2, ".L");
	fd2 = fopen (iname2, "w");
	if (! fd2) {
		printf (cannot_create_file, iname2);
		return;
	}

	strcpy (iname, lotname);
	strcat (iname, ".l");
	fd = fopen (iname, "r");
	if (fd) {
		int flag = 0;

		while (getstr (fd, line)) {
			if (! strcmp (line, subscriber))
				flag = 1;
			else
				fprintf (fd2, "%s\n", line);
		}
		fclose (fd);
		if (! flag) {
			fclose (fd2);
			unlink (iname2);
			printf (was_not_subscribed,
				subscriber, lotname);
			return;
		}
	}
	fclose (fd2);
	unlink (iname);
	if (link (iname2, iname) < 0) {
		printf (cannot_link_to, iname2, iname);
		return;
	}
	unlink (iname2);
	printf (unsubscribed_from, subscriber, lotname);

	error ("addressee %s unsubscribed from %s", subscriber, lotname);
}

dosend (ac, av)
char **av;
{
	struct lot *lot;
	FILE *fd, *fd2;
	char *lotname;
	char line [512], iname2 [20];

	if (ac != 2)
		fatal (bad_send_command);

	if (! tmpf)
		tmpf = mktemp (tmpfpattern);

	lotname = strlower (av [1]);
	if (strlen (lotname) > 12)
		lotname [12] = 0;

	lot = loadlot (lotname);
	if (! lot)
		fatal (lot_not_exist, lotname);
	if (!adminmode && strcmp (sender, lot->host) &&
	    (starttime < lot->t1 || starttime > lot->t2))
		fatal (lot_is_closed, lotname);

	fd = fopen (tmpf, "w");
	if (! fd)
		fatal (cannot_create, tmpf);
	strcpy (iname2, lotname);
	strcat (iname2, ".a");
	fd2 = fopen (iname2, "a");
	if (! fd2)
		fatal (cannot_write_to, iname2);

	fprintf (fd, "Subject: [Trade-%s] %s\n", lotname, lot->info);
	fprintf (fd, "From: %s\n", sender);
	fprintf (fd, "Sender: %s\n", MYADDRESS);
	fprintf (fd, "\n");

	fprintf (fd2, "From %s\n", sender);
	fprintf (fd2, "From: %s\n", sender);
	fprintf (fd2, "Date: %s", ctime (&starttime));
	fprintf (fd2, "\n");

	while (gets (line)) {
		if (! strcmp (line, "%"))
			break;
		fprintf (fd, "%s\n", line);
		fprintf (fd2, "%s\n", line);
	}
	fclose (fd);
	fprintf (fd2, "\n");
	fclose (fd2);

	send1 (tmpf, lot->host);
	printf (message_sent_to, lot->host);
	error ("send message from %s to owner %s", sender, lot->host);

	strcpy (iname2, lotname);
	strcat (iname2, ".l");
	fd2 = fopen (iname2, "r");
	if (fd2) {
		while (getstr (fd2, line))
			if (*line) {
				send1 (tmpf, line);
				printf (message_sent_to, line);
				error ("send message from %s to subscriber %s",
					sender, line);
			}
		fclose (fd2);
	}
	freelot (lot);
	unlink (tmpf);
}

send1 (filename, to)
char *filename, *to;
{
	run ("(echo 'To: %s'; cat '%s') | %s %s", to, filename, MAILTOCMD, to);
	sleep (1);
}

doget (ac, av)
char **av;
{
	char *lotname;
	FILE *fd, *fd2;
	char iname [20], line [512];
	struct lot *lot;

	if (ac != 2)
		fatal (bad_get_command);

	if (! tmpf)
		tmpf = mktemp (tmpfpattern);

	lotname = strlower (av [1]);
	if (strlen (lotname) > 12)
		lotname [12] = 0;

	lot = loadlot (lotname);
	if (! lot) {
		printf (lot_does_not_exist, lotname);
		return;
	}
	if (!adminmode && strcmp (lot->host, sender)) {
		printf (not_allowed_to_get, lotname);
		return;
	}

	fd = fopen (tmpf, "w");
	if (! fd)
		fatal (cannot_create, tmpf);

	fprintf (fd, "Subject: [Trade] %s %s\n", archive_of_lot, lotname);
	fprintf (fd, "From: %s\n", MYADDRESS);
	fprintf (fd, "\n");
	fprintf (fd, "%s %s, %s\n", archive_of_lot, lotname, lot->info);
	fprintf (fd, time_range_is, time2a (lot->t1));
	fprintf (fd, time_range_to, time2a (lot->t2));
	fprintf (fd, "\n");
	fprintf (fd, subscribers_are);

	strcpy (iname, lotname);
	strcat (iname, ".l");
	fd2 = fopen (iname, "r");
	if (fd2) {
		while (getstr (fd2, line))
			fprintf (fd, "    %s\n", line);
		fclose (fd2);
	}
	fprintf (fd, "\n");
	fprintf (fd, messages_are);

	strcpy (iname, lotname);
	strcat (iname, ".a");
	fd2 = fopen (iname, "r");
	if (fd2) {
		int n = 0;

		while (getstr (fd2, line))
			if (! strncmp (line, "From ", 5))
				fprintf (fd, "%d)\n", ++n);
			else
				fprintf (fd, "    %s\n", line);
		fclose (fd2);
	}
	fclose (fd);

	send1 (tmpf, sender);
	printf (archive_sent_to_you, lotname);
	error ("send archive %s to %s", lotname, sender);

	freelot (lot);
	unlink (tmpf);
}

dostatus (ac, av)
char **av;
{
	char *lotname;
	struct glist *glist;
	struct glistentry *p;

	if (! adminmode)
		fatal (permission_denied);
	glist = loadglist ();
	if (ac <= 1) {
		for (p=glist->list; p; p=p->next)
			dostatus1 (p->name, p->t0);
	} else while (*++av) {
		lotname = strlower (*av);
		if (strlen (lotname) > 12)
			lotname [12] = 0;
		for (p=glist->list; p; p=p->next)
			if (match (p->name, lotname))
				dostatus1 (p->name, p->t0);
	}
	freeglist (glist);
}

donewstatus (ac, av)
char **av;
{
	char *lotname;
	struct glist *glist;
	struct glistentry *p;
	long t0;

	if (ac <= 1)
		fatal (bad_newstatus_command);
	if (! adminmode)
		fatal (permission_denied);

	t0 = a2time (av [1]);
	--ac;
	++av;
	glist = loadglist ();
	if (ac <= 1) {
		for (p=glist->list; p; p=p->next)
			if (p->t0 >= t0)
				dostatus1 (p->name, p->t0);
	} else while (*++av) {
		lotname = strlower (*av);
		if (strlen (lotname) > 12)
			lotname [12] = 0;
		for (p=glist->list; p; p=p->next)
			if (p->t0 >= t0 && match (p->name, lotname))
				dostatus1 (p->name, p->t0);
	}
	freeglist (glist);
}

dostatus1 (lotname, t0)
char *lotname;
long t0;
{
	char lname [20];
	struct lot *lot;

	lot = loadlot (lotname);
	if (! lot)
		return;
	strcpy (lname, lot->name);
	strcat (lname, ".l");
	if (access (lname, 4) < 0)
		return;
	printf ("%-12s  %-18s %s\n", lot->name, lot->host, lot->info);
	printf ("%-12s  ", "");
	printf ("%-17s  ", time2a (t0));
	printf ("%-17s  ", time2a (lot->t1));
	printf ("%-17s\n", time2a (lot->t2));
	{
		char line [512];
		FILE *fd;

		fd = fopen (lname, "r");
		if (fd) {
			while (getstr (fd, line))
				printf (": %s\n", line);
			fclose (fd);
		} else
			printf (no_subscribers_available);
	}
	freelot (lot);
}
