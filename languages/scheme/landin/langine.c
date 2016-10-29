# include <stdio.h>

/* # define DEBUG */

# if defined (MSDOS) || defined (__MSDOS__)
#    define SMALL
# endif

# ifdef SMALL
#    define MEMSZ 8000		/* примерно 64к */
# else
#    define MEMSZ 32000		/* около 256к */
# endif

# ifndef __GNUC__
#    define inline static
# endif

/*
 * Метки типов.  Если car содержит метку типа, то cdr содержит
 * значение.  Для чисел это собственно число, для символов - индекс
 * в таблице символов, для строк - указатель на динамически выделенную
 * память.
 */

# define TNUMBER MEMSZ		/* метка "число" */
# define TSYMBOL (MEMSZ+1)	/* метка "символ" */

# define MAXLEX 256		/* максимальный размер одной лексемы */

# define C_LD	1		/* ввод */
# define C_LDC  2               /* ввод константы */
# define C_LDF  3               /* ввод функции */
# define C_AP   4               /* применение функции */
# define C_RTN  5               /* возврат */
# define C_DUM  6               /* создание формального окружения */
# define C_RAP  7               /* рекурсивное применение */
# define C_SEL  8               /* выбор подсписка управления */
# define C_JOIN 9               /* восстановление главного управления */
# define C_CAR  10              /* car от вершины стека */
# define C_CDR  11              /* cdr от вершины стека */
# define C_ATOM 12              /* предикат atom от вершины стека */
# define C_CONS 13              /* формирование cons двух эл-тов стека */
# define C_EQ   14              /* предикат eq от двух эл-тов стека */
# define C_ADD  15              /* сложение двух эл-тов стека */
# define C_SUB  16              /* вычитание двух эл-тов стека */
# define C_MUL  17              /* умножение двух эл-тов стека */
# define C_DIV  18              /* деление двух эл-тов стека */
# define C_REM  19              /* остаток от деления двух эл-тов стека */
# define C_LEQ  20              /* сравнение на <= двух эл-тов стека */
# define C_STOP 21              /* останов */

/*
 * Размер pairp обязан быть больше, чем размер указателя.
 * Это используется для хранения указателя на строку в cdr.
 */

typedef long pairp;		/* "указатель" на пару */

typedef struct {		/* пара, состоит из двух указателей */
	pairp a;		/* первый элемент */
	pairp d;		/* второй элемент */
} pair;

# ifdef DEBUG
pairp debugassert ();
# define car(p) mem[debugassert((pairp)(p),__LINE__)].a
# define cdr(p) mem[debugassert((pairp)(p),__LINE__)].d
# define setcar(p) mem[debugassert((pairp)(p),__LINE__)].a
# define setcdr(p) mem[debugassert((pairp)(p),__LINE__)].d
# else
# define car(p) mem[(int)p].a		/* доступ к первому элементу */
# define cdr(p) mem[(int)p].d		/* доступ ко второму элементу */
# define setcar(p) mem[(int)p].a	/* доступ к первому элементу */
# define setcdr(p) mem[(int)p].d	/* доступ ко второму элементу */
# endif

pair *mem;			/* память списков */
char *gclabel;			/* метки для сбора мусора */

pairp firstfree;		/* список свободных пар */

long lexval;			/* значение лексемы-числа */
char lexsym [MAXLEX];		/* имя лексемы-символа */
int backlexflag = 0;		/* флаг возврата лексемы */
int lexlex;			/* текущая лексема */

pairp S;                        /* регистр стека */
pairp E;                        /* регистр окружения (environment) */
pairp C;                        /* регистр команд */
pairp D;                        /* регистр хранилища (dump) */
pairp W;                        /* рабочий регистр */
pairp T;                        /* атом T */
pairp F;			/* атом F */
pairp NIL;			/* атом NIL */

int trace;			/* флаг трассировки */
int verbose;

char *cmdname [] = {
	"ERR",	"ld",	"ldc",	"ldf",	"ap",	"rtn",	"dum",	"rap",
	"sel",	"join",	"car",  "cdr",  "atom", "cons", "eq",   "add",
	"sub",  "mul",  "div",  "rem",  "leq",  "stop",
};

extern char *malloc (), *strcpy ();
char *strcopy (), *symval ();
pairp getexpr ();
void putexpr (), fatal (), error (), gc ();

