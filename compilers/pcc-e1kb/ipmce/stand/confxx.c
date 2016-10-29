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

extern xxopen (), xxclose (), xxread (), xxwrite (),
	xxlseek (), xxopendir (), xxreaddir (), xxstat ();

extern xxstrategy (), xxioctl ();

struct devsw devsw [] = {
	{
	"XX", "XX", 1,
	"XX device",
	"устройство XX",
		xxstrategy, xxopen, xxclose, xxioctl,
		xxread, xxwrite, xxlseek, xxopendir, xxreaddir, xxstat,
	},
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
};
