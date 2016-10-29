# include <dos.h>
# include <stdio.h>

# define PROGNAME	"tester"
# define VERSION	"Тестер Эльбрус-Б 1.0  (c) 1990 Демос/*\n"
# define LOGSTR		"Для выхода нажмите ^W\n"

/* BIOS printer port base */

# define PSEG   	0
# define POFF   	0x408
# define ptport   	0x378

/* register definitions */

# define RDATA          (ptport+0)      /* place characters here */
# define RSTATUS        (ptport+1)      /* non zero means busy */
# define RCNTRL         (ptport+2)      /* write control here */

/* control port definitions */

# define CSTROBE        1               /* 1 when sending byte */
# define CAUTOLF        2               /* 1 causes LF after CR */
# define CINIT          4               /* 0 resets the printer */
# define CSELECT        010             /* 1 selects the printer */
# define CIENABL        020             /* interrupt when SACK goes false */

/* status port definitions */

# define SERROR         010             /* 0=printer signals an error */
# define SSELECT        020             /* 1=printer is selected */
# define SPE            040             /* 1=out of paper */
# define SACK           0100            /* 0=ready for next character */
# define SBUSY          0200            /* 0=busy or offline or error */

/* Elbrus-B control commands */

/* read from to Elbrus-B */

# define BRDWORD       0210            /* 210-217 ПМЧ */

/* write to Elbrus-B */

# define BBUTTON1       0351            /* ПУСК, ПКТ, У0С, У0 */
# define BBUTTON2       0352            /* ГЧАПУ, ИКНПРЕ, ПУСПРЕ */
# define BBUTTON3       0353            /* ПУСКАН, ЗАПРОС, УГАПМ */

# define BINDEX         0357            /* ИА */
# define BSTEP		0362		/* ОР */

# define BCMD           0371            /* РКП */
# define BCMDADDR       0372            /* АКП */
# define BMEMADDR       0373            /* АЧП */
# define BADDR          0374            /* АПМ */
# define BWRWORD        0376            /* ПМЗ */

/* Elbrus-B phys memory mask */

# define ADDRMASK	0x7fffffL

# define inb		inportb
# define outb		outportb

/* toggle printer control bits */
# define ptstrobe()	(outb (RCNTRL, CINIT|CSTROBE), outb (RCNTRL, CINIT))

/* send byte to printer port */
# define ptputc(c)	(outb (RDATA, ~(c)), ptstrobe ())

/* send data to Elbrus-B */
# define ptdata(c)	(outb (RCNTRL, 0), outb (RCNTRL, CINIT), ptputc (c))

/* send command to Elbrus-B */
# define ptsend(c,d)	(ptdata (c), ptputc (BINDEX), ptputc (d))

# define INT(x)		((int) (x))

# define ishex(c)	(c>='0' && c<='9' || c>='a' && c<='f' || c>='A' && c<='F')
# define hex(c)		((c<='9' ? 0 : 9) + c&0xf)

struct word {				/* заголовок */
	int tag;			/* 1 byte */
	long left;			/* 4 bytes - address */
	long right;			/* 4 bytes - len */
} hdr;

# ifndef ptport
int ptport;                             /* base printer port address */
# endif
int trace;				/* trace flag */
int runflag;
int nflag;
long oldaddr = -1;
int halting;				/* процессор стоит */

long ptrecvl ();
extern printf ();

main (argc, argv)
char **argv;
{
	register i;

	if (argc==2 && !strcmp (argv [1], "-help")) {
		usage (PROGNAME);
		exit (1);
	}
	printf (VERSION);
	printf (LOGSTR);
	ptinit ();
	if (argc <= 1) {
		tty ();
		exit (0);
	}
	for (i=1; i<argc; ++i) {
		if (argv [i] [0] == '-') {
			/* scan flags */
			register char *p;

			for (p=&argv[i][1]; *p; ++p) switch (*p) {
			case 't':	/* turn on tracing */
				++trace;
				break;
			case 'z':	/* reset prozessor */
				zero ();
				break;
			case 'n':	/* not run */
				++nflag;
				break;
			case 'r':	/* run */
				zero ();
				run ();
			case 'v':	/* enter tty mode */
				tty ();
				break;
			default:
				printf ("unknown flag '%c'\n", *p);
				usage (PROGNAME);
				exit (1);
			}
			continue;
		}
		if (argv[i][0] <= '9')
			readmem (argv [i]);
		else {
			/* load .tst file into memory */
			stop ();
			if (loadfile (argv [i], printf) < 0) {
				printf ("Не могу открыть '%s'\n", argv [i]);
				exit (1);
			}
			zero ();
			++runflag;
		}
nextloop:	;
	}
	if (runflag && ! nflag) {
		zero ();
		run ();
		tty ();
	}
	return (0);
}

