/*
 * Устойчивая к сбоям (crash-proof) реализация базы данных
 * с интерфейсом NDBM.
 *
 * Основная идея - работа с основным массивом базы
 * только на чтение, и хранение списка дополнений/изменений
 * в отдельном файле.  Когда изменений накапливается достаточно много,
 * создается копия базы данных, в нее вносятся изменения,
 * после чего она становится на место предыдущей.
 * Файл изменений всегда дописывается только в конец.
 *
 * Такой алгоритм гарантирует, что если в какой-либо момент
 * программа (или машина) упадет, база останется в корректном
 * состоянии и будет потеряна только та информация,
 * которая еще не была занесена в файл изменений.
 *
 * Автор Сергей Вакуленко, <vak@kiae.su>.
 */

/*
 * CDBM *cdbm_open (char *name, int flags, int mode)
 *
 * void cdbm_close (CDBM *db)
 *
 * datum cdbm_firstkey (CDBM *db)
 *
 * datum cdbm_nextkey (CDBM *db, datum key)
 *
 * datum cdbm_fetch (CDBM *db, datum key)
 *
 * int cdbm_store (CDBM *db, datum key, datum val, int flag)
 *
 * int cdbm_delete (CDBM *db, datum key)
 *
 * Дополнительные функции:
 *
 * int cdbm_sync (CDBM *db)
 *              - внесение изменений в базу
 */
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>

# include "gdbm.h"

# define NO_DATUM
# include "cdbm.h"

/*
 * Начальная длина таблицы изменений.
 */
# define INITSZ         64

/*
 * Выравнивание счетчика p в большую сторону до длины,
 * кратной размеру длинного целого.
 */
# define ALIGN(p)       (((p) + sizeof (long) - 1) / sizeof (long) * sizeof (long))

/*
 * Вычисление по адресу ячейки celem адреса ключа.
 * Он находится в памяти сразу за структурой celem,
 * с учетом выравнивания на границу длинного целого.
 */
# define KEYDATA(p)     ((char *) (p) + ALIGN (sizeof (celem)))

/*
 * Вычисление по адресу ячейки celem адреса значения.
 * Он находится в памяти за структурой celem и ключом,
 * с учетом выравнивания на границу длинного целого.
 */
# define VALDATA(p)     ((char *) (p) + ALIGN (sizeof (celem)) + ALIGN ((p)->keysize))

struct cdbm_elem {
	short keysize;                  /* длина ключа */
	short valsize;                  /* длина данных, -1 - запись удалена */
	short hash;                     /* хэш-значение ключа */
	/* char key [keysize]; */       /* ключ */
	/* char val [valsize]; */       /* данные */
};

/* Максимальное значение размеров ключа/элемента */
# define MAX_KEYSIZE (1024)
# define MAX_VALSIZE (1024*30)

typedef struct cdbm_elem celem;

static short hash;
static long  db_mtime;
int  UPDATETIME;

static celem **cfind ();
static short chash ();
static int crehash ();
static int cload (), cupdate ();
static void cappend ();
extern long time();

extern int errno;
extern gdbm_error gdbm_errno;

extern char *memcpy (), *malloc (), *calloc (), *strdup (), *realloc ();
extern char *strcpy ();
extern void free ();
extern int memcmp ();
extern unsigned int strlen ();
extern long lseek ();

/*
 * Открывает базу с блокировкой.
 * Если не открывается, возвращает 0.
 * При этом errno содержит -1 если база заблокирована
 * иначе 0.
 */
