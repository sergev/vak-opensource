/*
 * Комплексный тест интерфейса Эльбрус-Б - PC AT.
 */

# include "svsb.h"
# include "bcio.h"

# define BCSZ           (3*1024)        /* size of BC input-output buffer */
# define BCCOUNT        64              /* number of transmissions */

# define in             _in_            /* put word into SVSB HW register */
# define out            _out_           /* get word from SVSB HW register */

# define ODD(c)         (_nbits_ (c) & 1) /* if word has odd number of bits */
# define HALT(c)        _halt_ (c)      /* stop SVSB processor */
# define CLOCK()        _in_ (0x1d)

# define IDLE()         _out_ (0x14, 0)

int bchan;              /* byte channel number */

int rfill;
int sfill;

int ofill;
int o2fill;

char rbbuf [BCSZ];      /* буфер приема по БК */
char bbuf [BCSZ];       /* буфер передачи по БК */
char frbbuf [BCSZ];     /* образец передачи по БК */

main ()
{
	bchan = bprobe ();      /* опознавание байтового канала */

	for (;;)
		bctest ();
}

bprobe ()
{
	struct bccmd ccw;
	int csw, waitmask;      /* биты канала в регистре прерываний */
	int recvmask, sendmask;
	int i, chan;

	bcinit ();

	ccw.addr = (int) bbuf;
	ccw.count = 1024 >> 3;
	ccw.bcount = 0;
	ccw.flg = 0;

	waitmask = 0;           /* наша маска в регистре прерываний БК */
	for (i=0; i<8; ++i)
		waitmask |= BCC_IN (i);

	for (i=0; i<8; ++i)
		out (BC_ICW (i) | BCA_START, ccw);      /* пуск каналов приема */

	while (! (in (BC_CTL) & waitmask))              /* ждем конца обмена */
		IDLE ();

	recvmask = in (BC_CTL) & waitmask;

	bcinit ();

	waitmask = 0;           /* наша маска в регистре прерываний БК */
	for (i=0; i<8; ++i)
		waitmask |= BCC_OUT (i);

	for (i=0; i<8; ++i)
		out (BC_OCW (i) | BCA_START, ccw);      /* пуск каналов выдачи */

	while (! (in (BC_CTL) & waitmask))              /* ждем конца обмена */
		IDLE ();

	sendmask = in (BC_CTL) & waitmask;

	bcinit ();

	chan = -1;
	for (i=0; i<8; ++i)
		if ((recvmask & BCC_IN (i)) ||
		    (sendmask & BCC_OUT (i))) {
			if (chan >= 0)
				HALT (0xbc00);
			chan = i;
		}
	if (chan < 0)
		HALT (0xbc01);

	return (chan);
}

/*****************  Выдача запроса в Э1КБ  *****************
 *
 *   Процедура выдает в Э1КБ 2 байта данных.
 *   В первом байте - код запроса
 *   Во втором байте - код росписи, заданный оператором
 *
 *
 *   Коды запросов в Э1КБ (reqcode):
 *
 *     Байт  | ИД
 *    0123456701
 *
 *   '0000xxxx00' && Запусти БК приема, нормальный обмен
 *   '1000xxxx00' && Запусти БК приема, нормальный обмен с ИПБ
 *   '0100xxxx00' && Запусти БК приема, будет укороченная выдача
 *   '1100xxxx00' && Запусти БК приема, будет удлиненная выдача
 *   '0010xxxx00' && Запусти БК приема, будет ошибка четности
 *
 *   '0001xxxx00' && Запусти БК выдачи, нормальный обмен
 *   '1001xxxx00' && Запусти БК выдачи, нормальный обмен с ИПБ
 *   '0101xxxx00' && Запусти БК выдачи, будет укороченный прием
 *   '1101xxxx00' && Запусти БК выдачи, будет удлиненный прием
 *   '0011xxxx00' && Запусти БК выдачи с формированием ошибки четности
 *
 *   'xxxx100000' && Код обмена: Постоянный код 00
 *   'xxxx010000' && Код обмена: Постоянный код FF
 *   'xxxx110000' && Код обмена: Постоянный код 55
 *   'xxxx001000' && Код обмена: Постоянный код AA
 *   'xxxx101000' && Код обмена: Постоянный код xx
 *
 *   'xxxx100100' && Код обмена: Переменный код 00
 *   'xxxx010100' && Код обмена: Переменный код 55
 *   'xxxx110100' && Код обмена: Переменный код xx
 *   'xxxx001100' && Код обмена: Переменный код бег.1
 *   'xxxx101100' && Код обмена: Переменный код бег.0
 *   'xxxx011100' && Код обмена: Переменный код счет.
 *   'xxxx111100' && Код обмена: Переменный код случ.
 *
 *   'xxxxxxxx10' && Запрос на 1000 обменов.
 */