usage (n)
char *n;
{
	printf ("Usage:\t%s [-tznrv] [file | addr:len | addr-addr]\n", n);
	printf ("\t-t\t- turn %s tracing\n", n);
	printf ("\t-z\t- reset processor\n");
	printf ("\t-n\t- load file & not run\n");
	printf ("\t-r\t- reset, run & enter tty mode\n");
	printf ("\t-v\t- enter tty mode\n");
	printf ("\nOr:\t%s\n", n);
	printf ("\t\t- enter tty mode\n");
}

tty ()
{
	/* emulate console tty */
	register doscount, haltcount;

	doscount = haltcount = 0;
	for (;;) {
		if (--haltcount <= 0) {
			halting = cpuhalted ();
			if (halting)
				flashhalt ();
			haltcount = 5;
		}
		dotty ();
		if (--doscount <= 0) {
			dodisk ();
			doscount = 100;
		}
	}
}

zero ()
{
	/* reset processor */
	ptsend (3, BBUTTON1);
	oldaddr = -1;
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

cpuhalted ()
{
	static long oldcr;
	static count;
	register long cr;

	cr = ptrecvl (0, 0120);
	if (cr == oldcr) {
		if (++count >= 50) {
			count = 0;
			return (1);
		}
		return (0);
	}
	oldcr = cr;
	count = 0;
	return (0);
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
register struct word *h;
{
	h->tag = getc (f);
	h->left = fgeth (f);
	h->right = fgeth (f);
}

loadfile (name, prf)
char *name;
int (*prf) ();
{
	FILE *fd;
	long addr, len;
	struct word word;
	register struct word *w = &word;
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
	(*prf) ("Загрузка файла '%s' с адреса %lxh, длина %lxh ...",
		name, addr, len);
	done = 0;
	for (; --len>=0; ++addr) {
		fgetword (fd, w);
		ptstore (w->left, w->right, addr);
		if (trace)
			(*prf) ("%lx := %08lx %08lx\n",
				addr, w->left, w->right);
		else if (! (addr % 256))
			(*prf) ("%3dk\b\b\b\b", done += 2);
	}
	(*prf) (" готово.\n");
	fclose (fd);
	return (0);
}

char *getaddr (p, l)
char *p;
long *l;
{
	char buf [40];
	register char *b;
	register long h;
	register o;

	for (b=buf; ishex (*p); ++b, ++p)
		*b = hex (*p);
	*b = -1;
	b = buf;
	h = 0;
	o = *b ? 16 : 8;
	while (*b >= 0)
		h = h*o + *b++;
	*l = h;
	return (p);
}

readmem (p)
char *p;
{
	/* [address][-address] */
	/* [address][:length] */
	long addr, fin, len;

	p = getaddr (p, &addr);
	if (*p == '-') {
		p = getaddr (p+1, &fin);
		len = fin - addr;
	} else if (*p == ':') {
		p = getaddr (p+1, &len);
		if (len > 0)
			--len;
		fin = addr + len;
	} else if (*p == 0) {
		fin = addr;
		len = 0;
	} else {
		printf ("bad format '%s'\n", p);
		return;
	}
	addr &= ADDRMASK;
	fin &= ADDRMASK;
	if (len < 0) {
		len = -len;
		addr = fin;
		fin = addr + len;
	}
	while (addr <= fin)
		ptread (addr++);
}

ptinit ()
{
# ifndef ptport
	int far *p;

	/* compute address of printer ports */
	p = (int far *) MK_FP (PSEG, POFF);
	ptport = *p;
# endif
	/* disable init and interrupts */
	outb (RCNTRL, CINIT);
}

ptread (addr)
long addr;
{
	long r, l;

	ptload (&l, &r, addr);
	printf ("%7lx:\t%04x %04x %04x %04x\n", addr, INT (l>>16), INT (l),
		INT (r>>16), INT (r));
}

/* internal routines */

ptparity (w)
register long w;
{
	register short s;

	/* compute parity of word w, return 1 if even */
	s = w >> 16;
	s ^= (short) w;
	s ^= s >> 8;
	s ^= s >> 4;
	s ^= s >> 2;
	s ^= s >> 1;
	return ((s ^ 1) & 1);
}

ptsendl (h, cmd)
register long h;
{
	/* send word h to Elbrus-B */
	ptsend (INT (h >> 24), cmd);
	ptsend (INT (h >> 16), cmd);
	ptsend (INT (h >> 8), cmd);
	ptsend (INT (h), cmd);
}

ptrecvb (index, cmd)
{
	register c = 0;

	/* receive byte by index & cmd */
	ptsend (index, cmd);
	if (! (inb (RSTATUS) & SPE)) c |= 1;		ptstrobe ();
	if (! (inb (RSTATUS) & SPE)) c |= 2;		ptstrobe ();
	if (! (inb (RSTATUS) & SPE)) c |= 4;		ptstrobe ();
	if (! (inb (RSTATUS) & SPE)) c |= 8;		ptstrobe ();
	if (! (inb (RSTATUS) & SPE)) c |= 0x10;		ptstrobe ();
	if (! (inb (RSTATUS) & SPE)) c |= 0x20;		ptstrobe ();
	if (! (inb (RSTATUS) & SPE)) c |= 0x40;		ptstrobe ();
	if (! (inb (RSTATUS) & SPE)) c |= 0x80;
	return (c);
}

long ptrecvl (index, cmd)
{
	register i;
	register long h;

	/* receive word h from Elbrus-B */
	h = 0;
	for (i=0; i<4; ++i)
		h |= (long) ptrecvb (index, cmd+i) << (i*8);
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
	register long r, l;
	register n;

	for (n=(len+7)/8; --n>=0; ++addr) {
		r = p[0] | (long) p[1]<<8 | (long) p[2]<<16 | (long) p[3]<<24;
		p += 4;
		l = p[0] | (long) p[1]<<8 | (long) p[2]<<16 | (long) p[3]<<24;
		p += 4;
		ptstore (l, r, addr);
	}
}

ptstore (l, r, a)	/* send word (l:r) to elbrus-b to address a */
long l, r;
register long a;
{
	int t;

	t = ptparity (l) << 6 | ptparity (r) << 5;	/* compute tag byte */
	ptaddr (a & ADDRMASK | 0x4000000L);		/* set address word */
	ptsend (t, BWRWORD);				/* send tag byte */
	ptsendl (l, BWRWORD);				/* send word */
	ptsendl (r, BWRWORD);
	ptdata (1);					/* УГАПМ */
	ptputc (BBUTTON3);
}

ptload (l, r, a)
long *l, *r;
register long a;
{
	/* load word (l:r) from elbrus-b address a */

	/* set address word */
	ptaddr (a & ADDRMASK);

	/* start memory transaction (УГАПМ) */
	ptdata (1);
	ptputc (BBUTTON3);

	/* receive word */
	*r = ptrecvl (0, BRDWORD);
	*l = ptrecvl (0, BRDWORD+4);
}

ptloadb (a)
register long a;
{
	/* load lower byte from address a */

	/* set address word */
	ptaddr (a & ADDRMASK);

	/* start memory transaction (УГАПМ) */
	ptdata (1);
	ptputc (BBUTTON3);

	/* receive char */
	return (ptrecvb (0, BRDWORD));
}

ptaddr (a)
long a;
{
	if (a == oldaddr)
		return;

	/* compute address word */
	a |= (long) ptparity (a & 0x40003ffL) << 27;
	a |= (long) ptparity (a & 0x07ffc00L) << 28;

	/* send address */
	ptsendl (a, BADDR);
	oldaddr = a;
}

bcopy (from, to, len)
register char *from, *to;
register unsigned len;
{
	while (len--)
		*to++ = *from++;
}

# ifndef inb
inb (p)
{
	register c;

	c = inportb (p);
	if (trace > 2)
		printf ("<<--- inb (%xh) => %xh\n", p, c);
	return (c);
}
# endif

# ifndef outb
outb (p, c)
{
	if (trace > 2)
		printf ("--->> outb (%xh, %xh)\n", p, c);
	outportb (p, c);
	return (c);
}
# endif
