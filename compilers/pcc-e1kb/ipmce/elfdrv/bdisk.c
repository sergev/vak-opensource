/*
 * Работа с дисками PC через байтовый канал и интерфейс ЭЛЬФ.
 */

/*$Header:$*/

/*$Log:$*/

#include <sys/param.h>
#include <sys/devconf.h>
#include <sys/systm.h>
#include <sys/buf.h>
#include <sys/conf.h>
#include <sys/user.h>
#include <sys/statistic.h>
#include <sys/interface.h>
#include <sys/bdconf.h>
#include <sys/bd.h>

/* Преобразование номера устройства в номер подканала и наоборот */

#define BDSUBCHAN(unit)         ((unit) + 1)
#define BDUNIT(subchan)         ((subchan) - 1)

/* Флаги состояния устройства */

#define OPEN            1       /* Устройство уже открыто */
#define WSTATUS         2       /* Опрашиваем статус (длину) устройства */
#define STWANTED        4       /* Дай wakeup после опроса статуса */

/* Состояние ожидания */

enum waitstate {                /* Состояния ожидания приемной части */
	WAITNOTNING,            /* Ничего не ждем */
	WAITSTATUS,             /* Ждем статус */
	WAITDATA,               /* Ждем данные */
};

struct bdrec {
	int             flags;          /* Флаги состояния устройства */
	int             dev;            /* Полный номер открытого устройства */
	int             size;           /* Длина устройства в блоках */
	int             active;         /* Активность передающей части */
	enum waitstate  state;          /* Состояние ожидания приемной части */
	struct buf      *recvbuf;       /* Принимаемый буфер при WAITDATA */
};

struct bdrec bdrec [NBDISK];
struct buf bdtab [NBDISK];
struct buf rbdbuf [NBDISK];

#ifdef BD_STATISTIC
struct bd_info bdinf;
int bd_begin [NBDISK];

# define IODONE(x) doneio (x)
#else
# define IODONE(x) (x->b_error = 0; iodone (x))
#endif /* BD_STATISTIC */

int bdstrategy ();
int bdrintr ();
int bdsintr ();

bdopen (dev, rws)
register dev_t dev;
register int rws;
{
	register int unit;
	register struct buf *dp;
	register struct bdrec *rp;
	int x;

	unit = minor (dev) & 7;
	if (unit >= NBDISK) {
		u.u_error = ENXIO;
		return;
	}

	dp = &bdtab [unit];
	rp = &bdrec [unit];

	x = spl4 ();

	/* Если кто-то уже пытается опрашивать статус, ждем освобождения */

	if (rp->flags & WSTATUS) {
		rp->flags |= STWANTED;
		while (rp->flags & WSTATUS)
			sleep (rp, PWAIT);
	}

	/* Если устройство уже открыто */

	if (rp->flags & OPEN) {
		/* Нельзя менять минор на ходу */
		if (dev != rp->dev)
			u.u_error = EBUSY;
		splx (x);
		return;
	}
	/* Захватили устройство, будем опрашивать статус */
	rp->flags |= WSTATUS;

	/* Установка подканала */

	rp->active = 0;
	bcsetup (BDSUBCHAN (unit), bdsintr, bdrintr);

	/* Опрашиваем статус */

	bcsend (BDSUBCHAN (unit), CGETSTATUS, minor (dev) >> 3, 0, 0);

	/*
	 * По приходу пакета CGETSTATUS в состоянии WAITSTATUS
	 * функция bdrintr заполнит поле .size и сделает
	 * wakeup на rp->size.
	 */

	rp->state = WAITSTATUS;
	sleep (&rp->size, PWAIT);

	if (! rp->size) {
		/* Нет такого устройства */
		u.u_error = ENXIO;
		goto out;
	}

	/* Устройство открыто */

	rp->dev = dev;
	dp->b_dev = dev;
	dp->b_actf = 0;
	dp->b_actl = 0;
	rp->recvbuf = 0;
	rp->flags |= OPEN;
out:
	rp->flags &= ~WSTATUS;

	if (rp->flags & STWANTED) {
		rp->flags &= ~STWANTED;
		wakeup (rp);
	}
	splx (x);
}

bdclose (dev)
register dev_t dev;
{
	register struct bdrec *rp;
	register int unit;

	unit = minor (dev) & 7;
	rp = &bdrec [unit];

	if (! (rp->flags & OPEN))
		return;

	/* Сброс подканала */
	bcsetup (BDSUBCHAN (unit), 0, 0);
	rp->flags = 0;
}

bdread (dev)
dev_t dev;
{
	physio (bdstrategy, &rbdbuf [minor (dev) & 7], dev, B_READ);
}

bdwrite (dev)
dev_t dev;
{
	physio (bdstrategy, &rbdbuf [minor (dev) & 7], dev, B_WRITE);
}

