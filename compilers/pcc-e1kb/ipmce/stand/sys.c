# include "param.h"
# include "inode.h"
# include "fs.h"
# include "dir.h"
# include "saio.h"
# include "svsb.h"

# define isdigit(n)	((n>='0') && (n<='9'))

struct dirstuff {
	int loc;
	struct iob *io;
};

int     lastdev = 0;    /* default unit is dos(0,0) */
int     lastunit;
int     lastoff;
int     lastdsz;
char    lastpname [40];

int     openinit;
int	errno;

ino_t dlook();
struct direct *rdir ();

static openi (n, io)
register struct iob *io;
{
	register struct dinode *dp;
	int cc;

	io->i_offset = 0;
	io->i_bn = fsbtodb(io->i_fs, itod(io->i_fs, n)) + io->i_boff;
	io->i_cc = io->i_fs->fs_bsize;
	io->i_ma = io->i_buf;
	cc = devread(io);
	dp = (struct dinode *)io->i_buf;
	io->i_ino.i_ic = dp[itoo(io->i_fs, n)].di_ic;
	return (cc);
}

static find (path, file)
register char *path;
struct iob *file;
{
	register char *q;
	char c;
	int n;

	if (path==NULL || *path=='\0') {
		printf (MSG ("null path\n", "пустой путь поиска\n"));
		return (0);
	}

	if (openi((ino_t) ROOTINO, file) < 0) {
		printf (MSG ("can't read root inode\n", "не могу прочитать корневой I-узел\n"));
		return (0);
	}
	while (*path) {
		while (*path == '/')
			path++;
		q = path;
		while(*q != '/' && *q != '\0')
			q++;
		c = *q;
		*q = '\0';
		if (q == path) path = "." ;	/* "/" means "/." */

		if ((n = dlook(path, file)) != 0) {
			if (c == '\0')
				break;
			if (openi(n, file) < 0)
				return (0);
			*q = c;
			path = q;
			continue;
		} else {
			printf (MSG ("%s: not found\n", "%s: не найден\n"), path);
			return (0);
		}
	}
	return (n);
}

static daddr_t sbmap (io, bn)
register struct iob *io;
daddr_t bn;
{
	register struct inode *ip;
	int i, j, sh;
	daddr_t nb, *bap;

	ip = &io->i_ino;
	if (bn < 0) {
		printf (MSG ("bn negative\n", "bn отрицательный\n"));
		return ((daddr_t)0);
	}

	/*
	 * blocks 0..NDADDR are direct blocks
	 */
	if(bn < NDADDR) {
		nb = ip->i_db[bn];
		return (nb);
	}

	/*
	 * addresses NIADDR have single and double indirect blocks.
	 * the first step is to determine how many levels of indirection.
	 */
	sh = 1;
	bn -= NDADDR;
	for (j = NIADDR; j > 0; j--) {
		sh *= NINDIR(io->i_fs);
		if (bn < sh)
			break;
		bn -= sh;
	}
	if (j == 0) {
		printf (MSG ("bn ovf %D\n", "переполнение bn %D\n"), bn);
		return ((daddr_t)0);
	}

	/*
	 * fetch the first indirect block address from the inode
	 */
	nb = ip->i_ib[NIADDR - j];
	if (nb == 0) {
		printf (MSG ("bn void %D\n", "bn пусто %D\n"), bn);
		return ((daddr_t)0);
	}

	/*
	 * fetch through the indirect blocks
	 */
	for (; j <= NIADDR; j++) {
		if (blknos[j] != nb) {
			io->i_bn = fsbtodb(io->i_fs, nb) + io->i_boff;
			io->i_ma = b[j];
			io->i_cc = io->i_fs->fs_bsize;
			if (devread(io) != io->i_fs->fs_bsize) {
				if (io->i_error)
					errno = io->i_error;
				printf (MSG ("bn %D: read error\n",
					"bn %D: ошибка чтения\n"), io->i_bn);
				return ((daddr_t)0);
			}
			blknos[j] = nb;
		}
		bap = (daddr_t *)b[j];
		sh /= NINDIR(io->i_fs);
		i = (bn / sh) % NINDIR(io->i_fs);
		nb = bap[i];
		if(nb == 0) {
			printf (MSG ("bn void %D\n", "bn пусто %D\n"), bn);
			return ((daddr_t)0);
		}
	}
	return (nb);
}

