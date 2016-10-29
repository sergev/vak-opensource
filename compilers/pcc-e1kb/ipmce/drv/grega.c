# include "../h/types.h"
# include "../h/param.h"
# include "../h/sysmacros.h"
# include "../h/dir.h"
# include "../h/signal.h"
# include "../h/user.h"
# include "../h/mmu.h"
# include "../h/file.h"
# include "../h/errno.h"

# define MBASE  0xA0000         /* video memory address */

# define NMODES 7               /* number of modes */
# define QSZ    512             /* size of write queue, must power of 2 */

/* color EGA port addresses */

# define CRTController                  0x3d4
# define CRTC_HorTotal                  0x00
# define CRTC_HorDisplayEnableEnd       0x01
# define CRTC_StartHorBlanking          0x02
# define CRTC_EndHorBlanking            0x03
# define CRTC_StartHorRetrace           0x04
# define CRTC_EndHorRetrace             0x05
# define CRTC_VertTotal                 0x06
# define CRTC_Overflow                  0x07
# define CRTC_PresetRowScan             0x08
# define CRTC_MaxScanLineAddress        0x09
# define CRTC_CursorStart               0x0a
# define CRTC_CursorEnd                 0x0b
# define CRTC_StartAddressHigh          0x0c
# define CRTC_StartAddressLow           0x0d
# define CRTC_CursorLocationHigh        0x0e
# define CRTC_CursorLocationLow         0x0f
# define CRTC_VertRetraceStart          0x10
# define CRTC_LightPenHigh              0x10
# define CRTC_VertRetraceEnd            0x11
# define CRTC_LightPenLow               0x11
# define CRTC_VertDisplayEnableEnd      0x12
# define CRTC_Offset                    0x13
# define CRTC_LogicalLineWidth          0x13
# define CRTC_UnderlineLocation         0x14
# define CRTC_StartVerticalBlanking     0x15
# define CRTC_EndVerticalBlanking       0x16
# define CRTC_ModeControl               0x17
# define CRTC_LineCompare               0x18

# define CRTStatus                      0x3da

# define Sequencer                      0x3c4
# define S_Reset                        0
# define S_ClockingMode                 1
# define S_MapMask                      2
# define S_CharMapSelect                3
# define S_MemoryMode                   4

# define GraphicsController             0x3ce
# define GC_SetReset                    0
# define GC_EnableSetReset              1
# define GC_ColorCompare                2
# define GC_DataRotate                  3
# define GC_FunctionSelect              3
# define GC_ReadMapSelect               4
# define GC_GraphicsMode                5
# define GC_Miscellaneous               6
# define GC_ColorDontCare               7
# define GC_BitMask                     8

# define AttributeController            0x3c0
# define AC_Palette                     0x00    /* 0x0 - 0xf */
# define AC_ModeControl                 0x10
# define AC_OverscanColor               0x11
# define AC_ColorPlaneEnable            0x12
# define AC_HorPixelPanning             0x13
# define AC_ColorSelect                 0x14

# define SETREG(r,n,v) (outb (r, n), outb (r+1, v))

# define MAXX   640
# define MAXY   350

char gr_ginit [9] = {
	/* Graphics Controller data */
	0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x05,   0x0f,   0xff,
};

char gr_g0init [9] = {
	/* Graphics Controller data for text mode */
	0x00,   0x00,   0x00,   0x00,   0x00,   0x10,   0x0e,   0x00,   0xff,
};

char gr_sinit [5] = {
	/* Sequencer data */
	0x00,   0x01,   0x0f,   0x00,   0x06,
};

char gr_s0init [5] = {
	/* Sequencer data for text mode */
	0x00,   0x01,   0x03,   0x00,   0x03,
};

char gr_clock [NMODES] = {
	/* clock rate */
	0xa7,   0xa7,   0xa7,   0xab,   0x23,   0xab,   0xab,
};

char gr_ac0init [20] = {
	/* AC Data for standard text mode 80 x 25 */
	0x00,   0x01,   0x02,   0x03,   0x04,   0x05,   0x14,   0x07,
	0x38,   0x39,   0x3a,   0x3b,   0x3c,   0x3d,   0x3e,   0x3f,
	0x00,   0x00,   0x0f,   0x00,
};

char gr_ac4init [20] = {
	/* CRTC Data for standard IBM mode E 640x200 */
	0x00,   0x01,   0x02,   0x03,   0x04,   0x05,   0x06,   0x07,
	0x10,   0x11,   0x12,   0x13,   0x14,   0x15,   0x16,   0x17,
	0x01,   0x00,   0x0f,   0x00,
};