bdstrategy (bp)
register struct buf *bp;
{
	register int unit;
	register struct bdrec *rp;
	register struct buf *dp;
	int x;

	unit = minor (bp->b_dev) & 7;
	rp = &bdrec [unit];
	dp = &bdtab [unit];
	if (unit >= NBDISK || bp->b_blkno < 0 || bp->b_blkno >= rp->size) {
		bp->b_flags |= B_ERROR;
		bp->b_error = 0;
		iodone (bp);
		return;
	}

	x = spl4 ();

	/* Ставим блок в очередь устройства */

	bp->av_forw = 0;
	if (! dp->b_actf) {
		/* Очередь была пуста, ставим блок в начало */
		dp->b_actf = bp;
		dp->b_actl = bp;
	} else {
		/* Добавляем блок в конец очереди */
		dp->b_actl->b_actf = bp;
		dp->b_actl = bp;
	}
#ifdef BD_STATISTIC
	{
	int qlen;

	/* Подсчитываем длину очереди */
	qlen = 0;
	for (bp=dp->b_actf; bp; bp=bp->b_actf)
		++qlen;
	if (qlen > bdinf.r_maxqueue)
		bdinf.r_maxqueue = qlen;
	bp->b_error = mtime ();
	bdinf.r_lcount++;
	}
#endif /* BD_STATISTIC */

	/* Активизируем устройство */

	if (! rp->active)
		bdstart (rp);
	splx (x);
}

bdstart (rp)
register struct bdrec *rp;
{
	register struct buf *dp;
	register struct buf *bp;
	register int unit;

	/* Берем блок из очереди и обрабатываем его */
	unit = minor (rp->dev);
	dp = &bdtab [unit];
	bp = dp->b_actf;
	if (! bp)
		return;
	rp->active = 1;

	/*
	 * Пускаем подканал. Когда канал кончит,
	 * будет вызван bdsintr, который сам поймет, что делать дальше.
	 */

	if (bp->b_flags & B_READ)
		bcsend (BDSUBCHAN (unit), CREADBLOCK, bp->b_blkno, 0, 0);
	else
		bcsend (BDSUBCHAN (unit), CWRITEBLOCK, bp->b_blkno,
			(bp->b_bcount + 7) / 8,
			(int *) bp->b_un.b_addr);

#ifdef BD_STATISTIC
	bd_begin [unit] = mtime ();
	bdinf.r_pcount++;
#endif /* BD_STATISTIC */
}

bdsintr (subchan)
{
	register struct bdrec *rp;
	register struct buf *dp;
	register struct buf *bp;
	int unit;

	unit = BDUNIT (subchan);
	if (unit<0 || unit >= NBDISK)
		return;
	rp = &bdrec [unit];
	if (! rp->active)
		return;
	dp = &bdtab [unit];

	rp->active = 0;
	bp = dp->b_actf;
	dp->b_actf = bp->av_forw;

	if (bp->b_flags & B_READ) {
		if (rp->state != WAITNOTHING)
			printf ("Потеря блока при чтении!\n);
		rp->recvbuf = bp;
		rp->state = WAITDATA;
	} else {
		bp->b_resid = 0;
		IODONE (bp);
	}
start:
#ifdef BD_STATISTIC
	{
	time_t iotime;
	iotime = mtime () - bd_begin [dn];
	if (bdinf.r_dmaxtime < iotime)
	    bdinf.r_dmaxtime = iotime;
	bdinf.r_dtottime += iotime;
	}
#endif /* BD_STATISTIC */
	bdstart (rp);
}

bdrintr (subchan, op, info, len, data)
int *data;
{
	register struct bdrec *rp;
	register struct buf *bp;
	int unit;

	unit = BDUNIT (subchan);
	if (unit<0 || unit >= NBDISK)
		return;
	rp = &bdrec [unit];

	switch (rp->state) {
	case WAITNOTHING:
		break;

	case WAITSTATUS:
		if (op != CGETSTATUS)
			printf ("Плохой ответ на запрос статуса: %d\n", op);
		if (len)
			printf ("Странная длина пакета статуса: %d\n", len);
		rp->size = info;
		wakeup (&rp->size);
		break;

	case WAITDATA:
		bp = rp->recvbuf;

		if (! len)
			printf ("Пришел пакет данных 0-й длины\n");
		if (len != (bp->b_bcount + 7) / 8)
			printf ("Пришел пакет данных неверной длины: ждем %d, получили %d\n",
				(bp->b_bcount + 7) / 8, len);

		/* Заполняем буфер */
		bcopy ((char *) data, bp->b_un.b_addr, bp->b_bcount);

		bp->b_resid = 0;
		IODONE (bp);
		break;
	}
	rp->state = WAITNOTHING;
}

#ifdef BD_STATISTIC

doneio (bp)
register struct buf *bp;
{
	time_t iotime;

	iotime = mtime () - bp->b_error;
	if (bdinf.r_maxtime < iotime)
	    bdinf.r_maxtime = iotime;
	bdinf.r_tottime += iotime;
	bp->b_error = 0;
	iodone (bp);
}

#endif /* BD_STATISTIC */
