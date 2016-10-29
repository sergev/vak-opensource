/*
 *      Elbrus-B console Xenix driver.
 *
 *      Author: Vakulenko S.V., KIAE
 *
 *      Name:   /dev/pult
 *
 *      Calls:
 *              int d = open ("/dev/pult", 2);
 *                      - open device. Default memory address is 0.
 *
 *              close (int d);
 *                      - close device.
 *
 *              ioctl (int d, PTADDR, long addr);
 *                      - set Elbrus-B phys memory address.
 *                      Addr is word address (23 bits).
 *                      Default (just after open) is 0.
 *
 *              ioctl (int d, PTSEEK, long increment);
 *                      - update Elbrus-B phys memory address.
 *                      If increment is negative, address is decremented.
 *
 *              ioctl (int d, PTSEND (char ib), char ia);
 *                      - send (ia, ib).
 *
 *              ioctl (int d, PTSENDL (char b), long *ia);
 *                      - send halfword.
 *
 *              ioctl (int d, PTRECV (char ib), char *ia);
 *                      - receive byte.
 *
 *              ioctl (int d, PTRECVL (char ib), long *ia);
 *                      - receive halfword.
 *
 *              write (int d, struct ptword *pbuf, int n);
 *                      - write array of words to Elbrus-B memory.
 *                      After write memory address is oncremented.
 *                      n is number of words in pbuf * sizeof (struct ptword).
 *
 *              read (int d, struct ptword *pbuf, int n);
 *                      - read array of words from Elbrus-B memory.
 *                      After read memory address is oncremented.
 *                      n is number of words in pbuf * sizeof (struct ptword).
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

# define PTPRI          (PZERO+8)       /* sleeping priority */

# define ASMPULTIO

/* Elbrus-B control commands */

/* read from to Elbrus-B */

# define BRDWORD       0210            /* 210-217 рню */

/* write to Elbrus-B */

# define BBUTTON1       0351            /* рхул, рлф, х0у, х0 */
# define BBUTTON2       0352            /* зюбрх, йлорте, рхурте */
# define BBUTTON3       0353            /* рхулбо, ъбртпу, хзбрн */

# define BINDEX         0357            /* йб */
# define BSTEP		0362		/* пт */

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
# define FLOCK          4               /* driver locked */

# define PTPORT         0x378           /* BIOS printer port base */

# define RDATA          (PTPORT+0)      /* place characters here */
# define RSTATUS        (PTPORT+1)      /* non zero means busy */
# define RCNTRL         (PTPORT+2)      /* write control here */

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

# ifdef ASMPULTIO

#    define ptsend(c,d) _ptwr (c, d)
#    define ptparity(h) _ptevn (h)
#    define ptdown(c,d) _ptdn (c, d)

# else
			/* toggle printer control bits */
#    define ptstrobe()  (outb (RCNTRL, CINIT|CSTROBE), outb (RCNTRL, CINIT))

			/* send byte to printer port */
#    define ptputc(c)   (outb (RDATA, ~(c)), ptstrobe ())

			/* send data to Elbrus-B */
#    define ptdata(c)   (outb (RCNTRL, 0), outb (RCNTRL, CINIT), ptputc (c))

			/* send command to Elbrus-B */
#    define ptsend(c,d) (ptdata (c), ptputc (BINDEX), ptputc (d))

			/* send command to Elbrus-B */
#    define ptdown(c,d) (ptdata (c), ptputc (d))

# endif /* ASMPULTIO */

# define INT(x)		((int) (x))
# define WORDSZ         sizeof (struct ptword)

# ifdef notdef
# define LOCK()         { while (ptflags & FLOCK) sleep (&ptflags, PTPRI); ptflags |= FLOCK; }
# define UNLOCK()       { ptflags &= ~FLOCK; wakeup (&ptflags); }
# else
# define LOCK()
# define UNLOCK()
# endif

int ptflags;                            /* internal driver flags */

long ptrecvl ();

/* public part of driver */

ptinit ()
{
	/* disable init and interrupts */
	outb (RCNTRL, CINIT);
	ptflags = 0;
}

ptopen ()
{
	/* multiple opens are not implemented yet */
	if (ptflags & FBUSY) {
		u.u_error = EIO;
		return;
	}
	ptflags |= FBUSY;
}

ptclose ()
{
	ptflags = 0;
}

ptread ()
{
	register faddr_t p;
	struct ptword w;

	p = (faddr_t) u.u_base;
	while (u.u_count >= WORDSZ) {
		LOCK ();
		ptload (&w.left, &w.right, u.u_offset / WORDSZ);
		UNLOCK ();
		copyout ((char *) &w, p, sizeof (struct ptword));
		u.u_offset += WORDSZ;
		u.u_count -= WORDSZ;
		p += WORDSZ;
	}
}

ptwrite ()
{
	register faddr_t p;
	struct ptword w;

	p = (faddr_t) u.u_base;
	while (u.u_count >= WORDSZ) {
		copyin (p, (char *) &w, WORDSZ);
		LOCK ();
		ptstore (w.left, w.right, u.u_offset / WORDSZ);
		UNLOCK ();
		u.u_offset += WORDSZ;
		u.u_count -= WORDSZ;
		p += WORDSZ;
	}
}

ptioctl (dev, cmd, arg, mode)
register faddr_t arg;
{
	long h;
	char c;

	switch (cmd & PTMASK) {
	case PT_SEND:                   /* set register */
		LOCK ();
		ptsend ((int) arg, cmd);
		UNLOCK ();
		break;
	case PT_SENDL:                   /* set register */
		LOCK ();
		ptsendl ((long) arg, cmd);
		UNLOCK ();
		break;
	case PT_RECV:                   /* get register */
		copyin (arg, &c, 1);
		LOCK ();
		c = ptrecvb (c, cmd);
		UNLOCK ();
		copyout (&c, arg, 1);
		break;
	case PT_RECVL:                  /* get register */
		copyin (arg, (char *) &h, 4);
		LOCK ();
		h = ptrecvl ((int) h, cmd);
		UNLOCK ();
		copyout ((char *) &h, arg, 4);
		break;
	default:
		u.u_error = EIO;
		break;
	}
}

/* internal routines */

# ifndef ASMPULTIO
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
# endif

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
# ifdef ASMPULTIO
	ptsend (index, cmd);
	return (_ptrd ());
# else
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
# endif
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

ptstore (l, r, a)	/* send word (l:r) to elbrus-b to address a */
long l, r;
register long a;
{
	int t;

	t = ptparity (l) << 6 | ptparity (r) << 5;	/* compute tag byte */
	ptmem (a & ADDRMASK | 0x4000000L);              /* set address word */
	ptsend (t, BWRWORD);				/* send tag byte */
	ptsendl (l, BWRWORD);				/* send word */
	ptsendl (r, BWRWORD);
	ptdown (1, BBUTTON3);
}

ptload (l, r, a)
long *l, *r;
register long a;
{
	/* load word (l:r) from elbrus-b address a */

	/* set address word */
	ptmem (a & ADDRMASK);

	/* start memory transaction (хзбрн) */
	ptdown (1, BBUTTON3);

	/* receive word */
	*r = ptrecvl (0, BRDWORD);
	*l = ptrecvl (0, BRDWORD+4);
}

ptmem (a)
long a;
{
	/* compute address word */
	a |= (long) ptparity (a & 0x40003ffL) << 27;
	a |= (long) ptparity (a & 0x07ffc00L) << 28;

	/* send address */
	ptsendl (a, BADDR);
}