char gr_acinit [20] = {
	/* CRTC Data for other modes */
	0x00,   0x01,   0x02,   0x03,   0x04,   0x05,   0x06,   0x07,
	0x38,   0x39,   0x3a,   0x3b,   0x3c,   0x3d,   0x3e,   0x3f,
	0x01,   0x00,   0x0f,   0x00,
};

char gr_crtinit [NMODES] [25] = {
	/* CRTC Data for standard text mode 80 x 25 */
	{
	0x5b,   0x4f,   0x53,   0x37,   0x51,   0x5b,   0x6c,   0x1f,
	0x00,   0x0d,   0x00,   0x07,   0x00,   0x00,   0x00,   0x00,
	0x5e,   0x2b,   0x5d,   0x28,   0x0e,   0x5e,   0x0a,   0xb3,
	0xff,
	},
	/* CRTC Data for standard IBM mode 640 x 350 */
	{
	0x5b,   0x4f,   0x53,   0x37,   0x52,   0x00,   0x6c,   0x1f,
	0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
	0x5e,   0x2b,   0x5d,   0x28,   0x0f,   0x5f,   0x0a,   0xe3,
	0xff,
	},
	/* CRTC Data for std. EGA and NEC monitor 512 x 430 */
	{
	0x4a,   0x3f,   0x43,   0x25,   0x41,   0x05,   0xc8,   0x1f,
	0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
	0xae,   0x05,   0xad,   0x20,   0x0f,   0xaf,   0x05,   0xe3,
	0xff,
	},
	/* CRTC Data for 19.3 Mhz EGA and NEC monitor 512 x 512 */
	{
	0x4e,   0x3f,   0x42,   0x28,   0x41,   0x08,   0x0a,   0x1d,
	0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
	0x00,   0x06,   0xff,   0x20,   0x0f,   0x00,   0x0a,   0xe7,
	0xff,
	},
	/* CRTC Data for standard IBM mode E 640x200 */
	{
	0x70,   0x4f,   0x59,   0x2d,   0x5e,   0x06,   0x04,   0x11,
	0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
	0xe0,   0x23,   0xc7,   0x28,   0x00,   0xdf,   0xef,   0xe3,
	0xff,
	},
	/* CRTC Data for 24 Mhz EGA and NEC monitor 640 x 480 */
	{
	0x64,   0x4f,   0x53,   0x21,   0x53,   0x00,   0xf0,   0x1f,
	0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
	0xe0,   0x2c,   0xdf,   0x28,   0x0f,   0xe1,   0x0c,   0xe3,
	0xff,
	},
	/* CRTC Data for 24 Mhz EGA and NEC monitor 752 x 480 */
	{
	0x76,   0x5d,   0x61,   0x31,   0x61,   0x0d,   0xa8,   0x1f,
	0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,
	0x9a,   0x26,   0x99,   0x2f,   0x0f,   0x9b,   0x08,   0xe3,
	0xff,
	},
};

struct gr {
	char active;            /* driver is opened */
	char queue [QSZ];       /* write queue */
	int qoff;               /* first free in queue */
	int qtop;               /* top of queue */
	int curx, cury;         /* current cursor position */
	int xul, yul, xlr, ylr; /* clipping rectangle */
	int color;              /* current color */
} gr;

struct point {
	short x;
	short y;
};

faddr_t gr_video;

grinit ()
{
	int sel;

	sel = dscralloc ();
	mmudescr (sel, MBASE, (unsigned) 65535, DSA_DATA);
	gr_video = sotofar (sel, 0);
}

gropen ()
{
	/* if device already opened - error */

	if (gr.active) {
		u.u_error = EIO;
		return;
	}

	/* set graphics mode 640x350, 16 colors */

	gr_setscrn (1);

	/* mark driver active */

	gr.active = 1;

	/* free write queue */

	gr.qoff = 0;

	/* set cursor position to upper left corner */

	gr.curx = gr.cury = 0;

	/* set color */

	gr.color = 017;

	/* set clipping rectangle to full screen */

	gr.xul = 0;
	gr.yul = 0;
	gr.xlr = MAXX-1;
	gr.ylr = MAXY-1;
}

grclose ()
{
	gr_setscrn (0);
	gr.active = 0;
}

grioctl (dev, cmd, arg, mode)
faddr_t arg;
{
}

grread ()
{
}

grwrite ()
{
	register count;

	while (u.u_count) {

		/* append request to queue */

		count = QSZ-gr.qoff < u.u_count ? QSZ-gr.qoff : u.u_count;
		gr_copy (u.u_base, (faddr_t) gr.queue+gr.qoff, count);
		gr.qoff += count;
		u.u_count -= count;
		u.u_base += count;

		/* exec request, shift rest of queue, update qoff */

		gr_exec ();
	}
}

