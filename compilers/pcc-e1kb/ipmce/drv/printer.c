# include "../h/types.h"
# include "../h/param.h"
# include "../h/sysmacros.h"
# include "../h/file.h"
# include "../h/tty.h"
# include "../h/conf.h"

# define LPPRI          (PZERO+5)
# define LOWAT          50
# define HIWAT          150

/* register definitions */

# define RBASE          0x000           /* base address of registers */
# define RDATA          (RBASE+0)       /* place characters here */
# define RSTATUS        (RBASE+1)       /* non zero means busy */
# define RCNTRL         (RBASE+2)       /* write control here */

/* control definitions */

# define CINIT          1               /* initialize the interface */
# define CIENABL        2               /* interrupt enable */
# define CRESET         4               /* interface reset */

/* flags definitions */

# define FIRST          1
# define ASLEEP         2
# define ACTIVE         4

struct clist lp_queue;
int lp_flags = 0;

int lpopen (), lpclose (), lpwrite (), lpinit ();

lpopen (dev)
int dev;
{
	if (! (lp_flags & FIRST)) {
		lp_flags |= FIRST;
		outb (RCNTRL, CRESET);
	}
	outb (RCNTRL, CIENABL);
}

lpclose (dev)
int dev;
{
}

lpwrite (dev)
int dev;
{
	register c;
	int x;

	while ((c = cpass ()) >= 0) {
		x = spl5 ();
		while (lp_queue.c_cc > HIWAT) {
			lpstart ();
			lp_flags |= ASLEEP;
			sleep (&lp_queue, LPPRI);
		}
		splx (x);
		putc (c, &lp_queue);
	}
	x = spl5 ();
	lpstart ();
	splx (x);
}

lpstart ()
{
	if (lp_flags & ACTIVE)
		return;         /* interrupt chain is keeping printer going */
	lp_flags |= ACTIVE;
	lpintr (0);
}

lpintr (vec)
int vec;
{
	int c;

	if (! (lp_flags & ACTIVE))
		return;                 /* ignore spurious interrupt */

	/* pass chars until busy */
	while (inb (RSTATUS) == 0 && (c = getc (&lp_queue) >= 0))
		outb (RDATA, c);

	/* wakeup the writer if necessary */
	if (lp_queue.c_cc < LOWAT && (lp_flags & ASLEEP)) {
		lp_flags &= ~ASLEEP;
		wakeup (&lp_queue);
	}

	/* wakeup writer if waiting for drain */
	if (lp_queue.c_cc <= 0 )
		lp_flags &= ~ACTIVE;
}