dofill (fill)
{
	if (fill != ofill) {
		fillcarray (bbuf, BCSZ, fill, 0);
		inverbuf ((int *) bbuf, BCSZ / sizeof (int));
		ofill = fill;
	}
}

do2fill (fill)
{
	if (fill != o2fill) {
		fillcarray (frbbuf, BCSZ, fill, 0);
		inverbuf ((int *) frbbuf, BCSZ / sizeof (int));
		o2fill = fill;
	}
}

bctest ()
{
	int i;

	ofill = -1;
	o2fill = -1;

	for (i=0; i<BCCOUNT; ++i)
		bc1test (0x10);     /* БКпр- Нормальный обмен, код 00. */
	for (i=0; i<BCCOUNT; ++i)
		bc1test (0x12);     /* БКпр- Источник короче приемника. */
	for (i=0; i<BCCOUNT; ++i)
		bc1test (0x13);     /* БКпр- Источник длиннее приемника. */
	for (i=0; i<BCCOUNT; ++i)
		bc1test (0x14);     /* БКпр- Имитация ошибки четности. */
	for (i=0; i<BCCOUNT; ++i)
		bc1test (0xc0);     /* БКпр- Нормальный обмен, код бег. 1. */
	for (i=0; i<BCCOUNT; ++i)
		bc1test (0xd0);     /* БКпр- Нормальный обмен, код бег. 0. */
	for (i=0; i<BCCOUNT; ++i)
		bc1test (0x30);     /* БКпр- Нормальный обмен, код 55. */
	for (i=0; i<BCCOUNT; ++i)
		bc1test (0x40);     /* БКпр- Нормальный обмен, код AA. */
	for (i=0; i<BCCOUNT; ++i)
		bc1test (0xa0);     /* БКпр- Нормальный обмен, код 55 и AA. */
	for (i=0; i<BCCOUNT; ++i)
		bc1test (0x90);     /* БКпр- Нормальный обмен, код 00 и FF. */
	for (i=0; i<BCCOUNT; ++i)
		bc1test (0xe0);     /* БКпр- Нормальный обмен, код счетчик. */
	for (i=0; i<BCCOUNT; ++i)
		bc1test (0xe1);     /* БКпр- Измен. порядка байт, код счет. */
	for (i=0; i<BCCOUNT; ++i)
		bc1test (0x18);     /* БКвыд- Нормальный обмен, код 00. */
	for (i=0; i<BCCOUNT; ++i)
		bc1test (0x1a);     /* БКвыд- Приемник короче источника. */
	for (i=0; i<BCCOUNT; ++i)
		bc1test (0x1b);     /* БКвыд- Приемник длиннее источника. */
	for (i=0; i<BCCOUNT; ++i)
		bc1test (0x1c);     /* БКвыд- Имитация ошибки четности. */
	for (i=0; i<BCCOUNT; ++i)
		bc1test (0xc8);     /* БКвыд- Нормальный обмен, код бег. 1. */
	for (i=0; i<BCCOUNT; ++i)
		bc1test (0xd8);     /* БКвыд- Нормальный обмен, код бег. 0. */
	for (i=0; i<BCCOUNT; ++i)
		bc1test (0x38);     /* БКвыд- Нормальный обмен, код 55. */
	for (i=0; i<BCCOUNT; ++i)
		bc1test (0x48);     /* БКвыд- Нормальный обмен, код AA. */
	for (i=0; i<BCCOUNT; ++i)
		bc1test (0xa8);     /* БКвыд- Нормальный обмен,код 55 и AA. */
	for (i=0; i<BCCOUNT; ++i)
		bc1test (0x98);     /* БКвыд- Нормальный обмен,код 00 и FF. */
	for (i=0; i<BCCOUNT; ++i)
		bc1test (0xe8);     /* БКвыд- Нормальный обмен,код счетчик. */
	for (i=0; i<BCCOUNT; ++i)
		bc1test (0xe9);     /* БКвыд- Измен. порядка байт,код счет. */
	bc1test (0x97);
	bc1test (0x9f);
	bc1test (0x100);            /* БК- Встречная передача, код 00 и FF. */
}

