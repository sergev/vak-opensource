/*
 * Информация о подписке хранится в формате DBM.
 * DBM игрет роль ассоциативного массива и выполняет
 * следующие преобразования:
 *
 * имя группы       --> тег группы
 * тег группы       --> имя группы
 * тег группы       --> номер последней статьи
 * тег группы       --> таблица подписки
 * имя пользователя --> тег пользователя
 * тег пользователя --> имя пользователя
 * тег пользователя --> флаги пользователя
 *
 * Теги группы и пользователя представляют собой
 * длинное целое вида:
 *  ------------------------------
 *  | 1 | N | X |      номер     |
 *  ------------------------------
 *        |   |
 *        |   флаги при выборке
 *       тип, 0 - группа, 1 - пользователь
 *
 * Важно, что все теги отличаются от нуля.
 */

# include <stdio.h>
# include <fcntl.h>
# include "cdbm.h"
# include "vdbm.h"
# include "groups.h"
# include "tagdefs.h"
# include "config.h"

# define DFLTLIMIT      32      /* Макс. длина файла изменений */

# define RANDOM(s)      (s = ((s)*1103515245 + 12345) & 0x7fffffff)

static CDBM *dbf;       /* дескриптор базы данных */
static char *groupsfile = "groups";

extern errno;
extern char *malloc (), *mktemp (), *strcpy (), *strcat (), *strncpy ();
extern char *strchr ();
extern long time ();

/*
 * Загрузка базы данных.
 * Причем DBM блокирует базу, чтобы всякие прочие
 * типа admin не могли ее испортить.
 * Возвращает 1, если база успешно открыта,
 * 0, если базу открыть не удается,
 * -1, если база заблокирована.
 */

int loadgroups (waitflag)
{
	char name [200];

	name [0] = 0;
	if (! strchr (groupsfile, '/')) {
		strcat (name, SERVDIR);
		strcat (name, "/");
	}
	strcat (name, groupsfile);
tryagain:
	dbf = cdbm_open (name, O_RDWR | O_CREAT, 0664);
	if (! dbf) {
		if (errno < 0) {
			if (waitflag) {
				sleep (2);
				goto tryagain;
			}
			return (-1);
		}
		error ("cannot open %s/groups database", SERVDIR);
		return (0);
	}
	dbf->updatelimit = DFLTLIMIT;
	return (1);
}

/*
 * Запись базы данных.
 */

int savegroups ()
{
	if (dbf) {
		cdbm_close (dbf);
		dbf = 0;
	}
	return (1);
}

void setgroupsfile (name)
char *name;
{
	if (name)
		groupsfile = name;
	else
		groupsfile = "groups";
}

/*
 * Выборка из базы тега по имени.  Применяется для
 * нахождения тега группы или пользователя по имени.
 * Первый параметр - символ, задает тип искомого объекта:
 * 'G' - группа, 'U' - пользователь.
 * Возвращает найденный тег или 0.
 */

static long getTagByName (typ, name)
char *name;
{
	char buf [200];
	datum key, rez;
	int len;
	long tag;

	len = strlen (name);
	/*
	 * Если длина имени слишком большая,
	 * укорачиваем.  Это, конечно, плохо,
	 * но делать нечего.
	 */
	if (len >= sizeof (buf))
		len = sizeof (buf) - 2;
	buf [0] = typ;
	strncpy (buf+1, name, len);
	buf [len+1] = 0;
	/*
	 * Выбираем запись из базы.
	 */
	key.dptr = buf;
	key.dsize = len+2;
	rez = cdbm_fetch (dbf, key);
	if (! rez.dptr)
		return (0);     /* нет такого */
	tag = *(long *) rez.dptr;
	return (tag);
}

/*
 * Выборка из базы массива байтов по тегу.  Применяется для
 * преобразования тега в имя или поиска таблицы подписки.
 * Возвращает указатель на внутренний буфер.
 * Вызывающая функция должна эту память переписать на нужное место.
 * Если объект не найден, возвращает 0.
 */