static ino_t dlook (s, io)
char *s;
register struct iob *io;
{
	register struct direct *dp;
	register struct inode *ip;
	struct dirstuff dirp;
	int len;

	if (s == NULL || *s == '\0')
		return (0);
	ip = &io->i_ino;
	if ((ip->i_mode&IFMT) != IFDIR) {
		printf (MSG ("%s: not a directory\n",
			"%s: не каталог\n"), s);
		return (0);
	}
	if (ip->i_size == 0) {
		printf (MSG ("%s: zero length directory\n",
			"%s: каталог нулевой длины\n"), s);
		return (0);
	}
	len = strlen(s);
	dirp.loc = 0;
	dirp.io = io;
	for (dp = rdir(&dirp); dp != NULL; dp = rdir(&dirp)) {
		if(dp->d_ino == 0)
			continue;
		if (dp->d_namlen == len && !strcmp(s, dp->d_name))
			return (dp->d_ino);
	}
	return (0);
}

/*
 * get next entry in a directory.
 */
static struct direct *rdir (dirp)
register struct dirstuff *dirp;
{
	register struct direct *dp;
	register struct iob *io;
	daddr_t lbn, d;
	int off;

	io = dirp->io;
	for(;;) {
		if (dirp->loc >= io->i_ino.i_size)
			return (NULL);
		off = blkoff(io->i_fs, dirp->loc);
		if (off == 0) {
			lbn = lblkno(io->i_fs, dirp->loc);
			d = sbmap(io, lbn);
			if(d == 0)
				return NULL;
			io->i_bn = fsbtodb(io->i_fs, d) + io->i_boff;
			io->i_ma = io->i_buf;
			io->i_cc = blksize(io->i_fs, &io->i_ino, lbn);
			if (devread(io) < 0) {
				errno = io->i_error;
				printf (MSG ("bn %D: directory read error\n",
					"bn %D: ошибка чтения каталога\n"),
					io->i_bn);
				return (NULL);
			}
		}
		dp = (struct direct *)(io->i_buf + off);
		dirp->loc += dp->d_reclen;
		if (dp->d_ino == 0)
			continue;
		return (dp);
	}
}

lseek (fdesc, addr, ptr)
int fdesc, ptr;
off_t addr;
{
	register struct iob *io;

	if (ptr != 0) {
		printf (MSG ("Seek not from beginning of file\n",
			"Seek не с начала файла\n"));
		errno = EOFFSET;
		return (-1);
	}
	fdesc -= 3;
	if (fdesc < 0 || fdesc >= NFILES ||
	    ((io = &iob[fdesc])->i_flgs & F_ALLOC) == 0) {
		errno = EBADF;
		return (-1);
	}
	if (io->i_flgs & F_NET)
		return (netlseek (io->i_ino.i_dev, io->i_unit, addr, ptr));
	io->i_offset = addr;
	io->i_bn = addr / DEV_BSIZE;
	io->i_cc = 0;
	return (0);
}

struct direct *readdir (d)
DIR *d;
{
	register struct iob *io;
	static struct direct dir;
	int fdesc;

	fdesc = d->dd_fd - 3;
	if (fdesc < 0 || fdesc >= NFILES ||
	    ((io = &iob[fdesc])->i_flgs & F_ALLOC) == 0) {
		errno = EBADF;
		return (0);
	}
	if (io->i_flgs & F_NET)
		if (netreaddir (io->i_ino.i_dev, io->i_unit, dir.d_name) > 0) {
			dir.d_ino = 0;
			dir.d_reclen = 0;
			dir.d_namlen = strlen (dir.d_name);
			return (&dir);
		} else
			return (0);
	printf (MSG ("Can't readdir on regular devices yet.. Sorry\n",
		"Читать каталоги на обычных устройствах пока нельзя.. Извините\n"));
	errno = EIO;
	return (0);
}

