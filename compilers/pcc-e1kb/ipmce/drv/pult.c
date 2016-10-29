/*
 *      Elbrus-B console Xenix driver.
 *
 *      Authors: Vakulenko S.V., KIAE, Jaroslavtsev M., ITM
 *
 *      Name:   /dev/pult
 *
 *      Calls:
 *              d = open ("/dev/pult", 2);
 *                      - open device. Default memory address is 0.
 *
 *              close (d);
 *                      - close device.
 *
 *              ioctl (d, PTADDR, addr);
 *                      - set Elbrus-B phys memory address and byte offset.
 *                      Addr is word address (23 bits) * 8 + byte offset (3 bits).
 *                      Default (just after open) is 0.
 *
 *              ioctl (d, PTSEEK, increment);
 *                      - update Elbrus-B phys memory address and byte offset.
 *                      If increment is negative, address is decremented.
 *
 *              ioctl (d, PTSETREG+regnum, data);
 *                      - set register.
 *
 *              data = ioctl (d, PTGETREG+regnum, 0);
 *                      - get register.
 *
 *              data = ioctl (d, PTGETINDX+regnum, index);
 *                      - get register using given index.
 *
 *              write (d, (char *) buf, n);
 *                      - write array of words to Elbrus-B memory.
 *                      After write memory address is oncremented.
 *
 *              read (d, (char *) buf, n);
 *                      - read array of words from Elbrus-B memory.
 *                      After read memory address is oncremented.
 */

# include "../h/types.h"
# include "../h/param.h"
# include "../h/errno.h"
# include "../h/sysmacros.h"
# include "../h/file.h"
# include "../h/dir.h"
# include "../h/signal.h"
# include "../h/user.h"

# include "pult.h"

/* BIOS printer port base */

# define PSEG   0
# define POFF   0x408

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

# define BRDWORD       0210            /* 210-217 рню */

/* write to Elbrus-B */

# define BBUTTON1       0351            /* рхул, рлф, х0у, х0 */
# define BBUTTON2       0352            /* зюбрх, йлорте, рхурте */
# define BBUTTON3       0353            /* рхулбо, ъбртпу, хзбрн */

# define BINDEX         0357            /* йб */

# define BCMD           0371            /* тлр */
# define BCMDADDR       0372            /* блр */
# define BMEMADDR       0373            /* бюр */
# define BADDR          0374            /* брн */
# define BWRWORD        0376            /* рнъ */

/* Elbrus-B phys memory mask */

# define ADDRMASK       0x7fffffL

/* flags definitions */

# define FBUSY          1               /* semaphore */
# define FASLEEP        2               /* there are waiting processes */

/* toggle printer control bits */
# define pttoggle(m) { outb(RCNTRL,ptctrl^(m)); outb(RCNTRL,ptctrl); }

/* send byte to printer port */
# define ptputc(c) { outb(RDATA,~(c)); pttoggle(CSTROBE); }

/* receive bit from Elbrus-B and invert it */
# define ptgetbit() ((inb (RSTATUS) & SPE) == 0)

/* send data to Elbrus-B */
# define ptdata(c) { pttoggle(CINIT); ptputc(c); }

/* send command to Elbrus-B */
# define ptsend(c,d) { ptdata (c); /* ptputc(BINDEX); */ ptputc(d); }

int ptflags;                            /* internal driver flags */
int ptport;                             /* base printer port address */
int ptctrl;                             /* current printer control byte */
long ptaddr;                            /* Elbrus-B phys memory address */
int ptaddroff;                          /* memory byte offset */

/* public part of driver */

ptinit ()
{
	int far *p;

	/* compute address of printer ports */
	p = (int far *) sotofar (PSEG, POFF);
	ptport = *p;

	/* disable init and interrupts */
	outb (RCNTRL, ptctrl = CINIT);
}

ptopen ()
{
	/* multiple opens not implemented yet */
	if (ptflags & FBUSY) {
		u.u_error = EIO;
		return;
	}
	ptflags |= FBUSY;
	ptaddr = 0;
	ptaddroff = 0;
}

ptclose ()
{
	ptflags &= ~FBUSY;
}

ptread ()
{
	register len;
	register struct word { long right, left; } far *p;

	len = u.u_count/8;              /* number of words */
	if (len <= 0)
		return;

	for (p=(struct word far *)u.u_base; --len>=0; ++p, ++ptaddr)
		ptload (&p->left, &p->right, ptaddr);

	u.u_count = 0;
	ptaddr &= ADDRMASK;
}

