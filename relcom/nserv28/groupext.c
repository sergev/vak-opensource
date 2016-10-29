# include <stdio.h>
# include "vdbm.h"
# include "groups.h"
# include "config.h"

static VDBM *ginfo;     /* дескриптор таблицы описания групп */
static VDBM *active;    /* дескриптор таблицы активных групп */

static long *ulist;
static int ulistlen, ulistptr;
static long *glist;
static int glistlen, glistptr;

static void loadginfo ();

extern FILE *f2open ();

/*
 * Функции для организации циклов перебора всех групп
 * или всех пользоватетей.
 * Пример:
 *      long tag
 *      for (tag=firstgroup(); tag; tag=nextgroup(tag)) {
 *              ... обработка тега группы ...
 *      }
 * Примечание: циклы по группам и по пользователям нельзя
 * вкладывать друг в друга.
 */

long firstuser ()
{
	if (! ulist)
		ulist = userlist (&ulistlen);
	if (! ulist)
		return (0);
	ulistptr = 0;
	return (nextuser (0L));
}

long nextuser (u)
long u;
{
	if (! ulist)
		return (0);
	if (ulistptr >= ulistlen)
		return (0);
	return (ulist [ulistptr++]);
}

long firstgroup ()
{
	if (! glist)
		glist = grouplist (&glistlen);
	if (! glist)
		return (0);
	glistptr = 0;
	return (nextgroup (0L));
}

long nextgroup (g)
long g;
{
	if (! glist)
		return (0);
	if (glistptr >= glistlen)
		return (0);
	return (glist [glistptr++]);
}

/*
 * Загрузка файла описания групп /usr/lib/news/newsgroups.
 * Создаем еще одну базу - преобразование тега группы
 * в ее описание.  База содержится в каталоге /tmp,
 * причем файл после создания базы сразу уничтожаем.
 */

static void loadginfo ()
{
	char line [512], *group, *info;
	vdatum key, val;
	FILE *fd;
	long g;
	int len;

	ginfo = vdbm_open (0);
	if (! ginfo)
		return;
	fd = f2open (NEWSLIBDIR, "newsgroups", "r");
	if (! fd)
		return;
	while (fgets (line, sizeof (line), fd)) {
		if (! *line || *line=='!')
			continue;
		group = line;
		info = line;
		while (*info && *info!=' ' && *info!='\t')
			++info;
		if (! *info)
			continue;
		*info++ = 0;
		g = grouptag (group);
		if (! g)
			continue;
		while (*info==' ' || *info=='\t')
			++info;
		if (! *info)
			continue;
		len = strlen (info);
		info [len-1] = 0;
		key.dptr = (char *) &g;
		key.dsize = sizeof (g);
		val.dptr = info;
		val.dsize = len;
		vdbm_store (ginfo, key, val, 1);
	}
	fclose (fd);
}

/*
 * Загрузка файла активных групп /usr/lib/news/active.
 * Создаем еще одну базу - преобразование имени группы
 * в ее инф. структуру.
 */

static loadactive ()
{
	char line [512], name [128];
	struct activetab ag;
	vdatum key, val;
	FILE *fd;

	active = vdbm_open (0);
	if (! active) {
		error ("cannot create VDBM database");
		quit ();
	}
	fd = f2open (NEWSLIBDIR, "active", "r");
	if (! fd) {
		error ("cannot read %s/active", NEWSLIBDIR);
		quit ();
	}
	while (fgets (line, sizeof (line), fd)) {
		sscanf (line, "%s %ld %ld %c", name, &ag.last, &ag.first,
			&ag.mode);
		key.dptr = name;
		key.dsize = strlen (name) + 1;
		val.dptr = (char *) &ag;
		val.dsize = sizeof (ag);
		vdbm_store (active, key, val, 1);
	}
	fclose (fd);
}

char *groupinfo (g)
long g;
{
	vdatum key, rez;

	if (! ginfo)
		loadginfo ();
	if (! ginfo)
		return (0);
	key.dptr = (char *) &g;
	key.dsize = sizeof (g);
	rez = vdbm_fetch (ginfo, key);
	return (rez.dptr);
}

struct activetab *getactive (name)
char *name;
{
	vdatum key, rez;

	if (! active)
		loadactive ();
	if (! active || ! name)
		return (0);
	key.dptr = name;
	key.dsize = strlen (name) + 1;
	rez = vdbm_fetch (active, key);
	return ((struct activetab *) rez.dptr);
}

char *firstactive ()
{
	vdatum key;

	if (! active)
		loadactive ();
	if (! active)
		return (0);
	key = vdbm_firstkey (active);
	return (key.dptr);
}

char *nextactive (prev)
char *prev;
{
	vdatum key;

	if (! active)
		return (0);
	key.dptr = prev;
	key.dsize = strlen (prev) + 1;
	key = vdbm_nextkey (active, key);
	return (key.dptr);
}