static char *getAddrByTag (tag)
long tag;
{
	datum key, rez;

	key.dptr = (char *) &tag;
	key.dsize = sizeof (tag);
	rez = cdbm_fetch (dbf, key);
	if (! rez.dptr)
		return (0);     /* нет такого */
	return (rez.dptr);
}

/*
 * Выборка из базы числа по тегу.  Применяется для
 * выдачи флагов пользователя или номера статьи в группе.
 * Возвращает найденное число.
 * Если объект не найден, возвращает -1.
 */

static long getValByTag (tag)
long tag;
{
	datum key, rez;
	long val;

	key.dptr = (char *) &tag;
	key.dsize = sizeof (tag);
	rez = cdbm_fetch (dbf, key);
	if (! rez.dptr)
		return (-1);     /* нет такого */
	val = *(long *) rez.dptr;
	return (val);
}

/*
 * Занесение в базу тега по имени.  Применяется для
 * нахождения тега группы или пользователя по имени.
 * Первый параметр - символ, задает тип искомого объекта:
 * 'G' - группа, 'U' - пользователь.
 */

static putTagByName (typ, name, tag)
char *name;
long tag;
{
	char buf [200];
	datum key, val;
	int len;

	len = strlen (name);
	/*
	 * Если длина имени слишком большая,
	 * укорачиваем.  Это, конечно, плохо,
	 * но делать нечего.
	 */
	if (len >= sizeof (buf))
		len = sizeof (buf) - 2;
	buf [0] = typ;
	strncpy (buf+1, name, len);
	buf [len+1] = 0;
	/*
	 * Добавляем запись в базу.
	 */
	key.dptr = buf;
	key.dsize = len+2;
	val.dptr = (char *) &tag;
	val.dsize = sizeof (tag);
	if (cdbm_store (dbf, key, val, CDBM_REPLACE))
		error ("dbm error in putTagByName");
}

/*
 * Занесение в базу массива байтов по тегу.  Применяется для
 * преобразования тега в имя или поиска таблицы подписки.
 */

static putAddrByTag (tag, addr, sz)
long tag;
char *addr;
{
	datum key, val;

	key.dptr = (char *) &tag;
	key.dsize = sizeof (tag);
	val.dptr = addr;
	val.dsize = sz;
	if (cdbm_store (dbf, key, val, CDBM_REPLACE))
		error ("dbm error in putAddrByTag, value size %d", sz);
}

/*
 * Занесение в базу числа по тегу.  Применяется для
 * выдачи флагов пользователя или номера статьи в группе.
 */

static putValByTag (tag, v)
long tag, v;
{
	datum key, val;

	key.dptr = (char *) &tag;
	key.dsize = sizeof (tag);
	val.dptr = (char *) &v;
	val.dsize = sizeof (v);
	if (cdbm_store (dbf, key, val, CDBM_REPLACE))
		error ("dbm error in putValByTag");
}

/*
 * Удаление записи по тегу.
 * Если записи в базе нет, молча игнорируем эту
 * неприятность.
 */

static delByTag (tag)
long tag;
{
	datum key;

	key.dptr = (char *) &tag;
	key.dsize = sizeof (tag);
	cdbm_delete (dbf, key);
}

/*
 * Удаление записи по имени.
 * Если записи в базе нет, молча игнорируем эту
 * неприятность.
 */

static delByName (typ, name)
char *name;
{
	char buf [200];
	datum key;
	int len;

	len = strlen (name);
	/*
	 * Если длина имени слишком большая,
	 * укорачиваем.  Это, конечно, плохо,
	 * но делать нечего.
	 */
	if (len >= sizeof (buf))
		len = sizeof (buf) - 2;
	buf [0] = typ;
	strncpy (buf+1, name, len);
	buf [len+1] = 0;

	key.dptr = buf;
	key.dsize = len+2;
	cdbm_delete (dbf, key);
}

/*
 * Проверяем ключ DBM на соответствие типу typ (G или U).
 * Фактически, это должен быть ключ преобразования из тега
 * в имя группы или пользователя.
 * Если тип не тот, продолжаем поиск в базе.
 * Признак конца - NULL в поле key.dptr.
 */

