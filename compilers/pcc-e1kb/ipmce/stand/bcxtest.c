# include "svsb.h"
# include "bcio.h"

# define DFLTUNIT       1

# define INCREMENT      0x155

# define K              1024
# define BUFSZ          (2*K)
# define IOSZ           K

# define in             _in_
# define out            _out_

# define TICK           0xf22a1
# define CLOCK()        _in_ (0x1d)

char sbuf1 [BUFSZ];
char sbuf2 [BUFSZ];
char *sbuf;
char rbuf [BUFSZ];

int rerr, serr, cerr;
long iotime, lasttime;

main ()
{
	int u;

	printf (MSG ("\nBCTEST %s    %s\n", "\nТЕСТ БК %s    %s\n"),
		BOOTVERSION, MSG (LCOPYRIGHT, RCOPYRIGHT));
	putchar ('\n');
	u = getint (MSG ("Enter channel number: (0-%d) [%d] ",
		"Введите номер канала (0-%d) [%d] "), DFLTUNIT, 7);
	printf (MSG ("\nTesting bc%d. Press ^^ to abort\n",
		"Работает тест БК %d. Чтобы прервать, нажмите ^^\n"), u);
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

account (u, n)
long n;
{
	printf (MSG ("\nTesting bc%d. Press ^^ to abort\n",
		"\nРаботает тест БК %d. Чтобы прервать, нажмите ^^\n"), u);
	printf ("За ");
	prtime (iotime);
	printf (" передано %ld Мбайт, скорость обмена %d кбайт/сек\n",
		n/1024, 64*1024 / (lasttime / TICK));
	printf ("Ошибок передачи %d, приема %d, несравнений %d\n",
		serr, rerr, cerr);
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
	for (i=0; i<len; ++i) {
		sbuf1 [i] = s;
		sbuf2 [i] = ~s;
		s = ~s;
	}
	s = 0;
	n = 0;
	sbuf = sbuf1;
	for (;;) {
		idle (1);
		timer (1);
		if (sendrecv (u, rbuf, sbuf, len) <= 0) {
			putchar ('\n');
			for (i=0; i<len; ++i)
				if (rbuf [i] != sbuf [i]) {
					printf ("%d: %xh # %xh\n",
						i, sbuf [i], rbuf [i]);
					break;
				}
			continue;
		}
		lasttime += timer (0);
		if (wcmp ((int *) rbuf, (int *) sbuf, len/8)) {
			putchar ('#');
			++cerr;
			continue;
		}
		if (sbuf == sbuf1)
			sbuf = sbuf2;
		else
			sbuf = sbuf1;
		if (++s >= 64) {
			s = 0;
			putchar ('.');
			if (++n % 1024 == 0) {
				iotime += lasttime / TICK;
				account (u, n*64);
				lasttime = 0;
			}
		}
	}
}

sendrecv (u, br, bw, sz)
char *br, *bw;
{
	int cr, cw, ret;

	ret = sz;
	cw = bcrdwr (u, br, bw, sz);
	cr = cw >> 8;
	cw &= 0xff;
	if (cw) {
		printf ("S<%xh>", cw);
		++serr;
		ret = -1;
	}
	if (cr) {
		printf ("R<%xh>", cr);
		++rerr;
		ret = -1;
	}
	return (ret);
}
