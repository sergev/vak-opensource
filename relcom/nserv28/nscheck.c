/*
 * Проверка корректности таблицы подписки (NDBM).
 */

# include <stdio.h>
# include "groups.h"
# include "vdbm.h"
# include "tagdefs.h"

char *progname;
int verbose = 1;
int reccnt;
VDBM *gtab, *utab, *gtagtab, *utagtab, *garbage;

extern char *malloc ();

usage ()
{
	fprintf (stderr, "Usage: %s [-vs] [groups]\n", progname);
	exit (-1);
}

/* VARARGS1 */
messg (s, a, b, c, d)
char *s, *a, *b, *c, *d;
{
	fprintf (stderr, s, a, b, c, d);
	fprintf (stderr, "\n");
}

/* VARARGS1 */
error (s, a, b, c, d)
char *s, *a, *b, *c, *d;
{
	fprintf (stderr, s, a, b, c, d);
	fprintf (stderr, "\n");
}

quit ()
{
	exit (-1);
}

main (argc, argv)
char **argv;
{
	progname = *(argv++);
	for (--argc; *argv && **argv=='-'; ++argv, --argc) {
		register char *p;

		for (p=1+*argv; *p; ++p)
			switch (*p) {
			case 'v':
				++verbose;
				break;
			case 's':
				verbose = 0;
				break;
			default:
				usage ();
			}
	}
	if (argc > 1)
		usage ();
	if (argc > 0) {
		setgroupsfile (*argv);
		if (verbose)
			printf ("Using file %s\n", *argv);
	}
loop:	switch (loadgroups (0)) {
	case -1:
		if (verbose)
			printf ("Groups database locked, waiting...\n");
		sleep (2);
		goto loop;
	case 0:
		return (-1);
	case 1:
		break;
	}
	if (verbose > 1)
		printf ("Groups database opened\n");
	groupslimit (200);
	check ();
	groupssync ();
	savegroups ();
	if (verbose > 1)
		printf ("Groups database closed\n");
	return (0);
}

char *checktag (tag, typ, flags)
long tag, flags;
{
	if (! (tag & TAGLABEL))
		return ("tag without tag label");
	if (typ=='G' && (tag & TAGUSER) || typ=='U' && ! (tag & TAGUSER))
		return ("invalid tag type");
	if ((tag & TAGFLAGMASK) != flags)
		return ("tag with invalid flag bits");
	if (tag & ~(TAGLABEL | TAGUSER | TAGFLAGMASK | TAGNUMMASK))
		return ("garbage in tag");
	return (0);
}

asciistr (s)
char *s;
{
	for (; *s; ++s)
		if (*s & 0x80)
			return (0);
	return (1);
}

filltab (keyptr, keysize, valptr, valsize)
char *keyptr, *valptr;
{
	vdatum key, val;
	long tag;
	char *err;

	++reccnt;

	if (valsize != sizeof (long) || *keyptr != 'U' && *keyptr != 'G' ||
	    keysize <= 2 || keyptr [keysize-1] || ! asciistr (keyptr))
		return;
	tag = *(long*) valptr;
	if (err = checktag (*(long*) valptr, *keyptr, 0L)) {
		prrecord (err, keyptr, keysize, valptr, valsize);
		storegarbage (keyptr, keysize);
		return;
	}
	key.dptr = keyptr + 1;
	key.dsize = keysize - 1;
	val.dptr = valptr;
	val.dsize = valsize;
	if (vdbm_store (*keyptr=='G' ? gtab : utab, key, val, 0) < 0) {
		error ("cannot store record in VDBM database");
		quit ();
	}
	if (vdbm_store (*keyptr=='G' ? gtagtab : utagtab, val, key, 0) < 0) {
		error ("cannot store record in VDBM database");
		quit ();
	}
}

storegarbage (k, ks)
char *k;
{
	vdatum key, val;

	key.dptr = k;
	key.dsize = ks;
	val.dptr = "";
	val.dsize = 1;
	if (vdbm_store (garbage, key, val, 0) < 0) {
		error ("cannot store garbage record in VDBM database");
		quit ();
	}
}

exists (vdb, tag)
VDBM *vdb;
long tag;
{
	vdatum key, val;

	key.dptr = (char *) &tag;
	key.dsize = sizeof (tag);
	val = vdbm_fetch (vdb, key);
	return (val.dptr != 0);
}

