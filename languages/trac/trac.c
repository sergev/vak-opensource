/*
 * Trac language interpreter.
 * Copyright (C) 1987-2006 Serge Vakulenko, <vak@cronyx.ru>
 *
 * Usage: trac [file...]
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>

#define QUANT	512		/* квант выделения памяти для string_t */
#define ARGSZ	2048		/* ограничение на количество аргументов */
#define FTNSZ	1024		/* ограничение на вложенность вызовов */
#define TABSZ	1024		/* ограничение на количество бланков */
#define BUFSZ	1024		/* ограничение на количество меток */
#define BLSZ	077777		/* ограничение на длину бланка */
#define FNAMSZ	100		/* ограничение на длину имени файла
				   в командах read, write */
typedef struct {
	char *line;		/* тело цепочки */
	int ind;		/* следующий свободный символ в line */
	int len;		/* длина line */
} string_t;

typedef struct {
	int place;		/* указатель на символ, следующий за меткой */
	int num;		/* порядковый номер метки */
} label_t;

typedef struct {
	char *name;		/* имя бланка */
	char *body;		/* тело бланка */
	label_t *lab;		/* массив меток */
	int ptr;		/* указатель */
	int nlab;		/* количество меток */
} form_t;

struct ftn {			/* элемент стека функций */
	char **argv;		/* указатель на массив аргументов */
	int act;		/* 0 - пассивный вызов, 1 - активный */
};

struct table {			/* элемент таблицы встроенных функций */
	char *name;		/* название */
	void (*fptr) ();	/* указатель на функцию-исполнитель */
};

string_t active_string, passive_string, tmp_string;
string_t *active = &active_string;	/* активная цепочка */
string_t *passive = &passive_string;	/* пассивная цепочка */
string_t *tmp = &tmp_string;		/* результат выполнения функции */

char metachar = '\'';		/* метасимвол */
jmp_buf jmpbuf;			/* возврат на основной цикл */
int eofl;

form_t tab [TABSZ];		/* таблица бланков */
int ntab;			/* количество бланков */

char *arg [ARGSZ];		/* указатели на строки аргументов */
struct ftn ftn [FTNSZ];		/* заголовки вызовов функций */
int narg, nftn;			/* соответствующие индексы */

int debug;			/* флаг отладки */
int trace;			/* признак трассировки */

form_t *create (), *lookloc ();

void f_exit (), f_halt (), f_input (), f_output (), f_stop ();
void f_trace (), f_notrace (), f_meta (), f_inpchar ();
void f_define (), f_remove (), f_clear (), f_list (), f_print ();
void f_rewind (), f_call (), f_getline (), f_getchar (), f_read (), f_write ();
void f_delete (), f_scan (), f_eq (), f_length (), f_range (), f_find ();
void f_char (), f_code (), f_gt (), f_get (), f_not (), f_shift ();
void f_rshift (), f_and (), f_or (), f_add (), f_sub (), f_mul (), f_div ();
void f_setin (), f_setout (), f_basis ();

struct table table [] = {	/* таблица встроенных функций */
	{ "add",                  f_add		},
	{ "and",                  f_and		},
	{ "basis",                f_basis	},
	{ "call",                 f_call	},
	{ "char",                 f_char	},
	{ "clear",                f_clear	},
	{ "code",                 f_code	},
	{ "define",               f_define	},
	{ "delete",               f_delete	},
	{ "div",                  f_div		},
	{ "eq",                   f_eq		},
	{ "exit",                 f_exit	},
	{ "find",                 f_find	},
	{ "get",                  f_get		},
	{ "getchar",              f_getchar	},
	{ "getline",              f_getline	},
	{ "gt",                   f_gt		},
	{ "halt",                 f_halt	},
	{ "input",                f_input	},
	{ "inputchar",            f_inpchar	},
	{ "length",               f_length	},
	{ "list",                 f_list	},
	{ "meta",                 f_meta	},
	{ "mul",                  f_mul		},
	{ "not",                  f_not		},
	{ "notrace",              f_notrace	},
	{ "or",                   f_or		},
	{ "output",               f_output	},
	{ "print",                f_print	},
	{ "range",                f_range	},
	{ "read",                 f_read	},
	{ "remove",               f_remove	},
	{ "rewind",               f_rewind	},
	{ "rshift",               f_rshift	},
	{ "scan",                 f_scan	},
	{ "setin",                f_setin	},
	{ "setout",               f_setout	},
	{ "shift",                f_shift	},
	{ "stop",                 f_stop	},
	{ "sub",                  f_sub		},
	{ "trace",                f_trace	},
	{ "write",                f_write	},
};

/*
 * Выдача диагностики о нефатальных ошибках.
 */
void error (char *fmt, ...)
{
	va_list ap;

	fprintf (stderr, "trac: ");
	va_start (ap, fmt);
	vfprintf (stderr, fmt, ap);
	va_end (ap);
	fprintf (stderr, "\n");
}

/*
 * Выдача диагностики о фатальной ошибке.
 */
void cerror (char *fmt, ...)
{
	va_list ap;

	fprintf (stderr, "trac: fatal error: ");
	va_start (ap, fmt);
	vfprintf (stderr, fmt, ap);
	va_end (ap);
	fprintf (stderr, "\n");
	exit (2);
}

/*
 * Опустошить цепочку c.
 */
void clear (string_t *c)
{
	if (c->len != QUANT) {
		if (c->len == 0)
			c->line = malloc (QUANT);
		else
			c->line = realloc (c->line, QUANT);

		if (! c->line)
			cerror ("out of memory in clear");

		c->len = QUANT;
	}
	c->line [0] = '\0';
	c->ind = 0;
}

/*
 * Считать символ из цепочки c.
 */
int agetc (string_t *c)
{
	if (c->ind <= 0)
		return (-1);
	-- c->ind;
	return (c->line [c->ind] & 0377);
}

/*
 * Дописать символ s в цепочку c.
 */
void aputc (char s, string_t *c)
{
	int i;
	char *oldplace;

	if (c->ind >= c->len) {
		if (c->ind > c->len)
			cerror ("bad string index");
		c->len += QUANT;
		oldplace = c->line;
		c->line = realloc (c->line, c->len);
		if (! c->line)
			cerror ("out of memory in aputc");
		if (c->line != oldplace && c == passive && narg) {
			for (i = 0; i < narg; i++)
				arg [i] += c->line - oldplace;
		}
	}
	c->line [c->ind] = s;
	c->ind ++;
}

/*
 * Дописать строку s в цепочку c.
 */
void aputs (char *s, string_t *c)
{
	for (; *s; s++)
		aputc (*s, c);
}

/*
 * Дописать строку s в цепочку c в обратном порядке.
 */
void bputs (char *s, string_t *c)
{
	char *p;

	p = s;
	while (*p)
		p++;
	while (s <= --p)
		aputc (*p, c);
}

int getint (FILE *file)
{
	int i, c;

	i = 0;
	while ((c = getc (file)) != '\n') {
		if (c == EOF || c < '0' || c > '9')
			return (-1);
		i = i * 10 + c - '0';
	}
	return (i);
}