gr_exec ()
{
	register e;

	for (gr.qtop=0; gr.qtop<gr.qoff; ++gr.qtop) {
		switch (gr.queue [gr.qtop]) {
		default:
			continue;
		case 'f':               /* fill screen with pattern */
			gr_fill (0xff);
			continue;
		case 'c':               /* clear screen */
			gr_fill (0);
			continue;
		case '!':               /* set color */
			if (! gr_color ())
				break;
			continue;
		case '-':               /* draw line */
			if (! gr_plot ())
				break;
			continue;
		case '=':               /* move current position */
			if (! gr_move ())
				break;
			continue;
		}
		break;
	}
	e = gr.qoff;
	for (gr.qoff=0; gr.qtop<e; ++gr.qtop, ++gr.qoff)
		gr.queue [gr.qoff] = gr.queue [gr.qtop];
}

gr_setscrn (mode)       /* switch adapter mode */
register mode;
{
	register i;
	register char *p;
	int s;

	if (mode<0 || mode>=NMODES)
		mode = 0;               /* use text on unknown mode */

	/* turn CRT off */

	SETREG (CRTController, 0, 0);

	/* initialize Attribute Controller */

	inb (CRTStatus);
	p = mode==0 ? gr_ac0init : mode==4 ? gr_ac4init : gr_acinit;
	for (i=0; i<20; ++i)            /* set Attribute Controller registers */
		SETREG (AttributeController, i, p [i]);

	/* set clock register */

	outb (0x3c2, gr_clock [mode]);

	/* initialize Sequencer */

	SETREG (Sequencer, 0, 1);       /* syncronous halt */
	p = mode==0 ? gr_s0init : gr_sinit;
	s = spl5 ();                    /* disable interrupts */
	for (i=1; i<5; ++i)            /* set Sequencer registers */
		SETREG (Sequencer, i, p [i]);
	SETREG (Sequencer, 0, 3);       /* start sequencer */
	splx (s);                       /* restore priority level */

	/* initialize CRT Controller */

	for (i=0; i<25; ++i)            /* set CRT Controller registers */
		SETREG (CRTController, i, gr_crtinit [mode] [i]);

	outb (0x3cc, 0);                /* set Graphics Position 1 */
	outb (0x3ca, 1);                /* set Graphics Position 2 */

	/* initialize Graphics Controller */

	p = mode==0 ? gr_g0init : gr_ginit;
	for (i=0; i<9; ++i)             /* set Graphics Controller registers */
		SETREG (GraphicsController, i, p [i]);

	/* clear screen */

	/* turn display on */

	inb (CRTStatus);
	outb (AttributeController, 0x20);
}

gr_fill (a)
{
	register i;
	register long f;
	struct zero { long l [40]; } zero;

	a &= 0377;
	f = a<<24 | a<<16 | a<<8 | a;
	for (i=0; i<40; ++i)
		zero.l[i] = f;
	for (i=0; i<MAXY/2; ++i)
		((struct zero far *) gr_video) [i] = zero;
}

gr_color ()
{
	char c;

	if (! gr_getpar (&c, 1))
		return (0);
	gr.color = c & 017;
	return (1);
}

gr_move ()
{
	struct point p;

	if (! gr_getpar ((char *) &p, (int) sizeof (p)))
		return (0);
	if (p.x<0 || p.x>=MAXX || p.y<0 || p.y>=MAXY)
		return (1);
	gr.curx = p.x;
	gr.cury = p.y;
	return (1);
}

gr_plot ()
{
	struct point p;

	if (! gr_getpar ((char *) &p, (int) sizeof (p)))
		return (0);
	if (p.x<0 || p.x>=MAXX || p.y<0 || p.y>=MAXY)
		return (1);
	gr_cont (p.x, p.y);
	gr.curx = p.x;
	gr.cury = p.y;
	return (1);
}

gr_getpar (s, n)        /* get parameter from queue, if no, return 0 */
register char *s;       /* else update qtop */
{
	register i;

	for (i=gr.qtop+1; --n>=0 && i<gr.qoff; ++i)
		*s++ = gr.queue [i];
	if (n < 0) {
		gr.qtop += n;
		return (1);
	} else
		return (0);
}

gr_copy (from, to, n)
register faddr_t from;
register faddr_t to;
{
	while (--n >= 0)
		*to++ = *from++;
}

