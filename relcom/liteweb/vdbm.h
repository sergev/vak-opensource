/*
 * Реализация базы данных в оперативной памяти
 * с хэш-поиском.  Интерфейс совместим с NDBM.
 *
 * Количество записей не ограничено, таблица растет
 * по мере заполнения и уменьшается по мере удаления записей.
 *
 * Автор Сергей Вакуленко, <vak@kiae.su>.
 */

/*
 * VDBM *vdbm_open (int sz)
 *
 * void vdbm_close (VDBM *db)
 *
 * vdatum vdbm_firstkey (VDBM *db)
 *
 * vdatum vdbm_nextkey (VDBM *db, vdatum key)
 *
 * vdatum vdbm_fetch (VDBM *db, vdatum key)
 *
 * int vdbm_store (VDBM *db, vdatum key, vdatum val, int flag)
 *
 * int vdbm_delete (VDBM *db, vdatum key)
 */

# define VDBM_INSERT  0
# define VDBM_REPLACE 1

typedef struct {
	char *dptr;     /* данные */
	int dsize;      /* длина данных */
} vdatum;

typedef struct _elem {
	short keysize;                  /* длина ключа */
	short valsize;                  /* длина данных */
	/* char key [keysize]; */       /* ключ */
	/* char val [valsize]; */       /* данные */
} velem;

typedef struct {
	int size;       /* длина таблицы tab, обязательно степень двойки */
	int cnt;        /* количество элементов в tab, не больше 70% от size */
	int nextindex;  /* следующий индекс в tab для перебора */
	velem **tab;    /* таблица элементов */
} VDBM;

extern VDBM     *vdbm_open ();
extern void     vdbm_close ();
extern vdatum   vdbm_fetch ();
extern int      vdbm_store ();
extern int      vdbm_delete ();
extern vdatum   vdbm_firstkey ();
extern vdatum   vdbm_nextkey ();