bc1test (ctl)
{
	int fill, rez;

	fill = filltype (ctl >> 4 & 0xf);

	switch (ctl & 0x30f) {
	/*
	 * Testing byte channel receiver.
	 */
	case 000:               /* нормальный обмен */
		dofill (fill);
		rez = bcrd (bchan, rbbuf, BCSZ);
		reply ((rez & 0xdf) == 0 && ! bcmp (rbbuf, bbuf, BCSZ));
		break;
	case 001:               /* нормальный обмен с ИПБ */
		dofill (fill);
		rez = bcrd (bchan, rbbuf, BCSZ);
		inverhbuf ((int *) rbbuf, BCSZ / sizeof (int));
		inverbuf ((int *) rbbuf, BCSZ / sizeof (int));
		reply ((rez & 0xdf) == 0 && ! bcmp (rbbuf, bbuf, BCSZ));
		break;
	case 002:               /* будет укороченная выдача */
		dofill (fill);
		rez = bcrd (bchan, rbbuf, BCSZ);
		reply ((rez & 0xdf) == 0x80 && ! bcmp (rbbuf, bbuf, BCSZ - recvlen (bchan)));
		break;
	case 003:               /* будет удлиненная выдача */
		dofill (fill);
		rez = bcrd (bchan, rbbuf, BCSZ);
		reply ((rez & 0xdf) == 0x40 && ! bcmp (rbbuf, bbuf, BCSZ));
		break;
	case 004:               /* будет ошибка четности */
		dofill (fill);
		rez = bcrd (bchan, rbbuf, BCSZ);
		reply ((rez & 0x17) == 0x10);
		break;
	case 007:
		rfill = fill;
		break;
	/*
	 * Testing byte channel transmitter.
	 */
	case 010:               /* нормальный обмен */
		dofill (fill);
		bcwr (bchan, bbuf, BCSZ);
		break;
	case 011:               /* нормальный обмен с ИПБ */
		dofill (fill);
		bcwr (bchan, bbuf, BCSZ);
		break;
	case 012:               /* будет укороченный прием */
		dofill (fill);
		bcwr (bchan, bbuf, BCSZ);
		break;
	case 013:               /* будет удлиненный прием */
		dofill (fill);
		bcwr (bchan, bbuf, BCSZ);
		break;
	case 014:               /* формирование ошибки четности */
		dofill (fill);
		bcwrflags = 0x80000;
		bcwr (bchan, bbuf, BCSZ);
		bcwrflags = 0;
		break;
	case 017:
		sfill = fill;
		break;
	/*
	 * Testing concurrent work of two channels.
	 */
	case 0x100:
		dofill (sfill);
		do2fill (rfill);
		rdwr ();
		reply (1);
		break;
	}
}

reply (ok)
{
	if (! ok)
		HALT (0xbad);
}

fillcarray (buf, length, mode, ucode)
char buf [];
{
	register count;

	switch (mode) {
	default:                /* 00 - Не расписывать, возврат */
		return;
	case 0x22:              /* 22 - Переменный код 00 */
		ucode = 0x00;
varloop:
		ucode = ucode & 0xff | ucode << 8 & 0x300;
		for (count=0; count<length; ++count) {
			buf [count] = ucode;
			ucode ^= 0x3ff;
		}
		break;
	case 0x23:              /* 23 - Переменный код 55 */
		ucode = 0x55;
		goto varloop;
	case 0x24:              /* 24 - Переменный код xx */
		goto varloop;
	case 0x25:              /* 25 - Переменный код бег.1 */
		ucode = 1;
		for (count=0; count<length; ++count) {
			buf [count] = ucode;
			ucode = ucode << 1 & 0x3ff;
			if (! ucode)
				ucode = 1;
		}
		break;
	case 0x26:              /* 26 - Переменный код бег.0 */
		ucode = 1;
		for (count=0; count<length; ++count) {
			buf [count] = ucode ^ 0x3ff;
			ucode = ucode << 1 & 0x3ff;
			if (! ucode)
				ucode = 1;
		}
		break;
	case 0x27:              /* 27 - Переменный код счет. */
		for (count=0; count<length; ++count)
			buf [count] = count & 0xff | count << 8 & 0x300;
		break;
	case 0x28:              /* 28 - Переменный код случ. */
		srand (ucode);
		for (count=0; count<length; ++count)
			buf [count] = rand () & 0x3ff;
		break;
	case 0x32:              /* 32 - Постоянный код 00 */
		ucode = 0x00;
fixloop:
		ucode = ucode & 0xff | ucode << 8 & 0x300;
		for (count=0; count<length; ++count)
			buf [count] = ucode;
		break;
	case 0x33:              /* 33 - Постоянный код FF */
		ucode = 0xff;
		goto fixloop;
	case 0x34:              /* 34 - Постоянный код 55 */
		ucode = 0x55;
		goto fixloop;
	case 0x35:              /* 35 - Постоянный код AA */
		ucode = 0xaa;
		goto fixloop;
	case 0x36:              /* 36 - Постоянный код xx */
		goto fixloop;
	}
}