int cdbm_error = 0;
CDBM *cdbm_open (name, flags, mode)
char *name;
int flags, mode;
{
	register CDBM *db;
	int len;

	errno = 0;
	cdbm_error = 1;
	db = (CDBM *) malloc (sizeof (CDBM));
	if (! db)
		return (0);

	len = strlen (name);
	db->basefile = malloc (len + 1);
	cdbm_error = 2;
	if (! db->basefile) {
		free ((char *) db);
		return (0);
	}
	db->updatefile = malloc (len + 2);
	cdbm_error = 3;
	if (! db->updatefile) {
		free (db->basefile);
		free ((char *) db);
		return (0);
	}
	strcpy (db->basefile, name);
	strcpy (db->updatefile, name);
	db->updatefile [len] = '+';
	db->updatefile [len + 1] = 0;

	db->mode = mode;
	db->size = INITSZ;
	db->updatelimit = 32;   /* 32 килобайта */
	db->tab = (celem **) calloc (db->size, sizeof (celem *));
	cdbm_error = 4;
	if (! db->tab) {
		free (db->basefile);
		free (db->updatefile);
		free ((char *) db);
		return (0);
	}
	db->cnt = 0;
	db->nextindex = 0;

	if (access (db->updatefile, 0) != 0)
		close (creat (db->updatefile, db->mode));
	cdbm_error = 5;
	db->fd = open (db->updatefile, 2);
	if (db->fd < 0) {
		free ((char *) db->tab);
		free (db->basefile);
		free (db->updatefile);
		free ((char *) db);
		return (0);
	}

	cdbm_error = 6;
	if ((flags & O_TRUNC) == O_TRUNC) {
		if (access (db->basefile, 0) == 0 && unlink (db->basefile) < 0 ||
		    access (db->updatefile, 0) == 0 && unlink (db->updatefile) < 0) {
			close (db->fd);
			free ((char *) db->tab);
			free (db->basefile);
			free (db->updatefile);
			free ((char *) db);
			return (0);
		}
	}
	flags &= O_RDONLY | O_RDWR | O_CREAT;
	db->readonly = (flags == O_RDONLY);

	if (access (db->basefile, 0) != 0) {
		if (flags != (O_RDWR | O_CREAT))
			goto err1;
		/* Нет такого файла, создаем новый GDBM-файл */
		db->dbm = gdbm_open (db->basefile, 512, 3, db->mode, (char *) 0);
		if (! db->dbm) {
err1:                   close (db->fd);
			free ((char *) db->tab);
			free (db->basefile);
			free (db->updatefile);
			free ((char *) db);
			return (0);
		}
		gdbm_close (db->dbm);
	}

	/*
	 * Открываем GDBM на запись с целью заблокировать
	 * дальнейшие обращения.  На самом деле
	 * писать туда не будем.
	 */
	cdbm_error = 7;
	db->dbm = gdbm_open (db->basefile, 512, 1, db->mode, (char *) 0);
	if (! db->dbm) {
		close (db->fd);
		free ((char *) db->tab);
		free (db->basefile);
		free (db->updatefile);
		free ((char *) db);
		if (gdbm_errno == GDBM_CANT_BE_WRITER)
			errno = -1;
		return (0);
	}

	/* загружаем список изменений */
	cdbm_error = 8;

	if (! cload (db)) {
		gdbm_close (db->dbm);
		close (db->fd);
		free ((char *) db->tab);
		free (db->basefile);
		free (db->updatefile);
		free ((char *) db);
		return (0);
	}
	return (db);
}

void cdbm_close (db)
register CDBM *db;
{
	int i;

	close (db->fd);
	gdbm_close (db->dbm);
	for (i=0; i<db->size; ++i)
		if (db->tab [i])
			free ((char *) db->tab [i]);
	free (db->basefile);
	free (db->updatefile);
	free ((char *) db->tab);
	free ((char *) db);
}

static datum cnextmem (db, p)
register CDBM *db;
register celem **p;
{
	datum key;

	/* Смотрим таблицу изменений */
	for (; p<db->tab+db->size; ++p) {
		if (*p && (*p)->valsize != -1) {
			key.dptr = KEYDATA (*p);
			key.dsize = (*p)->keysize;
			return (key);
		}
	}
	/* Нет больше записей */
	key.dptr = 0;
	key.dsize = 0;
	return (key);
}

datum cdbm_firstkey (db)
register CDBM *db;
{
	datum key;
	static char *garbage;

	/*
	 * Сначала перебираем то, что в базе.
	 * Но приходится проверять в таблице изменений,
	 * а вдруг запись уже удалена или изменилась.
	 */
	key = gdbm_firstkey (db->dbm);
	if (garbage)
		free (garbage);
	garbage = key.dptr;
	while (key.dptr) {
		if (! *cfind (db, key.dptr, key.dsize))
			return (key);
		key = gdbm_nextkey (db->dbm, key);
		if (garbage)
			free (garbage);
		garbage = key.dptr;
	}
	/* Если в базе записей нет, переходим к таблице изменений */
	return (cnextmem (db, db->tab));
}

