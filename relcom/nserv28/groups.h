/*
 * Информация о группе.
 * Группа имеет имя, количество и перечень подписанных пользователей
 * и перечень соответствующих режимов подписки.
 * Поле lastarticle хранит номер последней обработанной
 * статьи.  Все статьи с большим номером считаются "новыми".
 *
 * Кроме этого, неплохо бы хранить статистику по обращению к группе:
 * feedcnt        - счетчик разосланных статей (feed)
 * notifycnt      - счетчик разосланных каталогов (notify)
 * querycnt       - счетчик разосланных статей (article)
 * postcnt        - счетчик статей, отправленных в USENET
 * subscribecnt   - счетчик подписки
 * unsubscribecnt - счетчик отписки
 * totalbytes     - количество разосланных байт < 1 Гб
 * totalgbytes    - количество разосланных гигабайт
 * postbytes      - количество отправленных в USENET байт < 1 Гб
 * postgbytes     - количество отправленных в USENET гигабайт
 *
 * Информация о пользователе.
 * Пользователь имеет имя и режим упаковки.
 * Статистику бы, аналогично группе.
 */

/* Флаги пользователя */

# define UFLAGPACK      01      /* Режим упаковки */
# define UFLAGAGE_M    036      /* Маска поля - старение */
# define UFLAGAGE_N    036      /* Не стареть */
# define UFLAGAGE_1    002      /* Прибавляется к флагу при старении */
# define UFLAGAGE_O    010      /* 02 04 06  10 - все, выкинуть      */
# define UFLAG_NA(f)   (((f)&UFLAGAGE_M)>>1)

/*
 * Информация о подписке.
 */

struct subscrtab {
	long    tag;
	short   mode;
};

struct activetab {
	long first;
	long last;
	char mode;
};

struct stattab {
	int subs;
	int feed;
	int rfeed;
};

# ifdef __STDC__
#    define ARGS(x) x
#    define ARGS2(x,y) x,y
#    define ARGS3(x,y,z) x,y,z
#    define ARGS4(x,y,z,t) x,y,z,t
# else
#    define ARGS(x)
#    define ARGS2(x,y)
#    define ARGS3(x,y,z)
#    define ARGS4(x,y,z,t)
# endif

/* Информация из /usr/lib/news/active */
extern struct activetab *getactive (ARGS( char *name ));

/* Загрузка базы данных */
extern int loadgroups (ARGS( int ));

/* Запись базы данных */
extern int savegroups (ARGS( void ));

/* Выдает тег группы по имени */
extern long grouptag (ARGS( char *name ));

/* Выдает тег пользователя по имени */
extern long usertag (ARGS( char *name ));

/* Создание записей о пользователе */
extern long adduser (ARGS( char *name ));

/* Удаление записей о пользователе */
extern deluser (ARGS( char *name ));

/* Удаление нескольких пользователей */
extern int *delusertab (ARGS2( long *tab, int len ));

/* Выдает таблицу подписки группы */
extern struct subscrtab *groupsubscr (ARGS2( long tag, int *cnt ));

/* Выдает таблицу подписки пользователя */
extern struct subscrtab *usersubscr (ARGS3( long tag, char **gtab, int *cnt ));

/* Создание записей о новой группе */
extern long addgroup (ARGS2( char *name, long last ));

/* Удаление записей о группе */
extern delgroup (ARGS( char *name ));

/* Подписать пользователя на группу */
extern subscribe (ARGS3( long user, long group, int mode ));

/* Отписать пользователя от группы */
extern unsubscribe (ARGS2( long user, long group ));

/* Отписать пользователей от всех групп */
/* Память возвращаемого списка должен освобождать вызывающий */
extern int *unsubscrtab (ARGS2( long *tab, int len ));

/* Отписать от всех групп */
extern unsubscrall (ARGS( long user ));

/* Описание группы */
extern char *groupinfo (ARGS( long group ));

/* Флаги пользователя */
extern long userflags (ARGS( long user ));

/* Номер последней статьи группы */
extern long grouplast (ARGS( long group ));

/* Количество подписчиков группы */
extern void groupstat (ARGS4( long group, int *s, int *f, int *r ));

/* Информация о подписке пользователей */
/* Память возвращаемого списка должен освобождать вызывающий */
extern struct stattab *userstat (ARGS2( long *tab, int n ));

/* Название группы */
extern char *groupname (ARGS( long group ));

/* Адрес пользователя */
extern char *username (ARGS( long user ));

/* Список тегов всех пользователей */
/* Память возвращаемого списка должен освобождать вызывающий */
extern long *userlist (ARGS( int * ));

/* Список тегов всех групп */
/* Память возвращаемого списка должен освобождать вызывающий */
extern long *grouplist (ARGS( int * ));

extern long firstuser (ARGS( void ));
extern long nextuser (ARGS( long ));
extern long firstgroup (ARGS( void ));
extern long nextgroup (ARGS( long ));
extern char *firstactive (ARGS( void ));
extern char *nextactive (ARGS( char * ));

extern void groupsdump (ARGS( int (*) () ));
extern void groupsdelrec (ARGS2( char *, int ));
extern void groupslimit (ARGS( int ));
extern void groupssync (ARGS( void ));

extern void setuserflags (ARGS2( long tag, long flags ));
extern void setsubscr (ARGS3( long g, struct subscrtab *tab, int n ));
extern void setgroupsfile (ARGS( char *name ));
extern void setgrouplast (ARGS2( long tag, long last ));

# undef ARGS
# undef ARGS2
# undef ARGS3
