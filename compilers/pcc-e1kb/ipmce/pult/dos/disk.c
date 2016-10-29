# include <sys\stat.h>
# include <dos.h>
# include <dir.h>
# include <fcntl.h>
# include <string.h>
# include "dosio.h"

/* # define DEBUG */

static char dirname [100];
static firstdir;
static struct ffblk fb;

static char *readdir ();

extern char *malloc ();

dodisk ()
{
	long l, r;
	struct dosio io;
	struct dosreply reply;

	/* exec fileio */
	ptload (&l, &r, DOSADDR);
	if (! r)
		return;

# ifdef DEBUG
	printf ("----- dosio %08lx%08lx\n", l, r);
# endif /* DEBUG */

	io.op = r & 0xff;
	io.fd = r>>8 & 0xff;
	io.length = r>>16 & 0xffff;
	io.addr = l;
	reply.err = 0;
	reply.type = 0;
	reply.length = 0;
	reply.addr = 0;
	dosio (&io, &reply);
	r = reply.err & 0xff | (unsigned long) (reply.type & 0xff) << 8 |
		(unsigned long) (reply.length & 0xffff) << 16;
	l = reply.addr & 0x7fffffff | 0x80000000;

# ifdef DEBUG
	printf ("----- reply %08lx%08lx\n", l, r);
# endif /* DEBUG */

	ptstore (l, r, DOSADDR+1);
	ptstore (0L, 0L, DOSADDR);
}

dosio (i, r)
register struct dosio *i;
register struct dosreply *r;
{
	char *p;
	int fd;
	unsigned n;
	char name [100];
	struct stat st;

	switch (i->op) {
	case DOPEN:
		if (i->length <= 0)
			break;
		ptgetstr (name, i->addr, i->length);
		name [i->length] = 0;
		scanslash (name);
# ifdef DEBUG
		printf ("----- open '%s' mode 0x%x\n", name, i->fd);
# endif /* DEBUG */
		fd = open (name, i->fd | O_BINARY);
		if (fd < 0)
			break;
		r->type = fd;
		return;
	case DCLOSE:
		close (i->fd);
		return;
	case DREAD:
	case DWRITE:
		if (i->length <= 0)
			return;
		p = malloc (i->length);
		if (! p)
			break;
		if (i->op == DREAD) {
			n = read (i->fd, p, i->length);
			if (n != (unsigned) -1)
				ptputstr (p, i->addr, i->length);
		} else {
			ptgetstr (p, i->addr, i->length);
			n = write (i->fd, p, i->length);
		}
		free (p);
		if (n == (unsigned) -1)
			break;
		r->length = n;
		return;
	case DSEEK:
		if (lseek (i->fd, i->addr, i->length) < 0)
			break;
		return;
	case DSTAT:
# ifdef notdef
		if (i->length <= 0)
			break;
		ptgetstr (name, i->addr, i->length);
		name [i->length] = 0;
		scanslash (name);
		printf ("----- stat '%s' fd 0x%x\n", name, i->fd);
		if (stat (name, &st) < 0)
			break;
		switch (fb.fb_attrib & S_IFMT) {
		case S_IFREG:	r->type = 0;	break;
		case S_IFDIR:	r->type = 1;	break;
		default:	r->type = 2;	break;
		}
		r->addr = st.st_size;
# endif
		if (fb.ff_attrib & FA_DIREC)
			r->type = 1;
		else if (fb.ff_attrib & FA_ARCH)
			r->type = 0;
		else
			r->type = 2;
		r->addr = fb.ff_fsize;
		return;
	case DOPENDIR:
		if (i->length <= 0)
			break;
		ptgetstr (name, i->addr, i->length);
		name [i->length] = 0;
		scanslash (name);
# ifdef DEBUG
		printf ("----- opendir '%s'\n", name);
# endif /* DEBUG */
		fd = opendir (name);
		if (fd < 0)
			break;
		r->type = fd;
		return;
	case DREADDIR:
		if (i->length <= 0)
			return;
# ifdef DEBUG
		printf ("----- readdir '%s' ", dirname);
# endif /* DEBUG */
		p = readdir ();
# ifdef DEBUG
		printf ("returns '%s'\n", p ? p : "NULL");
# endif /* DEBUG */
		if (! p)
			r->type = 0;
		else {
			ptputstr (p, i->addr, i->length);
			r->type = 1;
		}
		return;
	defaut:
		break;
	}
	r->err = 1;
}

static scanslash (p)
register char *p;
{
	for (; *p; ++p)
		if (*p == '/')
			*p = '\\';
}

opendir (name)
char *name;
{
	register char *p, *s;

	if (! name || ! *name)
		return (-1);
	/* make dirname\*.* */
	for (p=dirname, s=name; *s; *p++ = *s++);
	if (p[-1] != '\\' && p[-1] != ':')
		*p++ = '\\';
	*p++ = '*';
	*p++ = '.';
	*p++ = '*';
	*p = 0;
	firstdir = 1;
	if (findfirst (dirname, &fb, -1) < 0)
		return (-1);
	return (100);
}

static char *readdir ()
{
	int ret;

	ret = firstdir ? findfirst (dirname, &fb, -1) : findnext (&fb);
	firstdir = 0;
	if (ret < 0)
		return (0);
	return (fb.ff_attrib==FA_ARCH ? strlwr (fb.ff_name) : fb.ff_name);
}
