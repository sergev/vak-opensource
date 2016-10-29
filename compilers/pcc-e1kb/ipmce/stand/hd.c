/*
 * Драйвер ЕС-диска для Эльбрус-Б
 * ==============================
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
 *      ББ - номер блока (сектора), 1-10
 *
 * (3) переход на предыдущую команду
 *
 *              YY 08 0c 0000 AAAAAA
 *
 *      AAAAAA - адрес предыдущей команды
 *
 * (4) команда обмена
 *
 *              YY ZZ 84 0083 AAAAAA
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

# define HPC            19              /* heads per cylinder */
# define BPT            10              /* blocks per track */
# define NCYL           808             /* number of cylinders in use */

# define BPC            (HPC*BPT)       /* blocks per cylinder */
# define NBLOCKS        (NCYL*HPC*BPT)

# define MAXRETRY       5               /* max number of retry */

# define MAXCPSZ        16              /* макс. длина канальной программы */

struct sccmd hdsense = {
/* уточнение состояния  addr    count   flg     cmd     unit    */
/* &hdsbuf */           0,      3,      0,      4,      0,
};

int hdsbuf [3];

# ifdef CORR
struct sccmd hdcorr [3] = {
/* возврат в кан. прог. addr    count   flg     cmd     unit    */
/* &hdsbuf[1] */        0,      0,      0x54,   0x31,   0,
/* .-1 */               0,      0,      0x04,   0x08,   0,
/* АСК */               0,      0,      0,      0x08,   0,
};
# endif

struct sccmd hdsample [4] = {
/* обмен блоком         addr    count   flg     cmd     unit    */
/* &hdcyl */            0,      0,      0x64,   0x07,   0,
/* &hdblock */          0,      0,      0x54,   0x31,   0,
/* .-1 */               0,      0,      0x0c,   0x08,   0,
/* &hdbuf */            0,      131,    0x84,   0x06,   0,
};

struct sccmd hdprog [4*MAXCPSZ];
int nprog;                      /* длина канальной программы в словах */

int hdchan = 0;                 /* номер дискового канала */

int hdcyl [MAXCPSZ];            /* 0000 0ccc 00hh xxxx */
int hdblock [MAXCPSZ];          /* 0ccc 00hh bb00 0000 */
int hdbuf [MAXCPSZ] [131];      /* io buffer */

int hdcsw;                      /* channel status word */
int hdunit;                     /* unit address for channel program */

extern chantab [];

hdopen (io)
register struct iob *io;
{
	static firsttime = 1;

	io->i_unit &= 7;
	screset (hdchan);

	if (io->i_partname [0]) {
		printf (MSG ("hd: cannot handle partitions yet\n",
			"hd: разделы пока не реализованы\n"));
		io->i_boff = 0;
		io->i_dsz = NBLOCKS;
	} else if (! io->i_dsz)
		io->i_dsz = NBLOCKS;
	if (! firsttime)
		return;

	hdsense.addr = (int) hdsbuf;
	firsttime = 0;
}

hdstrategy (io, func)
register struct iob *io;
{
	/* io->i_ma - адрес в памяти */
	/* io->i_cc - длина блока в байтах */
	/* io->i_bn - номер блока */
	int n, i;

	if (io->i_bn < 0 || io->i_bn >= io->i_boff+io->i_dsz)
		return (0);
	n = io->i_cc / DEV_BSIZE;
	if (io->i_cc != n*DEV_BSIZE) {
		printf (MSG ("hd: bad block length %d\n",
			"hd: плохая длина блока %d\n"), io->i_cc);
		return (-1);
	}
	if (io->i_bn+n > io->i_boff+io->i_dsz)
		n = io->i_dsz - io->i_bn;
	if (func == WRITE) {
		for (i=0; i<n; ++i) {
			bcopy (io->i_ma+i*DEV_BSIZE, hdbuf [i], DEV_BSIZE);
			hdbuf [i] [128] = crc (hdbuf [i]);
			hdbuf [i] [129] = hdbuf [i] [130] = 0;
			clrtg (hdbuf [i]);
		}
	}
	if (hdrw (hdchan, io->i_unit, func==WRITE ? 0 : 1, io->i_bn, n) < 0)
		return (-1);
	if (func == READ) {
		for (i=0; i<n; ++i) {
			if (hdbuf [i] [128] != crc (hdbuf [i])) {
				printf (MSG ("hd: bad checksum, block %d\n",
					"hd: плохая КС, блок %d\n"), io->i_bn+i);
				return (-1);
			}
			bcopy (hdbuf [i], io->i_ma+i*DEV_BSIZE, DEV_BSIZE);
		}
	}
	return (n * DEV_BSIZE);
}