stat (fdesc, name, ptype, plen)
int fdesc;
char *name;
int *ptype, *plen;
{
	register struct iob *io;

	fdesc -= 3;
	if (fdesc < 0 || fdesc >= NFILES ||
	    ((io = &iob[fdesc])->i_flgs & F_ALLOC) == 0) {
		errno = EBADF;
		return (-1);
	}
	if (io->i_flgs & F_NET)
		return (netstat (io->i_ino.i_dev, io->i_unit, name, ptype, plen));
	printf (MSG ("Can't readdir on regular devices yet.. Sorry\n",
		"Читать каталоги на обычных устройствах пока нельзя.. Извините\n"));
	errno = EIO;
	return (-1);
}

getc (fdesc)
int fdesc;
{
	register struct iob *io;
	register struct fs *fs;
	register char *p;
	int c, lbn, off, size, diff;

	if (fdesc >= 0 && fdesc <= 2)
		return (getchar());
	fdesc -= 3;
	if (fdesc < 0 || fdesc >= NFILES ||
	    ((io = &iob[fdesc])->i_flgs&F_ALLOC) == 0) {
		errno = EBADF;
		return (-1);
	}
	if (io->i_flgs & F_NET) {
		char buf [1];

		if ((size = netread (io->i_ino.i_dev, io->i_unit, buf, 1)) <= 0)
			return (-1);
		return (buf [0]);
	}
	p = io->i_ma;
	if (io->i_cc <= 0) {
		if ((io->i_flgs & F_FILE) != 0) {
			diff = io->i_ino.i_size - io->i_offset;
			if (diff <= 0)
				return (-1);
			fs = io->i_fs;
			lbn = lblkno(fs, io->i_offset);
			io->i_bn = fsbtodb(fs, sbmap(io, lbn)) + io->i_boff;
			off = blkoff(fs, io->i_offset);
			size = blksize(fs, &io->i_ino, lbn);
		} else {
			io->i_bn = io->i_offset / DEV_BSIZE;
			off = 0;
			size = DEV_BSIZE;
		}
		io->i_ma = io->i_buf;
		io->i_cc = size;
		if (devread(io) < 0) {
			errno = io->i_error;
			return (-1);
		}
		if ((io->i_flgs & F_FILE) != 0) {
			if (io->i_offset - off + size >= io->i_ino.i_size)
				io->i_cc = diff + off;
			io->i_cc -= off;
		}
		p = &io->i_buf[off];
	}
	io->i_cc--;
	io->i_offset++;
	c = (unsigned)*p++;
	io->i_ma = p;
	return (c);
}