checkrecords (keyptr, keysize, valptr, valsize)
char *keyptr, *valptr;
{
	vdatum key, val;
	long tag, typ;
	char *err;

	/* имя группы --> тег группы */
	/* имя пользователя --> тег пользователя */
	if (valsize == sizeof (long) && (*keyptr == 'U' || *keyptr == 'G') &&
	    keysize > 2 && ! keyptr [keysize-1] && asciistr (keyptr))
		return;
	++reccnt;

	if (keysize != sizeof (long)) {
		prrecord ("too short key in tag->xxx record", keyptr, keysize, valptr, valsize);
		storegarbage (keyptr, keysize);
		return;
	}
	tag = *(long*) keyptr;

	if (! (tag & TAGLABEL)) {
		prrecord ("tag without label", keyptr, keysize, valptr, valsize);
		storegarbage (keyptr, keysize);
		return;
	}
	if (tag & ~(TAGLABEL | TAGUSER | TAGFLAGMASK | TAGNUMMASK)) {
		prrecord ("garbage in tag", keyptr, keysize, valptr, valsize);
		storegarbage (keyptr, keysize);
		return;
	}
	typ = tag & (TAGUSER | TAGFLAGMASK);
	if (typ == TAGNAME) {                   /* тег группы --> имя группы */
		if (valsize < 2) {
			prrecord ("too short value in tag->gname record", keyptr, keysize, valptr, valsize);
			storegarbage (keyptr, keysize);
			return;
		}
	} else if (typ == (TAGUSER|TAGNAME)) {  /* тег пользователя --> имя */
		if (valsize < 2) {
			prrecord ("too short value in tag->uname record", keyptr, keysize, valptr, valsize);
			storegarbage (keyptr, keysize);
			return;
		}
	} else if (typ == TAGLASTART) {         /* тег группы --> номер посл. статьи */
		if (valsize != sizeof (long)) {
			prrecord ("invalid value size in tag->lastart record", keyptr, keysize, valptr, valsize);
			storegarbage (keyptr, keysize);
			return;
		}
	} else if (typ == (TAGUSER|TAGUFLAGS)) { /* тег пользователя --> флаги пользователя */
		if (valsize != sizeof (long)) {
			prrecord ("invalid value size in tag->userflags record", keyptr, keysize, valptr, valsize);
			storegarbage (keyptr, keysize);
			return;
		}
	} else if (typ == TAGSUBSCR) {          /* тег группы --> таблица подписки */
		if (valsize % sizeof (struct subscrtab)) {
			prrecord ("invalid value size in tag->subscr record", keyptr, keysize, valptr, valsize);
			storegarbage (keyptr, keysize);
			return;
		}
	} else {
		prrecord ("unknown tag type", keyptr, keysize, valptr, valsize);
		storegarbage (keyptr, keysize);
		return;
	}
	tag &= ~TAGFLAGMASK;
	if ((tag & TAGUSER) && ! exists (utagtab, tag)) {
		prrecord ("no such user", keyptr, keysize, valptr, valsize);
		storegarbage (keyptr, keysize);
		return;
	}
	if (! (tag & TAGUSER) && ! exists (gtagtab, tag)) {
		prrecord ("no such group", keyptr, keysize, valptr, valsize);
		storegarbage (keyptr, keysize);
		return;
	}
}

