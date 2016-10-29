/*
 * Драйвер ЕС-диска для Эльбрус-Б
 * ==============================
 * в формате 808 x 19 x 3 x 4k без тегов
 *
 * Структура канальной программы записи-чтения
 * -------------------------------------------
 *
 * (1) позиционирование головок. Параметр - номер
 * цилиндра и головки
 *
 *              YY 07 64 0000 AAAAAA
 *
 *      YY - номер устройства
 *
 *              Лог. номер      YY+номер
 *              канала          устройства
 *           ------------------------------
 *              0               38
 *              1               d8
 *              2               58
 *              3               80
 *
 *      AAAAAA - физ. адрес параметра
 *
 *              0000 0ЦЦЦ 00ГГ XXXX
 *
 *      ЦЦЦ - номер цилиндра, 0-814
 *      ГГ - номер головки, 0-18
 *
 * (2) ожидание подвода блока
 *
 *              YY 31 54 0000 AAAAAA
 *
 *      AAAAAA - номер цилиндра, головки, блока
 *
 *              0ЦЦЦ 00ГГ ББ00 0000
 *
 *      ЦЦЦ - номер цилиндра, 0-814
 *      ГГ - номер головки, 0-18
 *      ББ - номер блока (сектора), 1-3
 *
 * (3) переход на предыдущую команду
 *
 *              YY 08 0c 0000 AAAAAA
 *
 *      AAAAAA - адрес предыдущей команды
 *
 * (4) команда обмена
 *
 *              YY ZZ 04 0200 AAAAAA
 *
 *      ZZ - команда: чтение=06, запись=05
 *      AAAAAA - адрес буфера обмена, 131 слово
 *
 * Команды (1)-(4) могут повторяться снова с другими параметрами.
 * Если номер цилиндра не изменился, команду (1) можно не повторять
 *
 * У последней команды 43 бит =0.
 */

# include "param.h"
# include "inode.h"
# include "fs.h"

# include "saio.h"
# include "scio.h"
# include "svsb.h"

# define DEBUG

# define HPBSIZE        4096            /* block size */
# define W              ((int) sizeof (int))

# define HPC            19              /* heads per cylinder */
# define BPT            3               /* blocks per track */
# define NCYL           808             /* number of cylinders in use */

# define BPC            (HPC*BPT)       /* blocks per cylinder */
# define NBLOCKS        (NCYL*HPC*BPT)

# define MAXRETRY       5               /* max number of retry */

# define MAXCPSZ        16              /* макс. длина канальной программы */

struct sccmd hpsense = {
/* уточнение состояния  addr    count   flg     cmd     unit    */
/* &hpsbuf */           0,      3,      0,      4,      0,
};

int hpsbuf [3];

# ifdef CORR
struct sccmd hpcorr [3] = {
/* возврат в кан. прог. addr    count   flg     cmd     unit    */
/* &hpsbuf[1] */        0,      0,      0x54,   0x31,   0,
/* .-1 */               0,      0,      0x04,   0x08,   0,
/* АСК */               0,      0,      0,      0x08,   0,
};
# endif

struct sccmd hpsample [4] = {
/* обмен блоком         addr    count   flg     cmd     unit    */
/* &hpcyl */            0,      0,      0x64,   0x07,   0,
/* &hpblock */          0,      0,      0x54,   0x31,   0,
/* .-1 */               0,      0,      0x0c,   0x08,   0,
/* &buf */              0,      HPBSIZE/W, 0x04, 0x06,   0,
};

struct sccmd hpprog [4*MAXCPSZ];
int nprog;                      /* длина канальной программы в словах */

int hpchan = 0;                 /* номер дискового канала */

int hpcyl [MAXCPSZ];            /* 0000 0ccc 00hh xxxx */
int hpblock [MAXCPSZ];          /* 0ccc 00hh bb00 0000 */

int hpcsw;                      /* channel status word */
int hpunit;                     /* unit address for channel program */

extern chantab [];

hpopen (io)
register struct iob *io;
{
	static firsttime = 1;

	io->i_unit &= 7;
	screset (hpchan);

	if (! io->i_dsz)
		io->i_dsz = NBLOCKS;
	if (! firsttime)
		return;

	hpsense.addr = (int) hpsbuf;
	firsttime = 0;
}

hpstrategy (io, func)
register struct iob *io;
{
	/* io->i_ma - адрес в памяти */
	/* io->i_cc - длина блока в байтах */
	/* io->i_bn - номер блока */
	int n, i;

	if (io->i_bn < 0 || io->i_bn >= io->i_boff+io->i_dsz)
		return (0);
	n = io->i_cc / HPBSIZE;
	if (io->i_cc != n*HPBSIZE) {
		printf (MSG ("hp: bad block length %d\n",
			"hp: плохая длина блока %d\n"), io->i_cc);
		return (-1);
	}
	if (io->i_bn+n > io->i_boff+io->i_dsz)
		n = io->i_dsz - io->i_bn;
	if (hprw (hpchan, io->i_unit, func==WRITE ? 0 : 1, io->i_bn, n, io->i_ma) < 0)
		return (-1);
	return (n * HPBSIZE);
}

/* ARGSUSED */
hpioctl (io, cmd, arg)
struct iob *io;
int cmd;
caddr_t arg;
{
	return (ECMD);
}

int hprw (chan, unit, rd, nblock, sz, addr)
int *addr;
{
	if (nblock<0 || nblock+sz>NBLOCKS) {
		printf (MSG ("hp: bad block number %d:%d\n",
			"hp: плохой номер блока %d:%d\n"), nblock, sz);
		return (-1);
	}
	hpsetup (chan, unit, rd, nblock, sz, addr);
	if (hdio (chan, hpprog) < 0)
		return (-1);
	return (0);
}

hpsetup (chan, unit, rd, nblock, sz, addr)
int *addr;
{
	int c, h, b, i;

	/* формирование канальной программы */
	nprog = 0;
	for (i=0; i<sz; ++i, ++nblock) {
		c = nblock / BPC;       /* номер цилиндра */
		b = nblock - c * BPC;
		h = b / BPT;            /* номер головки */
		b = b - h * BPT + 1;    /* номер сектора */

		hpcyl [i] = c<<32 | h<<16 | b<<8;
		hpblock [i] = hpcyl [i] << 16;

		if (! i || (hpcyl[i] ^ hpcyl[i-1]) >> 16) {
			/* переход на следующий цилиндр */
			hpprog[nprog] = hpsample[0];
			hpprog[nprog++].addr = (int) &hpcyl[i];
		}
		hpprog[nprog] = hpsample[1];
		hpprog[nprog++].addr = (int) &hpblock[i];
		hpprog[nprog] = hpsample[2];
		hpprog[nprog].addr = (int) &hpprog[nprog-1];
		hpprog[++nprog] = hpsample[3];
		hpprog[nprog].addr = (int) (addr + i*(HPBSIZE/W));
		hpprog[nprog++].cmd = rd ? 6 : 5;
	}

	/* установка номера устройства */
	hpunit = chantab [chan & 3] + unit;
	hpsense.unit = hpunit;
	for (i=0; i<nprog; ++i)
		hpprog[i].unit = hpunit;

	/* конец цепочки команд */
	hpprog[nprog-1].flg &= ~4;
}