/*
 * Отметить текущую позицию в passive как начало аргумента.
 */
void stoarg ()
{
	if (narg >= ARGSZ)
		cerror ("argument table overflow");
	arg [narg++] = & passive->line [passive->ind];
}

/*
 * Отметить текущую позицию в argv как начало функции.
 */
void stoftn (int act)
{
	if (nftn >= FTNSZ)
		cerror ("function table overflow");
	ftn [nftn].argv = & arg [narg];
	ftn [nftn++].act = act;
	stoarg ();
}

/*
 * Поиск в таблице имени s и возврат указателя на функцию.
 * Метод деления пополам.
 */
void (*fswitch (char *s)) ()
{
	int i, l, r, c;

	l = 0;
	r = sizeof (table) / sizeof (struct table) - 1;
	do {
		i = (l + r) / 2;
		c = strcmp (s, table [i].name);
		if (c > 0)
			l = i+1;
		else if (c < 0)
			r = i-1;
		else
			return (table [i].fptr);
	} while (l <= r);

	return (0);
}

/*
 * Исполнитель функции #( call, ... )
 */
void callform (form_t *b, int npar, char **param)
{
	char *cp;
	label_t *l;

	if (! npar || ! b->nlab) {
		aputs (b->body, tmp);
		return;
	}
	l = b->lab;
	for (cp = b->body; *cp; cp++) {
		while (l && cp == b->body + l->place) {
			if (l->num < npar)
				aputs (param [l->num], tmp);
			if (++l - b->lab >= b->nlab)
				l = 0;
		}
		aputc (*cp, tmp);
	}
	while (l && cp == b->body + l->place) {
		if (l->num < npar)
			aputs (param [l->num], tmp);
		if (++l - b->lab >= b->nlab)
			l = 0;
	}
}

void execute ()
{
	int argc, act;
	char **argv;
	void (*f) ();

	if (debug)
		fprintf (stderr, "execute ()\n");
	nftn--;
	argv = ftn [nftn].argv;
	act = ftn [nftn].act;
	argc = arg - argv;
	argc += narg;
	if (argc > 256)
		cerror ("too many arguments in function %s", *argv);
	narg = argv - arg;
	passive->ind = *argv - passive->line;

	if (trace) {
		int i;

		if (! act)
			putc ('#', stderr);
		fputs ("#(", stderr);
		for (i = 0; i < argc; i++) {
			if (i)
				putc (',', stderr);
			fputs (argv [i], stderr);
		}
		fputs (")\n", stderr);
	}

	f = fswitch (argv [0]);
	clear (tmp);
	if (f) {
		if (debug)
			fprintf (stderr, "function %s call\n", argv [0]);
		(*f) (argc, argv);
		if (debug)
			fprintf (stderr, "function %s exit\n", argv [0]);
	} else if (argc) {
		form_t *b;

		b = lookloc (argv [0]);
		if (! b) {
			if (debug) fprintf (stderr,
				"form %s not found\n", argv [0]);
		} else {
			if (debug) fprintf (stderr,
				"form %s call\n", argv [0]);
			callform (b, argc - 1, argv + 1);
		}
	}
	if (tmp->ind) {
		aputc (0, tmp);
		if (act)
			bputs (tmp->line, active);
		else
			aputs (tmp->line, passive);
	}
	clear (tmp);
}

/*
 * Основной модуль интерпретатора.
 * Ввод из stdin, вывод в stdout, диагностика - в stderr.
 */
void trac ()
{
	int c, level;

	eofl = 0;
	if (setjmp (jmpbuf) == 1)
		return;				/* так работает #(stop) */
loop:                                           /* иначе это #(exit) */
	if (debug)
		fprintf (stderr, "***** clearing *****\n");
	clear (active);
	clear (passive);
	bputs ("#(output,#(input,(#(stop))))", active);
	narg = 0;
	nftn = 0;
	for (;;) {
		c = agetc (active);
		switch (c) {
		case -1:
			goto loop;

		case '\t':
		case '\n':
		case '\r':
		case ' ':
		case '\0':
			continue;

		case '(':
			level = 0;
			while (level >= 0) {
				c = agetc (active);
				switch (c) {
				case -1:
					goto loop;
				case '\0':
					continue;
				case '(':
					level++;
					aputc ('(', passive);
					continue;
				case ')':
					if (level--)
						aputc (')', passive);
					continue;
				default:
					aputc (c, passive);
				}
			}
			continue;

		case ',':
			aputc ('\0', passive);
			stoarg ();
			continue;

		case '#':
			c = agetc (active);
			switch (c) {

			case -1:
				goto loop;

			case '(':
				stoftn (1);
				continue;

			case '#':
				c = agetc (active);
				switch (c) {
				case -1:
					goto loop;
				case '(':
					stoftn (0);
					continue;
				default:
					aputc ('#', passive);
					aputc ('#', passive);
					aputc (c, active);
					continue;
				}

			default:
				aputc ('#', passive);
				aputc (c, active);
				continue;
			}

		case ')':
			if (! nftn)
				goto loop;
			aputc ('\0', passive);
			execute ();
			continue;

		default:
			aputc (c, passive);
			continue;
		}
	}
}

void cexit ()
{
	exit (1);
}

int main (int argc, char **argv)
{
	int i, k, stduse;

	if (signal (SIGINT, SIG_IGN) != SIG_IGN)
		signal (SIGINT, (void (*)()) f_exit);
	if (signal (SIGTERM, SIG_IGN) != SIG_IGN)
		signal (SIGTERM, cexit);
#ifdef SIGQUIT
	if (signal (SIGQUIT, SIG_IGN) != SIG_IGN)
		signal (SIGQUIT, cexit);
#endif
	stduse = 1;
	for (i = 1; i < argc; i++) if (argv [i][0] == '-') {
		for (k = 1; argv [i][k]; k++) {
			switch (argv [i][k]) {
			case 'd':       /* флаг отладки */
				debug++;
				break;
			default:
				cerror ("bad flag: %c", argv [i][k]);
			}
		}
	} else {
		stduse = 0;
		if (freopen (argv [i], "r", stdin) == NULL)
			error ("cannot open %s", argv [i]);
		else
			trac ();
	}
	if (stduse)
		trac ();
	exit (0);
}

/*
 *  #( stop ) - перейти к следующему файлу.
 */
void f_stop ()
{
	longjmp (jmpbuf, 1);
}

/*
 * #( exit ) - перейти к следующей программе.
 */
void f_exit ()
{
	if (signal (SIGINT, SIG_IGN) != SIG_IGN)
		signal (SIGINT, (void (*)()) f_exit);
	longjmp (jmpbuf, 2);
}

/*
 * #( halt ) - выход из интерпретатора.
 */
void f_halt ()
{
	exit (0);
}

