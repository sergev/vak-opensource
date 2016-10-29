/* маски полей тегов группы и пользователя */

# define TAGLABEL       0x80000000L     /* признак тега, есть всегда */
# define TAGUSER        0x40000000L     /* тег пользователя */
# define TAGFLAGMASK    0x30000000L     /* маска флагов */
# define TAGNUMMASK     0x000fffffL     /* маска номера */

/* флаги тега группы */

# define TAGNAME        0x10000000L     /* имя группы */
# define TAGSUBSCR      0x20000000L     /* таблица подписки */
# define TAGLASTART     0x30000000L     /* номер последней статьи */

/* флаги тега пользователя */

# define TAGNAME        0x10000000L     /* имя пользователя */
# define TAGUFLAGS      0x20000000L     /* флаги пользователя */