ptwrite ()
{
	register len;
	register struct word { long right, left; } far *p;

	len = u.u_count/8;              /* number of words */
	if (len <= 0)
		return;

	for (p=(struct word far *)u.u_base; --len>=0; ++p, ++ptaddr)
		ptstore (p->left, p->right, ptaddr);

	u.u_count = 0;
	ptaddr &= ADDRMASK;
}

ptioctl (dev, cmd, arg, mode)
faddr_t arg;
{
	register long ba;

	switch (cmd & ~0xff) {
	case PTADDR:                    /* set Elbrus-B memory address */
		ba = (long) arg;
		ptaddr = ba >> 3 & ADDRMASK;
		ptaddrof = ba & 7;
		break;
	case PTSEEK:                    /* update Elbrus-B memory address */
		ba = (long) arg;
		ba += ptaddr << 3 | ptaddrof;
		ptaddr = ba >> 3 & ADDRMASK;
		ptaddrof = ba & 7;
		break;
	case PTSETREG:                  /* set register */
		ptsend ((int) arg, cmd);
		break;
	case PTGETREG:                  /* get register */
		ptdata (0);
		return (ptrecv (cmd));
	case PTGETINDX:                 /* get indexed register */
		ptsend ((int) arg, BINDEX);
		return (ptrecv (cmd));
	default:
		u.u_error = EIO;
		break;
	}
	return (0);
}

/* internal routines */

ptparity (w)
long w;
{
	register short s;

	/* compute parity of word w, return 1 if odd */
	s = w >> 16;
	s ^= (short) w;
	s ^= s >> 8;
	s ^= s >> 4;
	s ^= s >> 2;
	s ^= s >> 1;
	return (s & 1);
}

ptsendl (h, cmd)
register long h;
{
	/* send word h to Elbrus-B */
	ptsend ((int) (h >> 24), cmd);
	ptsend ((int) (h >> 16), cmd);
	ptsend ((int) h >> 8, cmd);
	ptsend ((int) h, cmd);
}

ptrecv (cmd)
{
	register i;
	register c;

	/* receive byte from Elbrus-B */
	c = 0;
	for (i=0; i<8; ++i) {
		ptputc (cmd);
		c |= ptgetbit () << i;
	}
	return (c);
}

long ptrecvl (cmd)
{
	register i;
	register long h;

	/* receive word h from Elbrus-B */
	h = 0;
	for (i=0; i<4; ++i) {
		ptdata (0);
		h |= ptrecv (cmd+i) << (i*8);
	}
	return (h);
}

ptstore (l, r, a)
long l, r;
register long a;
{
	int x, t;

	/* send word (l:r) to elbrus-b to address a */

	/* compute address word */
	a = (a & ADDRMASK) | 0x4000000L;        /* 'write' bit */
	a |= ptparity (a & 0x40003ffL) << 27;
	a |= ptparity (a & 0x07ffc00L) << 28;

	/* compute tag byte */
	t = ptparity (l) << 6 | ptparity (r) << 5;

	/* disable interrupts while sending word */
	x = spl5 ();

	/* send address */
	ptsendl (a, BADDR);

	/* send tag byte */
	ptsend (t, BWRWORD);

	/* send word */
	ptsendl (l, BWRWORD);
	ptsendl (r, BWRWORD);

	/* start memory transaction (хзбрн) */
	pttoggle (CINIT);
	ptputc (1);
	ptputc (BBUTTON3);

	/* enable interrupts */
	splx (x);
}

ptload (l, r, a)
long *l, *r;
register long a;
{
	int x;

	/* load word (l:r) from elbrus-b address a */

	/* compute address word */
	a = (a & ADDRMASK);
	a |= ptparity (a & 0x40003ffL) << 27;
	a |= ptparity (a & 0x07ffc00L) << 28;

	/* disable interrupts while loading word */
	x = spl5 ();

	/* send address */
	ptsendl (a, BADDR);

	/* start memory transaction (хзбрн) */
	pttoggle (CINIT);
	ptputc (1);
	ptputc (BBUTTON3);

	/* receive word */
	*r = ptrecvl (BRDWORD);
	*l = ptrecvl (BRDWORD+4);

	/* enable interrupts */
	splx (x);
}