inline pairp cons (a, d)        /* выделение памяти под новую пару */
pairp a, d;
{
	register pairp p;

	/* во избежание потери данных при сборке мусора
	 * можно вызывать cons только от регистров
	 * S E C D T F W NIL
	 */
	if (firstfree < 0)
		gc ();
	p = firstfree;
	firstfree = cdr (firstfree);
	setcar (p) = a;
	setcdr (p) = d;
	return (p);
}

inline pairp symbol (name)      /* создание атома-символа с указанным именем */
char *name;
{
	return (cons ((pairp) TSYMBOL, (pairp) strcopy (name)));
}

inline pairp number (val)       /* создание атома-числа */
long val;
{
	return (cons ((pairp) TNUMBER, (pairp) val));
}

inline long numval (p)          /* выдать значение числа */
pairp p;
{
	if (car (p) != TNUMBER)
		fatal ("number expected");
	return ((long) cdr (p));
}

inline char *symval (p)         /* выдать строку - имя символа */
pairp p;
{
	if (car (p) != TSYMBOL)
		fatal ("symbol expected");
	return ((char *) cdr (p));
}

inline int iscons (p)
pairp p;
{
	switch (car (p)) {
	case TNUMBER:
	case TSYMBOL:
		return (0);
	}
	return (1);
}

inline int isnumber (p)
pairp p;
{
	return (car (p) == TNUMBER);
}

inline int isatom (p)
pairp p;
{
	switch (car (p)) {
	case TNUMBER:
	case TSYMBOL:
		return (1);
	}
	return (0);
}

inline int issymbol (p)
pairp p;
{
	return (car (p) == TSYMBOL);
}

inline int isnil (p)
pairp p;
{
	return (issymbol (p) && ! strcmp (symval (p), "nil"));
}

inline int isadigit (c)
register c;
{
	return (c>='0' && c<='9');
}

int isaletter (c)
register c;
{
	switch (c) {
	case '_': case '$':
	case 'a': case 'b': case 'c': case 'd': case 'e':
	case 'f': case 'g': case 'h': case 'i': case 'j':
	case 'k': case 'l': case 'm': case 'n': case 'o':
	case 'p': case 'q': case 'r': case 's': case 't':
	case 'u': case 'v': case 'w': case 'x': case 'y':
	case 'z':
	case 'A': case 'B': case 'C': case 'D': case 'E':
	case 'F': case 'G': case 'H': case 'I': case 'J':
	case 'K': case 'L': case 'M': case 'N': case 'O':
	case 'P': case 'Q': case 'R': case 'S': case 'T':
	case 'U': case 'V': case 'W': case 'X': case 'Y':
	case 'Z':
	case 'а': case 'б': case 'в': case 'г': case 'д':
	case 'е': case 'ж': case 'з': case 'и': case 'й':
	case 'к': case 'л': case 'м': case 'н': case 'о':
	case 'п': case 'р': case 'с': case 'т': case 'у':
	case 'ф': case 'х': case 'ц': case 'ч': case 'ш':
	case 'щ': case 'ь': case 'ы': case 'ъ': case 'э':
	case 'ю': case 'я':
	case 'А': case 'Б': case 'В': case 'Г': case 'Д':
	case 'Е': case 'Ж': case 'З': case 'И': case 'Й':
	case 'К': case 'Л': case 'М': case 'Н': case 'О':
	case 'П': case 'Р': case 'С': case 'Т': case 'У':
	case 'Ф': case 'Х': case 'Ц': case 'Ч': case 'Ш':
	case 'Щ': case 'Ь': case 'Ы': case '┐': case 'Э':
	case 'Ю': case 'Я':
		return (1);
	}
	return (0);
}

void initmem ()			/* инициализация списка свободных */
{
	register i;

	firstfree = -1;
	for (i=0; i<MEMSZ; ++i) {
		setcdr (i) = firstfree;
		setcar (i) = 0;
		firstfree = i;
		gclabel [i] = 0;
	}
}