gr_clip (x1, y1, x2, y2)        /* clip ends of line to clipping region */
register *x1, *y1, *x2, *y2;
{
	union {
		struct {
			unsigned c0 : 1;        /* x < xul */
			unsigned c1 : 1;        /* y < yul */
			unsigned c2 : 1;        /* x > xlr */
			unsigned c3 : 1;        /* y > ylr */
		} c;
		int i;
	} oc1, oc2;
	int inside, outside;
	register t;

	oc1.i = oc2.i = 0;

	oc2.c.c0 = *x2 < gr.xul;
	oc2.c.c1 = *y2 < gr.yul;
	oc2.c.c2 = *x2 > gr.xlr;
	oc2.c.c3 = *y2 > gr.ylr;

	for (;;) {
		oc1.c.c0 = *x1 < gr.xul;
		oc1.c.c1 = *y1 < gr.yul;
		oc1.c.c2 = *x1 > gr.xlr;
		oc1.c.c3 = *y1 > gr.ylr;

		inside = (oc1.i | oc2.i) == 0;
		outside = (oc1.i & oc2.i) != 0;

		if (outside || inside)
			return (inside);

		if (oc1.i == 0) {
			t = *x1, *x1 = *x2, *x2 = t;
			t = *y1, *y1 = *y2, *y2 = t;
			t = oc1.i, oc1.i = oc2.i, oc2.i = t;
		}
		if (oc1.c.c0) {
			*y1 += (long) (*y2-*y1) * (gr.xul-*x1) / (*x2-*x1);
			*x1 = gr.xul;
		} else if (oc1.c.c1) {
			*x1 += (long) (*x2-*x1) * (gr.yul-*y1) / (*y2-*y1);
			*y1 = gr.yul;
		} else if (oc1.c.c2) {
			*y1 += (long) (*y2-*y1) * (gr.xlr-*x1) / (*x2-*x1);
			*x1 = gr.xlr;
		} else if (oc1.c.c3) {
			*x1 += (long) (*x2-*x1) * (gr.ylr-*y1) / (*y2-*y1);
			*y1 = gr.ylr;
		}
	}
}

gr_cont (x2, y2)
{
	register dx, dy;
	int x1 = gr.curx, y1 = gr.cury;

	if (! gr_clip (&x1, &y1, &x2, &y2))
		return;
	dx = x2 - x1;
	gr_setmode (0, 2);              /* set write mode 2 */
	outb (GraphicsController, GC_BitMask);
	if ((dy = y2-y1) == 0) {
		/* draw horisontal line */
		if (dx == 0)
			gr_setpix (x1, y1);
		else if (dx < 0)
			gr_horiz (x2, y1, -dx+1);
		else
			gr_horiz (x1, y1, dx+1);
		return;
	}
	if (dx < 0) dx = -dx;
	if (dy < 0) dy = -dy;
	if (dx < dy)
		gr_liney (x1, y1, x2, y2);
	else
		gr_linex (x1, y1, x2, y2);
	gr_setmode (0, 0);              /* restore write mode */
}

gr_linex (x1, y1, x2, y2)
{
	register x, d, y;
	int Aincr, Bincr, yincr;
	int dx, dy;

	if (x1 > x2) {
		x = x1, x1 = x2, x2 = x;
		y = y1, y1 = y2, y2 = y;
	}
	yincr = y2>y1 ? 1 : -1;

	dx = x2 - x1;
	dy = yincr>0 ? y2-y1 : y1-y2;           /* dy = abs (y2 - y1) */
	d = 2*dy - dx;

	Aincr = 2 * (dy - dx);
	Bincr = 2 * dy;

	gr_setpix (x1, y1);

	y = y1;
	for (x=x1+1; x<=x2; ++x) {
		if (d >= 0)
			d += Aincr, y += yincr;
		else
			d += Bincr;
		gr_setpix (x, y);
	}
}

gr_liney (x1, y1, x2, y2)
{
	register x, d, y;
	int Aincr, Bincr, xincr;
	int dx, dy;

	if (y1 > y2) {
		x = x1, x1 = x2, x2 = x;
		y = y1, y1 = y2, y2 = y;
	}
	xincr = x2>x1 ? 1 : -1;

	dy = y2 - y1;
	dx = xincr>0 ? x2-x1 : x1-x2;           /* dy = abs (y2 - y1) */
	d = 2*dx - dy;

	Aincr = 2 * (dx - dy);
	Bincr = 2 * dx;

	gr_setpix (x1, y1);

	x = x1;
	for (y=y1+1; y<=y2; ++y) {
		if (d >= 0)
			d += Aincr, x += xincr;
		else
			d += Bincr;
		gr_setpix (x, y);
	}
}

gr_horiz (x, y, n)
register x, n;
{
	for (; --n>=0; ++x)
		gr_setpix (x, y);
}

gr_setpix (x, y)
{
	register char far *p;

	p = ((char far *) gr_video) + y*80 + (x>>3);
	outb (GraphicsController+1, 0200 >> (x&7));
	gr_poke (p, gr.color);
}

gr_setmode (r, w)
{
	outb (GraphicsController, GC_GraphicsMode);
	outb (GraphicsController+1, (r&1) << 3 | (w&3));
}