read (fdesc, buf, count)
int fdesc, count;
char *buf;
{
	register i, size;
	register struct iob *file;
	register struct fs *fs;
	int lbn, off;

	errno = 0;
	if (fdesc >= 0 & fdesc <= 2) {
		i = count;
		do {
			*buf = getchar();
		} while (--i && *buf++ != '\n');
		return (count - i);
	}
	fdesc -= 3;
	if (fdesc < 0 || fdesc >= NFILES ||
	    ((file = &iob[fdesc])->i_flgs&F_ALLOC) == 0) {
		printf (MSG ("Read: bad file descriptor %d\n",
			"Read: плохой дескриптор файла %d\n"), fdesc);
		errno = EBADF;
		return (-1);
	}
	if ((file->i_flgs&F_READ) == 0) {
		printf (MSG ("Read: permission denied\n",
			"Read: нет доступа на чтение\n"));
		errno = EBADF;
		return (-1);
	}
	if (file->i_flgs & F_NET)
		return (netread (file->i_ino.i_dev, file->i_unit, buf, count));
	if (! (file->i_flgs & F_FILE)) {
		file->i_cc = count;
		file->i_ma = buf;
		file->i_bn = file->i_boff + (file->i_offset / DEV_BSIZE);
		i = devread(file);
		file->i_offset += count;
		if (i < 0)
			errno = file->i_error;
		return (i);
	}
	if (file->i_offset+count > file->i_ino.i_size)
		count = file->i_ino.i_size - file->i_offset;
	if ((i = count) <= 0)
		return (0);
	/*
	 * While reading full blocks, do I/O into user buffer.
	 * Anything else uses getc().
	 */
	fs = file->i_fs;
	while (i) {
		off = blkoff(fs, file->i_offset);
		lbn = lblkno(fs, file->i_offset);
		size = blksize(fs, &file->i_ino, lbn);
		if (off == 0 && size <= i) {
			file->i_bn = fsbtodb(fs, sbmap(file, lbn)) +
			    file->i_boff;
			file->i_cc = size;
			file->i_ma = buf;
			if (devread(file) < 0) {
				errno = file->i_error;
				return (-1);
			}
			file->i_offset += size;
			file->i_cc = 0;
			buf += size;
			i -= size;
		} else {
			size -= off;
			if (size > i)
				size = i;
			i -= size;
			do {
				*buf++ = getc(fdesc+3);
			} while (--size);
		}
	}
	return (count);
}

void closedir (d)
DIR *d;
{
	close (d->dd_fd);
}

DIR *opendir (str)
char *str;
{
	static DIR d;
	int fd;

	if ((fd = opend (str, 0, 1)) < 0)
		return (0);
	d.dd_fd = fd;
	return (&d);
}

open (str, how)
char *str;
int how;
{
	return (opend (str, how, 0));
}

static opend (str, how, dir)
char *str;
int how;
{
	register char *cp;
	int i;
	register struct iob *file;
	int fdesc;
	int filemode;

	if (! openinit) {
		for (i=0; i<NFILES; ++i)
			iob[i].i_flgs = 0;
		iob[0].i_buf = iob_buf0;
		iob[1].i_buf = iob_buf1;
		iob[2].i_buf = iob_buf2;
		iob[3].i_buf = iob_buf3;
		iob[0].i_fs = &iob_fs0.fs;
		iob[1].i_fs = &iob_fs1.fs;
		iob[2].i_fs = &iob_fs2.fs;
		iob[3].i_fs = &iob_fs3.fs;
		openinit = 1;
	}

	for (fdesc=0; fdesc<NFILES; ++fdesc)
		if (iob[fdesc].i_flgs == 0)
			goto gotfile;
	_stop (MSG ("No more file slots", "Слишком много открытых файлов"));
gotfile:
	(file = &iob[fdesc])->i_flgs |= F_ALLOC;

	for (cp = str; *cp && *cp != '('; cp++)
			;
	if (*cp == '(') {
		if (getdev (str, &file->i_ino.i_dev, &file->i_unit,
		    &file->i_boff, &file->i_dsz, file->i_partname) < 0) {
			file->i_flgs = 0;
			return (-1);
		}
		for (cp = str; *cp && *cp != ')'; cp++);
		filemode = *cp == ')' && *++cp;
	} else {
		if (lastdev < 0) {
			printf (MSG ("Unknown device\n",
				"Неизвестное устройство\n"));
			file->i_flgs = 0;
			errno = ENXIO;
			return (-1);
		}
		file->i_ino.i_dev = lastdev;
		file->i_unit = lastunit;
		file->i_boff = lastoff;
		file->i_dsz = lastdsz;
		strcpy (file->i_partname, lastpname);
		cp = str;
		filemode = 1;
	}
	if (devsw[file->i_ino.i_dev].dv_net) {
		file->i_flgs |= (how+1) | F_NET;
		if (dir)
			i = netopendir (file->i_ino.i_dev, cp);
		else
			i = netopen (file->i_ino.i_dev, cp, how);
		if (i < 0) {
			file->i_flgs = 0;
			return (-1);
		}
		file->i_unit = i;
		return (fdesc+3);
	}
	if (dir) {
		printf (MSG ("Can't opendir on regular devices yet.. Sorry\n",
			"Opendir на обычных устройствах пока нет.. Извините\n"));
		errno = EIO;
		return (-1);
	}
	file->i_flgs |= how+1;
	file->i_partname [0] = 0;       /* временно */
	devopen(file);
	if (! filemode) {
		file->i_cc = 0;
		file->i_offset = 0;
		return (fdesc+3);
	}
	file->i_ma = (char *)(file->i_fs);
	file->i_cc = SBSIZE;
	file->i_bn = SBLOCK + file->i_boff;
	file->i_offset = 0;
	if (devread(file) < 0) {
		errno = file->i_error;
		printf (MSG ("super block read error\n",
			"ошибка чтения суперблока\n"));
		return (-1);
	}
	if ((i = find(cp, file)) == 0) {
		file->i_flgs = 0;
		errno = ESRCH;
		return (-1);
	}
	if (how != 0) {
		printf (MSG ("Can't write files yet.. Sorry\n",
			"Файлы писать пока нельзя.. Извините\n"));
		file->i_flgs = 0;
		errno = EIO;
		return (-1);
	}
	if (openi(i, file) < 0) {
		errno = file->i_error;
		return (-1);
	}
	file->i_offset = 0;
	file->i_cc = 0;
	file->i_flgs |= F_FILE | (how+1);
	return (fdesc+3);
}