filltype (fill)
{
	switch (fill) {
	default:        return (0x32);              /* 32 - Постоянный код 00 */
	case 002:       return (0x33);              /* 33 - Постоянный код FF */
	case 003:       return (0x34);              /* 34 - Постоянный код 55 */
	case 004:       return (0x35);              /* 35 - Постоянный код AA */
	case 005:       return (0x36);              /* 36 - Постоянный код xx */

	case 011:       return (0x22);              /* 22 - Переменный код 00 */
	case 012:       return (0x23);              /* 23 - Переменный код 55 */
	case 013:       return (0x24);              /* 24 - Переменный код xx */
	case 014:       return (0x25);              /* 25 - Переменный код бег.1 */
	case 015:       return (0x26);              /* 26 - Переменный код бег.0 */
	case 016:       return (0x27);              /* 27 - Переменный код счет. */
	case 017:       return (0x28);              /* 28 - Переменный код случ. */
	}
}

recvlen (u)             /* вычисление остаточной длины в канале */
{
	struct bccmd ccw;

	*((int *) &ccw) = in (BC_ICW (u) | BCA_NOHALT);
	return ((((ccw.count + 1) & 0xffff) << 3) - ccw.bcount);
}

rdwr ()
{
	struct bccmd ccwr, ccww;
	int cswr, csww;
	int rwaitmask, wwaitmask;       /* биты канала в регистре прерываний */
	int mask;
	int recvcount, sendcount;

	ccwr.addr = (int) rbbuf;
	ccwr.count = BCSZ >> 3;
	ccwr.bcount = 0;
	ccwr.flg = 0;
	ccww.addr = (int) bbuf;
	ccww.count = BCSZ >> 3;
	ccww.bcount = 0;
	ccww.flg = 0;

	rwaitmask = BCC_IN (bchan);     /* наша маска в регистре прерываний БК */
	wwaitmask = BCC_OUT (bchan);    /* наша маска в регистре прерываний БК */

	out (BC_ICW (bchan) | BCA_START, ccwr); /* пуск канала */
	out (BC_OCW (bchan) | BCA_START, ccww); /* пуск канала */

	recvcount = 1;
	sendcount = 1;

loop:
	IDLE ();
	mask = in (BC_CTL);

	if (recvcount < BCCOUNT && (mask & rwaitmask)) { /* ждем конца обмена */
		cswr = in (BC_ISW (bchan) | BCA_NOHALT | BCA_RESET);
		if (cswr & 0xff)                        /* проверили слово состояния */
		     HALT (0xbc0aaa);
		if (bcmp (rbbuf, frbbuf, BCSZ))
		     HALT (0xbc0aa1);

		out (BC_ICW (bchan) | BCA_START, ccwr); /* пуск канала */
		++recvcount;
	}
	if (sendcount < BCCOUNT && (mask & wwaitmask)) { /* ждем конца обмена */
		csww = in (BC_OSW (bchan) | BCA_NOHALT | BCA_RESET);
		if (csww & 0xff)                        /* проверили слово состояния */
		     HALT (0xbc0bbb);

		out (BC_OCW (bchan) | BCA_START, ccww); /* пуск канала */
		++sendcount;
	}

	if (recvcount < BCCOUNT || sendcount < BCCOUNT)
		goto loop;

	while (! (in (BC_CTL) & wwaitmask))      /* ждем конца обмена */
		IDLE ();
	csww = in (BC_OSW (bchan) | BCA_NOHALT | BCA_RESET);
	if (csww & 0xff)                        /* проверили слово состояния */
	     HALT (0xbc0bbb);

	while (! (in (BC_CTL) & rwaitmask))      /* ждем конца обмена */
		IDLE ();
	cswr = in (BC_ISW (bchan) | BCA_NOHALT | BCA_RESET);
	if (cswr & 0xff)                        /* проверили слово состояния */
	     HALT (0xbc0aaa);
}