datum cdbm_nextkey (db, key)
register CDBM *db;
datum key;
{
	register celem **p;
	static char *garbage;

	p = cfind (db, key.dptr, key.dsize);
	if (! *p) {
		/* Перебираем записи в базе. */
		for (;;) {
			key = gdbm_nextkey (db->dbm, key);
			if (garbage)
				free (garbage);
			garbage = key.dptr;
			if (! key.dptr)
				return (cnextmem (db, db->tab));
			if (! *cfind (db, key.dptr, key.dsize))
				return (key);
		}
	}
	/* Если в базе записей нет, переходим к таблице изменений */
	return (cnextmem (db, ++p));
}

datum cdbm_fetch (db, key)
register CDBM *db;
datum key;
{
	datum val;
	register celem **p;
	static char *garbage;

	val.dptr = 0;
	val.dsize = 0;
	if (garbage)
		free (garbage);
	garbage = 0;

	/* Сначала ищем в таблице изменений */
	p = cfind (db, key.dptr, key.dsize);
	if (*p) {
		/* Запись удалена? */
		if ((*p)->valsize == -1)
			return (val);
		/* Запись найдена. */
		val.dptr = VALDATA (*p);
		val.dsize = (*p)->valsize;
		return (val);
	}
	val = gdbm_fetch (db->dbm, key);
	garbage = val.dptr;
	return (val);
}

int cdbm_store (db, key, val, flag)
register CDBM *db;
datum key, val;
int flag;
{
	register celem **p;

	if (db->readonly)
		return (-1);
	p = cfind (db, key.dptr, key.dsize);
	if (*p) {
		if (! flag)
			return (1);
		--db->cnt;
		free ((char *) *p);
	}
	return (cupdate (db, p, key, val));
}

static int cupdate (db, p, key, val)
register CDBM *db;
register celem **p;
datum key, val;
{
	int sz;
	static int nnn = 0;
	long curtime = time((long *)0);

	sz = ALIGN (sizeof (celem)) + ALIGN (key.dsize);
	if (val.dptr)
		sz += val.dsize;
	*p = (celem *) malloc (sz);
	if (! *p)
		return (-1);
	(*p)->hash = hash;
	(*p)->keysize = key.dsize;
# if 0
	if ( nnn++ < 20 ) {
	       printf("cl=%d sz=%d key_size=%d data_size=%d kd=%d vd=%d\n",
	       sizeof (celem), sz, key.dsize, val.dsize, (int)(KEYDATA(*p))-(int)(*p),(int)(VALDATA(*p))-(int)(*p));
	}
# endif
	if (key.dsize)
		memcpy (KEYDATA (*p), key.dptr, key.dsize);
	if (val.dptr) {
		(*p)->valsize = val.dsize;
		if (val.dsize)
			memcpy (VALDATA (*p), val.dptr, val.dsize);
	} else
		(*p)->valsize = -1;

	++db->cnt;

	/* Делаем дозапись в файл изменений */
	cappend (db->fd, *p);

	if (db->cnt > db->size*3/4)
		if (crehash (db, db->size * 2) < 0)
			return (-1);
	if ( db_mtime == 0 ) {
	       struct stat stbuf;
	       if ( stat(db->basefile,&stbuf) == 0 )
		       db_mtime = stbuf.st_mtime;
	       else
		       db_mtime = curtime;
	}
	if ( (UPDATETIME > 0 && curtime - db_mtime > UPDATETIME*60 ) ||
	      lseek (db->fd, 0L, 1) > db->updatelimit * 1024L)
	{
		/* на всякий случай, если там не доработает */
		db_mtime = curtime;
		/* теперь пересобираем базу */
		cdbm_sync (db);
	}
	return (0);
}

int cdbm_delete (db, key)
register CDBM *db;
datum key;
{
	datum val;
	register celem **p;

	if (db->readonly)
		return (-1);
	p = cfind (db, key.dptr, key.dsize);
	if (*p) {
		if ((*p)->valsize == -1)
			return (0);
		--db->cnt;
		free ((char *) *p);
	} else {
		val = gdbm_fetch (db->dbm, key);
		if (! val.dptr)
			return (-1);
		free (val.dptr);
	}
	val.dptr = 0;
	return (cupdate (db, p, key, val));
}

static celem **cfind (db, k, sz)
register CDBM *db;
char *k;
int sz;
{
	register celem **p;

	hash = chash (k, sz);
	p = db->tab + hash % db->size;
	while (*p) {
		if ((*p)->hash == hash && (*p)->keysize == sz &&
		    ! memcmp (KEYDATA (*p), k, sz))
			break;
		++p;
		if (p >= db->tab + db->size)
			p = db->tab;
	}
	return (p);
}

