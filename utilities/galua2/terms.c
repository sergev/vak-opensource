/*
 * Разложение полиномов на множители.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned long long u64_t;	/* Полином в виде двоичного числа */

/* Структура данных для хранения полинома. */
typedef struct {
	unsigned char power;	/* Степень полинома */
	u64_t poly;		/* Полином в виде двоичного числа */
} poly_t;

poly_t *list;			/* Список неприводимых полиномов */
int count;			/* Количество полиномов */
int size;			/* Размер массива */

/*
 * Печать полинома.
 */
void poly_print (poly_t x, char *tail)
{
	unsigned char i;

	i = x.power;
	do
		printf ("%d", (char) (x.poly >> i) & 1);
	while (i-- > 0);
	printf (" ");

/*	printf ("%d:", x.power);*/
	for (i=x.power; i>1; --i)
		if (x.poly & 1<<i)
			printf ("x%d+", i);
	if (x.poly & 2)
		printf ("x+");
	printf ("%d%s", (char) x.poly & 1, tail);
}

/*
 * Добавление полинома в список неприводимых.
 */
void poly_append (poly_t x)
{
	if (count >= size) {
		size += 100;
		if (count)
			list = realloc (list, size * sizeof (*list));
		else
			list = malloc (size * sizeof (*list));
	}
	list [count] = x;
	++count;
}

/*
 * Вычисление остатка от деления полиномов.
 */
static inline int poly_remainder (poly_t x, poly_t y, poly_t *z)
{
	while (x.power >= y.power) {
		if (x.poly >> x.power)
			x.poly ^= y.poly << (x.power - y.power);
		--x.power;
	}
	if (x.poly == 0) {
		if (z) {
			z->power = 0;
			z->poly = 0;
		}
		return 0;
	} else {
		if (z) {
			while (! (x.poly >> x.power))
				--x.power;
			z->power = x.power;
			z->poly = x.poly;
		}
		return 1;
	}
}

/*
 * Деление полиномов.
 */
static inline void poly_divide (poly_t x, poly_t y, poly_t *t)
{
	unsigned char shift;
	poly_t z;

/*printf ("Divide "); poly_print (x, " by "); poly_print (y, " = ");*/
	z.power = 0;
	z.poly = 0;
	while (x.power >= y.power) {
		if (x.poly >> x.power) {
			shift = x.power - y.power;
			x.poly ^= y.poly << shift;
			z.poly |= 1 << shift;
			if (shift > z.power)
				z.power = shift;
		}
		--x.power;
	}
/*poly_print (*z, "\n");*/
	if (t)
		*t = z;
}

/*
 * Выделение множителя полинома.
 */
int poly_factorize (poly_t x, poly_t *a, poly_t *b)
{
	int i;
	poly_t *e;

	/* В прямом порядке - быстрее. */
	for (i=0; i<count; ++i) {
		e = &list[i];
		if (e->power >= x.power)
			break;
		if (! poly_remainder (x, *e, 0)) {
			poly_divide (x, *e, b);
			if (a)
				*a = *e;
			return 1;
		}
	}
	return 0;
}

/*
 * Проверка неприводимости полинома.
 */
int poly_simple (poly_t x)
{
	int i;
	poly_t *e;

	/* В прямом порядке - быстрее. */
	for (i=0; i<count; ++i) {
		e = &list[i];
		if (e->power >= x.power)
			break;
		if (! poly_remainder (x, *e, 0))
			return 0;
	}
	return 1;
}

/*
 * Генерация полиномов до указанной степени (не включая).
 * Каждый полином проверяем по очереди на делимость
 * всеми предыдущими неприводимыми.
 * Дополняем список неприводимых.
 */
void poly_generate (unsigned char power, unsigned char verbose)
{
	poly_t x;

	if (count == 0) {
		poly_append ((poly_t) {1, 3});	/* x + 1 */
		if (verbose)
			poly_print (list[0], "\n");
	}
	for (x.power=1+list[count-1].power; x.power<power; ++x.power) {
		for (x.poly = 1 | 1<<x.power; x.poly < 2<<x.power; x.poly += 2)
			if (! poly_factorize (x, 0, 0)) {
				if (verbose)
					poly_print (x, "\n");
				poly_append (x);
			}
	}
}

/*
 * Генерация таблицы для вычисления контрольной суммы.
 */
void poly_table (poly_t x)
{
	poly_t a;
	int i;

	for (i=0; i<256; ++i) {
		a.power = x.power + 8;
		a.poly = i << x.power;
		poly_remainder (a, x, &a);
/*		printf ("%d - %ld - ", i, (unsigned long) a.poly);*/
/*		poly_print (a, "\n");*/
		printf ("%3ld,", (unsigned long) a.poly);
		if (i % 16 == 15)
			printf ("\n");
	}
}

int main (int argc, char **argv)
{
	poly_t x, a;
	unsigned char i;

	if (argc < 2) {
usage:		fprintf (stderr, "Usage:\n");
		fprintf (stderr, "\tterms --generate <power>\n");
		fprintf (stderr, "\tterms --factorize <power>...\n");
		fprintf (stderr, "\tterms --table <power>...\n");
		fprintf (stderr, "Example:\n\tterms --factorize 16 12 5 0\n");
		fprintf (stderr, "Output:\n\tx16 + x12 + x5 + 1 = ...\n");
		exit (0);
	}
	--argc;
	++argv;

	if (strcmp ("--generate", argv[0]) == 0) {
		if (argc < 2)
			goto usage;
		poly_generate (atoi (argv[1]), 1);

	} else if (strcmp ("--factorize", argv[0]) == 0) {
		if (argc < 2)
			goto usage;
		x.power = atoi (argv[1]);
		x.poly = 0;
		while (--argc > 0)
			x.poly |= 1 << atoi (*++argv);

		if (x.power > 13) {
			printf ("Generating polynoms of power:");
			for (i=2; i<=x.power; ++i) {
				printf (" %d", i-1);
				fflush (stdout);
				poly_generate (i, 0);
			}
			printf (" - done.\n");
		} else
			poly_generate (x.power, 0);

		poly_print (x, " = (");
		while (poly_factorize (x, &a, &x)) {
			poly_print (a, ") * (");
		}
		poly_print (x, ")\n");

	} else if (strcmp ("--table", argv[0]) == 0) {
		if (argc < 2)
			goto usage;
		x.power = atoi (argv[1]);
		if (x.power != 8 && x.power != 16 &&
		    x.power != 24 && x.power != 32) {
			fprintf (stderr, "Incorrect polynome power %d, must be N*8\n",
				x.power);
			exit (1);
		}
		x.poly = 0;
		while (--argc > 0)
			x.poly |= 1 << atoi (*++argv);

		printf ("/* Table for ");
		poly_print (x, " */\n");
		poly_table (x);

	} else
		goto usage;
	return 0;
}
