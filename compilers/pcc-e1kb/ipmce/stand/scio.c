# include "svsb.h"
# include "scio.h"

/* # define DEBUG */

# define DELAY(n)       { int i=n; while (--i>=0); }

struct sccmd scgo = { 0, 0, 0, 8, 0, };

int chantab [4] = { 0x38, 0xd8, 0x58, 0x80, };

/*
 * Обмен по селекторному каналу
 * Возвращает слово состояния канала
 * ТРБ-А игнорируется
 * Если устройство занято, обмен повторяется
 *
 * Буфер обмена выровнен на границу слова
 */

int scio (chan, unit, opcode, addr, size, tags)
int chan;               /* номер канала, 2 бита */
int unit;               /* номер устройства, 2 бита */
int opcode;             /* операция, 8 бит */
int addr;               /* физ адрес массива обмена, 23 бита */
int size;               /* размер массива в байтах, 19 бит */
int tags;               /* флаг обмена с тегами */
{
	struct sccmd ccw;

	ccw.addr = addr;
	ccw.count = size >> 3;
	ccw.flg = tags << 7 | (size & 7) << 4;
	ccw.cmd = opcode;
	ccw.unit = chantab [chan] + unit;

	return (scwait (chan, &ccw));
}

int scwait (chan, prog)         /* возвращает слово состояния канала */
int chan;                       /* номер канала, 0-3 */
int *prog;                      /* канальная программа */
{
	int csw;                /* слово состояния канала */
	int waitmask;           /* биты канала в регистре прерываний */

	waitmask = SCC_IOEND (chan) | SCC_CSTATUS (chan);
# ifdef DEBUG
	printf ("scio: %w\n");
# endif
repeat:
	_flush_ ();                             /* выталкивание БРЗ */
	_out_ (SC_CAW (chan) | SCA_START, prog); /* пуск канала */
wait:
	/* ждем конца обмена или изменения состояния */
	while (! (_in_ (SC_CTL) & waitmask))
		idle (1);
	/* прочли слово состояния */
	csw = _in_ (SC_CSW (chan) | SCA_NOHALT);
	if (csw & 1) {                          /* ТРБ-А */
		_in_ (SC_CSW (chan) | SCA_NOHALT | SCA_RESET);
		goto repeat;
	}
	if ((csw & 0xf) == 0xa) {               /* устройство занято */
		_in_ (SC_CSW (chan) | SCA_NOHALT | SCA_RESET);
		goto wait;
	}
	return (csw);
}

/*
 * Сброс селекторного канала
 */

screset (chan)
int chan;               /* номер канала, 2 бита */
{
	register i, a;

	_out_ (SC_CTL, SCC_HRESET (chan));      /* обнуление канала */
	_out_ (SC_CTL, 0);                      /* сброс регистра прерываний */
	_out_ (SC_CAW (chan) | SCA_START, &scgo); /* переход в 0 */
	DELAY (100);                            /* задержка */
	a = SC_LMEM (chan);                     /* база МПК */
	for (i=0; i<8; ++i)
		_out_ (a + i, 0);               /* роспись МПК нулями */
	_in_ (SC_CSW (chan) | SCA_NOHALT | SCA_RESET);
}

/*
 * Инициализация селекторных каналов
 */

scinit ()
{
	int chan;

	for (chan=0; chan<4; ++chan)
		screset (chan);
}
