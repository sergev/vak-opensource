/*
 * Моделирование полинома с _одной_ обратной связью.
 */
#include <stdio.h>

#define N 16
/* #define N 5 */
/* #define N 7 */
#define P ((1<<N)-1)

/*
 * Моделирование циклического сдвигового регистра длины `N'
 * с обратной связью. Прозводит сдвиг на один такт
 * и возвращает значение выдвинутого бита.
 * Параметры:
 * `mem' - хранит состояние регистра
 * `tap' - определяет номер бита обратной связи, от 1 до N-1
 * `in' - значение бита для вдвигания
 *
 *              <-----N----->
 *
 *             ---------------
 * return <-,--|  << mem <<  |---< in
 *          |  --------------- ^
 *          |     tap ^        |
 *          |         |        |
 *          \_________/________/
 */
int poly (int *mem, int tap, int in)
{
	int rez = *mem >> (N-1) & 1;
	*mem = *mem<<1 ^ rez << tap ^ rez ^ in;
	return rez;
}

/*
 * Печать битовой последовательности `s' длины `len'
 * (по одному биту на байт) в двоичном виде.
 * .
 */
void seqprint (unsigned char *s, int len)
{
	int i;

	for (i=0; i<len; ++s, ++i)
		putchar (*s ? '1' : '0');
	putchar ('\n');
}

/*
 * Печать битовой последовательности `s' длины `len'
 * (по одному биту на байт) в шестнадцатеричном виде.
 */
void hexprint (unsigned char *s, int len)
{
	int n, i;
	unsigned char byte;

	for (n=0; n<len; ++n) {
		byte = 0;
		for (i=0; i<8; ++i)
			byte |= s [(n*8+i)%len] << i;
		printf ("0x%02x,", byte);
	}
	putchar ('\n');
}

/*
 * Генерация и печать битовой последовательности длины `len',
 * используя сдвиговый регистр длины `N' с обратной связью `tap'.
 */
void makeseq (int tap, int len)
{
	unsigned char seq [2*P];
	int mem, i;

	mem = 0;
	for (i=0; i<P; ++i)
		poly (&mem, tap, 1);
	for (i=0; i<2*P; ++i)
		seq [i] = poly (&mem, tap, 1);

	printf ("\nx^%d + x^%d + 1 =\n", N, tap);
	seqprint (seq, len);
/*	hexprint (seq, len);*/

	for (i=0; i<2*P; ++i)
		seq [i] ^= 1;
	seqprint (seq, len);
}

int main ()
{
	int i;

	for (i=1; i<N; ++i)
		makeseq (i, 2*P);
	return 0;
}