/* ARGSUSED */
hdioctl (io, cmd, arg)
struct iob *io;
int cmd;
caddr_t arg;
{
	return (ECMD);
}

int hdrw (chan, unit, rd, nblock, sz)
{
	if (nblock<0 || nblock+sz>NBLOCKS) {
		printf (MSG ("hd: bad block number %d:%d\n",
			"hd: плохой номер блока %d:%d\n"), nblock, sz);
		return (-1);
	}
	hdsetup (chan, unit, rd, nblock, sz);
	if (hdio (chan, hdprog) < 0)
		return (-1);
	return (0);
}

hdsetup (chan, unit, rd, nblock, sz)
{
	int c, h, b, i;

	/* формирование канальной программы */
	nprog = 0;
	for (i=0; i<sz; ++i, ++nblock) {
		c = nblock / BPC;       /* номер цилиндра */
		b = nblock - c * BPC;
		h = b / BPT;            /* номер головки */
		b = b - h * BPT + 1;    /* номер сектора */

		hdcyl [i] = c<<32 | h<<16 | b<<8;
		hdblock [i] = hdcyl [i] << 16;

		if (! i || (hdcyl[i] ^ hdcyl[i-1]) >> 16) {
			/* переход на следующий цилиндр */
			hdprog[nprog] = hdsample[0];
			hdprog[nprog++].addr = (int) &hdcyl[i];
		}
		hdprog[nprog] = hdsample[1];
		hdprog[nprog++].addr = (int) &hdblock[i];
		hdprog[nprog] = hdsample[2];
		hdprog[nprog].addr = (int) &hdprog[nprog-1];
		hdprog[++nprog] = hdsample[3];
		hdprog[nprog].addr = (int) hdbuf[i];
		hdprog[nprog++].cmd = rd ? 6 : 5;
	}

	/* установка номера устройства */
	hdunit = chantab [chan & 3] + unit;
	hdsense.unit = hdunit;
	for (i=0; i<nprog; ++i)
		hdprog[i].unit = hdunit;

	/* конец цепочки команд */
	hdprog[nprog-1].flg &= ~4;
}

int hdio (chan, prog)
struct sccmd *prog;
{
	int retry, csb;
	int caw, ccw;