void f_setin (int argc, char **argv)
{
	static FILE finp [22];
	static int feofinp [22];
	static int ninp = 0;
	FILE *f;

	if (argc < 2 || argv [1] [0] == '\0') {
		if (ninp) {
			fclose (stdin);
			*stdin = finp [--ninp];
			eofl = feofinp [ninp];
		} else {
err:                    if (argc >= 3)
				aputs (argv [2], active);
		}
	} else {
		if (ninp >= 22)
			goto err;
		f = fopen (argv [1], "r");
		if (f == NULL)
			goto err;
		feofinp [ninp] = eofl;
		finp [ninp++] = *stdin;
		*stdin = *f;
		eofl = 0;
	}
}

void f_setout (int argc, char **argv)
{
	static FILE fout [22];
	static int nout = 0;
	FILE *f;

	if (argc < 2 || argv [1] [0] == '\0') {
		if (nout) {
			fclose (stdout);
			*stdout = fout [--nout];
		} else
err:                    if (argc >= 3) aputs (argv [2], active);
	} else {
		if (nout >= 22) goto err;
		f = fopen (argv [1], "w");
		if (f == NULL) goto err;
		fout [nout++] = *stdout;
		*stdout = *f;
	}
}

/*
 * #( input, Z ) - ввод строки до металитеры.
 *                 Если встретился конец файла,
 *                 то возвращается Z в активном режиме.
 */
void f_input (int argc, char **argv)
{
	int c;

	if (eofl) {
		if (argc > 1) bputs (argv [1], active);
		return;
	}
	while ((c = getchar ()) != EOF) {
		if (c == metachar) return;
		aputc (c, tmp);
	}
	eofl = 1;
}

/*
 * #( inputchar, Z ) - ввод символа.
 *		       Если встретился конец файла,
 *		       то возвращается Z в активном режиме.
 */
void f_inpchar (int argc, char **argv)
{
	int c;

	if (! eofl && (c = getchar ()) != EOF) {
		aputc (c, tmp);
		return;
	}
	if (argc > 1) bputs (argv [1], active);
}

/*
 * #( output, x ) - вывод всех аргументов в файл вывода.
 */
void f_output (int argc, char **argv)
{
	if (argc >= 2) fputs (argv [1], stdout);
}

/*
 * #( meta, x ) - установить металитеру.
 */
void f_meta (int argc, char **argv)
{
	if (argc == 1) aputc (metachar, tmp);
	else if (*argv [1] != '\0') metachar = *argv [1];
	else metachar = '\'';
}

/*
 * #( define, name, body ) - определение бланка.
 */
void f_define (int argc, char **argv)
{
	form_t *b;
	char *line;

	if (argc < 2 || *argv [1] == '\0') return;
	b = create (argv [1], 0);
	if (argc == 2) line = "\0"; else line = argv [2];
	b->body = malloc (strlen (line) + 1);
	if (! b->body) cerror ("out of memory in define (%s)", argv [1]);
	strcpy (b->body, line);
	b->ptr = 0;
	b->lab = 0;
	b->nlab = 0;
}

/*
 * #( rewind, name ) - установка указателя в начало.
 */
void f_rewind (int argc, char **argv)
{
	int i;
	form_t *b;

	for (i = 1; i < argc; i++) {
		if (argv [i] [0] == '\0') continue;
		if (! (b = lookloc (argv [i]))) continue;
		b->ptr = 0;
	}
}

/*
 * #( write, file, name ) - запись бланков во внешний файл.
 */
void f_write (int argc, char **argv)
{
	int i, k;
	form_t *b;
	char fname [FNAMSZ+10];                 /* здесь держим имя файла */
	FILE *file;

	if (argc < 2 || *argv [1] == '\0')
		return;
	strncpy (fname, argv [1], FNAMSZ);
	fname [FNAMSZ] = '\0';
	strcat (fname, ".t");
	file = fopen (fname, "w");
	if (file == NULL) {
		error ("cannot create %s", fname);
		return;
	}
	for (i = 2; i < argc; i++) {
		if (argv [i] [0] == '\0')
			continue;
		if (! (b = lookloc (argv [i])))
			continue;
		fprintf (file, "%ld\n", (long) strlen (b->name));
		fprintf (file, "%s\n", b->name);
		fprintf (file, "%ld\n", (long) strlen (b->body));
		fprintf (file, "%s\n", b->body);
		fprintf (file, "%d\n", b->ptr);
		fprintf (file, "%d\n", b->nlab);
		for (k = 0; k < b->nlab; k++) {
			fprintf (file, "%d\n", b->lab [k].place);
			fprintf (file, "%d\n", b->lab [k].num);
		}
	}
	fprintf (file, "0\n");
	fclose (file);
}

/*
 * #( read, file ) - считывание бланков из внешних файлов.
 */
void f_read (int argc, char **argv)
{
	form_t *b;
	char fname [FNAMSZ+10];                 /* здесь держим имя файла */
	char *buf;
	FILE *file;
	int i, n, k, c;

	for (i = 1; i < argc; i++) {
		if (*argv [i] == '\0') continue;
		strncpy (fname, argv [i], FNAMSZ);
		fname [FNAMSZ] = '\0';
		strcat (fname, ".t");
		file = fopen (fname, "r");
		if (file == NULL) {
			error ("cannot open %s", fname);
			continue;
		}
		for (;;) {
			b = 0;
			if ((k = getint (file)) == -1)
				goto err;
			if (! k)
				break;
			buf = malloc (k+1);
			if (! buf) cerror ("out of memory in read");
			for (n = 0; n < k; n++) {
				if ((c = getc (file)) == EOF) goto err;
				buf [n] = c;
			}
			buf [k] = '\0';
			if (getc (file) != '\n') goto err;
			b = create (buf, 1);
			b->body = 0;
			b->nlab = 0;

			if ((k = getint (file)) == -1)
				goto err;
			b->body = malloc (k+1);
			if (! b->body) cerror ("out of memory in read");
			for (n = 0; n < k; n++) {
				if ((c = getc (file)) == EOF)
					goto err;
				b->body [n] = c;
			}
			b->body [k] = '\0';
			if (getc (file) != '\n') goto err;

			if ((b->ptr = getint (file)) == -1)
				goto err;
			if ((k = getint (file)) == -1)
				goto err;

			b->nlab = k;
			if (b->nlab != 0) {
				b->lab = (label_t*) malloc (k * sizeof(label_t));
				if (! b->lab)
					cerror ("out of memory in read");
			} else
				b->lab = 0;

			for (n = 0; n < b->nlab; n++) {
				if ((k = getint (file)) == -1) goto err;
				b->lab [n].place = k;
				if ((k = getint (file)) == -1) goto err;
				b->lab [n].num = k;
			}
			continue;

err:                    error ("bad format");
			if (b) {
				free (b->name);
				if (b->body) free (b->body);
				if (b->nlab) free ((char*) b->lab);
				*b = tab [--ntab];
			}
			break;
		}
		fclose (file);
	}
}

/*
 * #( delete, file ) - уничтожение внешних файлов.
 */
void f_delete (int argc, char **argv)
{
	int i;
	char fname [20];

	for (i = 1; i < argc; i++) {
		if (*argv [i] == '\0') continue;
		strncpy (fname, argv [i], 8);
		fname [8] = '\0';
		strcat (fname, ".t");
		unlink (fname);
	}
}

/*
 * #( remove, name ) - уничтожение бланка.
 */