static short chash (p, sz)
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
	return (v & 0x7fff);
}

static int crehash (db, newsz)
register CDBM *db;
int newsz;
{
	CDBM newdb;
	celem **p, **q;

	newdb.size = newsz;
	newdb.tab = (celem **) calloc (newdb.size, sizeof (celem *));
	if (! newdb.tab)
		return (-1);
	for (q=db->tab; q<db->tab+db->size; ++q)
		if (*q) {
			p = cfind (&newdb, KEYDATA (*q), (*q)->keysize);
			*p = *q;
		}
	free ((char *) db->tab);
	db->size = newdb.size;
	db->tab = newdb.tab;
	return (0);
}

static void cappend (fd, p)
int fd;
register celem *p;
{
	/* Добавление еще одной записи в конец файла изменений */

	write (fd, (char *) p, sizeof (celem));
	write (fd, (char *) KEYDATA (p), p->keysize);
	if (p->valsize != -1)
		write (fd, (char *) VALDATA (p), p->valsize);
}

static int cload (db)
register CDBM *db;
{
	/* Загрузка файла изменений в память */
	/* Я тут проправил все места так, что если файл недописан,
	 * ничего страшного не происходит - просто последняя запись не читается
	 */
	celem h, *p, **q;
	int sz, rez;
	int _nrec=0;

	for (;;) {
		rez = read (db->fd, (char *) &h, sizeof (celem));
		_nrec++;
		cdbm_error = 101;
		if (rez < 0)
			return (0);
		if (rez == 0)
			return (1);
		if (rez != sizeof (celem))
			return (1);
		if (h.keysize < 0 || h.keysize > MAX_KEYSIZE) {
		       error("Bad groups+: NREC=%d",_nrec);
		       error("... shift=%ld", lseek(db->fd,0l,1) - rez);
		       error("Bad groups+: key_size = %d (reading + aborted)",h.keysize);
		       return(1);
		}
		if ( h.valsize < -1 || h.valsize >= MAX_VALSIZE) {
		       error("Bad groups+: NREC=%d",_nrec);
		       error("... shift=%ld", lseek(db->fd,0l,1) - rez);
		       error("Bad groups+: val_size = %d (reading + aborted)",h.valsize);
		       return(1);
		}
		sz = ALIGN (sizeof (celem)) + ALIGN (h.keysize);
		if (h.valsize != -1)
			sz += h.valsize;
		p = (celem *) malloc (sz);
		if (! p)
			return (0);
		*p = h;
		if (p->keysize) {
			if (read (db->fd, KEYDATA (p), p->keysize) != p->keysize)
				return (1);
		}
		if (p->valsize > 0 ) {
			if (read (db->fd, VALDATA (p), p->valsize) != p->valsize)
				return (1);
		}
		q = db->tab + p->hash % db->size;
		while (*q) {
			if ((*q)->hash == p->hash &&
			    (*q)->keysize == p->keysize &&
			    ! memcmp (KEYDATA (*q), KEYDATA (p), p->keysize))
				break;
			++q;
			if (q >= db->tab + db->size)
				q = db->tab;
		}
		*q = p;

		++db->cnt;
		if (db->cnt > db->size*3/4)
			if (crehash (db, db->size * 2) < 0)
				return (-1);
	}
}


