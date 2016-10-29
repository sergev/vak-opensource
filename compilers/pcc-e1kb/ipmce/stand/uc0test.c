/*
 * Тест интерфейса Эльбрус-Б - PC AT, универсальный канал
 *
 * Основная идея состоит в следующем.
 * На Эльбрусе-Б работает подчиненный процесс,
 * который принимает байты от УК и передает их обратно.
 * На PC работает ведущая тестирующая программа,
 * передающая байты в канал, принимающая эхо-ответ
 * и сравнивающая его с эталоном.
 * При передаче с той и другой стороны в передаваемый байт
 * иногда вводится помеха - неверная четность.
 * Принимающая сторона обязана ответить корректной квитанцией.
 */

# include "svsb.h"
# include "ucio.h"

# define DFLTUNIT       2

# define in             _in_
# define out            _out_

# define odd(c)         (_nbits_ (c) & 1)

# define outack(u,a)    out (UCREG(u), oreg=UC_RDY|(a)?oreg|UC_ACK:oreg&~UC_ACK)
# define outdata(u,a)   out (UCREG(u), oreg=UC_STROBE|oreg&~UC_PDATA|(a)&UC_PDATA)
# define outclr(u,a)    out (UCREG(u), oreg&=~(a))

int oreg, ireg;
int rerr, serr, cerr;

main ()
{
	int u;

	printf (MSG ("\nUCTEST %s    %s\n", "\nТЕСТ УК %s    %s\n"),
		BOOTVERSION, MSG (LCOPYRIGHT, RCOPYRIGHT));
	putchar ('\n');
	u = getint (MSG ("Enter channel number: (0-3) [%d] ",
		"Введите номер канала (0-3) [%d] "), DFLTUNIT);
	printf (MSG ("\nTesting uc%d.\nPress ^^ to abort\n",
		"Работает тест УК %d\nЧтобы прервать, нажмите ^^\n"), u);
	uctest (u);
}

getint (s, dflt)
char *s;
{
	int i;
	char line [80];

	do {
		printf (s, dflt);
		gets (line);
		if (! line [0])
			return (dflt);
		i = atoi (line);
	} while (i<0 || i>3);
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
	printf (" передано %ld кбайт, скорость обмена %d кбайт/сек\n",
		n, 2*1024 / (tnew - tlast));
	printf ("Ошибок приема %d, передачи %d, несравнений %d\n",
		rerr, serr, cerr);
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

uctest (u)
{
	int c, i, w;
	long n;

	oreg = 0;
	outack (u, 0);

	i = 0;
	w = 0;
	n = 0;
	account (0L);
	for (;;) {
		idle (1);
		c = recv (u, w);
		if (c != w) {
			printf ("(%xh # %xh)", w, c);
			++cerr;
		}
		send (u, c);
		w = c ^ UC_DATA;
		if (++i >= 0x400) {
			if (++n % 1024 == 0)
				account (n);
			i = 0;
			putchar ('.');
		}
	}
}

recv (u, w)
{
	int c;

	for (;;) {
		ireg = in (UCREG (u));
		if (! (ireg & UC_STROBE)) {     /* wait for strobe */
			idle (1);
			continue;
		}
		outclr (u, UC_RDY);             /* clear 'ready' */

		c = ireg & UC_PDATA;            /* get data & parity */
		if (! odd (c)) {
			outack (u, 0);          /* bad parity */
			putchar ('R');
			if (w >= 0)
				printf ("<%x # %x>", w & UC_DATA, c);
			idle (1);
			++rerr;
			continue;               /* wait for retry */
		}
		outack (u, 1);                  /* receive ok */
		return (c & UC_DATA);
	}
}

send (u, d)
{
	d &= UC_DATA;
	if (! odd (d))
		d |= UC_PARITY;
	outdata (u, d);

	for (;;) {
		ireg = in (UCREG (u));
		if (! (ireg & UC_RDY)) {        /* wait for 'ready' */
			idle (1);
			continue;
		}
		outclr (u, UC_STROBE);          /* clear strobe */
		if (! (ireg & UC_ACK)) {
			putchar ('S');          /* bad parity */
			outdata (u, d);         /* retry */
			idle (1);
			++serr;
			continue;
		}
		return;                         /* send ok */
	}
}
