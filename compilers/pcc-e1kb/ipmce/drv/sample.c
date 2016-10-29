# include "../h/types.h"
# include "../h/param.h"
# include "../h/sysmacros.h"
# include "../h/dir.h"
# include "../h/signal.h"
# include "../h/user.h"
# include "../h/mmu.h"
# include "../h/file.h"
# include "../h/errno.h"

# define INDEX  0x3d4                   /* index and data register location */
# define DATA   0x3d5                   /* for the CRT Controller 6845 */
# define MODE   0x3d8                   /* address for mode register */
# define COLOR  0x3d9                   /* address for color register */
# define MBASE  0xb8000L                /* starting address even lines */
# define SCN_GRF 0x0a                   /* graphics mode to put in MODE reg */
# define SCN_TXT 0x0d                   /* text mode to put in MODE reg */
# define SIZE   0x4000                  /* size of the video RAM */
# define HLIN   0x10                    /* hi-intensity */
# define ACOL   0x20                    /* main color or alternate color */

char grchar_mode [] = { 0x71, 0x50, 0x5a, 0x0a, 0x1f, 0x06, 0x19,
	0x1c, 0x02, 0x07, 0x06, 0x07, 0x00, 0x00, };

char grgraf_mode [] = { 0x38, 0x28, 0x2d, 0x0a, 0x7f, 0x06, 0x64,
	0x70, 0x02, 0x01, 0x06, 0x07, 0x00, 0x00, };

struct gr_mode {
	char scr_color;
	int scr_off;
} gr_mode;

faddr_t gr_basaddr;

grinit ()
{
	int sel;

	sel = dscralloc();
	mmudescr (sel, MBASE, SIZE-1, DSA_DATA);
	gr_basaddr = sotofar (sel, 0);
}

gropen ()
{
	gr_mode.scr_color = 0x00;
	gr_mode.scr_off = 0x00;
}

grclose ()
{
	gr_mode.scr_color = 0x00;
	gr_setcolor ();
	gr_setscrn (0);
}

grread ()
{
	gr_rw (FREAD);
}

grwrite ()
{
	gr_rw (FWRITE);
}

gr_rw (mode)
{
	register extent;
	int count;

	extent = SIZE - gr_mode.scr_off;
	if (extent <= 0) {
		u.u_error = EIO;
		return;
	}

	if (extent < u.u_count)
		count = extent;
	else
		count = u.u_count;

	if (mode == FREAD)
		grcopy (gr_basaddr + gr_mode.scr_off, u.u_base, count);
	else
		grcopy (u.u_base, gr_basaddr + gr_mode.scr_off, count);

	u.u_count -= count;
	gr_mode.scr_off += count;
}

grioctl (dev, cmd, arg, mode)
faddr_t arg;
{
	switch (cmd) {
	case 'a':               /* select color palette */
		if ((int) arg)
			gr_mode.scr_color |= ACOL;
		else
			gr_mode.scr_color &= ~ACOL;
		gr_setcolor ();
		break;
	case 'b':               /* set background color */
		gr_mode.scr_color &= 0xf0;
		gr_mode.scr_color |= ((int) arg) & 0x0f;
		gr_setcolor ();
		break;
	case 'c':               /* clear screen */
		grclear (gr_basaddr, SIZE);
		break;
	case 'g':               /* set graphics mode */
		gr_setscrn (1);
		break;
	case 'i':               /* set background intensity */
		if ((int) arg)
			gr_mode.scr_color |= HLIN;
		else
			gr_mode.scr_color &= ~HLIN;
		gr_setcolor ();
		break;
	case 's':               /* seek read/write position */
		if ((unsigned) arg >= SIZE) {
			u.u_error = EINVAL;
			return;
		}
		gr_mode.scr_off = (unsigned) arg;
		break;
	case 't':               /* set text mode */
		gr_setscrn (0);
		break;
	}
}

gr_setcolor ()
{
	outb (COLOR, gr_mode.scr_color);
}

gr_setscrn (mode)
{
	register i, s;

	s = spl5 ();
	for (i=0; i<=0x0d; ++i) {
		outb (INDEX, i);
		outb (DATA, mode ? grgraf_mode[i] : grchar_mode[i]);
	}
	outb (MODE, mode ? SCN_GRF : SCN_TXT);
	splx (s);
}

grcopy (from, to, count)
register faddr_t from;
register faddr_t to;
{
	while (--count >= 0)
		*to++ = *from++;
}

grclear (to, count)
register faddr_t to;
{
	while (--count >= 0)
		*to++ = 0;
}
