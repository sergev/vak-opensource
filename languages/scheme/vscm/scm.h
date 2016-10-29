#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef DEBUG
#   define assert(x) { if (! (x)) {\
	fprintf (stderr, "assertion failed: file \"%s\", line %d\n",\
	__FILE__, __LINE__); exit (-1); } }
#   define inline /* no inline */
#else
#   define assert(x)
#endif

/*
 * Метки типов.
 */
#define TPAIR           1               /* метка "пара" */
#define TSYMBOL         2               /* метка "символ" */
#define TBOOL           3               /* метка "булевское" */
#define TCHAR           4               /* метка "буква" */
#define TINTEGER        5               /* метка "целое" */
#define TREAL           6               /* метка "вещественное" */
#define TSTRING         7               /* метка "строка" */
#define TVECTOR         8               /* метка "вектор" */
#define THARDW          9               /* метка "встроенная функция" */
#define TCLOSURE       10               /* метка "замыкание" */

#define NIL             ((LISP) -1)     /* пустой список */
#define TOPLEVEL        ((LISP) -2)     /* признак контекста верхнего уровня */

/*
 * Размер LISP обязан быть больше, чем размер указателя.
 * Это используется для хранения указателя на строку в cdr.
 */
typedef long LISP;                      /* "указатель" на пару */
typedef LISP (*LISPFUNC) (LISP, LISP);  /* указатель на встроенную функцию */

typedef struct {                        /* элементарная ячейка */
	short type;                     /* тип */
	union {
		struct {                /* пара */
			LISP a;         /* первый элемент */
			LISP d;         /* второй элемент */
		} pair;
		struct {                /* строка */
			int length;     /* длина */
			char *array;    /* массив байтов длины length */
		} string;
		struct {                /* вектор */
			int length;     /* длина */
			LISP *array;    /* массив элементов длины length */
		} vector;
		char *symbol;           /* символ */
		unsigned char chr;      /* буква */
		long integer;           /* целое, встроенная функция */
		double real;            /* вещественное */
	} as;
} cell;

typedef struct {
	char *name;
	LISPFUNC func;
} functab;

extern LISP T, ZERO, ENV;

int eqv (LISP a, LISP b);               /* сравнение объектов */
int equal (LISP a, LISP b);             /* рекурсивное сравнение */
LISP evalblock (LISP expr, LISP ctx);   /* вычисление блока */
LISP evalclosure (LISP func, LISP expr); /* вычисление замыкания */
LISP evalfunc (LISP func, LISP arg, LISP ctx); /* вычисление функции */
LISP eval (LISP expr, LISP *ctxp);      /* вычисление */
LISP getexpr ();                        /* чтение выражения */
void putexpr (LISP p, FILE *fd);        /* печать списка */
LISP copy (LISP a, LISP *t);            /* копирование списка */
LISP alloc (int type);                  /* выделение памяти под новую пару */
void fatal (char*);                     /* фатальная ошибка */

extern cell *mem;                       /* память списков */
extern unsigned memsz;                  /* размер памяти */
extern void *memcopy (void*, int);

static inline LISP car (LISP p)         /* доступ к первому элементу */
{
	assert (p>=0 && p<memsz && mem[p].type==TPAIR);
	return mem[p].as.pair.a;
}

static inline LISP cdr (LISP p)         /* доступ ко второму элементу */
{
	assert (p>=0 && p<memsz && mem[p].type==TPAIR);
	return mem[p].as.pair.d;
}

static inline void setcar (LISP p, LISP v) /* доступ к первому элементу */
{
	assert (p>=0 && p<memsz && mem[p].type==TPAIR);
	mem[p].as.pair.a = v;
}

static inline void setcdr (LISP p, LISP v) /* доступ ко второму элементу */
{
	assert (p>=0 && p<memsz && mem[p].type==TPAIR);
	mem[p].as.pair.d = v;
}

static inline LISP cons (LISP a, LISP d) /* выделение памяти под новую пару */
{
	LISP p = alloc (TPAIR);
	setcar (p, a);
	setcdr (p, d);
	return (p);
}

static inline LISP symbol (char *name)  /* создание атома-символа */
{
	LISP p = alloc (TSYMBOL);
	mem[p].as.symbol = memcopy (name, strlen (name) + 1);
	return (p);
}

static inline LISP number (long val)    /* создание атома-числа */
{
	LISP p = alloc (TINTEGER);
	mem[p].as.integer = val;
	return (p);
}

static inline LISP character (int val)  /* создание атома-буквы */
{
	LISP p = alloc (TCHAR);
	mem[p].as.chr = val;
	return (p);
}

static inline LISP real (double val)    /* создание атома-вещественного числа */
{
	LISP p = alloc (TREAL);
	mem[p].as.real = val;
	return (p);
}

static inline LISP string (int len, char *array) /* создание строки */
{
	LISP p = alloc (TSTRING);
	mem[p].as.string.length = len;
	if (len > 0)
		mem[p].as.string.array = memcopy (array, len);
	return (p);
}

static inline LISP vector (int len, LISP *array) /* создание вектора */
{
	LISP p = alloc (TVECTOR);
	mem[p].as.vector.length = len;
	if (len > 0)
		mem[p].as.vector.array = memcopy (array, len * sizeof (LISP));
	return (p);
}

static inline LISP closure (LISP body, LISP ctx) /* создание замыкания */
{
	LISP p = alloc (TCLOSURE);
	mem[p].as.pair.a = body;
	mem[p].as.pair.d = ctx;
	return (p);
}

static inline LISP hardw (LISPFUNC func) /* создание встроенной функции */
{
	LISP p = alloc (THARDW);
	mem[p].as.integer = (long) func;
	return (p);
}

static inline long numval (LISP p)      /* выдать значение целого числа */
{
	assert (p>=0 && p<memsz && mem[p].type==TINTEGER);
	return (mem[p].as.integer);
}

static inline long charval (LISP p)     /* выдать значение буквы */
{
	assert (p>=0 && p<memsz && mem[p].type==TCHAR);
	return (mem[p].as.chr);
}

static inline double realval (LISP p)   /* выдать значение веществ. числа */
{
	assert (p>=0 && p<memsz && mem[p].type==TREAL);
	return (mem[p].as.real);
}

static inline LISPFUNC hardwval (LISP p) /* выдать адрес встроенной функции */
{
	assert (p>=0 && p<memsz && mem[p].type==THARDW);
	return ((LISPFUNC) mem[p].as.integer);
}

static inline LISP closurebody (LISP p) /* выдать замыкание */
{
	assert (p>=0 && p<memsz && mem[p].type==TCLOSURE);
	return (mem[p].as.pair.a);
}

static inline LISP closurectx (LISP p)  /* выдать замыкание */
{
	assert (p>=0 && p<memsz && mem[p].type==TCLOSURE);
	return (mem[p].as.pair.d);
}

static inline char *symname (LISP p)    /* выдать строку - имя символа */
{
	assert (p>=0 && p<memsz && mem[p].type==TSYMBOL);
	return (mem[p].as.symbol);
}

static inline int istype (LISP p, int type) /* проверить соответствие типа */
{
	if (p == NIL) return (0);
	assert (p>=0 && p<memsz);
	return (mem[p].type == type);
}
