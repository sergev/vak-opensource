# include "regio.h"

# define A 22
# define C 8
# define X 10

# define Y (9+X)
# define Z (5+X)
# define B (40+A)
# define D (40+C)

# define BIT(i)		(1L << (i))

struct bcstate {
	int row, col;
	char *name;
};

static struct bcstate rstate [] = {
	X,	B,	"ток",
	X+1,	B,	"тпс",
	X+2,	B,	"тги",
	X+3,	B,	"тначп",
	X+4,	B,	"стри(п)",
	X+5,	B,	"ги",
	X+6,	B,	"запросп(п)",
	X+7,	B,	"готп",
	Y,	B,	"бчк",
	Y+1,	B,	"бчм",
	Y+2,	B,	"бчс",
	Y+3,	B,	"бт12",
	Z,	D,	"ошам",
	Z+1,	D,	"ошас",
	Z+2,	D,	"ошчм",
	Z+3,	D,	"ошчс",
	Z+4,	D,	"ошчк",
	Z+5,	D,	"крп",
	Z+6,	D,	"сччм",
	Z+7,	D,	"сччс",
};

static struct bcstate tstate [] = {
	X,	A,	0,
	X+1,	A,	"тпс",
	X+2,	A,	"тгп",
	X+3,	A,	"тначи",
	X+4,	A,	"запросп(и)",
	X+5,	A,	"гп",
	X+6,	A,	"стри(и)",
	X+7,	A,	"готи",
	Y,	A,	0,
	Y+1,	A,	"бгпр",
	Y+2,	A,	"им",
	Y+3,	A,	"бчд",
	Z,	C,	"ошам",
	Z+1,	C,	"ошас",
	Z+2,	C,	"сччм",
	Z+3,	C,	"сччс",
	Z+4,	C,	"нош",
	Z+5,	C,	"кри",
	Z+6,	C,	0,
	Z+7,	C,	0,
};

extern halting;				/* процессор стоит */

extern long ptrecvl ();

notyet ()
{
	win0 ();
	Jmove (22, 30);
	Jrbold (0x4e);
	Jprintf ("Еще не реализовано");
	Jnorm (0x1b);
}

bc0 () { bc (0); }
bc1 () { bc (1); }
bc2 () { bc (2); }
bc3 () { bc (3); }
bc4 () { bc (4); }
bc5 () { bc (5); }
bc6 () { bc (6); }
bc7 () { bc (7); }

bc (n)
{
	register i;
	struct bchan b;

	loadbchan (n, &b);
	Jmprintf (2, 30, "Байтовый канал %d", n);
	Jmprintf (4, 10, "Источник %d", n);
	Jmprintf (4, 50, "Приемник %d", n);

	Jmprintf (6, 5, "уск  ");
	prbclcm (&b, 0);
	Jmprintf (7, 5, "сск  ");
	prbclcm (&b, 1);
	Jmprintf (6, 45, "уск  ");
	prbclcm (&b, 2);
	Jmprintf (7, 45, "сск  ");
	prbclcm (&b, 3);

	Jmprintf (X, C-1, "%02x  дани", b.td);
	Jmprintf (X, D-1, "%02x  данп", b.rd);
	Jmprintf (X+1, C, "%x  сси", b.tis);
	Jmprintf (X+1, D, "%x  ссп", b.ris);
	Jmprintf (X+2, C, "%x  кбпс", b.tbc);
	Jmprintf (X+2, D, "%x  кбпс", b.rbc);

	for (i=0; i<20; ++i) {
		prbcstate (&tstate[i], b.ts & BIT (i) ? '1' : '0');
		prbcstate (&rstate[i], b.rs & BIT (i) ? '1' : '0');
	}
}

prbclcm (b, n)
register struct bchan *b;
register n;
{
	prword (&b->lcm[n]);
	Jprintf ("  %02x", b->lcmt [n] & 0xff);
}

static prbcstate (s, c)
register struct bcstate *s;
{
	if (s->name)
		Jmprintf (s->row, s->col, "%c  %s", c, s->name);
}

static loadbchan (n, r)
register n;
register struct bchan *r;
{
	register long h;
	register i;

	/* enable access to cpu registers */
	stop ();
	ptenable ();

	r->rd = ptrecvb (0100|n, 0334) & 0xff;
	r->td = ptrecvb (0100|n, 0324) & 0xff;

	r->ris = ptrecvb (0100|n, 0333) & 0xf;
	r->tis = ptrecvb (0100|n, 0323) & 0xf;

	r->rbc = ptrecvb (0100|n, 0335) >> 4 & 0xf;
	r->tbc = ptrecvb (0100|n, 0325) >> 4 & 0xf;

	h = ptrecvb (0100|n, 0332) & 0xff;
	h |= (long) (ptrecvb (0100|n, 0335) & 0xf) << 8;
	h |= (long) (ptrecvb (0100|n, 0336) & 0xff) << 12;
	r->rs = h;

	h = ptrecvb (0100|n, 0322) & 0xff;
	h |= (long) (ptrecvb (0100|n, 0325) & 0xf) << 8;
	h |= (long) (ptrecvb (0100|n, 0326) & 0xff) << 12;
	r->ts = h;

	for (i=0; i<4; ++i)
		load1lcm (&r->lcm[i], &r->lcmt[i], 32+n*4+i);

	/* disable access to cpu registers */
	if (! halting)
		run ();
}
