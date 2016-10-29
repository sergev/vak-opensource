# include "param.h"
# include "inode.h"
# include "fs.h"

# include "saio.h"
# include "svsb.h"

# define K              1024
# define W              ((int) sizeof (int))
# define MEMSZ          (60*K*K)
# define MEMBEG         (4*K*K)
# define INFINITE       0xfffffff

static char *memaddr;

memopen (io)
register struct iob *io;
{
	if (! io->i_dsz)
		io->i_dsz = MEMSZ / DEV_BSIZE;
	memaddr = 0;
	memaddr += MEMBEG;
}

memstrategy (io, func)
register struct iob *io;
{
	/* io->i_ma - адрес в памяти */
	/* io->i_cc - длина блока в байтах */
	/* io->i_bn - номер блока */
	int n;

	if (io->i_bn < 0 || io->i_bn >= io->i_boff+io->i_dsz)
		return (0);
	n = io->i_cc / DEV_BSIZE;
	if (io->i_bn+n > io->i_boff+io->i_dsz)
		n = io->i_dsz - io->i_bn;
	n *= DEV_BSIZE;
	if (func == WRITE)
		bcopy (io->i_ma, memaddr + io->i_bn*DEV_BSIZE, n);
	else
		bcopy (memaddr + io->i_bn*DEV_BSIZE, io->i_ma, n);
	return (n);
}

zeroopen (io)
register struct iob *io;
{
	if (! io->i_dsz)
		io->i_dsz = INFINITE;
}

zerostrategy (io, func)
struct iob *io;
{
	/* io->i_ma - адрес в памяти */
	/* io->i_cc - длина блока в байтах */
	/* io->i_bn - номер блока */
	int n;

	if (io->i_bn < 0 || io->i_bn >= io->i_boff+io->i_dsz)
		return (0);
	n = io->i_cc / DEV_BSIZE;
	if (io->i_bn+n > io->i_boff+io->i_dsz)
		n = io->i_dsz - io->i_bn;
	n *= DEV_BSIZE;
	if (func == READ)
		bzero (io->i_ma, n);
	return (n);
}

nullstrategy (io, func)
struct iob *io;
{
	/* io->i_ma - адрес в памяти */
	/* io->i_cc - длина блока в байтах */
	/* io->i_bn - номер блока */
	int n;

	if (io->i_bn < 0 || io->i_bn >= io->i_boff+io->i_dsz)
		return (0);
	n = io->i_cc / DEV_BSIZE;
	if (io->i_bn+n > io->i_boff+io->i_dsz)
		n = io->i_dsz - io->i_bn;
	return (func==WRITE ? n*DEV_BSIZE : 0);
}
