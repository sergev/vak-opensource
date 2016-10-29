# include <stdio.h>
# include "sys/pult.h"

# include "jlib.h"

# define DEVPULT        "/dev/pult"

/* read from Elbrus-B */

# define BRDWORD       0210            /* 210-217 ðíþ */

/* write to Elbrus-B */

# define BBUTTON1       0351            /* ðõóë, ðëô, õ0ó, õ0 */
# define BBUTTON2       0352            /* çþáðõ, éëîðòå, ðõóðòå */
# define BBUTTON3       0353            /* ðõóëáî, úáðòïó, õçáðí */

# define BINDEX         0357            /* éá */
# define BSTEP		0362		/* ïò */

# define BCMD           0371            /* òëð */
# define BCMDADDR       0372            /* áëð */
# define BMEMADDR       0373            /* áþð */
# define BADDR          0374            /* áðí */
# define BWRWORD        0376            /* ðíú */

# define ptsend(a,b)    ioctl (pfd, PTSEND (b & 0xff), (char far *) (long) a);

struct ptword hdr;

int halting;				/* ÐÒÏÃÅÓÓÏÒ ÓÔÏÉÔ */
int daemonflag;
static pfd;
static long pseek = -1;
static struct ptword wordbuf [256];

long ptrecvl ();

extern curgroup, curwin;

main (argc, argv)
char **argv;
{
	register char *p;

	for (++argv; --argc>0; ++argv)
		if (**argv == '-')
			for (p= *argv+1; *p; ++p)
				switch (*p) {
				case 'd':
					++daemonflag;
					break;
				}
	if (daemonflag && fork ())
		exit (0);
	init ();
	ptinit ();
	Jopen ();
	Jclear ();
	pultwin (curgroup, curwin);
	dowin ();
	Jclear ();
	Jclose ();
	return (0);
}

zero ()
{
	/* reset processor */
	ptsend (3, BBUTTON1);
}

stop ()
{
	/* enable step-by step mode */
	ptsend (1, BSTEP);
}

send (d, r)
{
	ptsend (d, r);
}

ptenable ()
{
	/* enable reading registers */
	ptsend (3, BSTEP);
}

run ()
{
	/* disable step-by step mode */
	ptsend (0, BSTEP);
	step ();
}

step ()
{
	/* start execution */
	ptsend (010, BBUTTON1);
}

setcrp ()
{
	/* start execution */
	ptsend (4, BBUTTON1);
}

long fgeth (f)
register FILE *f;
{
	register long h;

	h = (unsigned long) getc (f) << 24;
	h |= (unsigned long) getc (f) << 16;
	h |= (unsigned long) getc (f) << 8;
	h |= (unsigned long) getc (f);
	return (h);
}

fgetword (f, h)
register FILE *f;
register struct ptword *h;
{
	h->tag = getc (f);
	h->left = fgeth (f);
	h->right = fgeth (f);
}

fget256word (f, h, len)
register FILE *f;
register struct ptword *h;
long len;
{
	if (len > 256)
		len = 256;
	while (--len >= 0) {
		h->tag = getc (f);
		h->left = fgeth (f);
		h->right = fgeth (f);
		++h;
	}
}

loadfile (name, prf)
char *name;
int (*prf) ();
{
	FILE *fd;
	long addr, len;
	struct ptword word;
	register struct ptword *w = &word;
	char buf [40];
	int done;

	strcpy (buf, name);
	strcat (buf, ".tst");
	name = buf;
	fd = fopen (name, "rb");
	if (! fd)
		return (-1);
	fgetword (fd, w);
	addr = w->left;
	len = w->right;
	(*prf) ("úÁÇÒÕÚËÁ ÆÁÊÌÁ '%s' Ó ÁÄÒÅÓÁ %lxh, ÄÌÉÎÁ %lxh ...",
		name, addr, len);
	VSync ();
	ptputfname (name);
	done = 0;
	for (; len>0; len-=256, addr+=256) {
		fget256word (fd, wordbuf, len);
		pt256store (wordbuf, len, addr);
		(*prf) ("%3dk\b\b\b\b", done += 2);
		VSync ();
	}
	(*prf) (" ÇÏÔÏ×Ï.\n");
	fclose (fd);
	return (0);
}

ptinit ()
{
	pfd = open (DEVPULT, 2);
	if (pfd < 0) {
		fprintf (stderr, "Cannot open %s\n", DEVPULT);
		exit (1);
	}
}

ptsendl (h, cmd)
register long h;
{
	ioctl (pfd, PTSENDL (cmd), (char far *) h);
}

ptrecvb (index, cmd)            /* receive byte by index & cmd */
{
	char c;

	c = index;
	ioctl (pfd, PTRECV (cmd), (char far *) &c);
	return (c & 0xff);
}

long ptrecvl (index, cmd)       /* receive 4 bytes by index & cmd */
{
	long h;

	h = index;
	ioctl (pfd, PTRECVL (cmd), (char far *) &h);
	return (h);
}

ptgetstr (p, addr, len)
register char *p;
register long addr;
unsigned len;
{
	long r, l;
	register n;

	for (n=(len+7)/8; --n>=0; ++addr) {
		ptload (&l, &r, addr);
		*p++ = r;
		*p++ = r>>8;
		*p++ = r>>16;
		*p++ = r>>24;
		*p++ = l;
		*p++ = l>>8;
		*p++ = l>>16;
		*p++ = l>>24;
	}
}

ptputstr (p, addr, len)
register unsigned char *p;
long addr;
unsigned len;
{
	struct ptword h;
	register long r, l;
	register n;

	for (n=(len+7)/8; --n>=0; ++addr) {
		h.right = p[0] | (long) p[1]<<8 | (long) p[2]<<16 | (long) p[3]<<24;
		p += 4;
		h.left = p[0] | (long) p[1]<<8 | (long) p[2]<<16 | (long) p[3]<<24;
		p += 4;
		h.tag = 0;
		ptstore (&h, addr);
	}
}

ptstore (h, a)          /* send word to elbrus-b to address a */
register struct ptword *h;
register long a;
{
	ptseek (a);
	write (pfd, (char *) h, sizeof (struct ptword));
}

pt256store (h, len, a)       /* send word (l:r) to elbrus-b to address a */
register struct ptword *h;
register long len;
long a;
{
	if (len > 256)
		len = 256;
	ptseek (a);
	write (pfd, (char *) h, (unsigned) len * sizeof (struct ptword));
}

ptload (l, r, a)
long *l, *r;
register long a;
{
	struct ptword w;

	ptseek (a);
	read (pfd, (char *) &w, sizeof (struct ptword));
	*l = w.left;
	*r = w.right;
}

ptseek (a)
register long a;
{
	a *= sizeof (struct ptword);
	if (a == pseek)
		return;
	lseek (pfd, a, 0);
	pseek = a + sizeof (struct ptword);
}

ptputfname (name)
register char *name;
{
	register char *p;
	register i;
	char buf [8];

	for (p=name; *p; ++p)
		if (*p == '/')
			name = p+1;
	for (p=buf+7; p>=buf; --p)
		*p = ' ';
	for (i=0; i<8; ++i)
		buf [i] = name [7-i];
	ptputstr (buf, 0L, 8);
}