void glabelit (r)
register pairp r;
{
	/* if (p<0 || p>=MEMSZ) fatal ("bad glabelit"); */
#ifndef notdef
loop:
	if (gclabel [r])
		return;
	gclabel [r] = 1;
	if (isatom (r))
		return;
	glabelit (car (r));
	r = cdr (r);
	goto loop;		/* avoid recursion */
#else
	register pairp r1, r2;

	r1 = NIL;
forw:	if (! (gclabel [r] & GCMARK)) {
		gclabel [r] |= GCMARK;
		if (! isatom (r)) {
			r2 = r1;
			r1 = r;
			r = car (r1);
			gclabel [r] &= ~GCRIGHT;
			setcar (r1) = r2;
			goto forw;
		}
	}
backw:	if (r1 != NIL) {
		if (! (gclabel [r1] & GCRIGHT)) {
			r2 = car (r1);
			setcar (r1) = r;
			gclabel [r1] |= GCRIGHT;
			r = cdr (r1);
			setcdr (r1) = r2;
			goto forw;
		}
		r2 = cdr (r1);
		setcdr (r1) = r;
		gclabel [r1] &= ~GCRIGHT;
		r = r1;
		r1 = r2;
		goto backw;
	}
#endif
}

int gcollect ()
{
	register i, n;

	firstfree = -1;
	n = 0;
	for (i=0; i<MEMSZ; ++i)
		if (gclabel [i])
			gclabel [i] = 0;
		else {
			if (car (i) == TSYMBOL)
				free (symval ((pairp) i));
			setcdr (i) = firstfree;
			setcar (i) = 0;
			firstfree = i;
			++n;
		}
	return (n);
}

void gc ()			/* сбор мусора */
{
	register n;

	if (trace)
		fputs ("GC...", stderr);
	glabelit (S);
	glabelit (E);
	glabelit (C);
	glabelit (D);
	glabelit (W);
	glabelit (T);
	glabelit (F);
	glabelit (NIL);
	n = gcollect ();
	if (firstfree < 0)
		fatal ("Garbage collector out of memory");
	if (trace)
		fprintf (stderr, "%d OK ", n);
}

char *strcopy (p)
register char *p;
{
	register char *q;

	q = malloc ((unsigned) strlen (p) + 1);
	if (! q)
		fatal ("out of dynamic memory");
	strcpy (q, p);
	return (q);
}

char *long2str (v)
register long v;
{
	static char buf [32];
	register char *p;
	int sign;

	if (v < 0) {
		sign = 1;
		v = -v;
	} else
		sign = 0;
	p = buf + sizeof (buf) - 1;
	*p-- = 0;
	if (v == 0)
		*p-- = '0';
	else while (v) {
		*p-- = v % 10 + '0';
		v /= 10;
	}
	if (sign)
		*p-- = '-';
	return (p+1);
}

long str2long (p)
register char *p;
{
	register long v;
	register sign;

	if (*p == '-') {
		sign = 1;
		++p;
	} else
		sign = 0;
	v = 0;
	for (v=0; *p; ++p)
		v = v*10 + *p - '0';
	return (sign ? -v : v);
}

void ungetlex ()
{
	backlexflag = 1;
}

int getlex ()			/* ввод лексемы */
{
	/* побочный эффект - заполнение lexval и lexsym */
	register c, i;

	if (backlexflag) {
		backlexflag = 0;
		return (lexlex);
	}
loop:
	while ((c = getchar ())==' ' || c=='\t' || c=='\f' || c=='\n')
		continue;
	if (c < 0)
		return (0);
	if (c == ';') {
		while ((c = getchar ()) >= 0 && c!='\n')
			continue;
		goto loop;
	}
	if (isadigit (c) || c=='-') {
		i = 0;
		while (i < MAXLEX-1) {
			lexsym [i++] = c;
			c = getchar ();
			if (c < 0)
				break;
			if (! isadigit (c)) {
				ungetc (c, stdin);
				break;
			}
		}
		lexsym [i] = 0;
		lexval = str2long (lexsym);
		return (lexlex = TNUMBER);
	}
	if (isaletter (c) || c=='-') {
		i = 0;
		while (i < MAXLEX-1) {
			lexsym [i++] = c;
			c = getchar ();
			if (c < 0)
				break;
			if (! isaletter (c) && ! isadigit (c)) {
				ungetc (c, stdin);
				break;
			}
		}
		lexsym [i] = 0;
		return (lexlex = TSYMBOL);
	}
	return (lexlex = c);
}

pairp getlist ()		/* чтение списка ВЫР ('.' СПИС | ВЫР)... */
{
	register pairp p, oldw;

	/* использует регистр W для хранения временной пары */
	oldw = W;
	p = cons (NIL, W);
	W = p;
	setcar (p) = getexpr ();
	switch (getlex ()) {
	case 0:
		fatal ("unexpected eof");
		/* NOTREACHED */
	case '.':
		setcdr (p) = getexpr ();
		break;
	case ')':
		ungetlex ();
		setcdr (p) = NIL;
		break;
	default:
		ungetlex ();
		setcdr (p) = getlist ();
		break;
	}
	W = oldw;
	return (p);
}