void f_remove (int argc, char **argv)
{
	int i;
	form_t *b;

	for (i = 1; i < argc; i++) {
		if (argv [i] [0] == '\0') continue;
		if (! (b = lookloc (argv [i]))) continue;
		free (b->name);
		free (b->body);
		if (b->nlab) free ((char*) b->lab);
		*b = tab [--ntab];
	}
}

/*
 * #( eq, a, b, T, F ) - сравнение строк.
 */
void f_eq (int argc, char **argv)
{
	if (argc <= 3) return;
	if (! strcmp (argv [1], argv [2])) aputs (argv [3], tmp);
	else if (argc >= 5) aputs (argv [4], tmp);
}

/*
 * #( gt, a, b, T, F ) - сравнение строк.
 */
void f_gt (int argc, char **argv)
{
	char *a, *b, *fa, *fb;
	int nega, negb, gt;

	if (argc < 4) return;
	fa = argv [1];
	fb = argv [2];
	while (*fa) fa++;
	while (*fb) fb++;
	a = fa;
	b = fb;
	while (--a >= argv [1] && *a >= '0' && *a <= '9');
	while (--b >= argv [2] && *b >= '0' && *b <= '9');
	nega = a >= argv [1] && *a == '-';
	negb = b >= argv [2] && *b == '-';
	if (nega != negb) gt = negb;
	else {
		while (++a < fa && *a == '0');
		while (++b < fb && *b == '0');
		if (fa - a != fb - b) gt = nega ^ (fa - a > fb - b);
		else for (gt = 0; *a && *b; a++, b++) if (*a != *b) {
			gt = nega ^ (*a > *b);
			break;
		}
	}
	if (gt) aputs (argv [3], tmp);
	else if (argc >= 5) aputs (argv [4], tmp);
}

/*
 * #( char, x ) - выдача символа по коду.
 */
void f_char (int argc, char **argv)
{
	int c;
	char *p;

	if (argc < 2 || ! *argv [1])
		return;

	p = argv [1];
	do {
		p++;
	} while (*p);

	do {
		p--;
	} while (p >= argv [1] && *p >= '0' && *p <= '9');

	if (p >= argv [1] && *p == '-')
		return;
	c = 0;
	while (*++p)
		if ((c = c * 10 + *p - '0') > 0377)
			return;
	aputc (c, tmp);
}

/*
 * #( code, x ) - выдача кода символа.
 */
void f_code (int argc, char **argv)
{
	char buf [20];

	if (argc < 2 || ! *argv [1]) return;
	else {
		sprintf (buf, "%d", *argv [1]);
		aputs (buf, tmp);
	}
}

/*
 * #( shift, n, value ) - логический сдвиг.
 */
void f_shift (int argc, char **argv)
{
	char *p, *e;
	int c, n, sign;

	if (argc < 3)
		return;

	p = argv [1];
	do {
		p++;
	} while (*p);

	do {
		p--;
	} while (p >= argv [1] && *p >= '0' && *p <= '9');

	sign = p >= argv [1] && *p == '-';
	c = 0;
	while (*++p)
		if ((c = c * 10 + *p - '0') > BLSZ)
		return;
	p = argv [2];
	while (*p)
		p++;
	e = p;
	while (--p >= argv [2] && *p >= '0' && *p <= '1')
		continue;
	p++;
	n = e - p;
	if (n <= c) {
		while (n--)
			aputc ('0', tmp);
		return;
	}
	if (sign) {                     /* отрицательно - вправо */
		n -= c;
		while (c--)
			aputc ('0', tmp);
		while (n--)
			aputc (*p++, tmp);
	} else {                        /* положительно - влево */
		p += c;
		while (*p)
			aputc (*p++, tmp);
		while (c--)
			aputc ('0', tmp);
	}
}

/*
 * #( rshift, n, value ) - циклический логический сдвиг.
 */
void f_rshift (int argc, char **argv)
{
	char *p, *e;
	int c, n, sign;

	if (argc < 3)
		return;

	p = argv [1];
	do {
		p++;
	} while (*p);

	do {
		p--;
	} while (p >= argv [1] && *p >= '0' && *p <= '9');

	sign = p >= argv [1] && *p == '-';
	c = 0;
	while (*++p)
		if ((c = c * 10 + *p - '0') > BLSZ)
		return;
	p = argv [2];
	while (*p)
		p++;
	e = p;
	while (--p >= argv [2] && *p >= '0' && *p <= '1')
		continue;
	p++;
	if ( ! (n = e - p))
		return;
	c %= n;
	if (sign)
		c = n - c;
	e -= n - c;
	while (*e)
		aputc (*e++, tmp);
	while (c--)
		aputc (*p++, tmp);
}

void add (char *l, char *le, char *r, char *re)
{
	string_t bufstring;
	string_t *buf = &bufstring;
	int s, c;

	buf->len = 0;
	clear (buf);
	c = 0;
	for (;;) {
		if (--le < l) {
			for (re--; re >= r; re--)
				if (c) {
					c = (*re == '9');
					if (c)
						aputc ('0', buf);
					else
						aputc (*re + 1, buf);
				} else
					aputc (*re, buf);
			break;
		}
		if (--re < r) {
			for (; le >= l; le--)
				if (c) {
					c = (*le == '9');
					if (c)
						aputc ('0', buf);
					else
						aputc (*le + 1, buf);
				} else
					aputc (*le, buf);
			break;
		}
		s = *re + *le - '0' + c;
		c = (s > '9');
		if (c)
			s -= 10;
		aputc (s, buf);
	}
	if (c)
		aputc ('1', buf);
	aputc (0, buf);
	bputs (buf->line, tmp);
	free (buf->line);
}

void sub (char *l, char *le, char *r, char *re)
{
	string_t bufstring;
	string_t *buf = &bufstring;
	int s, c;

	buf->len = 0;
	clear (buf);
	c = 0;
	for (;;) {
		if (--le < l)
			break;
		if (--re < r) {
			for (; le >= l; le--)
				if (c) {
					c = (*le == '0');
					if (c)
						aputc ('9', buf);
					else
						aputc (*le - 1, buf);
				} else
					aputc (*le, buf);
			break;
		}
		s = *le - *re + '0' - c;
		c = (s < '0');
		if (c)
			s += 10;
		aputc (s, buf);
	}
	while (buf->line [buf->ind-1] == '0' && buf->ind > 1)
		buf->ind--;
	aputc (0, buf);
	bputs (buf->line, tmp);
	free (buf->line);
}

/*
 * #( add, a, b ) - арифметическое сложение.
 */