static datum matchtag (key, typ)
datum key;
{
	long tag;

	for (;;) {
		if (! key.dptr)
			return (key);
		if (key.dsize == sizeof (tag) &&
		    (tag = *(long *) key.dptr) & TAGLABEL)
			switch (typ) {
			case 'G':
				if ((tag & (TAGUSER | TAGFLAGMASK)) ==
				    TAGNAME)
					return (key);
				break;
			case 'U':
				if ((tag & (TAGUSER | TAGFLAGMASK)) ==
				    (TAGUSER | TAGNAME))
					return (key);
				break;
			}
		key = cdbm_nextkey (dbf, key);
	}
}

long *taglist (typ, cnt)
int *cnt;
{
	long *tab;
	long tag;
	int len, ptr;
	datum key;

	*cnt = ptr = 0;
	len = 512;

	key = cdbm_firstkey (dbf);
	key = matchtag (key, typ);
	if (! key.dptr)
		return (0);

	tab = (long *) malloc (len * sizeof (long));
	if (! tab)
		return (0);

	while (key.dptr) {
		tag = *(long *) key.dptr;
		tag &= ~TAGFLAGMASK;
		if (ptr >= len) {
			len += 512;
			tab = (long *) realloc ((char *) tab, len * sizeof (long));
			if (! tab)
				return (0);
		}
		tab [ptr++] = tag;

		key = cdbm_nextkey (dbf, key);
		key = matchtag (key, typ);
	}
	if (! ptr) {
		free ((char *) tab);
		return (0);
	}
	if (ptr < len) {
		tab = (long *) realloc ((char *) tab, ptr * sizeof (long));
		if (! tab)
			return (0);
	}
	*cnt = ptr;
	return (tab);
}

/*
 * Возвращает список тегов всех пользователей.
 * Память под список выделяется посредством malloc(),
 * Освобождать память должна вызывающая сторона.
 */

long *userlist (cnt)
int *cnt;
{
	return (taglist ('U', cnt));
}

/*
 * Возвращает список тегов всех групп.
 * Память под список выделяется посредством malloc(),
 * Освобождать память должна вызывающая сторона.
 */

long *grouplist (cnt)
int *cnt;
{
	return (taglist ('G', cnt));
}

/*
 * Генерация уникального тега группы/пользователя.
 */

static long gentag (typ)
{
	long tag;
	char *name;
	static long seed;

	if (! seed)
		seed = time ((long *) 0);
	for (;;) {
		tag = RANDOM (seed) & TAGNUMMASK | TAGLABEL;
		if (typ == 'U')
			tag |= TAGUSER;
		name = getAddrByTag (tag | TAGNAME);
		if (! name)
			return (tag);
	}
}

/*
 * Выдает тег группы по имени или 0 если такой группы нет.
 */

long grouptag (name)
char *name;
{
	return (getTagByName ('G',  name));
}

/*
 * Выдает тег пользователя по имени или 0 если такого пользователя нет.
 */

long usertag (name)
char *name;
{
	return (getTagByName ('U',  name));
}

/*
 * Выдает таблицу подписки по тегу группы.
 * В cnt записывает количество пользователей в таблице
 * или 0.
 */

struct subscrtab *groupsubscr (g, cnt)
long g;
int *cnt;
{
	datum key, rez;

	g |= TAGSUBSCR;
	key.dptr = (char *) &g;
	key.dsize = sizeof (g);
	rez = cdbm_fetch (dbf, key);
	*cnt = rez.dsize / sizeof (struct subscrtab);
	if (!rez.dptr || !*cnt) {
		*cnt = 0;
		return (0);     /* нет такого */
	}
	return ((struct subscrtab *) rez.dptr);
}

/*
 * Выдает количество подписчиков группы,
 * отдельно по subscribe, feed и restricted feed.
 */

