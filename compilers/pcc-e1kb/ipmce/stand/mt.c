# include "param.h"
# include "inode.h"
# include "fs.h"

# include "saio.h"
# include "scio.h"
# include "svsb.h"

# include "mtreg.h"

# define MAXRETRY 5
# define INFINITE 0xfffffff

# define WORDSZ ((int) sizeof (int))

/* # define DEBUG */

# define ERROK          0
# define ERRHARD        1
# define ERRMARKER      2
# define ERRRETRY       3

int mtchan = 3;                 /* номер ленточного канала */
int mtsense [1];
int mtbuf [2048];               /* макс. длина блока на ленте 16k */
int mtunit;
int mtlen;

extern chantab [];

mtopen (io)
struct iob *io;
{
	mtinit (io, 0);
}

rmtopen (io)
struct iob *io;
{
	mtinit (io, 1);
}

mtclose (io)
register struct iob *io;
{
	mtend (io, 0);
}

rmtclose (io)
register struct iob *io;
{
	mtend (io, 1);
}

mtinit (io, rewflg)
register struct iob *io;
{
	register skip;

	if (io->i_partname [0])
		printf (MSG ("mt: cannot handle partitions\n",
			"mt: разделы не реализованы\n"));
	if (! io->i_dsz)
		io->i_dsz = INFINITE;

	screset (mtchan);

	if (rewflg) {
		/* rewind tape */
		mtioctl (io, MT_REW, 0);

		/* units 10-17 are 800 bpi */
		/* units 0-7 are 1600 bpi */
		if (io->i_unit >= 10) {
			/* set 800 bpi */
			mtioctl (io, MT_D32, 0);
			io->i_unit %= 10;
		} else
			/* set 1600 bpi */
			mtioctl (io, MT_D63, 0);

		if ((io->i_flgs & F_WRITE) && ! io->i_boff)
			/* erase some space at the beginning of tape */
			mtioctl (io, MT_ERG, 0);
	}
	/* skip boff files */
	skip = io->i_boff;
	while (--skip >= 0)
		mtioctl (io, MT_FSF, 0);
}

mtend (io, rewflg)
register struct iob *io;
{
	if (io->i_flgs & F_WRITE) {
		/* if tape was written */
		/* then write two tape markers */
		mtioctl (io, MT_WTM, 0);
		mtioctl (io, MT_WTM, 0);
		if (rewflg) {
			/* rewind */
			mtioctl (io, MT_REW, 0);
		} else
			mtioctl (io, MT_BSB, 0);
	} else if (rewflg) {
		/* rewind */
		mtioctl (io, MT_REW, 0);
	}
}

/* ARGSUSED */
mtioctl (io, cmd, arg)
struct iob *io;
caddr_t arg;
{
	if (mtio (io->i_unit, cmd, (int *) 0, 0) == ERRHARD) {
		printf (MSG ("mt: error on op %d\n",
			"mt: ошибка при операции %d\n"), cmd);
		return (EIO);
	}
	return (0);
}

mtstrategy (io, func)
register struct iob *io;
{
	/* io->i_ma - адрес в памяти */
	/* io->i_cc - длина блока в байтах */
	/* io->i_bn - номер блока */
	int op, len;

	if (io->i_bn < 0 || io->i_bn+io->i_cc/1024 > io->i_dsz)
		return (0);
	if (io->i_cc % WORDSZ) {
		printf (MSG ("mt: bad block length %d\n",
			"mt: плохая длина блока %d\n"), io->i_cc);
		return (-1);
	}
	if (func == WRITE) {
		bcopy (io->i_ma, mtbuf, io->i_cc);
		inverbuf (mtbuf, io->i_cc / WORDSZ);
	}
	if (func == WRITE) {
		op = MT_WRITE;
		len = io->i_cc;
	} else {
		op = MT_READ;
		len = 32*1024;
	}
	switch (mtrw (io->i_unit, op, len)) {
	case ERROK:             /* ok */
		break;
	case ERRHARD:           /* фатальная ошибка */
		return (-1);
	case ERRMARKER:         /* EOF */
		return (0);
	}
	if (func == READ) {
# ifdef DEBUG
		printf ("***** mtlen = %d\n", mtlen);
# endif
		if (mtlen > io->i_cc)
			mtlen = io->i_cc;
		inverbuf (mtbuf, (mtlen+WORDSZ-1) / WORDSZ);
		bcopy (mtbuf, io->i_ma, mtlen);
		return (mtlen);         /* <= io->i_cc */
	} else
		return (io->i_cc);
}

mtrw (unit, op, len)
{
	int errcnt, ret;

	errcnt = 0;
retry:
	if ((ret = mtio (unit, op, mtbuf, len)) != ERRRETRY)
		return (ret);
	/* r/w error */
	if (++errcnt > MAXRETRY) {
		printf (MSG ("mt: unrecovered error\n",
			"mt: фатальная ошибка\n"));
		return (ERRHARD);
	}
	mtio (unit, MT_SENSE, mtsense, 5);
	/* skip bad block backward */
	mtio (unit, MT_BSB, (int *) 0, 0);
	/* erase some space */
	if (op == MT_WRITE)
		mtio (unit, MT_ERG, (int *) 0, 0);
	goto retry;
}

mtio (unit, op, addr, len)
int *addr;
{
	int csw, csb, x, ccw;

	mtunit = chantab [mtchan] + unit;
	csw = scio (mtchan, unit, op, addr, len, 0);
	ccw = _in_ (SC_CCW (mtchan) | SCA_RESET | SCA_NOHALT);
	x = csw>>1 & 7;
	switch (x) {
	case 2:         /* отказ канала */
		printf (MSG ("mt: unit not ready\n", "mt: устройство не готово\n"));
badcsw:
		printf (MSG ("csw=%w\n", "ССК=%w\n"), csw);
		return (ERRHARD);
	case 3:         /* ош. адреса ВУ */
		printf (MSG ("mt: bad unit address\n", "mt: ош. адреса ВУ\n"));
		goto badcsw;
	case 4:         /* ВУ не найдено */
		printf (MSG ("mt: unit not found\n", "mt: устройство не найдено\n"));
		goto badcsw;
	case 6:         /* ош. чтения ОЗУ */
		printf (MSG ("mt: RAM error\n", "mt: ошибка ОЗУ\n"));
		goto badcsw;
	case 7:         /* переход в канале */
		printf (MSG ("mt: bad transfer\n", "mt: плохой переход\n"));
		goto badcsw;
	case 0:
		if ((csw >> 32 & 0xff) != mtunit) {
badunit:                printf (MSG ("mt: bad unit in csw\n",
				"mt: плохой номер устройства в коде ответа\n"));
			goto badcsw;
		}
		if (csw & 0x1f80) {
badram:                 printf (MSG ("mt: RAM error\n",
				"mt: ошибка ОЗУ\n"));
			goto badcsw;
		}
		csb = csw >> 24 & 0xf;
		break;
	case 1:
		if ((csw >> 24 & 0xff) != mtunit)
			goto badunit;
		if (csw & 0x1f80)
			goto badram;
		csb = csw >> 16 & 0xf;
		break;
	}
	if (csb == 0xc || csb == 0x8) {
		mtlen = ((ccw & 0x7fffff) - (int) addr) << 3;
		if (x = (csw>>13 & 7))
			mtlen += x-8;
		return (ERROK);
	}
	if (csb & 1)            /* EOF */
		return (ERRMARKER);
	if (csb & 2)            /* r/w error */
		return (ERRRETRY);
	printf (MSG ("mt: io error\n", "mt: ошибка обмена\n"));
	goto badcsw;
}