static getdev (str, pdev, punit, poff, psz, pname)
char *str, *pname;
int *pdev, *punit, *psz;
register *poff;
{
	register char *cp;
	register struct devsw *dp;
	int sav;

	/* dev ( [ unit [ , boff [ : size ] ] ] ) */
	for (cp=str; *cp && *cp!='('; ++cp);
	sav = *cp;
	*cp = 0;
	for (dp=devsw; dp->dv_name; ++dp)
		if (!strcmp (str, dp->dv_name) || !strcmp (str, dp->dv_rname))
			goto gotdev;
	printf (MSG ("Unknown device\n", "Неизвестное устройство\n"));
	errno = ENXIO;
	return (-1);
gotdev:
	*cp = sav;
	*pdev = dp - devsw;
	*punit = 0;
	*poff = 0;
	*psz = 0;
	*pname = 0;
	if (*cp == 0)
		return (0);
	if (*cp != '(')
		goto baddev;
	++cp;
	if (*cp >= '0' && *cp <= '9') {
		while (*cp >= '0' && *cp <= '9')
			*punit = *punit * 10 + *cp++ - '0';
		if (*punit < 0 || *punit > 63) {
			printf (MSG ("Bad unit specifier\n",
				"Плохой номер устройства\n"));
			errno = EUNIT;
			return (-1);
		}
	}
	if (*cp == ',') {
		++cp;
		while (*cp >= '0' && *cp <= '9')
			*poff = *poff * 10 + *cp++ - '0';
	}
	if (*cp == '/') {
		++cp;
		while (*cp && *cp != ':' && *cp != ')')
			*pname++ = *cp++;
		*pname = 0;
	}
	if (*cp == ':') {
		++cp;
		while (*cp >= '0' && *cp <= '9')
			*psz = *psz * 10 + *cp++ - '0';
	}
	if (*cp == ')')
		return (0);
baddev:
	printf (MSG ("Bad device\n", "Плохое имя устройства\n"));
	errno = EDEV;
	return (-1);
}