pairp getexpr ()		/* чтение выражения АТОМ | ЧИСЛО | '(' СПИС ')' */
{
	pairp p;

	switch (getlex ()) {
	default:
		fatal ("syntax error");
	case ')':
		ungetlex ();
	case 0:
		return (NIL);
	case '(':
		p = getlist ();
		if (getlex () != ')')
			fatal ("right parence expected");
		break;
	case TNUMBER:
		p = number (lexval);
		if (trace > 2)
			fprintf (stderr, "%ld\n", lexval);
		break;
	case TSYMBOL:
		p = symbol (lexsym);
		if (trace > 2)
			fprintf (stderr, "%s\n", lexsym);
		break;
	}
	return (p);
}

void putatom (p, fd)
register pairp p;
register FILE *fd;
{
	if (isnumber (p))
		fputs (long2str (numval (p)), fd);
	else if (issymbol (p))
		fputs (symval (p), fd);
	else
		fputs ("<?>", fd);
}

void putlist (p, fd)
register pairp p;
register FILE *fd;
{
	putexpr (car (p), fd);
	p = cdr (p);
	while (iscons (p)) {
		putc (' ', fd);
		putexpr (car (p), fd);
		p = cdr (p);
	}
	if (! isnil (p)) {
		putc (' ', fd);
		putc ('.', fd);
		putc (' ', fd);
		putatom (p, fd);
	}
}

void putexpr (p, fd)
register pairp p;
register FILE *fd;
{
	if (iscons (p)) {
		putc ('(', fd);
		putlist (p, fd);
		putc (')', fd);
	} else
		putatom (p, fd);
}

void error (s)
char *s;
{
	fputs (s, stderr);
	fputc ('\n', stderr);
}

void fatal (s)
char *s;
{
	fputs (s, stderr);
	fputc ('\n', stderr);
	exit (-1);
}

# ifdef DEBUG
pairp debugassert (p, line)
register pairp p;
{
	if (p<0 || p>=MEMSZ) {
		fprintf (stderr, "\npointer %ld out of range in line %d\n",
			p, line);
		exit (-1);
	}
	return (p);
}
# endif