void f_add (int argc, char **argv)
{
	char *lp, *rp, *s, *le, *re;
	int l = 0, r = 0, lsign, rsign, llong, rlong;

	if (argc < 2) {
		aputc ('0', tmp);
		return;
	}
	for (lp = argv [1]; *lp; lp++)
		;
	le = lp;
	do
		lp--;
	while (lp >= argv [1] && *lp >= '0' && *lp <= '9');

	lsign = lp >= argv [1] && *lp == '-';
	for (s = argv [1]; s < lp; s++)
		aputc (*s, tmp);
	if (lp >= argv[1] && *lp != '-')
		aputc (*lp, tmp);
	lp++;
	while (*lp && *lp == '0')
		lp++;
	if (argc < 3 || argv [2] [0] == '\0') {
		if (lsign && lp != le)
			aputc ('-', tmp);
		if (*lp)
			aputs (lp, tmp);
		else
			aputc ('0', tmp);
		return;
	}
	if (le - lp < 5) {
		for (l=0, s=lp; *s; s++)
			l = l * 10 + *s - '0';
		if (lsign)
			l = -l;
		llong = 0;
	} else
		llong = 1;

	for (rp = argv [2]; *rp; rp++)
		;
	re = rp;
	do
		rp--;
	while (rp >= argv [2] && *rp >= '0' && *rp <= '9');

	rsign = rp >= argv [2] && *rp == '-';
	rp++;
	while (*rp && *rp == '0')
		rp++;
	if (re - rp < 5) {
		for (r=0, s=rp; *s; s++)
			r = r * 10 + *s - '0';
		if (rsign)
			r = -r;
		rlong = 0;
	} else
		rlong = 1;

	if (! llong && ! rlong) {
		char buf [10];

		sprintf (buf, "%d", l + r);
		aputs (buf, tmp);
		return;
	}
	if (lsign == rsign) {
		if (lsign)
			aputc ('-', tmp);
		add (lp, le, rp, re);
	} else {
		if (le - lp != re - rp) {
			if (le - lp > re - rp) {
leftgt:                         if (lsign) aputc ('-',tmp);
				sub (lp, le, rp, re);
			} else {
rightgt:                        if (rsign) aputc ('-',tmp);
				sub (rp, re, lp, le);
			}
		} else {
			char *ll, *rr;

			for (ll=lp, rr=rp; *ll && *rr; ll++, rr++)
				if (*ll != *rr) {
					if (*ll > *rr)
						goto leftgt;
					else
						goto rightgt;
				}
			aputc ('0', tmp);
			return;
		}
	}
}

/*
 * #( sub, a, b ) - арифметическое вычитание.
 */
void f_sub (int argc, char **argv)
{
	char *lp, *rp, *s, *le, *re;
	int l = 0, r = 0, lsign, rsign, llong, rlong;

	if (argc < 2) {
		aputc ('0', tmp);
		return;
	}
	for (lp = argv [1]; *lp; lp++)
		;
	le = lp;
	do
		lp--;
	while (lp >= argv [1] && *lp >= '0' && *lp <= '9');

	lsign = lp >= argv [1] && *lp == '-';
	for (s = argv [1]; s < lp; s++)
		aputc (*s, tmp);
	if (lp >= argv[1] && *lp != '-')
		aputc (*lp, tmp);
	lp++;
	while (*lp && *lp == '0')
		lp++;
	if (argc < 3 || argv [2] [0] == '\0') {
		if (lsign && lp != le)
			aputc ('-', tmp);
		if (*lp)
			aputs (lp, tmp);
		else
			aputc ('0', tmp);
		return;
	}
	if (le - lp < 5) {
		for (l=0, s=lp; *s; s++)
			l = l * 10 + *s - '0';
		if (lsign)
			l = -l;
		llong = 0;
	} else
		llong = 1;

	for (rp = argv [2]; *rp; rp++)
		;
	re = rp;
	do
		rp--;
	while (rp >= argv [2] && *rp >= '0' && *rp <= '9');

	rsign = ! (rp >= argv [2] && *rp == '-');
	rp++;
	while (*rp && *rp == '0')
		rp++;
	if (re - rp < 5) {
		for (r=0, s=rp; *s; s++)
			r = r * 10 + *s - '0';
		if (rsign)
			r = -r;
		rlong = 0;
	} else
		rlong = 1;

	if (! llong && ! rlong) {
		char buf [10];

		sprintf (buf, "%d", l + r);
		aputs (buf, tmp);
		return;
	}
	if (lsign == rsign) {
		if (lsign)
			aputc ('-', tmp);
		add (lp, le, rp, re);
	} else {
		if (le - lp != re - rp) {
			if (le - lp > re - rp) {
leftgt:                         if (lsign)
					aputc ('-',tmp);
				sub (lp, le, rp, re);
			} else {
rightgt:                        if (rsign)
					aputc ('-',tmp);
				sub (rp, re, lp, le);
			}
		} else {
			char *ll, *rr;

			for (ll=lp, rr=rp; *ll && *rr; ll++, rr++)
				if (*ll != *rr) {
					if (*ll > *rr)
						goto leftgt;
					else
						goto rightgt;
				}
			aputc ('0', tmp);
			return;
		}
	}
}

void mul (char *l, char *le, char *r, char *re)
{
	int nl, nr, n, i, k, carry;
	int *left, *right, *res;
	char *p;

	nl = (le - l + 1) / 2;
	nr = (re - r + 1) / 2;
	n = (le - l + re - r + 1) / 2;
	left = (int*) malloc (nl * sizeof (int));
	right = (int*) malloc (nr * sizeof (int));
	res = (int*) malloc (n * sizeof (int));
	for (p = le-1, i = 0; p >= l; p--, i = 1-i) {
		if (i)
			left [(le-1 - p)/2] += (*p - '0') * 10;
		else
			left [(le-1 - p)/2] = *p - '0';
	}
	for (p = re-1, i = 0; p >= r; p--, i = 1-i) {
		if (i)
			right [(re-1 - p)/2] += (*p - '0') * 10;
		else
			right [(re-1 - p)/2] = *p - '0';
	}
	for (i = 0; i < n; i++)
		res [i] = 0;
	for (i = 0; i < nl; i++) {
		carry = 0;
		for (k = 0; k < nr; k++) {
			res [i+k] += left [i] * right [k] + carry;
			carry = res [i+k] / 100;
			res [i+k] %= 100;
		}
		if (carry)
			res [i+nr] = carry;
	}
	if (res [n-1]) {
		k = res [n-1] / 10;
		if (k)
			aputc ('0' + k, tmp);
		aputc ('0' + res [n-1] % 10, tmp);
	}
	for (i = n-2; i >= 0; i--) {
		aputc ('0' + res [i] / 10, tmp);
		aputc ('0' + res [i] % 10, tmp);
	}
	free ((char*) left);
	free ((char*) right);
	free ((char*) res);
}

/*
 * #( mul, a, b ) - арифметическое умножение.
 */
