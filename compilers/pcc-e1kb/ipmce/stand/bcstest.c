/*
 * Тест интерфейса Эльбрус-Б - PC AT, байтовый канал
 *
 * Основная идея состоит в следующем.
 * На Эльбрусе-Б работает подчиненный процесс,
 * который принимает массивы от БК и передает их обратно.
 * На PC работает ведущая тестирующая программа,
 * передающая данные в канал, принимающая эхо-ответ
 * и сравнивающая его с эталоном.
 */

# include "svsb.h"
# include "bcio.h"

# define DFLTUNIT       0

# define K              1024
# define BUFSZ          (16*K)
# define IOSZ           K

# define INCREMENT      0x155

# define in             _in_
# define out            _out_

char buf [BUFSZ];
int rerr, serr;

main ()
{
	int u;

	printf (MSG ("\nBCTEST %s    %s\n", "\nТЕСТ БК %s    %s\n"),
		BOOTVERSION, MSG (LCOPYRIGHT, RCOPYRIGHT));
	putchar ('\n');
	u = getint (MSG ("Enter channel number: (0-%d) [%d] ",
		"Введите номер канала (0-%d) [%d] "), DFLTUNIT, 7);
	printf (MSG ("\nTesting bc%d.\nPress ^^ to abort\n",
		"Работает тест БК %d\nЧтобы прервать, нажмите ^^\n"), u);
	bctest (u);
}

getint (s, dflt, max)
char *s;
{
	int i;
	char line [80];

	do {
		printf (s, max, dflt);
		gets (line);
		if (! line [0])
			return (dflt);
		i = atoi (line);
	} while (i<0 || i>max);
	return (i);
}

account (n)
long n;
{
	static long t0, tlast;
	long tnew;

	if (! n) {
		tlast = t0 = time (0L);
		return;
	}
	tnew = time (0L);
	printf ("\nЗа ");
	prtime (tnew - t0);
	printf (" передано %ld Мбайт, скорость обмена %d кбайт/сек\n",
		n/1024, 64*1024 / (tnew - tlast));
	printf ("Ошибок передачи %d\n", serr);
	tlast = time (0L);
}

prtime (t)
register long t;
{
	register h, m, s;

	h = t / 3600;
	m = (t -= (long) h * 3600) / 60;
	s = t - m * 60;
	if (h)
		printf ("%d:%d:%d", h, m, s);
	else
		printf ("%d:%d", m, s);
}

bctest (u)
{
	int len, s, i;
	long n;

	bcinit ();
	len = IOSZ;
	s = 0;
	for (i=0; i<len; ++i)
		buf [i] = s += INCREMENT;
	s = 0;
	n = 0;
	account (0L);
	for (;;) {
		idle (1);
		if (send (u, buf, len) <= 0)
			continue;
		if (++s >= 64) {
			s = 0;
			putchar ('.');
			if (++n % 1024 == 0)
				account (n*64);
		}
	}
}

send (u, b, sz)
char *b;
{
	int c;

	c = bcwr (u, b, sz) & 0xff;
	if (c) {
		printf ("S<%xh>", c);
		++serr;
		return (-1);
	}
	return (sz);
}
