/*
 * Реализация базы данных в оперативной памяти
 * с хэш-поиском.  Интерфейс совместим с NDBM.
 *
 * Количество записей не ограничено, таблица растет
 * по мере заполнения и уменьшается по мере удаления записей.
 *
 * Автор Сергей Вакуленко, <vak@kiae.su>.
 */

# include "vdbm.h"

# define INITSZ 64
# define KEYDATA(p) ((char *) ((p)+1))
# define VALDATA(p) ((char *) ((p)+1) + (p)->keysize)

static velem **vfind ();
static long vhash ();
static int vrehash ();

extern char *memcpy (), *malloc (), *calloc ();
extern void free ();
extern int memcmp ();
extern unsigned int strlen ();

VDBM *vdbm_open (sz)
int sz;
{
	register VDBM *h;

	h = (VDBM *) malloc (sizeof (VDBM));
	if (! h)
		return (0);
	if (sz < INITSZ)
		sz = INITSZ;
	h->tab = (velem **) calloc (sz, sizeof (velem *));
	if (! h->tab) {
		free ((char *) h);
		return (0);
	}
	h->size = sz;
	h->cnt = 0;
	h->nextindex = 0;
	return (h);
}

void vdbm_close (db)
register VDBM *db;
{
	int i;

	for (i=0; i<db->size; ++i)
		if (db->tab [i])
			free ((char *) db->tab [i]);
	free ((char *) db->tab);
	free ((char *) db);
}

vdatum vdbm_firstkey (db)
register VDBM *db;
{
	vdatum key;

	key.dptr = 0;
	key.dsize = 0;
	return (vdbm_nextkey (db, key));
}

vdatum vdbm_nextkey (db, key)
register VDBM *db;
vdatum key;
{
	register velem **p;

	if (! key.dptr)
		p = db->tab;
	else {
		p = vfind (db, key.dptr, key.dsize);
		if (! *p)
			goto notfound;
		++p;
	}
	for (; p<db->tab+db->size; ++p)
		if (*p) {
			key.dptr = KEYDATA (*p);
			key.dsize = (*p)->keysize;
			return (key);
		}
notfound:
	key.dptr = 0;
	key.dsize = 0;
	return (key);
}

vdatum vdbm_fetch (db, key)
register VDBM *db;
vdatum key;
{
	vdatum val;
	register velem **p;

	p = vfind (db, key.dptr, key.dsize);
	if (! *p) {
		val.dptr = 0;
		val.dsize = 0;
	} else {
		val.dptr = VALDATA (*p);
		val.dsize = (*p)->valsize;
	}
	return (val);
}

int vdbm_store (db, key, val, flag)
register VDBM *db;
vdatum key, val;
int flag;
{
	register velem **p;

	p = vfind (db, key.dptr, key.dsize);
	if (*p) {
		if (! flag)
			return (1);
		--db->cnt;
		free ((char *) *p);
	}
	*p = (velem *) malloc (sizeof (velem) + key.dsize + val.dsize);
	if (! *p)
		return (-1);
	(*p)->keysize = key.dsize;
	(*p)->valsize = val.dsize;
	if (key.dsize)
		memcpy (KEYDATA (*p), key.dptr, key.dsize);
	if (val.dsize)
		memcpy (VALDATA (*p), val.dptr, val.dsize);
	++db->cnt;
	if (db->cnt > db->size*3/4)
		if (vrehash (db, db->size * 2) < 0)
			return (-1);
	return (0);
}

int vdbm_delete (db, key)
register VDBM *db;
vdatum key;
{
	register velem **p;

	p = vfind (db, key.dptr, key.dsize);
	if (! *p)
		return (-1);
	free ((char *) *p);
	*p = 0;
	--db->cnt;
	if (db->cnt < db->size/4 && db->size > INITSZ)
		vrehash (db, db->size / 2);
	return (0);
}

static velem **vfind (db, k, sz)
register VDBM *db;
char *k;
int sz;
{
	register velem **p;

	p = db->tab + vhash (k, sz) % db->size;
	while (*p) {
		if ((*p)->keysize == sz &&
		    ! memcmp (KEYDATA (*p), k, sz))
			break;
		++p;
		if (p >= db->tab + db->size)
			p = db->tab;
	}
	return (p);
}

static long vhash (p, sz)
register char *p;
register sz;
{
	register long v;

# ifdef GNUHASH
	register i;

	/* hash function from GDBM 1.5 */
	v = 0x238f13af * sz;
	for (i=0; i<sz; ++i, ++p)
		v = (v + (*p << (i*5 % 24))) & 0x7fffffff;
	v = 1103515243 * v + 12345;
# else
	/* hash function from SDBM */
	v = 0;
	while (sz--)
		v = *p++ + 65599 * v;
#endif
	return (v & 0x7fffffff);
}

static int vrehash (db, newsz)
register VDBM *db;
int newsz;
{
	VDBM *newdb;
	velem **p, **q;

	newdb = vdbm_open (newsz);
	if (! newdb)
		return (-1);
	for (q=db->tab; q<db->tab+db->size; ++q)
		if (*q) {
			p = vfind (newdb, KEYDATA (*q), (*q)->keysize);
			*p = *q;
		}
	newdb->cnt = db->cnt;
	free ((char *) db->tab);
	*db = *newdb;
	free ((char *) newdb);
	return (0);
}

# ifdef TESTVDBM
# include <stdio.h>

main ()
{
	VDBM *db;
	char keystr [256], valstr [256], *s;
	vdatum key, val;

	db = vdbm_open (0);
	while (! feof (stdin)) {
		scanf ("%s ", keystr);
		gets (valstr);
		for (s=valstr; *s==' ' || *s=='\t'; ++s);
		key.dptr = keystr;
		key.dsize = strlen (keystr) + 1;
		val.dptr = valstr;
		val.dsize = strlen (s) + 1;
		if (vdbm_store (db, key, val, 1) < 0)
			fprintf (stderr, "cannot store '%s' - '%s'\n", keystr, valstr);
	}
	key = vdbm_firstkey (db);
	while (key.dptr) {
		val = vdbm_fetch (db, key);
		if (val.dptr)
			printf ("%s\t%s\n", key.dptr, val.dptr);
		else
			fprintf (stderr, "cannot fetch '%s' (%d)\n",
				key.dptr, key.dsize);
		key = vdbm_nextkey (db, key);
	}
	return (0);
}
# endif
