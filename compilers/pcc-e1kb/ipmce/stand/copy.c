/* перепись файлов */

# include "svsb.h"

# define K      1024
# define BSIZ   8

char buffer [BSIZ*K];

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

	printf (MSG ("\nCOPY %s    %s\n", "\nПЕРЕПИСЬ %s    %s\n"),
		BOOTVERSION, MSG (LCOPYRIGHT, RCOPYRIGHT));
	putchar ('\n');
	from = getdev (MSG ("From", "Откуда"), fbuf, 0);
	to = getdev (MSG ("To", "Куда"), tbuf, 1);
	printf (MSG ("Copy from '%s' to '%s'", "Переписать '%s' на '%s'"),
		fbuf, tbuf);
	if (! yes ())
		return;
	len = 0;
	for (record=0; ; ++record) {
		if (! (record % 4))
			printf ("\r%d", len/1024);
		rcc = read (from, buffer, sizeof (buffer));
		if (rcc == 0)
			break;
		if (rcc < 0) {
			printf (MSG ("\nRecord %d: read error\n",
				"\nЗапись %d: ошибка чтения\n"), record);
			break;
		}
		/* For bug in ht driver */
		if (rcc > sizeof (buffer))
			rcc = sizeof (buffer);
		wcc = write (to, buffer, rcc);
		if (wcc < 0) {
			printf (MSG ("\nRecord %d: write error\n",
				"\nЗапись %d: ошибка записи\n"), record);
			break;
		}
		if (wcc < rcc) {
			printf (MSG ("\nRecord %d: write short; expected %d, got %d\n",
				"\nКороткая запись: считано %d, записано %d\n"),
				record, rcc, wcc);
			break;
		}
		len += wcc;
	}
	printf ("\r%d\n", len/1024);
	close (from);
	close (to);
	printf (MSG ("Copied %d records, %d bytes\n",
		"Переписано %d записей, %d байтов\n"), record, len);
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