void groupstat (g, sm, fm, rm)
long g;
int *sm, *fm, *rm;
{
	datum key, rez;
	struct subscrtab *s;
	int cnt;

	*sm = *fm = *rm = 0;
	g |= TAGSUBSCR;
	key.dptr = (char *) &g;
	key.dsize = sizeof (g);
	rez = cdbm_fetch (dbf, key);
	if (! rez.dptr)
		return;
	cnt = rez.dsize / sizeof (struct subscrtab);
	s = (struct subscrtab *) rez.dptr;
	for (; cnt>0; --cnt, ++s) {
		switch (s->mode) {
		case 's':       ++*sm;  break;
		case 'f':       ++*fm;  break;
		default:        ++*rm;  break;
		}
	}
}

/*
 * Выдает для каждого пользователя количество групп, на которые он подписан,
 * отдельно по subscribe, feed и restricted feed.
 * Память под список выделяется посредством malloc(),
 * Освобождать память должна вызывающая сторона.
 */

struct stattab *userstat (tab, n)
long *tab;
{
	struct stattab *st;
	struct subscrtab *s;
	long g;
	int i, k, cnt;

	st = (struct stattab *) calloc (n, sizeof (struct stattab));
	if (! st)
		return (0);
	for (g=firstgroup(); g; g=nextgroup(g)) {
		s = groupsubscr (g, &cnt);
		if (! s)
			continue;
		for (i=0; i<n; ++i) {
			for (k=0; k<cnt; ++k) {
				if (tab [i] != s[k].tag)
					continue;
				switch (s[k].mode) {
				case 's':       ++st[i].subs;  break;
				case 'f':       ++st[i].feed;  break;
				default:        ++st[i].rfeed; break;
				}
			}
		}

	}
	return (st);
}

static void addsubscr (u, g, s, len, ptr)
long u, g;
struct subscrtab **s;
int *len, *ptr;
{
	struct subscrtab *gs;
	int ns, n;

	gs = groupsubscr (g, &ns);
	if (! gs)
		return;
	for (n=0; n<ns; ++n, ++gs)
		if (gs->tag == u) {
			if (*ptr >= *len) {
				*len += 10;
				*s = (struct subscrtab *) realloc ((char *) *s,
					*len * sizeof (struct subscrtab));
				if (! *s)
					return;
			}
			(*s)[*ptr].tag = g;
			(*s)[*ptr].mode = gs->mode;
			++*ptr;
		}
}

/*
 * Выдает таблицу подписки по тегу пользователя.
 * В cnt записывает количество групп в таблице или 0.
 */

struct subscrtab *usersubscr (u, gtab, cnt)
long u;
char **gtab;
int *cnt;
{
	static struct subscrtab *s;
	int slen, sptr;
	long g;

	if (s) {
		free ((char *) s);
		s = 0;
	}
	*cnt = sptr = 0;
	slen = 10;
	s = (struct subscrtab *) malloc (slen * sizeof (struct subscrtab));
	if (! s)
		return (0);
	if (! gtab || ! *gtab) {
		for (g=firstgroup(); g && s; g=nextgroup(g))
			addsubscr (u, g, &s, &slen, &sptr);
		goto ret;
	}
	for (; *gtab; ++gtab) {
		int len = strlen (*gtab);
		for (g=firstgroup(); g && s; g=nextgroup(g)) {
			char *gname = groupname (g);
			if (! gname)
				continue;
			if (! strncmp (*gtab, gname, len) &&
			    gname[len]==0 || gname[len]=='.')
				addsubscr (u, g, &s, &slen, &sptr);
		}
	}
ret:
	if (! s)
		return (0);
	if (sptr < slen) {
		s = (struct subscrtab *) realloc ((char *) s,
			sptr * sizeof (struct subscrtab));
		if (! s)
			return (0);
	}
	*cnt = sptr;
	return (s);
}

/*
 * Создание записей о новой группе.
 * Если группа уже есть, ничего не делаем.
 */

long addgroup (name, last)
char *name;
long last;
{
	long tag;

	if (tag = grouptag (name))      /* такая группа уже есть */
		return (tag);
	tag = gentag ('G');             /* создаем уникальный тег группы */
	putTagByName ('G', name, tag);  /* записывает его в базу */
	/*
	 * Запоминаем имя группы.
	 */
	putAddrByTag (tag | TAGNAME, name, strlen (name) + 1);
	/*
	 * Инициализируем номер посл. статьи группы.
	 */
	putValByTag (tag | TAGLASTART, last);
	/*
	 * Инициализируем таблицу подписки.
	 */
	putAddrByTag (tag | TAGSUBSCR, "", 0);
	return (tag);
}