void f_mul (int argc, char **argv)
{
	char *lp, *rp, *s, *le, *re;
	int l = 0, r = 0, lsign, rsign, llong, rlong;

	if (argc < 2 || argv [1] [0] == '\0') {
		aputc ('0', tmp);
		return;
	}
	for (lp = argv [1]; *lp; lp++)
		;
	le = lp;
	do
		lp--;
	while (lp >= argv [1] && *lp >= '0' && *lp <= '9');

	lsign = lp >= argv [1] && *lp == '-';
	for (s = argv [1]; s < lp; s++)
		aputc (*s, tmp);
	if (lp >= argv[1] && *lp != '-')
		aputc (*lp, tmp);
	lp++;
	if (argc < 3 || argv [2] [0] == '\0') {
		aputc ('0', tmp);
		return;
	}
	while (*lp && *lp == '0')
		lp++;
	if (le - lp < 5) {
		for (l=0, s=lp; *s; s++)
			l = l * 10 + *s - '0';
		if (lsign)
			l = -l;
		llong = 0;
	} else
		llong = 1;

	for (rp = argv [2]; *rp; rp++)
		;
	re = rp;
	do
		rp--;
	while (rp >= argv [2] && *rp >= '0' && *rp <= '9');

	rsign = rp >= argv [2] && *rp == '-';
	rp++;
	while (*rp && *rp == '0')
		rp++;
	if (re - rp < 5) {
		for (r=0, s=rp; *s; s++)
			r = r * 10 + *s - '0';
		if (rsign)
			r = -r;
		rlong = 0;
	} else
		rlong = 1;

	if (! llong && ! rlong) {
		char buf [10];

		sprintf (buf, "%ld", (long) l * r);
		aputs (buf, tmp);
		return;
	}
	if ((! llong && ! l) || (! rlong && ! r)) {
		aputc ('0', tmp);
		return;
	}
	if (lsign != rsign)
		aputc ('-', tmp);
	mul (lp, le, rp, re);
}

void divide (char *l, char *le, char *r, char *re)
{
	int *left, *right, *res;
	int i, k, nl, nr, n;
	char *p;

	/* деление ведем по основанию 100 */

	if (debug > 2)
		fprintf (stderr, "div (%s [%d], %s [%d])\n",
			l, le-l, r, re-r);

	nl = (le - l + 1) / 2;
	nr = (re - r + 1) / 2;
	n = nl - nr + 1;

	if (debug > 2)
		fprintf (stderr, "\tnl=%d, nr=%d, n=%d\n",
			nl, nr, n);

	left = (int*) malloc (nl * sizeof (int));
	if (! left)
		cerror ("out of memory in div");
	right = (int*) malloc (nr * sizeof (int));
	if (! right)
		cerror ("out of memory in div");
	res = (int*) malloc (n * sizeof (int));
	if (! res)
		cerror ("out of memory in div");

	if (debug > 2)
		fprintf (stderr, "\tleft=%p, right=%p, res=%p\n",
			left, right, res);

	for (p = le-1, i = 0; p >= l; p--, i = 1-i) {
		if (i)
			left [(le-1 - p)/2] += (*p - '0') * 10;
		else
			left [(le-1 - p)/2] = *p - '0';
	}
	for (p = re-1, i = 0; p >= r; p--, i = 1-i) {
		if (i)
			right [(re-1 - p)/2] += (*p - '0') * 10;
		else
			right [(re-1 - p)/2] = *p - '0';
	}
	for (i = 0; i < n; i++)
		res [i] = 0;

	if (debug > 2)
		fprintf (stderr, "\tleft, right, res cleaned\n");

	if (nr == 1) {
		int v;

		/* делитель одноцифровой (по основанию 100) */
		v = 0;
		for (i = nl-1; i >= 0 ; i--) {
			v = v * 100 + left [i];
			res [i] = v / right [0];
			if (! res [i])
				continue;
			v -= res [i] * right [0];
		}
	} else {
		long v;
		int dv, shift;

		v = left [nl-1];
		/* кратность оцениваем по двум цифрам делителя */
		dv = right [nr-1] * 100 + right [nr-2] + 1;
		for (shift=nl-nr; shift>=0; shift--) {
			/* оценим кратность с учетом сдвига */
			v = v * 100 + left [nr-2+shift];
			k = v/dv;
			/* есть очередная цифра результата */
			res [shift] = k;
			/* вычтем это из делимого */
			if (k) {
				int cur;

				for (i=0; i<nr; i++) {
					cur = left [shift+i] - right [i] * k;
					/* так сложно из-за особенностей
					 * деления отрицательных чисел */
					cur += 10000;
					left [shift+i] = cur % 100;
					cur /= 100;
					cur -= 100;
					/* теперь в cur находится перенос */
					if (cur) {
						if (shift+i+1 >= nl)
							cerror ("bad div");
						left [shift+i+1] += cur;
					}
				}
			}
			/* если делимое больше делителя, вычтем еще раз */
			k = 1;
			for (i = nr-1; i>=0; i--)
				if (left [shift+i] != right [i]) {
					k = left [shift+i] > right [i];
					break;
				}
			if (k) {
				int cur;

				for (i=0; i<nr; i++) {
					cur = left [shift+i] - right [i];
					/* так сложно из-за особенностей
					деления отрицательных чисел */
					cur += 10000;
					left [shift+i] = cur % 100;
					cur /= 100;
					cur -= 100;
					/* теперь в cur находится перенос */
					if (cur) {
						if (shift+i+1 >= nl)
							cerror ("bad div");
						left [shift+i+1] += cur;
					}
				}
				/* и скорректируем результат */
				res [shift] ++;
			}
			v = left [nr-1+shift] * 100 + left [nr-2+shift];
		}
	}

	if (debug > 2)
		fprintf (stderr, "\tres calculated\n");

	for (i = n-1; i>0 && !res[i]; i--)
		;
	k = res [i] / 10;
	if (k)
		aputc ('0' + k, tmp);
	aputc ('0' + res [i] % 10, tmp);
	for (i--; i>=0; i--) {
		aputc ('0' + res [i] / 10, tmp);
		aputc ('0' + res [i] % 10, tmp);
	}

	if (debug > 2)
		fprintf (stderr, "\tres printed\n");

	free ((char*) left);
	free ((char*) right);
	free ((char*) res);

	if (debug > 2)
		fprintf (stderr, "\tleft, right, res are free\n");
}

/*
 * #( div, a, b ) - арифметическое деление.
 */
void f_div (int argc, char **argv)
{
	char *lp, *rp, *s, *le, *re;
	int l = 0, r = 0, lsign, rsign, llong, rlong;

	if (argc < 3)
		return;
	for (lp = argv [1]; *lp; lp++)
		;
	le = lp;
	do
		lp--;
	while (lp >= argv [1] && *lp >= '0' && *lp <= '9');

	lsign = lp >= argv [1] && *lp == '-';
	for (s = argv [1]; s < lp; s++)
		aputc (*s, tmp);
	if (lp >= argv[1] && *lp != '-')
		aputc (*lp, tmp);
	lp++;
	while (*lp && *lp == '0')
		lp++;
	if (le - lp < 5) {
		for (l=0, s=lp; *s; s++)
			l = l * 10 + *s - '0';
		if (lsign)
			l = -l;
		llong = 0;
	} else
		llong = 1;

	for (rp = argv [2]; *rp; rp++)
		;
	re = rp;
	do
		rp--;
	while (rp >= argv [2] && *rp >= '0' && *rp <= '9');

	rsign = rp >= argv [2] && *rp == '-';
	rp++;
	while (*rp && *rp == '0')
		rp++;
	if (re - rp < 5) {
		for (r=0, s=rp; *s; s++)
			r = r * 10 + *s - '0';
		if (rsign)
			r = -r;
		rlong = 0;
	} else
		rlong = 1;

	if (! rlong && ! r) {
		if (argc >= 4 && argv [3] [0])
			bputs (argv [3], active);
		return;
	}
	if (! llong && ! rlong) {
		char buf [10];

		sprintf (buf, "%ld", (long) l / r);
		aputs (buf, tmp);
		return;
	}
	if ((! llong && ! l) || le-lp < re-rp) {
		aputc ('0', tmp);
		return;
	}
	if (lsign != rsign)
		aputc ('-', tmp);
	divide (lp, le, rp, re);
}