void cdbm_sync (db)
register CDBM *db;
{
	/* Перепись базы данных с внесением изменений */
	char *newname, *oldname, *oldoldname;
	char *garbage;
	int len;
	GDBM_FILE newdbm;
	datum key, val;
	register celem **p;

	if (db->cnt == 0)
		return;
	newdbm = 0;

	/* Заводим имена database~~, database~, database# */
	len = strlen (db->basefile);
	newname = malloc (len + 2);
	oldname = malloc (len + 2);
	oldoldname = malloc (len + 3);
	if (! newname || ! oldname || !oldoldname)
		return;
	strcpy (newname, db->basefile);
	strcpy (oldname, db->basefile);
	strcpy (oldoldname, db->basefile);
	newname [len] = '#';
	newname [len+1] = 0;
	oldname [len] = '~';
	oldname [len+1] = 0;
	oldoldname [len] = '~';
	oldoldname [len+1] = '~';
	oldoldname [len+2] = 0;

	/* Создаем новую базу данных */
	newdbm = gdbm_open (newname, 512, 3, db->mode, (char *) 0);
	if (! newdbm)
		goto ret;

	/* Переписываем базу на новое место */
	key = gdbm_firstkey (db->dbm);
	while (key.dptr) {
		val = gdbm_fetch (db->dbm, key);
		if (val.dptr) {
			if (gdbm_store (newdbm, key, val, 0)) {
				free (key.dptr);
				free (val.dptr);
				goto ret;
			}
			free (val.dptr);
		}
		garbage = key.dptr;
		key = gdbm_nextkey (db->dbm, key);
		free (garbage);
	}

	/* Вносим изменения */
	for (p=db->tab; p<db->tab+db->size; ++p) {
		if (! *p)
			continue;
		if ((*p)->valsize == -1) {
			key.dptr = KEYDATA (*p);
			key.dsize = (*p)->keysize;
			gdbm_delete (newdbm, key);
			continue;
		}
		key.dptr = KEYDATA (*p);
		key.dsize = (*p)->keysize;
		val.dptr = VALDATA (*p);
		val.dsize = (*p)->valsize;
		if (gdbm_store (newdbm, key, val, 1))
			goto ret;
	}

	/* Закрываем новую базу */
	gdbm_close (newdbm);
	newdbm = 0;

	/*
	 * Переименовываем базу :
	 * database~ -> database~~
	 * database  -> database~
	 * database# -> database
	 */
	unlink (oldoldname);                    /* Удаляем database~~ */
	link (oldname, oldoldname);             /* database~ -> database~~ */
	unlink (oldname);                       /* Удаляем database~ */
	if (link (db->basefile, oldname) < 0)   /* database -> database~ */
		goto ret;
	unlink (db->basefile);                  /* Удаляем database */
	/*
	 * Самое тонкое место.
	 * Если в этот момент машина упадет,
	 * база данных будет испорчена!
	 * Вот если бы можно было переименовывать
	 * два файла одновременно, за один системный вызов...
	 */
	if (link (newname, db->basefile) < 0) { /* database# -> database */
		link (oldname, db->basefile);
		goto ret;
	}
	unlink (newname);                       /* Удаляем database# */

	/* Удаляем список изменений */
	close (db->fd);
	for (p=db->tab; p<db->tab+db->size; ++p)
		if (*p)
			free ((char *) *p);
	unlink (db->updatefile);

	/* Создаем пустой список изменений */
	db->size = INITSZ;
	db->cnt = 0;
	db->nextindex = 0;
	free ((char *) db->tab);
	db->tab = (celem **) calloc (db->size, sizeof (celem *));
	close (creat (db->updatefile, db->mode));
	db->fd = open (db->updatefile, 2);
	if (! db->tab || db->fd < 0)    /* Этого не может быть! */
		abort ();

	/* Переоткрываем новую базу */
	gdbm_close (db->dbm);
	/*
	 * Открываем GDBM на запись с целью заблокировать
	 * дальнейшие обращения.  На самом деле
	 * писать туда не будем.
	 */
	db->dbm = gdbm_open (db->basefile, 512, 1, db->mode, (char *) 0);
	if (! db->dbm)
		abort ();
ret:
	free (newname);
	free (oldname);
	free (oldoldname);
	if (newdbm)
		gdbm_close (newdbm);
	db_mtime = time((long *)0);
	return;
}

# ifdef TESTCDBM
# include <stdio.h>

main ()
{
	CDBM *db;
	char keystr [256], valstr [256], *s;
	datum key, val;

	db = cdbm_open (0);
	while (! feof (stdin)) {
		scanf ("%s ", keystr);
		gets (valstr);
		for (s=valstr; *s==' ' || *s=='\t'; ++s);
		key.dptr = keystr;
		key.dsize = strlen (keystr) + 1;
		val.dptr = valstr;
		val.dsize = strlen (s) + 1;
		if (cdbm_store (db, key, val, 1) < 0)
			fprintf (stderr, "cannot store '%s' - '%s'\n", keystr, valstr);
	}
	key = cdbm_firstkey (db);
	while (key.dptr) {
		val = cdbm_fetch (db, key);
		if (val.dptr)
			printf ("%s\t%s\n", key.dptr, val.dptr);
		else
			fprintf (stderr, "cannot fetch '%s' (%d)\n",
				key.dptr, key.dsize);
		key = cdbm_nextkey (db, key);
	}
	return (0);
}
# endif
