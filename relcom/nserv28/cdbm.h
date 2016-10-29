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
 * vdatum cdbm_firstkey (CDBM *db)
 *
 * vdatum cdbm_nextkey (CDBM *db, vdatum key)
 *
 * vdatum cdbm_fetch (CDBM *db, vdatum key)
 *
 * int cdbm_store (CDBM *db, vdatum key, vdatum val, int flag)
 *
 * int cdbm_delete (CDBM *db, vdatum key)
 */

# define CDBM_INSERT  0
# define CDBM_REPLACE 1

# ifndef NO_DATUM
typedef struct {
	char *dptr;                     /* данные */
	int dsize;                      /* длина данных */
} datum;
# endif

struct cdbm_elem;

typedef struct {
	char *basefile;         /* имя файла базы данных */
	char *updatefile;       /* имя файла изменений */
	void *dbm;              /* база данных */
	struct cdbm_elem **tab; /* таблица элементов */
	int mode;               /* режим доступа к базе */
	int fd;                 /* дескриптор файла изменений */
	int size;               /* длина таблицы tab, степень (!) двойки */
	int cnt;                /* количество элементов в tab, <=70% от size */
	int nextindex;          /* следующий индекс в tab для перебора */
	int readonly;           /* только читать */
	int updatelimit;        /* максимальный размер файла изменений (K) */
} CDBM;

extern CDBM     *cdbm_open ();
extern void     cdbm_close ();
extern datum    cdbm_fetch ();
extern int      cdbm_store ();
extern int      cdbm_delete ();
extern datum    cdbm_firstkey ();
extern datum    cdbm_nextkey ();

extern void     cdbm_sync ();
