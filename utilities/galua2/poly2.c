/*
 * Моделирование полинома с _двумя_ обратными связями.
 */
#include <stdio.h>

#define N 8
/* #define N 5 */
/* #define N 7 */
#define P ((1<<N)-1)

unsigned char seq [2*P+N];

/*
 * Моделирование циклического сдвигового регистра длины `N'
 * с двумя обратными связями. Прозводит сдвиг на один такт
 * и возвращает значение выдвинутого бита.
 * Параметры:
 * `mem' - хранит состояние регистра
 * `tap1', `tap2' - определяют номера битов обратных связей, от 1 до N-1
 * `in' - значение бита для вдвигания
 *
 *              <-----N----->
 *
 *             ---------------
 * return <-,--|  << mem <<  |---< in
 *          |  --------------- ^
 *          |    tap1^ tap2^   |
 *          |        |     |   |
 *          \________/_____/___/
 */
int poly2 (int *mem, int tap1, int tap2, int in)
{
	int rez = *mem >> (N-1) & 1;
	*mem = *mem<<1 ^ rez << tap1 ^ rez << tap2 ^ rez ^ in;
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
 * используя сдвиговый регистр длины `N' с двумя обратными
 * связями `tap1' и `tap2'.
 */
void makeseq2 (unsigned char *s, int tap1, int tap2, int len)
{
	int mem, i;

	mem = 0;
	for (i=0; i<P; ++i)
		poly2 (&mem, tap1, tap2, 1);
	for (i=0; i<2*P; ++i)
		s[i] = poly2 (&mem, tap1, tap2, 1);
}

/*
 * Поиск N нулей в последовательности `s' длины `len'.
 * Возвращает смещение или -1.
 */
int find_zeros (unsigned char *s, int len)
{
	int i, k;

	for (i=0; i<len-N; ++i) {
		for (k=0; k<N; ++k) {
			if (s[i+k] != 0)
				goto next;
		}
		return i;
next:;	}
	return -1;
}

int main ()
{
	int tap1, tap2;
	int start, len;

	for (tap2=1; tap2<N-1; ++tap2)
		for (tap1=tap2+1; tap1<N; ++tap1) {
			makeseq2 (seq, tap1, tap2, 2*P+N);

			start = find_zeros (seq, 2*P);
			if (start < 0) {
				printf ("Cannot find %d zeros!\n", N);
				continue;
			}
			len = find_zeros (seq + start + 1, 2*P+N - start - 1);
			if (len < 0) {
				printf ("Cannot find repeating %d zeros!\n", N);
				len = P;
			}

			if (len > P-N) {
				printf ("x%d + x%d + x%d + 1 = [ %d ]\n",
					N, tap1, tap2, len);
/*				seqprint (seq + start, len);*/
			}
			/* hexprint (seq, len);*/

			/* for (i=0; i<2*P; ++i)*/
			/*	seq [i] ^= 1;*/
			/* seqprint (seq, len);*/
		}
	return 0;
}