close (fdesc)
int fdesc;
{
	struct iob *file;

	fdesc -= 3;
	if (fdesc < 0 || fdesc >= NFILES ||
	    ((file = &iob[fdesc])->i_flgs&F_ALLOC) == 0) {
		errno = EBADF;
		return (-1);
	}
	if (file->i_flgs & F_NET) {
		file->i_flgs = 0;
		return (netclose (file->i_ino.i_dev, file->i_unit));
	}
	if ((file->i_flgs&F_FILE) == 0)
		devclose(file);
	file->i_flgs = 0;
	return (0);
}

exit ()
{
	_stop (MSG ("Exit called", "Вызван exit"));
}

_stop (s)
char *s;
{
	int i;

	for (i = 0; i < NFILES; i++)
		if (iob[i].i_flgs != 0)
			close(i);
	printf("%s\n", s);
	_rtt();
}

chdev (name)
char *name;
{
	int dev, unit, boff, dsz;
	char pname [40];

	if (! name || !*name) {
		if (lastdev >= 0)
			printf ("%s(%d,%d)\n", MSG (devsw [lastdev].dv_name,
				devsw [lastdev].dv_rname),
				lastunit, lastoff);
		else
			printf (MSG ("No current device\n",
				"Нет текущего устройства\n"));
		return;
	}
	if (getdev (name, &dev, &unit, &boff, &dsz, pname) < 0)
		return;
	lastdev = dev;
	lastunit = unit;
	lastoff = boff;
	lastdsz = dsz;
	strcpy (lastpname, pname);
	decode (lastpname);
}

write (fdesc, buf, count)
int fdesc, count;
char *buf;
{
	register i;
	register struct iob *file;

	errno = 0;
	if (fdesc >= 0 && fdesc <= 2) {
		i = count;
		while (i--)
			putchar(*buf++);
		return (count);
	}
	fdesc -= 3;
	if (fdesc < 0 || fdesc >= NFILES ||
	    ((file = &iob[fdesc])->i_flgs&F_ALLOC) == 0) {
		printf (MSG ("Write: bad file descriptor %d\n",
			"Write: плохой дескриптор файла %d\n"), fdesc);
		errno = EBADF;
		return (-1);
	}
	if (! (file->i_flgs & F_WRITE)) {
		printf (MSG ("Write: permission denied\n",
			"Write: нет доступа на запись\n"));
		errno = EBADF;
		return (-1);
	}
	if (file->i_flgs & F_FILE) {
		printf (MSG ("Can't write files yet.. Sorry\n",
			"Файлы писать пока нельзя.. Извините\n"));
		errno = EIO;
		return (-1);
	}
	file->i_cc = count;
	file->i_ma = buf;
	file->i_bn = file->i_boff + (file->i_offset / DEV_BSIZE);
	i = devwrite (file);
	file->i_offset += count;
	if (i < 0)
		errno = file->i_error;
	return (i);
}

decode (s)
char *s;
{
	/* перекодировка \123 в символы */
	char *p;
	int c;

	for (p=s; *s; ++s) {
		if (*s == '\\') {
			if (! *++s)
				break;
			if (*s>='0' && *s<='3') {
				c = *s - '0';
				if (s[1]>='0' && s[1]<='7')
					c = c<<3 | (*++s - '0');
				if (s[1]>='0' && s[1]<='7')
					c = c<<3 | (*++s - '0');
				*p++ = c;
				continue;
			}
		}
		*p++ = *s;
	}
	*p = 0;
}

dodev (p, cmd)
char *p, *cmd;
{
	register struct devsw *d;
	int lat;

	if (! p || ! *p) {
		lat = ! strcmp (cmd, "dev");
		for (d=devsw; d->dv_name; ++d)
			printf ("%s\t%s\n", lat ? d->dv_name : d->dv_rname,
				MSG (d->dv_help, d->dv_rhelp));
		printf (MSG ("\nCurrent device: ","\nТекущее устройство: "));
	}
	chdev (p);
}