/*
 * Удаление записей о группе.
 * Если группы нет, ничего не делаем.
 */

delgroup (name)
char *name;
{
	long tag;

	tag = grouptag (name);
	if (! tag)
		return (0);
	delByTag (tag | TAGNAME);
	delByTag (tag | TAGLASTART);
	delByTag (tag | TAGSUBSCR);
	delByName ('G', name);
	return (1);
}

/*
 * Создание записей о новом пользователе.
 * Если такой пользователь уже есть, ничего не делаем.
 * Возвращает тег пользователя.
 */

long adduser (name)
char *name;
{
	long tag;

	if (tag = usertag (name))       /* такой пользователь уже есть */
		return (tag);
	tag = gentag ('U');             /* создаем уникальный тег пользователя */
	putTagByName ('U', name, tag);  /* записывает его в базу */
	/*
	 * Запоминаем имя пользователя.
	 */
	putAddrByTag (tag | TAGNAME, name, strlen (name) + 1);
	/*
	 * Инициализируем флаги пользователя.
	 */
	putValByTag (tag | TAGUFLAGS, 0L);
	return (tag);
}

/*
 * Удаление записей о пользователе.
 * Если такого нет, ничего не делаем.
 */

deluser (name)
char *name;
{
	long tag;

	tag = usertag (name);
	if (! tag)
		return (0);
	unsubscrall (tag);
	delByTag (tag | TAGUFLAGS);
	delByTag (tag | TAGNAME);
	delByName ('U', name);
	return (1);
}

/*
 * Удаление записей о пользователях.
 */

int *delusertab (tab, len)
long *tab;
{
	int *rez, i;

	rez = unsubscrtab (tab, len);
	if (! rez)
		return (0);
	for (i=0; i<len; ++i) {
		delByName ('U', username (tab [i]));
		delByTag (tab[i] | TAGUFLAGS);
		delByTag (tab[i] | TAGNAME);
	}
	return (rez);
}

/*
 * Подписать пользователя на группу.
 */

subscribe (u, g, smode)
long u, g;
{
	struct subscrtab *s;
	char *p;
	int n, i;

	s = groupsubscr (g, &n);
	for (i=0; i<n; ++i)
		if (s[i].tag == u) {
			s[i].mode = smode;
			putAddrByTag (g | TAGSUBSCR, (char *) s,
				sizeof (struct subscrtab) * n);
			return (1);
		}
	/* Make new user index cell. */
	p = malloc ((n+1) * sizeof (struct subscrtab));
	if (! p) {
		error ("out of memory in subscribe");
		return (0);
	}
	if (n)
		memcpy (p, (char *) s, n * sizeof (struct subscrtab));
	s = (struct subscrtab *) p;
	s[n].tag = u;
	s[n].mode = smode;
	++n;
save:
	putAddrByTag (g | TAGSUBSCR, (char *) s,
		sizeof (struct subscrtab) * n);
	free ((char *) s);
	return (1);
}

static shrinksubscr (s, n, i)
struct subscrtab **s;
int n;
{
	if (n == 0) {
		/* User list is empty. */

		*s = (struct subscrtab *) malloc (1);
		if (! *s) {
nomem:                  error ("out of memory in shrinksubscr");
			return (0);
		}
	} else {
		char *p = malloc (n * sizeof (struct subscrtab));

		if (! p)
			goto nomem;
		for (; i<n; ++i)        /* перепись в буфере DBM - нехорошо */
			(*s) [i] = (*s) [i+1];
		memcpy (p, (char *) *s, n * sizeof (struct subscrtab));
		*s = (struct subscrtab *) p;
	}
	return (1);
}

/*
 * Отписать пользователя от группы.
 */