check ()
{
	vdatum key;

	/*
	 * Создаем таблицы групп и пользователей
	 */
	gtab = vdbm_open (0);
	utab = vdbm_open (0);
	gtagtab = vdbm_open (0);
	utagtab = vdbm_open (0);
	garbage = vdbm_open (0);
	if (! gtab || ! utab || ! gtagtab || ! utagtab || ! garbage) {
		error ("cannot create VDBM database");
		quit ();
	}

	/*
	 * Сканируем базу данных.  Выбираем все записи типа
	 * "имя -> тег" и заносим их в таблицы групп или
	 * пользователей, в зависимости от типа.
	 */
	if (verbose)
		printf ("** Pass 1: Bulding tables of groups and users\n");
	reccnt = 0;
	groupsdump (filltab);
	if (verbose)
		printf ("   %d records, %d groups, %d users\n",
			reccnt, gtab->cnt, utab->cnt);

	/*
	 * Сканируем базу данных, проверяем все записи
	 * на соответствие таблицам групп и пользователей.
	 */
	if (verbose)
		printf ("** Pass 2: Roughly checking all records\n");
	reccnt = 0;
	groupsdump (checkrecords);
	if (verbose)
		printf ("   %d records checked\n", reccnt);

	/*
	 * Удаляем все некорректные записи, накопленные
	 * в таблице garbage.
	 */
	if (garbage->cnt) {
		if (verbose)
			printf ("DELETING %d INVALID RECORDS\n", garbage->cnt);
		key = vdbm_firstkey (garbage);
		while (key.dptr) {
			groupsdelrec (key.dptr, key.dsize);
			key = vdbm_nextkey (garbage, key);
		}
	}
	vdbm_close (garbage);

	/*
	 * Проходим по таблицам gtab и utab, проверяем
	 * наличие в базе всех соответствующих записей
	 * и их корректность.
	 */
	if (verbose)
		printf ("** Pass 3: Thoroughly checking user records\n");
	reccnt = 0;
	key = vdbm_firstkey (utab);
	while (key.dptr) {
		ucheck (key.dptr);
		++reccnt;
		key = vdbm_nextkey (utab, key);
	}
	if (verbose)
		printf ("   %d user records checked\n", reccnt);

	if (verbose)
		printf ("** Pass 4: Thoroughly checking group records\n");
	reccnt = 0;
	key = vdbm_firstkey (gtab);
	while (key.dptr) {
		gcheck (key.dptr);
		++reccnt;
		key = vdbm_nextkey (gtab, key);
	}
	if (verbose)
		printf ("   %d group records checked\n", reccnt);
}

prstring (s, n)
char *s;
{
	int c;

	for (; --n >= 0; ++s) {
		c = (unsigned char) *s;
		if (c == '\\') {
			putchar ('\\');
			putchar ('\\');
		} else if (c >= ' ' && c <= '~' || c >= 0300 && c <= 0377 ||
		    c == 0xa3 || c==0xb3)
			putchar (c);
		else
			printf ("\\%03o", c);
	}
}

prrecord (str, k, ks, v, vs)
char *str, *k, *v;
{
	printf ("%s: ", str);
	if (ks == sizeof (long) && (*(long*)k & TAGLABEL))
		printf ("0x%08lx", *(long *) k);
	else if (ks) {
		putchar ('"');
		prstring (k, ks);
		putchar ('"');
	} else
		printf ("<null>");
	printf (" --> ");
	if (vs == sizeof (long) && (*(long*)v & TAGLABEL))
		printf ("0x%08lx", *(long *) v);
	else if (vs) {
		putchar ('"');
		prstring (v, vs);
		putchar ('"');
	} else
		printf ("<null>");
	putchar ('\n');
}

gcheck (name)
char *name;
{
	long tag;
	char *name1;
	struct subscrtab *s, *p;
	int n, i, k;

	tag = grouptag (name);
	if (! tag) {
		error ("cannot find group %s", name);
del:            error ("DELETING GROUP %s", name);
		delgroup (name);
		return;
	}

	/* Проверяем наличие и корректность записи тег -> имя */
	name1 = groupname (tag);
	if (strcmp (name1, name)) {
		error ("group name '%s' != '%s'", name1, name);
		goto del;
	}

	/* Проверяем наличие и корректность таблицы подписки */
	s = groupsubscr (tag, &n);
	if (! n)
		return;
	p = (struct subscrtab *) malloc (n * sizeof (struct subscrtab));
	if (! p) {
		error ("out of memory in gcheck");
		quit ();
	}
	memcpy (p, s, n * sizeof (struct subscrtab));
	s = p;
	for (i=k=0; i<n; ++i) {
		if (username (s[i].tag)) {
			if (i != k)
				s[k] = s[i];
			++k;
		}
	}
	if (k < n) {
		error ("UPDATING SUBSCRIPTION LIST OF GROUP %s", name);
		setsubscr (tag, s, k);
	}
	free ((char *) s);
}

ucheck (name)
char *name;
{
	long tag;
	char *name1;

	tag = usertag (name);
	if (! tag) {
		error ("cannot find user %s", name);
del:            error ("DELETING USER %s", name);
		deluser (name);
		return;
	}

	/* Проверяем наличие и корректность записи тег -> имя */
	name1 = username (tag);
	if (! name1) {
		error ("user without name '%s'", name);
		goto del;
	}
	if (strcmp (name1, name)) {
		error ("user name '%s' != '%s'", name1, name);
		goto del;
	}
}
