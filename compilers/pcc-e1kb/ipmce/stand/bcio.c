# include "svsb.h"
# include "bcio.h"

/* # define DEBUG */

# define in             _in_
# define out            _out_

# define DELAY(n)       { int i=n; while (--i>=0); }

int bcrdflags;
int bcwrflags;

/*
 * Прием по байтовому каналу
 * Возвращает слово состояния канала
 * Буфер обмена должен быть выровнен на границу слова
 */

int bcrd (chan, addr, size)
int chan;               /* номер канала, 3 бита */
char *addr;             /* физ адрес массива обмена, 23 бита */
int size;               /* размер массива в байтах, 19 бит */
{
	struct bccmd ccw;
	int csw, waitmask;      /* биты канала в регистре прерываний */

	ccw.addr = (int) addr;
	ccw.count = size >> 3;
	ccw.bcount = size & 7;
	ccw.flg = bcrdflags;

	waitmask = BCC_IN (chan);       /* наша маска в регистре прерываний БК */

# ifdef DEBUG
	printf ("scrd: %w", ccw);
# endif
	_flush_ ();                             /* выталкивание БРЗ */
	out (BC_ICW (chan) | BCA_START, ccw);   /* пуск канала */

	while (! (in (BC_CTL) & waitmask))      /* ждем конца обмена */
		idle (1);
	csw = in (BC_ISW (chan) | BCA_NOHALT | BCA_RESET);      /* прочли слово состояния */
# ifdef DEBUG
	printf (" -> %w\n", csw);
# endif
	return (csw);
}

/*
 * Передача по байтовому каналу
 * Возвращает слово состояния канала
 * Буфер обмена должен быть выровнен на границу слова
 */

int bcwr (chan, addr, size)
int chan;               /* номер канала, 3 бита */
char *addr;             /* физ адрес массива обмена, 23 бита */
int size;               /* размер массива в байтах, 19 бит */
{
	struct bccmd ccw;
	int csw, waitmask;      /* биты канала в регистре прерываний */

	ccw.addr = (int) addr;
	ccw.count = size >> 3;
	ccw.bcount = size & 7;
	ccw.flg = bcwrflags;

	waitmask = BCC_OUT (chan);      /* наша маска в регистре прерываний БК */

# ifdef DEBUG
	printf ("scwr: %w", ccw);
# endif
	_flush_ ();                             /* выталкивание БРЗ */
	out (BC_OCW (chan) | BCA_START, ccw);   /* пуск канала */

	while (! (in (BC_CTL) & waitmask))      /* ждем конца обмена */
		idle (1);
	csw = in (BC_OSW (chan) | BCA_NOHALT | BCA_RESET);      /* прочли слово состояния */
# ifdef DEBUG
	printf (" -> %w\n", csw);
# endif
	return (csw);
}

/*
 * Прием-передача по байтовому каналу
 * Возвращает слово состояния канала
 * Буфер обмена должен быть выровнен на границу слова
 */

int bcrdwr (chan, raddr, waddr, size)
int chan;               /* номер канала, 3 бита */
char *raddr, *waddr;    /* физ адрес массива обмена, 23 бита */
int size;               /* размер массива в байтах, 19 бит */
{
	struct bccmd ccwr, ccww;
	int cswr, csww;
	int rwaitmask, wwaitmask;       /* биты канала в регистре прерываний */

	ccwr.addr = (int) raddr;
	ccwr.count = size >> 3;
	ccwr.bcount = size & 7;
	ccwr.flg = bcrdflags;
	ccww.addr = (int) waddr;
	ccww.count = size >> 3;
	ccww.bcount = size & 7;
	ccww.flg = bcwrflags;

	rwaitmask = BCC_IN (chan);       /* наша маска в регистре прерываний БК */
	wwaitmask = BCC_OUT (chan);      /* наша маска в регистре прерываний БК */

# ifdef DEBUG
	printf ("scrd=%w  scwr=%w", ccwr, ccww);
# endif
	_flush_ ();                             /* выталкивание БРЗ */
	out (BC_ICW (chan) | BCA_START, ccwr);  /* пуск канала */
	out (BC_OCW (chan) | BCA_START, ccww);  /* пуск канала */

	while (! (in (BC_CTL) & rwaitmask))      /* ждем конца обмена */
		idle (1);
	while (! (in (BC_CTL) & wwaitmask))      /* ждем конца обмена */
		idle (1);
	cswr = in (BC_ISW (chan) | BCA_NOHALT | BCA_RESET);     /* прочли слово состояния */
	csww = in (BC_OSW (chan) | BCA_NOHALT | BCA_RESET);     /* прочли слово состояния */
# ifdef DEBUG
	printf (" -> %w  %w\n", cswr, scww);
# endif
	return ((cswr & 0xff) << 8 | (csww & 0xff));
}

/*
 * Сброс байтового канала
 */

bcreset (chan)
int chan;               /* номер канала, 3 бита */
{
	register i, a;

	in (BC_OSW (chan) | BCA_NOHALT | BCA_RESET);
	in (BC_ISW (chan) | BCA_NOHALT | BCA_RESET);
	a = BC_LMEM (chan);                     /* база МПК */
	for (i=0; i<4; ++i)
		out (a + i, 0);                 /* роспись МПК нулями */
	out (BC_CTL, 0);                        /* сброс регистра прерываний */
}

/*
 * Инициализация байтовых каналов
 */

bcinit ()
{
	int chan;

	for (chan=0; chan<8; ++chan)
		bcreset (chan);
}