unsubscribe (u, g)
long u, g;
{
	struct subscrtab *s;
	int n, i;

	s = groupsubscr (g, &n);
	if (! s)
		return (0);
	for (i=0; i<n; ++i)
		if (s[i].tag == u)
			break;
	if (i >= n)
		return (0);
	--n;
	if (! shrinksubscr (&s, n, i))
		return (0);
	putAddrByTag (g | TAGSUBSCR, (char *) s,
		sizeof (struct subscrtab) * n);
	free ((char *) s);
	return (1);
}

/*
 * Отписать пользователя от всех групп.
 */

unsubscrall (u)
long u;
{
	long g;

	for (g=firstgroup(); g; g=nextgroup(g))
		unsubscribe (u, g);
}

/*
 * Отписать пользователей от всех групп.
 * Возвращает кто на сколько групп был подписан.
 * Память списка должен освобождать вызывающий.
 */

int *unsubscrtab (tab, len)
long *tab;
int len;
{
	struct subscrtab *s, *p;
	int n, i, k, delflag;
	static int *cnttab;
	long g;

	if (! len)
		return (0);
	if (cnttab)
		free ((char *) cnttab);
	cnttab = (int *) malloc (len * sizeof (int));
	if (! cnttab)
		return (0);
	for (k=0; k<len; ++k)
		cnttab [k] = 0;
	for (g=firstgroup(); g; g=nextgroup(g)) {
		delflag = 0;
		s = groupsubscr (g, &n);
		if (! s)
			continue;
		p = (struct subscrtab *) malloc (n * sizeof (struct subscrtab));
		if (! p) {
			error ("out of memory in unsubscrtab");
			free ((char *) cnttab);
			return (0);
		}
		memcpy ((char *) p, (char *) s, n * sizeof (struct subscrtab));
		s = p;
		for (i=0; i<n; ++i) {
			for (k=0; k<len; ++k)
				if (s[i].tag == tab[k]) {
					struct subscrtab *olds = s;
					int rez = shrinksubscr (&s, --n, i);
					free ((char *) olds);
					if (! rez) {
						free ((char *) cnttab);
						return (0);
					}
					++cnttab[k];
					++delflag;
				}
		}
		if (delflag)
			putAddrByTag (g | TAGSUBSCR, (char *) s,
				sizeof (struct subscrtab) * n);
		free ((char *) s);
	}
	return (cnttab);
}

long grouplast (tag)
long tag;
{
	long last;

	last = getValByTag (tag | TAGLASTART);
	return (last<0 ? 0 : last);
}

void setgrouplast (tag, last)
long tag, last;
{
	putValByTag (tag | TAGLASTART, last);
}

long userflags (tag)
long tag;
{
	long flags;

	flags = getValByTag (tag | TAGUFLAGS);
	return (flags<0 ? 0 : flags);
}

void setuserflags (tag, flags)
long tag, flags;
{
	putValByTag (tag | TAGUFLAGS, flags);
}

void setsubscr (g, tab, n)
long g;
struct subscrtab *tab;
{
	putAddrByTag (g | TAGSUBSCR, (char *) tab,
		n * sizeof (struct subscrtab));
}

char *groupname (tag)
long tag;
{
	return (getAddrByTag (tag | TAGNAME));
}

char *username (tag)
long tag;
{
	return (getAddrByTag (tag | TAGNAME));
}

void groupsdump (proc)
int (*proc) ();
{
	datum key, val;

	key = cdbm_firstkey (dbf);
	while (key.dptr) {
		if (! key.dptr)
			break;
		val = cdbm_fetch (dbf, key);
		(*proc) (key.dptr, key.dsize, val.dptr, val.dsize);
		key = cdbm_nextkey (dbf, key);
	}
}

void groupsdelrec (k, sz)
char *k;
{
	datum key;

	key.dptr = k;
	key.dsize = sz;
	cdbm_delete (dbf, key);
}

void groupslimit (lim)
{
	if (lim < 1)
		dbf->updatelimit = DFLTLIMIT;
	else if (lim > 1024)
		dbf->updatelimit = 1024;
	else
		dbf->updatelimit = lim;
}

void groupssync ()
{
	cdbm_sync (dbf);
}