pairp execute (fn, arg)
pairp fn, arg;
{
	pairp cmd;
	register i;

	W = arg;
	S = cons (W, NIL);
	E = NIL;
	C = fn;
	D = NIL;
loop:
	W = NIL;
	cmd = car (C);
	if (! isnumber (cmd)) {
		error ("no secd command");
		fprintf (stderr, "C="); putexpr (C, stderr); putc ('\n', stderr);
		exit (-1);
	}
	if (trace > 1) {
		fprintf (stderr, "S="); putexpr (S, stderr); putc ('\n', stderr);
		/* E может оказаться рекурсивным, например, после РЕК */
		/* fprintf (stderr, "E="); putexpr (E, stderr); putc ('\n', stderr); */
		fprintf (stderr, "C="); putexpr (C, stderr); putc ('\n', stderr);
		fprintf (stderr, "D="); putexpr (D, stderr); putc ('\n', stderr);
	}
	if (trace)
		fprintf (stderr, "%s ", cmdname [numval (cmd)]);
	switch (numval (cmd)) {
	case C_LD:		/* ввод */
		/* s e (ld i.c) d --> (load{i,e}.s) e c d */
		W = E;
		i = numval (car (car (cdr (C))));
		while (--i>=0 && iscons (W))
			W = cdr (W);
		if (iscons (W)) {
			W = car (W);
			i = numval (cdr (car (cdr (C))));
			while (--i>=0 && iscons (W))
				W = cdr (W);
			if (iscons (W))
				W = car (W);
			else
				W = NIL;
		} else
			W = NIL;
		S = cons (W, S);
		C = cdr (cdr (C));
		break;
	case C_LDC:             /* ввод константы */
		/* s e (ldc x.c) d --> (x.s) e c d */
		S = cons (car (cdr (C)), S);
		C = cdr (cdr (C));
		break;
	case C_LDF:             /* ввод функции */
		/* s e (ldf c'.c) d --> ((c'.e).s) e c d */
		W = cons (car (cdr (C)), E);
		S = cons (W, S);
		C = cdr (cdr (C));
		break;
	case C_AP:              /* применение функции */
		/* ((c'.e') v.s) e (ap.c) d --> nil (v.e') c' (s e c.d) */
		W = cons (cdr (C), D);
		W = cons (E, W);
		D = cons (cdr (cdr (S)), W);
		E = cons (car (cdr (S)), cdr (car (S)));
		C = car (car (S));
		S = NIL;
		break;
	case C_RTN:             /* возврат */
		/* (a) e' (rtn) (s e c.d) --> (a.s) e c d */
		S = cons (car (S), car (D));
		E = car (cdr (D));
		C = car (cdr (cdr (D)));
		D = cdr (cdr (cdr (D)));
		break;
	case C_DUM:             /* создание формального окружения */
		/* s e (dum.c) d --> s (nil.e) c d */
		E = cons (NIL, E);
		C = cdr (C);
		break;
	case C_RAP:             /* рекурсивное применение */
		/* ((c'.e') v.s) (nil.e) (rap.c) d -->
		 * nil завершение(e'.v) c' (s e c.d) */
# ifdef notdef
		putc ('\n', stderr);
		fprintf (stderr, "S="); putexpr (S, stderr); putc ('\n', stderr);
		fprintf (stderr, "c'="); putexpr (car (car (S)), stderr); putc ('\n', stderr);
		fprintf (stderr, "e'="); putexpr (cdr (car (S)), stderr); putc ('\n', stderr);
		fprintf (stderr, "v="); putexpr (car (cdr (S)), stderr); putc ('\n', stderr);
# endif
		W = cons (cdr (C), D);
		W = cons (cdr (E), W);
		D = cons (cdr (cdr (S)), W);
		E = cdr (car (S));
		if (car (E) != NIL)
			fatal ("bad rap");
		setcar (E) = car (cdr (S));
		C = car (car (S));
		S = NIL;
# ifdef notdef
		fprintf (stderr, "S="); putexpr (S, stderr); putc ('\n', stderr);
		fprintf (stderr, "C="); putexpr (C, stderr); putc ('\n', stderr);
		fprintf (stderr, "D="); putexpr (D, stderr); putc ('\n', stderr);
# endif
		break;
	case C_SEL:             /* выбор подсписка управления */
		/* (x.s) e (sel ct cf.c) d --> s e cx (c.d) */
		D = cons (cdr (cdr (cdr (C))), D);
		if (! strcmp (symval (car (S)), "t"))
			C = car (cdr (C));
		else
			C = car (cdr (cdr (C)));
		S = cdr (S);
		break;
	case C_JOIN:            /* восстановление главного управления */
		/* s e (join) (c.d) --> s e c d */
		C = car (D);
		D = cdr (D);
		break;
	case C_CAR:             /* car от вершины стека */
		/* ((a.b).s) e (car.c) d --> (a.s) e c d */
		S = cons (car (car (S)), cdr (S));
		C = cdr (C);
		break;
	case C_CDR:             /* cdr от вершины стека */
		/* ((a.b).s) e (car.c) d --> (b.s) e c d */
		S = cons (cdr (car (S)), cdr (S));
		C = cdr (C);
		break;
	case C_ATOM:            /* предикат atom от вершины стека */
		S = cons (isatom (car (S)) ? T : F, cdr (S));
		C = cdr (C);
		break;
	case C_CONS:            /* формирование cons двух эл-тов стека */
		/* (a b.s) e (cons.c) d --> ((a.b).s) e c d */
		W = cons (car (S), car (cdr (S)));
		S = cons (W, cdr (cdr (S)));
		C = cdr (C);
		break;
	case C_EQ:              /* предикат eq от двух эл-тов стека */
		if (issymbol (car (S)) && issymbol (car (cdr (S))))
			W = !strcmp (symval (car (cdr (S))), symval (car (S))) ?
				T : F;
		else if (isnumber (car (S)) && isnumber (car (cdr (S))))
			W = numval (car (cdr (S))) == numval (car (S)) ?
				T : F;
		else
			W = F;
		S = cons (W, cdr (cdr (S)));
		C = cdr (C);
		break;
	case C_ADD:             /* сложение двух эл-тов стека */
		/* (a b.s) e (add.c) d --> (b+a.s) e c d */
		W = number (numval (car (cdr (S))) + numval (car (S)));
		S = cons (W, cdr (cdr (S)));
		C = cdr (C);
		break;
	case C_SUB:             /* вычитание двух эл-тов стека */
		/* (a b.s) e (sub.c) d --> (b-a.s) e c d */
		W = number (numval (car (cdr (S))) - numval (car (S)));
		S = cons (W, cdr (cdr (S)));
		C = cdr (C);
		break;
	case C_MUL:             /* умножение двух эл-тов стека */
		/* (a b.s) e (mul.c) d --> (b*a.s) e c d */
		W = number (numval (car (cdr (S))) * numval (car (S)));
		S = cons (W, cdr (cdr (S)));
		C = cdr (C);
		break;
	case C_DIV:             /* деление двух эл-тов стека */
		/* (a b.s) e (div.c) d --> (b/a.s) e c d */
		W = number (numval (car (cdr (S))) / numval (car (S)));
		S = cons (W, cdr (cdr (S)));
		C = cdr (C);
		break;
	case C_REM:             /* остаток от деления двух эл-тов стека */
		/* (a b.s) e (rem.c) d --> (b%a.s) e c d */
		W = number (numval (car (cdr (S))) % numval (car (S)));
		S = cons (W, cdr (cdr (S)));
		C = cdr (C);
		break;
	case C_LEQ:             /* сравнение на <= двух эл-тов стека */
		W = numval (car (cdr (S))) <= numval (car (S)) ? T : F;
		S = cons (W, cdr (cdr (S)));
		C = cdr (C);
		break;
	case C_STOP:            /* останов */
		goto endloop;
	default:
		fatal ("bad secd command");
		/* NOTREACHED */
	}
	goto loop;
endloop:
	if (trace) {
		fprintf (stderr, "S="); putexpr (S, stderr); putc ('\n', stderr);
		/* E может оказаться рекурсивным, например, после РЕК */
		/* fprintf (stderr, "E="); putexpr (E, stderr); putc ('\n', stderr); */
		fprintf (stderr, "C="); putexpr (C, stderr); putc ('\n', stderr);
		fprintf (stderr, "D="); putexpr (D, stderr); putc ('\n', stderr);
	}
	return (car (S));
}

