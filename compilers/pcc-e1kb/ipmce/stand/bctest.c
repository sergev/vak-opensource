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
# define POINTSZ        32
# define ACCSZ          256

# define in             _in_
# define out            _out_

# define TICK           0xf22a1
# define CLOCK()        _in_ (0x1d)

char buf [BUFSZ];
int rerr, serr, lerr;
long iotime, lasttime;

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

long timer (on)
{
	static long ton;

	if (on)
		return (ton = CLOCK ());
	return (CLOCK () - ton);
}

account (n, ln)
long n, ln;
{
	printf ("\nЗа ");
	prtime (iotime);
	printf (" передано %ld Мбайт, скорость обмена %d кбайт/сек\n",
		n/K, 2*ln*1024L / (lasttime / TICK));
	printf ("Ошибок приема %d, передачи %d, длины %d\n",
		rerr, serr, lerr);
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
	int rlen, slen, s;
	long n, total, lasttotal;

	bcinit ();
	s = 0;
	n = 0;
	lasttotal = total = 0;
	for (;;) {
		idle (1);
		timer (1);
		rlen = recv (u, buf);
		if (rlen < 0)
			continue;
		slen = send (u, buf, rlen);
		if (slen < 0)
			continue;
		if (rlen != slen) {
			/* cannot happen */
			printf ("(%xh # %xh)", rlen, slen);
			++lerr;
			continue;
		}
		lasttime += timer (0);
		lasttotal += rlen / K;
		if (++s >= POINTSZ) {
			s = 0;
			putchar ('.');
			if (++n % ACCSZ == 0) {
				iotime += lasttime / TICK;
				total += lasttotal;
				account (total, lasttotal);
				lasttime = 0;
				lasttotal = 0;
			}
		}
	}
}

recv (u, b)
char *b;
{
	int c, sz, res;
	struct bccmd ccw;

	sz = BUFSZ;
	c = bcrd (u, b, sz) & 0xff;
	if (c & 0x7f) {
		printf ("R<%xh>", c);
		++rerr;
		return (-1);
	}
	*((int *) &ccw) = in (BC_ICW (u) | BCA_NOHALT);
	return (sz + ccw.bcount - (((ccw.count + 1) & 0xffff) << 3));
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
