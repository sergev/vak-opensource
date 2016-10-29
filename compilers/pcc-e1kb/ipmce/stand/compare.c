/* сравнение файлов */

# include "svsb.h"

# define K      1024
# define BSIZ   16

char buffer1 [BSIZ*K];
char buffer2 [BSIZ*K];

yes ()
{
	char rep [80];

	printf ("? (y, n, д, н) [н] ");
	gets (rep);
	return (rep[0]=='y' || rep[0]=='Y' || rep[0]=='д' || rep[0]=='Д');
}

main ()
{
	int from, to;
	char fbuf [80], tbuf [80];
	register record;
	int rcc, wcc, len;

	printf (MSG ("\nCOMPARE %s    %s\n", "\nСРАВНЕНИЕ %s    %s\n"),
		BOOTVERSION, MSG (LCOPYRIGHT, RCOPYRIGHT));
	putchar ('\n');
	from = getdev (MSG ("What to compare", "Что сравнивать"), fbuf, 0);
	to = getdev (MSG ("With", "С чем"), tbuf, 0);
	printf (MSG ("Compare '%s' with '%s'", "Сравнить '%s' и '%s'"),
		fbuf, tbuf);
	if (! yes ())
		return;
	len = 0;
	for (record=0; ; ++record) {
		if (! (record % 4))
			printf ("\r%d", len/1024);
		rcc = read (from, buffer1, sizeof (buffer1));
		if (rcc == 0)
			break;
		if (rcc < 0) {
			printf (MSG ("\nRecord %d: read error\n",
				"\nЗапись %d: ошибка чтения\n"), record);
			break;
		}
		/* For bug in ht driver */
		if (rcc > sizeof (buffer1))
			rcc = sizeof (buffer1);
		wcc = read (to, buffer2, sizeof (buffer2));
		if (wcc < 0) {
			printf (MSG ("\nRecord %d: read error\n",
				"\nЗапись %d: ошибка чтения\n"), record);
			break;
		}
		/* For bug in ht driver */
		if (wcc > sizeof (buffer2))
			wcc = sizeof (buffer2);
		if (wcc != rcc) {
			printf (MSG ("\nRecord %d: different length: %d != %d\n",
				"\nЗапись %d: разные длины: %d != %d\n"),
				record, rcc, wcc);
			break;
		}
		if (bcmp (buffer1, buffer2, rcc)) {
			printf (MSG ("\nRecord %d differ\n",
				"\nЗаписи %d не равны\n"),
				record, rcc, wcc);
			break;
		}
		len += wcc;
	}
	printf ("\r%d\n", len/1024);
	close (from);
	close (to);
	printf (MSG ("Compared %d records, %d bytes\n",
		"Сравнено %d записей, %d байтов\n"), record, len);
}

getdev (prompt, buf, mode)
char *prompt, *buf;
int mode;
{
	register int i;

	do {
		printf ("%s: ", prompt);
		gets (buf);
		i = open (buf, mode);
	} while (i <= 0);
	return (i);
}
