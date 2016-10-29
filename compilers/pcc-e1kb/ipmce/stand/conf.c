# include "param.h"
# include "inode.h"
# include "fs.h"

# include "saio.h"
# include "svsb.h"

devread(io)
register struct iob *io;
{
	int cc;

	io->i_flgs |= F_RDDATA;
	io->i_error = 0;
	cc = (*devsw[io->i_ino.i_dev].dv_strategy)(io, READ);
	io->i_flgs &= ~F_TYPEMASK;
	return (cc);
}

devwrite(io)
register struct iob *io;
{
	int cc;

	io->i_flgs |= F_WRDATA;
	io->i_error = 0;
	cc = (*devsw[io->i_ino.i_dev].dv_strategy)(io, WRITE);
	io->i_flgs &= ~F_TYPEMASK;
	return (cc);
}

devopen(io)
register struct iob *io;
{
	(*devsw[io->i_ino.i_dev].dv_open)(io);
}

devclose(io)
register struct iob *io;
{
	(*devsw[io->i_ino.i_dev].dv_close)(io);
}

netread (dev, fd, buf, len)
char *buf;
{
	int cc;

	cc = (*devsw [dev].dv_read) (fd, buf, len);
	if (cc < 0) {
		printf (MSG ("read error on dev %d\n",
			"ошибка чтения на устройстве %d\n"), dev);
		errno = EIO;
		return (-1);
	}
	return (cc);
}

netstat (dev, fd, name, ptype, plen)
char *name;
int *ptype, *plen;
{
	int cc;

	cc = (*devsw [dev].dv_stat) (fd, name, ptype, plen);
	if (cc < 0) {
		printf (MSG ("stat '%s' error on dev %d\n",
			"ошибка stat '%s' на устройстве %d\n"), name, dev);
		errno = EIO;
		return (-1);
	}
	return (cc);
}

netreaddir (dev, fd, buf)
char *buf;
{
	int cc;

	cc = (*devsw [dev].dv_readdir) (fd, buf);
	if (cc < 0) {
		printf (MSG ("readdir error on dev %d\n",
			"ошибка чтения каталога на устройстве %d\n"), dev);
		errno = EIO;
		return (-1);
	}
	return (cc);
}

netwrite (dev, fd, buf, len)
char *buf;
{
	int cc;

	cc = (*devsw [dev].dv_write) (fd, buf, len);
	if (cc < 0) {
		printf (MSG ("write error on dev %d\n",
			"ошибка записи на устройстве %d\n"), dev);
		errno = EIO;
		return (-1);
	}
	return (cc);
}

netopen (dev, name, mode)
char *name;
{
	int cc;

	cc = (*devsw [dev].dv_open) (name, mode);
	if (cc < 0) {
		printf (MSG ("cannot open file '%s' on dev %d\n",
			"не могу открыть файл '%s' на устройстве %d\n"),
			name, dev);
		errno = EIO;
		return (-1);
	}
	return (cc);
}

netopendir (dev, name)
char *name;
{
	int cc;

	cc = (*devsw [dev].dv_opendir) (name);
	if (cc < 0) {
		printf (MSG ("cannot open directory '%s' on dev %d\n",
			"не могу открыть каталог '%s' на устройстве %d\n"),
			name, dev);
		errno = EIO;
		return (-1);
	}
	return (cc);
}

netclose (dev, fd)
{
	int cc;

	cc = (*devsw [dev].dv_close) (fd);
	if (cc < 0) {
		printf (MSG ("cannot close file %d on dev %d\n",
			"не могу закрыть файл %d на устройстве %d\n"),
			fd, dev);
		errno = EIO;
		return (-1);
	}
	return (cc);
}

netlseek (dev, fd, addr, ptr)
{
	int cc;

	cc = (*devsw [dev].dv_seek) (fd, addr, ptr);
	if (cc < 0) {
		printf (MSG ("cannot lseek file %d on dev %d\n",
			"не могу lseek файл %d на устройстве %d\n"),
			fd, dev);
		errno = EIO;
		return (-1);
	}
	return (cc);
}

devioctl(io, cmd, arg)
register struct iob *io;
int cmd;
caddr_t arg;
{
	return ((*devsw[io->i_ino.i_dev].dv_ioctl)(io, cmd, arg));
}

null () {}

nullioctl () { return (ECMD); }

extern dosopen (), dosclose (), nullioctl (), dosread (), doswrite (),
	doslseek (), dosopendir (), dosreaddir (), dosstat ();

extern hdstrategy (), hdopen (), hdioctl ();

extern hpstrategy (), hpopen (), hpioctl ();

extern nullstrategy (), zerostrategy (), zeroopen ();

extern memstrategy (), memopen ();

extern mtstrategy (), mtopen (), mtclose (), mtioctl ();
extern rmtopen (), rmtclose ();

struct devsw devsw [] = {
	{
	"dos", "дос", 1,
	"dos file system",
	"файловая система ДОС",
		null, dosopen, dosclose, null,
		dosread, doswrite, doslseek, dosopendir, dosreaddir, dosstat,
	},
	{
	"hd", "дк", 0,
	"hard disk 200 Mb 1k on 0 sc",
	"ЕС-диск 200 МБ блок 1 К на 0 СК",
		hdstrategy, hdopen, null, hdioctl,
		null, null, null, null, null, null,
	},
	{
	"hp", "дт", 0,
	"hard disk 200 Mb 4k on 0 sc",
	"ЕС-диск 200 МБ блок 4 К на 0 СК",
		hpstrategy, hpopen, null, hpioctl,
		null, null, null, null, null, null,
	},
	{
	"mt", "лт", 0,
	"magnetic tape on 3 sc",
	"магнитная лента на 3 СК",
		mtstrategy, mtopen, mtclose, mtioctl,
		null, null, null, null, null, null,
	},
	{
	"rmt", "плт", 0,
	"magnetic tape, rewind on open\n",
	"магнитная лента, перемотка в начале",
		mtstrategy, rmtopen, mtclose, mtioctl,
		null, null, null, null, null, null,
	},
	{
	"mtr", "лтп", 0,
	"magnetic tape, rewind on close\n",
	"магнитная лента, перемотка в конце",
		mtstrategy, mtopen, rmtclose, mtioctl,
		null, null, null, null, null, null,
	},
	{
	"rmtr", "плтп", 0,
	"magnetic tape, rewind on open and close\n",
	"магнитная лента, перемотка в начале и в конце",
		mtstrategy, rmtopen, rmtclose, mtioctl,
		null, null, null, null, null, null,
	},
	{
	"null", "пуст", 0,
	"null device",
	"пустое устройство",
		nullstrategy, null, null, nullioctl,
		null, null, null, null, null, null,
	},
	{
	"zero", "ноль", 0,
	"zeros on read",
	"нули при чтении",
		zerostrategy, zeroopen, null, nullioctl,
		null, null, null, null, null, null,
	},
	{
	"mem", "пам", 0,
	"memory 60 Mb",
	"память 60 МБ",
		memstrategy, memopen, null, nullioctl,
		null, null, null, null, null, null,
	},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
};
