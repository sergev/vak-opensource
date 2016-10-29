/*
 * Типы символов
 */

# define _U     01      /* Прописная буква */
# define _L     02      /* Строчная буква */
# define _N     04      /* Цифра */
# define _S     010     /* Пробел (' ', '\t', '\n') */
# define _P     020     /* Знак пунктуации */
# define _C     040     /* Управляющий символ */
# define _X     0100    /* Шестнадцатиричная цифра */
# define _R     0200    /* Русская буква */

/*
 * Таблица типов символов кода КОИ-8.
 * Должна находится в библиотеке /lib/libc.a.
 * Исходный текст в /src/lib/libc/gen/ctype_.c
 */

extern  char _ctype_ [];

# define isalpha(c)     ((_ctype_+1)[((c)&0377)]&(_U|_L))
# define isupper(c)     ((_ctype_+1)[((c)&0377)]&_U)
# define islower(c)     ((_ctype_+1)[((c)&0377)]&_L)
# define isdigit(c)     ((_ctype_+1)[((c)&0377)]&_N)
# define isxdigit(c)    ((_ctype_+1)[((c)&0377)]&(_N|_X))
# define isspace(c)     ((_ctype_+1)[((c)&0377)]&_S)
# define ispunct(c)     ((_ctype_+1)[((c)&0377)]&_P)
# define isalnum(c)     ((_ctype_+1)[((c)&0377)]&(_U|_L|_N))
# define isprint(c)     ((_ctype_+1)[((c)&0377)]&(_P|_U|_L|_N))
# define iscntrl(c)     ((_ctype_+1)[((c)&0377)]&_C)
# define iscyrill(c)    ((_ctype_+1)[((c)&0377)]&_R)
# define islatin(c)     (isalpha(c) && !iscyrill(c))
# define toupper(c)     (iscyrill(c)?((c)-'а'+'А'):((c)-'a'+'A'))
# define tolower(c)     (iscyrill(c)?((c)-'А'+'а'):((c)-'A'+'a'))
# define isascii(c)     ((((c)&0377)<=0177)||((((c)&0377)>=0300)&&(((c)&0377)<=0377)))
# define toascii(c)     ((c)&0377)

/* Ретро */
# define iscirill(c)    iscyrill(c)
# define iskirill(c)    iscyrill(c)
# define islat(c)       islatin(c)
