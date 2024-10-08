/* ПРЕОБРАЗОВАТЕЛЬ ЦЕЛЫХ: ЧИСЛО --> ЕГО ТЕКСТОВОЕ ПРЕДСТАВЛЕНИЕ */
/* на выходе: указатель на строку длиной 'length', дополненную \000,
	      содержащую преобразованное по данной системе счисления число,
	      начинающееся спереди символами 'begins' */

char *conv (value, length, radix, begins)
    register unsigned value;            /* преобразуемое значение */
    int length;                         /* длина выходного поля */
    register radix;                     /* основание системы счисления */
    char begins;                        /* чем дополнить число спереди */
{
    char *ptr;
    static char buf[16+1];
    register i;

    buf[16]='\000';
    for(i=0; i<16; ++i)  buf[i]=begins;
    ptr = &buf[16];

    do {
	*--ptr = (i=value%radix) < 10 ? i+'0' : i-10+'A';
	value /= radix;
     } while(value);

    return(&buf[16-length]);
}