int isdigit (char c, int b)
{
	if (b <= 10)
		return (c >= '0' && c < '0'+b);
	if (c >= '0' && c <= '9')
		return (1);
	if (c >= 'a')
		c -= 040;
	return (c >= 'A' && c < 'A'+b-10);
}

int tointeger (char c)
{
	if (c >= '0' && c <= '9')
		c -= '0';
	else if (c >= 'A' && c <= 'Z')
		c -= 'A' - 10;
	else if (c >= 'a' && c <= 'z')
		c -= 'a' - 10;
	else
		c = 0;
	return (c);
}

int todigit (char c)
{
	if (c <= 9)
		return (c + '0');
	return (c + 'a' - 10);
}

void basis (char *l, int nl, int f, int t)
{
	int i;
	string_t bufstring, *buf = &bufstring;

	buf->len = 0;
	clear (buf);

	if (debug > 2)
		fprintf (stderr, "basis: %s [ %d ], %d->%d\n",
			l, nl, f, t);
	for (i = 0; i < nl; i++)
		l [i] = tointeger (l [i]);

	for (;;) {
		int v, idle;

		v = 0;
		idle = 1;
		for (i = 0; i < nl; i++) {
			if (! l [i] && !v)
				continue;
			v = v * f + l [i];
			l [i] = v / t;
			v -= l [i] * t;
			idle = 0;
		}
		if (idle)
			break;
		aputc (todigit (v), buf);
	}
	aputc ('\0', buf);
	bputs (buf->line, tmp);
	free (buf->line);
}

/*
 * #( basis, b1, b2, value ) - изменение основания.
 */
void f_basis (int argc, char **argv)
{
	char *p, *e;
	int f, t, sign;

	if (argc < 3)
		return;
	f = argv [1][0];
	t = argv [2][0];
	if (! f || ! t || argv [1][1] || argv [2][1])
		return;
	f = tointeger (f) + 1;
	t = tointeger (t) + 1;
	if (f < 2 || t < 2 || f > 36 || t > 36)
		return;

	if (argc < 4 || ! argv [3][0]) {
		aputc ('0', tmp);
		return;
	}

	for (p = argv [3]; *p; p++)
		;
	e = p;
	do
		p--;
	while (p >= argv [3] && isdigit (*p, f));

	sign = p >= argv [3] && *p == '-';
	p++;
	while (*p && *p == '0')
		p++;

	if (! *p) {
		aputc ('0', tmp);
		return;
	}
	if (sign)
		aputc ('-', tmp);
	basis (p, e-p, f, t);
}

/*
 * #( or, a, b ) - логическое "или".
 */
void f_or (int argc, char **argv)
{
	char *l, *r;
	int nl, nr;

	if (argc < 3) return;
	else {
		l = argv [1];
		nl = 0;
		while (*l) l++;
		while (--l >= argv [1] && *l >= '0' && *l <= '1') nl++;
		r = argv [2];
		nr = 0;
		while (*r) r++;
		while (--r >= argv [2] && *r >= '0' && *r <= '1') nr++;
		if (nl > nr) while (nl-- > nr) aputc (*++l, tmp);
		else while (nr-- > nl) aputc (*++r, tmp);
		while (*++l && *++r) aputc (*l | *r, tmp);
	}
}

/*
 * #( and, a, b ) - логическое "и".
 */
void f_and (int argc, char **argv)
{
	char *l, *r;
	int nl, nr;

	if (argc < 3) return;
	else {
		l = argv [1];
		nl = 0;
		while (*l) l++;
		while (--l >= argv [1] && *l >= '0' && *l <= '1') nl++;
		r = argv [2];
		nr = 0;
		while (*r) r++;
		while (--r >= argv [2] && *r >= '0' && *r <= '1') nr++;
		if (nl > nr) l += nl - nr;
		else r += nr - nl;
		while (*++l && *++r) aputc (*l & *r, tmp);
	}
}

/*
 * #( not, x ) - логическое дополнение.
 */
void f_not (int argc, char **argv)
{
	char *p;

	if (argc < 2) return;
	else {
		p = argv [1];
		while (*p) p++;
		while (--p >= argv [1] && *p >= '0' && *p <= '1');
		while (*++p) aputc (*p ^ 01, tmp);
	}
}

/*
 * #( length, x ) - выдача длины цепочки.
 */
void f_length (int argc, char **argv)
{
	char buf [20];

	if (argc < 2) aputc ('0', tmp);
	else {
		sprintf (buf, "%ld", (long) strlen (argv [1]));
		aputs (buf, tmp);
	}
}

/*
 * #( range, x ) - выдача максимального номера метки бланка x.
 */
void f_range (int argc, char **argv)
{
	int i;
	label_t *l;
	form_t *b;
	char buf [20];

	if (argc < 2) return;
	b = lookloc (argv [1]);
	if (! b || ! b->nlab) i = 0;
	else {
		i = 0;
		for (l = b->lab; l < & b->lab [b->nlab]; l++)
			if (l->num >= i) i = l->num + 1;
	}
	sprintf (buf, "%d", i);
	aputs (buf, tmp);
}

/*
 * #( list, x ) - выдача списка всех бланков через разделитель x.
 */
void f_list (int argc, char **argv)
{
	char *x;
	form_t *b;

	if (argc < 2) x = ",";
	else x = argv [1];
	for (b = tab; b < & tab [ntab]; b++) {
		if (b != tab) aputs (x, tmp);
		aputs (b->name, tmp);
	}
}

/*
 * #( print, x ) - печать бланков.
 */
void f_print (int argc, char **argv)
{
	form_t *b;
	char *cp;
	label_t *l;
	int i;

	for (i = 1; i < argc; i++) {
		if (! (b = lookloc (argv [i]))) continue;
		if (b->nlab) l = b->lab;
		else l = 0;
		for (cp = b->body; *cp; cp++) {
			while (l && cp == b->body + l->place) {
				fprintf (stderr, "<%d>", l->num + 1);
				if (++l - b->lab >= b->nlab) l = 0;
			}
			if (cp == b->body + b->ptr) fprintf (stderr, "<*>");
			putc (*cp, stderr);
		}
		while (l && cp == b->body + l->place) {
			fprintf (stderr, "<%d>", l->num + 1);
			if (++l - b->lab >= b->nlab) l = 0;
		}
		if (cp == b->body + b->ptr) fprintf (stderr, "<*>");
	}
}

/*
 * #( call, name, arg ) - вызов бланка.
 */