main (argc, argv)
char **argv;
{
	pairp fn, arg, rez;
	int oldstdin;
	char *fnfile = 0;
	char *argfile = 0;

	fprintf (stderr, "Landin engine, Release 1.0\n");
	for (--argc, ++argv; **argv=='-'; --argc, ++argv) {
		register char *p;

		for (p=1+*argv; *p; ++p) switch (*p) {
		case 'h':
usage:			fprintf (stderr, "Usage: langine [-h] [-v] [-t] [prog [arg]]\n");
			return (0);
		case 't':
			++trace;
			break;
		case 'v':
			++verbose;
			break;
		}
	}
	if (argc > 0) {
		fnfile = *argv;
		++argv;
		--argc;
	}
	if (argc > 0) {
		argfile = *argv;
		++argv;
		--argc;
	}
	if (argc > 0)
		goto usage;
	mem = (pair *) malloc (sizeof (pair) * MEMSZ); /* память списков */
	gclabel = malloc (MEMSZ);
	if (!mem || !gclabel) {
		fprintf (stderr, "Out of memory\n");
		return (-1);
	}
	initmem ();
	NIL = symbol ("nil");
	T = symbol ("t");
	F = symbol ("f");
	W = NIL;
	S = NIL;
	E = NIL;
	C = NIL;
	D = NIL;
	if (fnfile) {
		oldstdin = dup (fileno (stdin));
		if (! freopen (fnfile, "r", stdin)) {
			fprintf (stderr, "Cannot read %s\n", fnfile);
			return (-1);
		}
	}
	fn = getexpr ();
	if (fnfile) {
		fclose (stdin);
		if (dup (oldstdin) != 0) {
			fprintf (stderr, "Cannot dup stdin %d\n", oldstdin);
			return (-1);
		}
		close (oldstdin);
		if (! fdopen (0, "r")) {
			fprintf (stderr, "Cannot reopen stdin\n");
			return (-1);
		}
	}
	if (verbose) {
		putexpr (fn, stdout);
		putchar ('\n');
	}
	if (argfile) {
		if (! freopen (argfile, "r", stdin)) {
			fprintf (stderr, "Cannot read %s\n", argfile);
			return (-1);
		}
	}
	for (;;) {
		arg = getexpr ();
		if (feof (stdin))
			break;
		if (verbose) {
			putexpr (arg, stdout);
			printf (" --> ");
		}
		rez = execute (fn, arg);
		putexpr (rez, stdout);
		putchar ('\n');
	}
	return (0);
}