	retry = 0;
repeat:
	hdcsw = scwait (chan, prog);
swtch:
	switch (hdcsw>>1 & 7) {
	case 2:         /* отказ канала */
		printf (MSG ("hd: unit not ready\n", "hd: устройство не готово\n"));
		break;
	case 3:         /* ош. адреса ВУ */
		printf (MSG ("hd: bad unit address\n", "hd: ош. адреса ВУ\n"));
		return (-1);
	case 4:         /* ВУ не найдено */
		printf (MSG ("hd: unit not found\n", "hd: устройство не найдено\n"));
		return (-1);
	case 6:         /* ош. чтения ОЗУ */
		printf (MSG ("hd: RAM error\n", "hd: ошибка ОЗУ\n"));
		return (-1);
	case 7:         /* переход в канале */
		printf (MSG ("hd: bad transfer\n", "hd: плохой переход\n"));
		return (-1);
	case 0:
		if ((hdcsw >> 32 & 0xff) != hdunit) {
badunit:                printf (MSG ("hd: bad unit in csw\n",
				"hd: плохой номер устройства в коде ответа\n"));
			break;
		}
		if (hdcsw & 0x1f80) {
badram:                 printf (MSG ("hd: RAM error\n",
				"hd: ошибка ОЗУ\n"));
			break;
		}
		csb = hdcsw >> 16 & 0xffff;
		if (csb == 0x0804) {
done:                   /* обнуление канала */
			_in_ (SC_CSW (chan) | SCA_RESET | SCA_NOHALT);
			return (0);
		}
		if (csb == 0x4a04) {
unitbusy:               /* обнуление канала */
			_in_ (SC_CCW (chan) | SCA_RESET|SCA_NOHALT|SCA_SBLOCK);
			goto repeat;
		}
		break;
	case 1:
		if ((hdcsw >> 24 & 0xff) != hdunit)
			goto badunit;
		if (hdcsw & 0x1f80)
			goto badram;
		csb = hdcsw >> 16 & 0xff;
		if (csb == 0x0c || csb == 0x08)
			goto done;
		if (csb == 0x4e)
			goto unitbusy;
		break;
	}
	ccw = _in_ (SC_CCW (hdchan) | SCA_NOHALT);              /* УСК */
	caw = _in_ (SC_CAW (hdchan) | SCA_NOHALT) & 0x7fffff;   /* АСК */
	_in_ (SC_CCW (chan) | SCA_RESET | SCA_NOHALT);          /* УСК */

	/* уточнение состояния */
	scwait (chan, &hdsense);

	/* обнуление канала */
	_in_ (SC_CSW (chan) | SCA_RESET | SCA_NOHALT);

# ifdef DEBUG
	printf ("\nУСК=%w  ССК=%w  АСК=%w\n", ccw, hdcsw, caw);
	printf ("БУС0=%w  БУС1=%w  БУС2=%w\n", hdsbuf[0], hdsbuf[1], hdsbuf[2]);
# endif
	if (retry > MAXRETRY) {
		printf (MSG ("hd: cannot recover by retry\n",
			"hd: повтор не помогает\n"));
		return (-1);
	}
	if ((hdsbuf [0] & 0xff) == 0x53) {
		printf (MSG ("hd: adjustable io error",
			"hd: корректируемая ошибка"));
# ifdef CORR
		/* коррекция */
		if (hdcsw = hdcorr (ccw, caw))
			goto swtch;
		goto done;
# endif
	} else
		printf (MSG ("hd: io error", "hd: ошибка обмена"));
	printf (MSG (", csw=%w, retrying\n", ", ССК=%w, повтор\n"), hdcsw);
	++retry;
	goto repeat;
}

# ifdef CORR
hdcorr (ccw, caw)
{
	/* коррекция */
	/* возвращает ССК или 0 */
	char *nb;
	register *p;
	int i;

	if (ccw >> 47 & 1) {
		/* был обмен с тегами */
		???
	}
	nb = (ccw & 0x7fffff) << 3 + (hdsbuf[2]>>48 & 0xffff) -
		(hdsbuf[2]>>32 & 0xffff);
	p = (int *) (nb >> 3);
	nb &= 7;
	for (i=0; i<3; ++i)
		*p ^= (hdsbuf[2] >> (24-i*8) & 0xff) << (56-nb*8);
	if (! (ccw >> 42 & 1))          /* команда последняя в цепочке */
		return (0);
	/* возврат в прерванную канальную программу */
	hdcorr[0].unit = hdunit;
	hdcorr[1].unit = hdunit;
	hdcorr[2].unit = hdunit;
	hdcorr[2].addr = caw;
	return (scwait (hdchan, hdcorr));
}
# endif