void f_call (int argc, char **argv)
{
	form_t *b;

	if (argc < 2 || *argv [1] == '\0') return;
	if (! (b = lookloc (argv [1]))) return;
	callform (b, argc - 2, argv + 2);
}

/*
 * #( getline, name, Z ) - выдача строки бланка до метки.
 */
void f_getline (int argc, char **argv)
{
	form_t *b;
	label_t *l;

	if (argc < 2 || *argv [1] == '\0') return;
	if (! (b = lookloc (argv [1]))) return;
	if (! b->body [b->ptr]) {
		if (argc >= 3) bputs (argv [2], active);
		return;
	}
	if (b->nlab)
		for (l = b->lab; l < & b->lab [b->nlab]; l++)
			if (l->place > b->ptr) {
				while (b->ptr < l->place)
					aputc (b->body [b->ptr++], tmp);
				return;
			}
	while (b->body [b->ptr]) aputc (b->body [b->ptr++], tmp);
}

/*
 * #( get, name, n, Z ) - выдача строки бланка длиной n литер.
 */
void f_get (int argc, char **argv)
{
	form_t *b;
	char *cp;
	int neg;
	long n;

	if (argc < 3 || ! *argv [1] || ! *argv [2]) return;
	if (! (b = lookloc (argv [1]))) return;
	cp = argv [2];
	do cp++; while (*cp);
	do cp--; while (cp >= argv [2] && *cp >= '0' && *cp <= '9');
	neg = cp >= argv [2] && *cp == '-';
	n = 0;
	while (*++cp)
		if ((n = n * 10 + *cp - '0') > BLSZ)
			goto err;
	cp = b->body + b->ptr;
	if (neg) {
		cp -= n;
		if (cp < b->body) goto err;
		b->ptr = cp - b->body;
		while (n--) aputc (*cp++, tmp);
	} else {
		while (n--) {
			if (! *cp) {
err:                            clear (tmp);
				if (argc >= 4) bputs (argv [3], active);
				return;
			}
			aputc (*cp++, tmp);
		}
		b->ptr = cp - b->body;
	}
}

/*
 * #( getchar, name, Z ) - выдача символа из бланка.
 */
void f_getchar (int argc, char **argv)
{
	form_t *b;

	if (argc < 2 || *argv [1] == '\0') return;
	if (! (b = lookloc (argv [1]))) return;
	if (b->body [b->ptr]) aputc (b->body [b->ptr++], tmp);
	else if (argc >= 3) bputs (argv [2], active);
}

/*
 * #( scan, name, arg ) - сегментация бланка.
 */
void f_scan (int argc, char **argv)
{
	form_t *b;
	char *cp;
	label_t *l, *lw = 0;
	static label_t buf [BUFSZ];
	int i, k, ind, eq, nbuf;

	if (argc < 3 || *argv [1] == '\0')
		return;
	if (! (b = lookloc (argv [1])))
		return;
	l = b->lab;
	nbuf = 0;
	for (cp = b->body; *cp; cp++)
	{                                       /* цикл по телу бланка */
		ind = -1;
		k = 0;
		for (i = 2; i < argc; i++)
		{                               /* цикл по аргументам */
			eq = 1;
			lw = l;
			for (k = 0; argv [i][k]; k++)
			{
						/* цикл по символам */
				if (lw)
					while (b->body + lw->place < cp + k)
						lw++;
				if (argv [i][k] != cp [k] || (lw &&
					b->body + lw->place == cp + k))
				{
					eq = 0;
					break;
				}
			}
			if (! k || ! eq)
				continue;
			ind = i - 2;
			break;
		}
		if (ind < 0) {
			aputc (*cp, tmp);
			continue;
		}
		if (lw)
			while (l < lw) {
				buf [nbuf++] = *l++;
				if (nbuf >= BUFSZ)
					cerror ("label table overflow");
			}
		buf [nbuf].place = tmp->ind;
		buf [nbuf++].num = ind;
		if (nbuf >= BUFSZ)
			cerror ("label table overflow");
		cp += k-1;
	}
	if (nbuf > b->nlab) {
		aputc ('\0', tmp);
		b->body = realloc (tmp->line, strlen (tmp->line) + 1);
		if (! b->body)
			cerror ("out of memory in scan");
		b->nlab = nbuf;
		if (b->lab)
			free ((char*) b->lab);
		b->lab = (label_t*) malloc (sizeof (label_t) * nbuf);
		if (! b->lab)
			cerror ("out of memory in scan");
		for (i = 0; i < nbuf; i++)
			b->lab [i] = buf [i];
	}
	b->ptr = 0;
	tmp->len = 0;
	clear (tmp);
}

/*
 * #( find, name, line, Z ) - выдача бланка name до строки,
 *                            совпадающей с line.
 */
void f_find (int argc, char **argv)
{
	form_t *b;
	char *cp;
	label_t *l, *lw;
	int k = 0, eq;

	if (argc <= 2 || *argv [1] == '\0' || *argv [2] == '\0')
		return;
	if (! (b = lookloc (argv [1])))
		return;
	l = b->lab;
	for (cp = b->body + b->ptr; *cp; cp++) {
		eq = 1;
		lw = l;
		for (k = 0; argv [2][k]; k++) {
			while (b->body + lw->place < cp + k)
				lw++;
			if (argv [2][k] != cp [k] ||
				b->body + lw->place == cp + k)
			{
				eq = 0;
				break;
			}
		}
		if (eq)
			break;
		aputc (*cp, tmp);
	}
	b->ptr = cp - b->body;
	if (*cp)
		b->ptr += k;
}

/*
 * #( clear ) - уничтожение всех бланков.
 */
void f_clear ()
{
	form_t *b;

	for (b = tab; b < & tab [ntab]; b++) {
		free (b->name);
		free (b->body);
		if (b->nlab)
			free ((char*) b->lab);
	}
	ntab = 0;
}

/*
 * #( trace ) - включение трассировки.
 */
void f_trace ()
{
	trace = 1;
}

/*
 * #( notrace ) - выключение трассировки.
 */
void f_notrace ()
{
	trace = 0;
}

form_t *lookloc (char *name)
{
	int i;

	for (i = 0; i < ntab; i++)
		if (! strcmp (name, tab [i].name))
			return (& tab [i]);
	return (0);
}

/*
 * Создать новый бланк с именем name. Если такой уже есть, уничтожить
 * его тело. Если usename, то при заведении нового бланка можно не
 * копировать имя, а использовать name.
 */
form_t *create (char *name, int usename)
{
	form_t *b;
	int i;

	b = 0;
	for (i = 0; i < ntab; i++) if (! strcmp (name, tab [i].name)) {
		b = & tab [i];
		break;
	}
	if (! b) {
		if (ntab >= TABSZ) cerror ("form table overflow");
		b = & tab [ntab];
		ntab++;
		if (usename) b->name = name;
		else {
			b->name = malloc (strlen (name) + 1);
			if (! b->name)
				cerror ("out of memory in create (%s)", name);
			strcpy (b->name, name);
		}
	} else {
		free (b->body);
		if (usename) free (name);
	}
	return (b);
}
