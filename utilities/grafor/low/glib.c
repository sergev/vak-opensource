/*
 *      GOpen (scrflag, maxx, maxy)
 *              - open graph library. Set color number 1, set current
 *              x and y positions to 0.
 *              If scrflag is 0, draw to metafile.
 *              If scrflag is 1, try to use screen, else draw to metafile.
 *              If scrflag is 2, draw to stdout as a printer.
 *              If scrflag is 3, draw to stdout as epson.
 *              If scrflag is 4, draw to stdout as laserjet.
 *              If scrflag is 5, draw to stdout using BGL (BENSON, etc)
 *              If scrflag is 6, draw to stdout using HP-GL (HP plotters)
 *              If scrflag is 7, draw to stdout using REGIS (vt240, vt340)
 *              If scrflag is 100, run under XWindows
 *              Return 1 on success, else 0.
 *
 *      GClose ()
 *              - close graph library. Don't close metafile,
 *              next drawes will append it.
 *
 *      GExit ()
 *              - do final actions just before exit.
 *
 *      GReverse ()
 *              - reverse axes.
 *
 *      GFile (name)
 *      char *name;
 *              - set metafile name. Close previus metafile.
 *              Return 1 on success.
 *
 *      GMove (x, y)
 *              - move pen to specified position.
 *
 *      GPlot (x, y)
 *              - draw line from current to specified position.
 *
 *      GColor (n)
 *              - set color. Return 1 on success.
 *
 *      GPaint (n, x, y)
 *              - paint region from point (x, y) with color n.
 *
 *      GComment (str)
 *      char *str;
 *              - print comment to metafile.
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#ifdef __MSDOS__
#   define NSIG		23
#endif
#define SIGFUN		void

#define LASERJET
#define EPSON

#define SLMETA		0
#define SLCGI		1
#define SLTEK		2
#define SLWYSE		3
#define SLJET		4
#define SLEPS		5
#define SLBGL		6
#define SLHPGL		7
#define SLDOS		8
#define SLRGS		9
#define SLXW		10

#define DMAXX		32767
#define DMAXY		32767

#define DCOLOR		1       /* default color */
#define MAXCOLOR	16    /* max color number */

#define MARKCOLOR(n)	plotcmap [(n-1)/8] |= 1 << ((n-1)%8)
#define COLORUSED(n)	(plotcmap [(n-1)/8] & 1 << ((n-1)%8))

#define DMAP		10000

struct swtch {
	char tty;
	char plot;
	int (*open) ();
	int (*close) ();
	int (*move) ();
	int (*cont) ();
	int (*color) ();
	int (*paint) ();
	int (*sync) ();
	int (*exit) ();
};

struct point {
	short x, y;
};

static struct swtch *sw;                /* ptr to graph function table */
static maxx, maxy;                      /* draw space */
static curx, cury;                      /* current position */
static lastx, lasty;                    /* last plotted position */
static shiftx, shifty;                  /* shifts for centering page */
static limx, limy;                      /* hardware page size limits */
static long mapx, mapy;                 /* map scale factors */
static color = DCOLOR;                  /* current color */
static openflag;                        /* graphics enabled */
static reverse, revflag;                /* graphics enabled */
static FILE *metafile;                  /* metafile descriptor */
static SIGFUN (*sigsave [NSIG]) ();     /* save signal cathers */
static struct point *plotline;          /* line buffer for plotters */
static plotlen;                         /* length of plotline malloc'ed */
static plotnext;                        /* next free point in plotline */
static plotcmap [(MAXCOLOR+7)/8];       /* scale of colors used */
static int cgilibrary (void);
static int laserjet (void);
static int epson (void);
static int tektronix (void);
static int wyse99 (void);
static int regis (void);

static Mopen (), Mclose (), Mmove (), Mcont (), Mcolor (), Mpaint ();
static Dopen (), Dclose (), Dmove (), Dcont (), Dcolor (), Dpaint ();
static Copen (), Cclose (), Cmove (), Ccont (), Ccolor (), Csync ();
static Topen (), Tclose (), Tmove (), Tcont ();
static Wopen (), Wclose ();
static Hopen (), Hclose (), Hmove (), Hcont ();
static Eopen (), Eclose (), Emove (), Econt ();
static Bopen (), Bclose (), Bmove (), Bcont (), Bcolor ();
static Lopen (), Lclose (), Lmove (), Lcont (), Lcolor ();
static Ropen (), Rclose (), Rmove (), Rcont (), Rcolor ();
static Xopen (), Xclose (), Xmove (), Xcont (), Xcolor (), Xexit ();

static plotalloc ()
{
	if (plotline) {
		plotlen += 64;
		plotline = (struct point *) realloc ((char *) plotline,
			plotlen * sizeof (struct point));
	} else {
		plotlen = 64;
		plotline = (struct point *) malloc (plotlen * sizeof (struct point));
	}
	if (! plotline) {
		fprintf (stderr, "metaplot: Out of memory.\n");
		exit (1);
	}
}

static plotpoint (x, y)
{
	if (plotnext >= plotlen)
		plotalloc ();
	plotline[plotnext].x = x;
	plotline[plotnext++].y = y;
}

static initmap ()
{
	mapx = (long) limx * DMAP / maxx;
	mapy = (long) limy * DMAP / maxy;
	if (mapx > mapy) {
		mapx = mapy;
		shiftx += (limx - (long) maxx * limy/maxy) / 2;
	} else {
		mapy = mapx;
		shifty += (limy - (long) maxy * limx/maxx) / 2;
	}
}

static map (x, y)
register *x, *y;
{
	register t;

	if (reverse) {
		t = *x;
		*x = maxx - *y;
		*y = t;
	}
	if (*x < 0)
		*x = 0;
	else if (*x > maxx)
		*x = maxx;
	if (*y < 0)
		*y = 0;
	else if (*y > maxy)
		*y = maxy;
	*x = (long) *x * mapx / DMAP + shiftx;
	*y = (long) *y * mapy / DMAP + shifty;
}

static sigignor ()
{
	register i;

	for (i=1; i<NSIG; ++i)
		signal (i, SIG_IGN);
}

static SIGFUN sigcatch ()
{
	sigignor ();
	if (openflag) {
		(*sw->close) ();
		openflag = 0;
	}
	exit (1);
}

static siginit ()
{
	register i;

	for (i=1; i<NSIG; ++i)
		sigsave [i] = (SIGFUN (*) ()) signal (i, sigcatch);
}

static sigrestore ()
{
	register i;

	for (i=1; i<NSIG; ++i)
		signal (i, sigsave [i]);
}

static hitkey ()
{
# ifdef __MSDOS__
	switch (bioskey (0)) {
	case 0:    	/* Ctrl Break */
	case 283:	/* Esc */
	case 11779:	/* Ctrl C */
	case 21248:	/* Delete */
		sigcatch ();
	default:
		break;
	}
# else
	register d;
	char c;

	if (isatty (0))
		fgetc (stdin);
	else if ((d = open ("/dev/tty", 0)) >= 0) {
		read (d, &c, 1);
		close (d);
	}
# endif
}

static struct swtch selector [] = {
	/* metafile */  0,      0,
	Mopen,  Mclose, Mmove,  Mcont,  Mcolor, Mpaint, 0,      0,

	/* CGI */       1,      0,
	Copen,  Cclose, Cmove,  Ccont,  Ccolor, 0,      Csync,  0,

	/* Tek 4014 */  1,      0,
	Topen,  Tclose, Tmove,  Tcont,  0,      0,      0,      0,

	/* Wyse 99 */   1,      0,
	Wopen,  Wclose, Tmove,  Tcont,  0,      0,      0,      0,

	/* HP LJet */   0,      0,
	Hopen,  Hclose, Hmove,  Hcont,  0,      0,      0,      0,

	/* Epson 80 */  0,      0,
	Eopen,  Eclose, Emove,  Econt,  0,      0,      0,      0,

	/* BGL */       0,      1,
	Bopen,  Bclose, Bmove,  Bcont,  Bcolor, 0,      0,      0,

	/* HP-GL */     0,      1,
	Lopen,  Lclose, Lmove,  Lcont,  Lcolor, 0,      0,      0,

	/* MSDOS */     1,      0,
	Dopen,  Dclose, Dmove,  Dcont,  Dcolor, Dpaint, 0,      0,

	/* REGIS */     1,      0,
	Ropen,  Rclose, Rmove,  Rcont,  Rcolor, 0,      0,      0,

	/* XWindows */  0,      0,
	Xopen,  Xclose, Xmove,  Xcont,  Xcolor, 0,      0,      Xexit,
};

GOpen (scrflag, mx, my)
{
	if (reverse = revflag) {
		register t;

		t = mx;
		mx = my;
		my = t;
	}
	maxx = mx>0 && mx<=DMAXX ? mx : DMAXX;
	maxy = my>0 && my<=DMAXY ? my : DMAXY;
	sw = 0;
	switch (scrflag) {
	case 100:
		sw = selector + SLXW;
		break;
	case 7:
		sw = selector + SLRGS;
		break;
	case 6:
		sw = selector + SLHPGL;
		break;
	case 5:
		sw = selector + SLBGL;
		break;
	case 4:
		sw = selector + SLJET;
		break;
	case 3:
		sw = selector + SLEPS;
		break;
	case 2:
		/* detect type of printer */
		if (laserjet ())
			sw = selector + SLJET;
		else if (epson ())
			sw = selector + SLEPS;
		else
			return (0);
		break;
	case 1:
		if (! isatty (1))
			break;
		/* detect type of screen graphics */
# ifdef __MSDOS__
		if (msdos ())
			sw = selector + SLDOS;
		else
# endif
# ifdef XWINDOWS
		if (xwindows ())
			sw = selector + SLXW;
		else
# endif
		if (tektronix ())
			sw = selector + SLTEK;
		else if (regis ())
			sw = selector + SLRGS;
		else if (wyse99 ())
			sw = selector + SLWYSE;
		else if (cgilibrary ())
			sw = selector + SLCGI;
		break;
	case 0:
		sw = selector + SLMETA;
		break;
	}
	if (! sw)
		sw = selector + SLMETA;
	curx = cury = 0;        /* reset current point */
	lastx = lasty = -1;     /* forget last plotted point */
	mapx = mapy = DMAP;
	shiftx = shifty = 0;
	if (sw->plot) {
		register c;

		for (c=0; c<(MAXCOLOR+7)/8; ++c)
			plotcmap [c] = 0;
		if (plotline)
			free ((char *) plotline);
		plotlen = plotnext = 0;
	}
	if (! (*sw->open) (&limx, &limy))
		return (0);
	if (sw->color)
		if (sw->plot) {
			plotpoint (-1, color);
			MARKCOLOR (color);      /* color used */
		} else
			(*sw->color) (color);
	if (sw != selector + SLMETA) {
		initmap ();
		if (sw->tty)
			siginit ();
	}
	openflag = 1;
	return (1);
}

GClose ()
{
	if (! openflag)
		return;
	if (sw->plot) {
		register c;

		for (c=1; c<=MAXCOLOR; ++c)
			if (COLORUSED (c))
				plotcolor (c);
	}
	if (sw->sync)
		(*sw->sync) ();
	if (sw != selector+SLMETA) {
		fflush (stdout);
		if (sw->tty) {
			hitkey ();
			sigrestore ();
		}
	}
	(*sw->close) ();
	openflag = 0;
}

GReverse ()
{
	revflag ^= 1;
}

GFile (name)
char *name;
{
	if (openflag)
		return (0);
	if (metafile && metafile != stdout)
		fclose (metafile);
	metafile = fopen (name, "w");
	return (metafile != 0);
}

GMove (x, y)
{
	if (! openflag)
		return;
	map (&x, &y);
	curx = x;
	cury = y;
}

GPlot (x, y)
{
	if (! openflag)
		return;
	map (&x, &y);
	if (curx != lastx || cury != lasty) {
		if (sw->plot) {
			plotpoint (-1, -1);
			plotpoint (curx, cury);
		} else
			(*sw->move) (curx, cury);
	}
	if (sw->plot)
		plotpoint (x, y);
	else
		(*sw->cont) (x, y);
	curx = lastx = x;
	cury = lasty = y;
}

GColor (n)
{
	if (n > MAXCOLOR || color == n)
		return;
	if (! openflag) {
		color = n;
		return;
	}
	if (sw->plot) {
		plotpoint (-1, n);
		MARKCOLOR (n);          /* color used */
		plotpoint (curx, cury);
		return;
	}
	if (sw->color && (*sw->color) (n))
		color = n;
}

GComment (s)
char *s;
{
	if (! openflag || ! metafile)
		return;
	putc (';', metafile);
	while (*s)
		putc (*s++, metafile);
	putc ('\n', metafile);
}

GPaint (n, x, y)
{
	if (! openflag || sw->plot || ! sw->paint)
		return;
	map (&x, &y);
	(*sw->paint) (n, x, y);
}

GExit ()
{
	if (sw->exit)
		(*sw->exit) ();
}

plotcolor (c)
{
	register struct point *p;
	register pen = 0;

# define NEXTPOINT if (++p >= plotline+plotnext) return

	p = plotline;
	for (;;) {
		/* find color number c */
		while (p->x >= 0 || p->y != c)
			NEXTPOINT;
		do {
			/* plot polyline */
			NEXTPOINT;
			if (p->x < 0)
				continue;
			(*sw->move) (p->x, p->y);
			NEXTPOINT;
			while (p->x >= 0 && p->y >= 0) {
				if (! pen) {
					(*sw->color) (c);
					pen = 1;
				}
				(*sw->cont) (p->x, p->y);
				NEXTPOINT;
			}
		} while (p->y < 0);
	}
# undef NEXTPOINT
}

/*********************** CGI ROUTINES *************************/

# ifdef CGI
# define LINESZ 40      /* number of points in line */

static short devhandle, savary [66];
static short line [2*LINESZ];
static next;

static cgilibrary ()
{
	/* check if cgi library present */
	return (getenv ("VDIPATH") && getenv ("DISPLAY"));
}

static Copen (lx, ly)
int *lx, *ly;
{
	static short savin [20] =
		{1,1,1,3,1,1,1,0,0,1,1,'D','I','S','P','L','A','Y',' '};

	/* open the workstation and save output in savary array*/
	if (v_opnwk (savin, &devhandle, savary) < 0)
		return (0);
	next = 0;
	*lx = savary [51];      /* maximum width of screen */
	*ly = savary [52];      /* maximum height of screen */
	return (1);
}

static Cclose ()
{
	v_clswk (devhandle);
}

static Ccolor (n)
{
	Cmove (curx, cury);
	if (vsl_color (devhandle, n) < 0)
		return (0);
	return (1);
}

static Csync ()
{
	if (next > 3)
		v_pline (devhandle, next/2, line);
	next = 0;
}

static Cmove (x, y)
{
	Csync ();
	line [next++] = x;
	line [next++] = y;
}

static Ccont (x, y)
{
	line [next++] = x;
	line [next++] = y;
	if (next >= 2*LINESZ)
		Cmove (x, y);
}
# else /* CGI */
		static cgilibrary () { return (0); }
/* VARARGS */   static Copen () { return (0); }
		static Cclose () {}
		static Csync () {}
/* VARARGS */   static Ccolor () { return (0); }
/* VARARGS */   static Cmove () {}
/* VARARGS */   static Ccont () {}
# endif /* CGI */

/*********************** HP LaserJet **************************/

# ifdef LASERJET

# define Hxsz           1000            /* X axis size in pixels */
# define Hysz           1500            /* Y axis size in pixels */
# define Hdotsz         2               /* size of point in pixels */

# define Hmapsz         ((Hxsz+Hdotsz+6)/8)

# define Hsetdot(x,y)   Hfield [(x)>>3] [Hysz-(y)-1] |= 1 << (7-((x)&7))

static char **Hfield;
static Hx, Hy;

static laserjet ()
{
	register char *prn;

	return ((prn = getenv ("PRINTER")) && ! strcmp (prn, "laserjet"));
}

static Hopen (lx, ly)
int *lx, *ly;
{
	register i, n;

	if (! (Hfield = (char **) calloc (Hmapsz+1, sizeof (char *))))
		return (0);
	for (i=0; i<Hmapsz+1; ++i)
		if (! (Hfield [i] = calloc (Hysz+Hdotsz-1, 1))) {
			for (n=0; n<i; ++n)
				free (Hfield [n]);
			free ((char *) Hfield);
			return (0);
		}
	*lx = Hxsz-1;           /* maximum width of screen */
	*ly = Hysz-1;           /* maximum height of screen */
	Hmove (0, 0);
	return (1);
}

static Hputline (line)
register line;
{
	register len, i;

	for (len=Hmapsz; len>1; --len)
		if (Hfield [len-1] [line])
			break;
	printf ("\033*b%dW", len);
	for (i=0; i<len; ++i)
		putchar (Hfield [i] [line]);
}

static Hclose ()
{
	register i;

	printf ("\033*t150R");                  /* 150 dot per inch */
	/* printf ("\033*t300R");               /* 300 dot per inch */
	printf ("\033*p150x150Y");              /* left and right margins */
	printf ("\033*r1A");                    /* relative addressing */
	for (i=0; i<Hysz; ++i)
		Hputline (i);
	printf ("\033*rB");                     /* exit from graphics */
	putchar ('\f');                         /* form feed */

	for (i=0; i<Hmapsz+1; ++i)
		free (Hfield [i]);
	free ((char *) Hfield);
}

static Hsetpix (x, y)
register x, y;
{
	Hsetdot (x, y);
	Hsetdot (x, y-1);
	++x;
	Hsetdot (x, y);
	Hsetdot (x, y-1);
}

static Hmove (x, y)
{
	Hx = x;
	Hy = y;
}

static Hcont (x1, y1)
{
	register x, y;
	int a, b, D, dx, dy, dd1, dd2, dd3;

	x = Hx;
	y = Hy;
	b = x1 - x;
	a = y1 - y;
	if (a < 0)
		a = -a;
	if (b > 0)
		b = -b;
	dx = (x > x1) ? -1 : 1;
	dy = (y > y1) ? -1 : 1;
	D = 0;
	while (x != x1 || y != y1) {
		Hsetpix (x, y);
		dd1 = D + a;
		dd2 = D + b;
		dd3 = dd1 + b;
		if (dd1 < 0)
			dd1 = -dd1;
		if (dd2 < 0)
			dd2 = -dd2;
		if (dd3 < 0)
			dd3 = -dd3;
		if (dd1 < dd2) {
			if (dd3 < dd1)
				x += dx, y += dy, D += a+b;
			else
				x += dx, D += a;
		} else {
			if (dd3 < dd2)
				x += dx, y += dy, D += a+b;
			else
				y += dy, D += b;
		}
	}
	Hsetpix (x1, y1);
	Hmove (x1, y1);
}

# else /* LASERJET */
/* VARARGS */   static Hopen () { return (0); }
		static Hclose () {}
/* VARARGS */   static Hmove () {}
/* VARARGS */   static Hcont () {}
# endif /* LASERJET */

/*********************** Epson 80 *****************************/

# ifdef EPSON

# define Exsz           400             /* X axis size in pixels */
# define Eysz           600             /* Y axis size in pixels */
# define Epin           7               /* number of pins */

# define Emapsz         ((Eysz+Epin-1)/Epin)

# define Esetpix(x,y)   Efield [(y)/Epin] [x] |= 1 << ((y)%Epin)

static char **Efield;
static Ex, Ey;

static epson ()
{
	register char *prn;

	return (! (prn = getenv ("PRINTER")) || ! strcmp (prn, "epson"));
}

static Eopen (lx, ly)
int *lx, *ly;
{
	register i, n;

	if (! (Efield = (char **) calloc (Emapsz+1, sizeof (char *))))
		return (0);
	for (i=0; i<Emapsz+1; ++i)
		if (! (Efield [i] = calloc (Exsz, 1))) {
			for (n=0; n<i; ++n)
				free (Efield [n]);
			free ((char *) Efield);
			return (0);
		}
	*lx = Exsz-1;           /* maximum width of screen */
	*ly = Eysz-1;           /* maximum height of screen */
	Emove (0, 0);
	return (1);
}

static Eclose ()
{
	register i, maxi, l;

	putchar (033);
	putchar ('A');
	putchar (Epin);                 /* PINS*1/72" shift */
	putchar (033);
	putchar ('E');                  /* font ELITA (1/12") */
	for (l=Emapsz-1; l>=0; --l) {
		for (maxi=Exsz-1; maxi>=0; --maxi)
			if (Efield [l] [maxi])
				break;
		if (maxi >= 0) {
			++maxi;
			if (maxi & 0200)
				maxi = (maxi + 0400) & ~0377;
			putchar (033);
			putchar ('K');
			/* putchar ('*'); */
			/* putchar (5); */                      /* 1/72" */
			putchar (maxi);
			putchar (maxi >> 8);
			for (i=0; i<maxi; ++i)
				if (i < Exsz)
					putchar (Efield [l] [i]);
				else
					putchar (0);
		}
		putchar (012);
		putchar (015);
	}
	putchar ('\f');

	for (i=0; i<Emapsz+1; ++i)
		free (Efield [i]);
	free ((char *) Efield);
}

static Emove (x, y)
{
	Ex = x;
	Ey = y;
}

static Econt (x1, y1)
{
	register x, y;
	int a, b, D, dx, dy, dd1, dd2, dd3;

	x = Ex;
	y = Ey;
	b = x1 - x;
	a = y1 - y;
	if (a < 0)
		a = -a;
	if (b > 0)
		b = -b;
	dx = (x > x1) ? -1 : 1;
	dy = (y > y1) ? -1 : 1;
	D = 0;
	while (x != x1 || y != y1) {
		Esetpix (x, y);
		dd1 = D + a;
		dd2 = D + b;
		dd3 = dd1 + b;
		if (dd1 < 0)
			dd1 = -dd1;
		if (dd2 < 0)
			dd2 = -dd2;
		if (dd3 < 0)
			dd3 = -dd3;
		if (dd1 < dd2) {
			if (dd3 < dd1)
				x += dx, y += dy, D += a+b;
			else
				x += dx, D += a;
		} else {
			if (dd3 < dd2)
				x += dx, y += dy, D += a+b;
			else
				y += dy, D += b;
		}
	}
	Esetpix (x1, y1);
	Emove (x1, y1);
}

# else /* EPSON */
/* VARARGS */   static Eopen () { return (0); }
		static Eclose () {}
/* VARARGS */   static Emove () {}
/* VARARGS */   static Econt () {}
# endif /* EPSON */

/*********************** METAFILE PRINTING ********************/

static Mopen (lx, ly)
int *lx, *ly;
{
	if (! metafile && isatty (1) && (metafile = fopen ("grafor.mtf", "w")))
		fprintf (stderr, "metaplot: plotting to file grafor.mtf\n");
	if (! metafile)
		metafile = stdout;
	fprintf (metafile, "(%d,%d\n", maxx, maxy);
	*lx = DMAXX;
	*ly = DMAXY;
	return (1);
}

static Mclose ()
{
	fprintf (metafile, ")\n");
}

static Mcolor (n)
{
	fprintf (metafile, "!%d\n", n);
	return (1);
}

static Mmove (x, y)
{
	fprintf (metafile, "=%d,%d\n", x, y);
}

static Mcont (x, y)
{
	fprintf (metafile, "-%d,%d\n", x, y);
}

static Mpaint (n, x, y)
{
	fprintf (metafile, "@%d,%d,%d\n", x, y, n);
}

/*********************** TEKTRONIX 4014 ***********************/

# define TEKMAXX 4095
# define TEKMAXY 3119

# define PUTCH(c) putc (c, stdout)

static oloy = -1;
static ohiy = -1;
static ohix = -1;
static oextra = -1;

static tektronix ()
{
	register char *term;
	register char **p;
	static char *tab [] = {
		"tek4014", "4014", "tek4014sm", "4014sm", 0,
	};

	/* ckeck if stdout is tektronix 4014 */
	if (! (term = getenv ("TERM")))
		return (0);
	for (p=tab; *p; ++p)
		if (! strcmp (term, *p))
			return (1);
	return (0);
}

static Tclear ()
{
	/* erase */
	PUTCH (033);
	PUTCH (014);
	ohiy = -1;
	ohix = -1;
	oextra = -1;
	oloy = -1;
}

static Topen (lx, ly)
int *lx, *ly;
{
	*lx = TEKMAXX;
	*ly = TEKMAXY;
	Tclear ();
	return (1);
}

static Tclose ()
{
	PUTCH (037);
}

static Tmove (xi, yi)
{
	PUTCH (035);
	Tcont (xi, yi);
}

static Tcont (x, y)
{
	register hix, hiy, lox, loy, extra;

	hix = (x>>7) & 037;
	hiy = (y>>7) & 037;
	lox = (x>>2) & 037;
	loy = (y>>2) & 037;
	extra = x & 03 + (y<<2) & 014;

	if (hiy != ohiy) {
		PUTCH (hiy | 040);
		ohiy = hiy;
	}
	if (hix != ohix) {
		if (extra != oextra) {
			PUTCH (extra | 0140);
			oextra = extra;
		}
		PUTCH (loy | 0140);
		PUTCH (hix | 040);
		ohix = hix;
		oloy = loy;
	} else {
		if (extra != oextra) {
			PUTCH (extra | 0140);
			PUTCH (loy | 0140);
			oextra = extra;
			oloy = loy;
		} else if (loy != oloy) {
			PUTCH (loy | 0140);
			oloy = loy;
		}
	}
	PUTCH (lox | 0100);
}

/*********************** WYSE 99 ******************************/

static wyse99 ()
{
	register char *term;

	/* ckeck if stdout is tektronix 4014 */
	if (! (term = getenv ("TERM")))
		return (0);
	return (! strncmp (term, "wyse99", 6) ||
		! strncmp (term, "wy99", 4));
}

static Wopen (lx, ly)
int *lx, *ly;
{
	PUTCH (033);
	PUTCH ('[');
	PUTCH ('?');
	PUTCH ('3');
	PUTCH ('8');
	PUTCH ('h');
	return (Topen (lx, ly));
}

static Wclose ()
{
	Tclose ();
	PUTCH (033);
	PUTCH ('[');
	PUTCH ('?');
	PUTCH ('3');
	PUTCH ('8');
	PUTCH ('l');
}

/*********************** BGL (BENSON) *************************/

# define Bmaxx 8000                     /* 40 sm */
# define Bmaxy 6000                     /* 30 sm */

static Bopen (lx, ly)
int *lx, *ly;
{
	*lx = Bmaxx;
	*ly = Bmaxy;
	printf ("BP 1 0 18\n");         /* begin plotting */
	printf ("DE 2 1\n");            /* set 20 lines/mm */
	printf ("OP 0 11\n");           /* set message length */
	printf ("VI 11 KIAE-GRAFOR\n"); /* send message */
	return (1);
}

static Bclose ()
{
	printf ("EP 1 0 18\n");         /* end plotting */
}

static Bcolor (n)
{
	if (n<1 || n>16)
		return (0);
	/* printf ("NP %d\n", n-1); */
	return (1);
}

static Bmove (x, y)
{
	printf ("MA %d %d\n", x, y);
}

static Bcont (x, y)
{
	printf ("DA %d %d\n", x, y);
}

/*********************** HP-GL (HP plotters) ******************/

# define Lmaxx 10600
# define Lmaxy 7600

static Lpen;            /* 1 if pen down */

static char Lcolormap [7] = {
	1,      /* BLACK */
	2,      /* RED */
	3,      /* GREEN */
	5,      /* BLUE */
	4,      /* YELLOW */
	7,      /* CYAN */
	6,      /* MAGENTA */
};

static Lopen (lx, ly)
int *lx, *ly;
{
	*lx = Lmaxx;
	*ly = Lmaxy;

	/* set logical i/o buffer size to 1024 bytes, 0-12800 allowed */
	/* enable hardware handshake (bit 0, mask 01) */
	/* no back responce (bit 1, mask 02) */
	printf ("\033.@1024;3:\n");

	/* initialize plotter */
	printf ("IN\n");

	/* home */
	printf ("PA 0,0\n");

	/* define window limits */
	printf ("IW 0,0,%d,%d\n", Lmaxx+1, Lmaxy+1);

	/* no pen */
	printf ("SP\n");

	Lpen = 0;               /* pen up */
	return (1);
}

static Lclose ()
{
	/* no pen */
	printf ("SP\n");

	/* undefine window */
	printf ("IW\n");

	/* pen up */
	printf ("PU\n");

	/* home */
	printf ("PA 0,0\n");

	/* page feed */
	printf ("PG;\n");
}

static Lcolor (n)
{
	if (n<1 || n>7)
		return (0);
	printf ("SP %d\n", Lcolormap [n-1]);
	return (1);
}

static Lmove (x, y)
{
	if (Lpen) {
		printf ("PU\n");
		Lpen = 0;
	}
	printf ("PA %d,%d\n", x, y);
}

static Lcont (x, y)
{
	if (! Lpen) {
		printf ("PD\n");
		Lpen = 1;
	}
	printf ("PA %d,%d\n", x, y);
}

/*********************** MSDOS ******************************/

# ifdef __MSDOS__
static msdos () { return (1); }

# include <bios.h>
# include <graphics.h>

/* # define DEBUG */

static Dmaxx=1023, Dmaxy=1023;
static char Dcolormap [16] = {
	BLACK,
	WHITE,
	LIGHTRED,
	LIGHTGREEN,
	LIGHTBLUE,
	YELLOW,
	LIGHTCYAN,
	LIGHTMAGENTA,
	DARKGRAY,
	LIGHTGRAY,
	RED,
	GREEN,
	BLUE,
	BROWN,
	CYAN,
	MAGENTA,
};

static Dopen (lx, ly)
int *lx, *ly;
{
	int drv, mode, xasp, yasp;

	registerbgidriver (CGA_driver);
	registerbgidriver (EGAVGA_driver);
	registerbgidriver (Herc_driver);

# ifdef DEBUG
	printf ("initgraph\n");
# else
	detectgraph (&drv, &mode);
	initgraph (&drv, &mode, (char *) 0);
	Dmaxx = getmaxx ();
	Dmaxy = getmaxy ();
	getaspectratio (&xasp, &yasp);
# endif
	*lx = Dmaxx;
	*ly = Dmaxy;
	return (1);
}

static Dclose ()
{
# ifdef DEBUG
	printf ("endgraph\n");
# else
	closegraph ();
# endif
}

static Dcolor (n)
{
# ifdef DEBUG
	printf ("setcolor %d\n", n);
# else
	setcolor (Dcolormap [n]);
# endif
	return (1);
}

static Dpaint (n, x, y)
{
# ifdef DEBUG
	printf ("paint %d, %d, %d\n", x, y, n);
# else
	setfillstyle (SOLID_FILL, Dcolormap [n]);
	floodfill (x, Dmaxy-y, Dcolormap [n]);
# endif
}

static Dmove (x, y)
{
# ifdef DEBUG
	printf ("moveto %d %d\n", x, y);
# else
	moveto (x, Dmaxy-y);
# endif
}

static Dcont (x, y)
{
# ifdef DEBUG
	printf ("lineto %d %d\n", x, y);
# else
	lineto (x, Dmaxy-y);
# endif
}

# else /* MSDOS */
		static msdos () { return (0); }
/* VARARGS */   static Dopen () { return (0); }
		static Dclose () {}
/* VARARGS */   static Dcolor () { return (0); }
/* VARARGS */   static Dpaint () {}
/* VARARGS */   static Dmove () {}
/* VARARGS */   static Dcont () {}
# endif /* MSDOS */

/*********************** REGIS ********************************/

# define Rmaxx 799
# define Rmaxy 479

static char Rcolormap [16] = {
	0,      /* BLACK */
	15,     /* WHITE */
	2,      /* LIGHTRED */
	7,      /* LIGHTGREEN */
	1,      /* LIGHTBLUE */
	6,      /* YELLOW */
	5,      /* LIGHTCYAN */
	4,      /* LIGHTMAGENTA */
	10,     /* DARKGRAY */
	3,      /* LIGHTGRAY */
	8,      /* RED */
	11,     /* GREEN */
	9,      /* BLUE */
	14,     /* BROWN */
	13,     /* CYAN */
	12,     /* MAGENTA */
};

static regis ()
{
	register char *term;
	register char **p;
	static char *tab [] = {
		"vt220", "vt240", "vt300", "vt330", "vt340",
		"VT220", "VT240", "VT300", "VT330", "VT340",
		0,
	};

	/* ckeck if stdout is REGIS terminal */
	if (! (term = getenv ("TERM")))
		return (0);
	for (p=tab; *p; ++p)
		if (! strcmp (term, *p))
			return (1);
	return (0);
}

static Ropen (lx, ly)
int *lx, *ly;
{
	*lx = Rmaxx;
	*ly = Rmaxy;
	printf ("\033Pp\n");
	printf ("S(M0(AH0L0S0))\n");
	printf ("S(M1(AH0L50S60))\n");
	printf ("S(M2(AH120L46S72))\n");
	printf ("S(M7(AH240L50S60))\n");
	printf ("S(M4(AH60L50S60))\n");
	printf ("S(M5(AH300L50S60))\n");
	printf ("S(M6(AH180L50S60))\n");
	printf ("S(M3(AH0L53S0))\n");
	printf ("S(M10(AH0L26S0))\n");
	printf ("S(M9(AH0L46S29))\n");
	printf ("S(M8(AH120L43S39))\n");
	printf ("S(M11(AH240L46S29))\n");
	printf ("S(M12(AH60L46S29))\n");
	printf ("S(M13(AH300L46S29))\n");
	printf ("S(M14(AH180L46S29))\n");
	printf ("S(M15(AH0L80S0))\n");
	printf ("S(C0)S(I0)S(E)W(V)\n");
	Rmove (0, 0);
	return (1);
}

static Rclose ()
{
	printf ("S(C1)\033\\\n\033[H\033[J");
}

static Rcolor (n)
{
	if (n<1 || n>15)
		return (0);
	printf ("W(I%d)\n", Rcolormap [n]);
	return (1);
}

static Rmove (x, y)
{
	printf ("P[%d,%d]\n", x, Rmaxy-y);
}

static Rcont (x, y)
{
	printf ("V[%d,%d]\n", x, Rmaxy-y);
}

/*********************** X Windows *****************************/

# ifdef XWINDOWS
# include <X11/Xlib.h>

# define XWIDTH 100             /* default window width */
# define XHEIGHT 60             /* default window height */
# define XWIN   32              /* maximum number of windows */
# define XLINE  64              /* maximum length of polyline */
# define XCOLORS 8              /* number of colors */

static Display *Xdpy;           /* current display */
static Xscreen;                 /* current screen */
static Xn;                      /* number of windows */

struct {
	Window win;             /* window descriptor */
	XPoint *line;           /* array of image points */
	int len;                /* length of line malloc'ed */
	int next;               /* next free point in line */
	int maxx, maxy;         /* page size */
	int wid, hgh;           /* old window size */
} X [XWIN], *Xc;

static GC Xgc [XCOLORS];        /* colors */

static char *Xcolorname [XCOLORS] = {
	"#000", "#fff", "#f00", "#0f0", "#00f", "#ff0", "#0ff", "#f0f",
};

static Xmapx, Xmapy;            /* scale coeffitients */
static Xshiftx, Xshifty;        /* image shift */
static Xwmm, Xhmm;              /* display size in millimeters */
static Xw, Xh;                  /* display size in pixels */

static xwindows ()
{
	register char *term;

	/* ckeck if we are running under X Windows */
	if (! getenv ("DISPLAY"))
		return (0);
	if (! (term = getenv ("TERM")))
		return (0);
	return (! strncmp (term, "xterm", 6) ||
		! strncmp (term, "xpcterm", 6) ||
		! strncmp (term, "xt", 4));
}

static GC Xgetgc (name)
char *name;
{
	XGCValues v;
	XColor c;

	XParseColor (Xdpy, DefaultColormap (Xdpy, Xscreen), name, &c);
	XAllocColor (Xdpy, DefaultColormap (Xdpy, Xscreen), &c);
	v.foreground = c.pixel;
	return (XCreateGC (Xdpy, RootWindow (Xdpy, Xscreen), GCForeground, &v));
}

static Xopen (lx, ly)
int *lx, *ly;
{
	if (Xn >= XWIN)
		return (0);
	if (! Xdpy) {
		XGCValues gcvalues;

		if (! (Xdpy = XOpenDisplay ((char *) 0)))
			return (0);
		Xscreen = DefaultScreen (Xdpy);
		Xh = DisplayHeight (Xdpy, Xscreen);
		Xw = DisplayWidth (Xdpy, Xscreen);
		Xhmm = DisplayHeightMM (Xdpy, Xscreen);
		Xwmm = DisplayWidthMM (Xdpy, Xscreen);
	}
	X[Xn].maxx = *lx = maxx;        /* maximum width of screen */
	X[Xn].maxy = *ly = maxy;        /* maximum height of screen */
	return (1);
}

static Xclose ()
{
	Xcont (-1, -1);
	++Xn;
}

static Xcolor (n)
{
	if (n<1 || n>7)
		return (0);
	Xcont (-1, n);
	Xcont (curx, cury);
	return (1);
}

static Xmove (x, y)
{
	Xcont (-1, -1);
	Xcont (x, y);
}

static Xcont (x, y)
{
	if (X[Xn].next >= X[Xn].len)
		Xalloc ();
	X[Xn].line[X[Xn].next].x = x;
	X[Xn].line[X[Xn].next++].y = y;
}

static Xalloc ()
{
	if (X[Xn].line) {
		X[Xn].len += 64;
		X[Xn].line = (XPoint *) realloc ((char *) X[Xn].line,
			X[Xn].len * sizeof (XPoint));
	} else {
		X[Xn].len = 64;
		X[Xn].line = (XPoint *) malloc (X[Xn].len * sizeof (XPoint));
	}
	if (! X[Xn].line) {
		fprintf (stderr, "xmetaplot: Out of memory.\n");
		exit (1);
	}
}

static Xexit ()
{
	Window w;
	register i;
	XEvent event;
	char name [10];

	for (i=0; i<XCOLORS; ++i)
		Xgc [i] = Xgetgc (Xcolorname [i]);
	for (Xc=X; Xc<X+Xn; ++Xc) {
		Xc->win = XCreateSimpleWindow (Xdpy, RootWindow (Xdpy, Xscreen),
			10, 10, XWIDTH, XHEIGHT, 6,
			WhitePixel (Xdpy, Xscreen), BlackPixel (Xdpy, Xscreen));
		sprintf (name, "grafor%d", Xc-X+1);
		XStoreName (Xdpy, Xc->win, name);
		XMapWindow (Xdpy, Xc->win);
		XSelectInput (Xdpy, Xc->win, ExposureMask |
			StructureNotifyMask | ButtonPressMask);
	}
	while (Xn) {
		XNextEvent (Xdpy, &event);
		w = event.xany.window;
		for (i=0; i<Xn; ++i)
			if (X[i].win == w)
				break;
		if (i >= Xn)
			continue;
		Xc = X + i;
		switch (event.type) {
		case ButtonPress:
			if (event.xbutton.button == Button3) {
				XDestroyWindow (Xdpy, w);
				if (Xc->line)
					free ((char *) Xc->line);
				--Xn;
				if (i < Xn)
				*Xc = X[Xn];
			}
			continue;
		case ConfigureNotify:
			Xc->wid = event.xconfigure.width;
			Xc->hgh = event.xconfigure.height;
			continue;
		case Expose:
			if (! event.xexpose.count)
				Xdraw ();
			continue;
		}
	}
}

static Xsetmap (width, height)
{
	int wmm, hmm;

	wmm = (long) Xwmm * width / Xw;
	hmm = (long) Xhmm * height / Xh;
	if ((long) wmm * Xc->maxy > (long) hmm * Xc->maxx) {
		Xmapy = height;
		Xmapx = (long) Xmapy * Xc->maxx / Xc->maxy;
		Xshiftx = (width - Xmapx) / 2;
		Xshifty = 0;
	} else {
		Xmapx = width;
		Xmapy = (long) Xmapx * Xc->maxy / Xc->maxx;
		Xshifty = (height - Xmapy) / 2;
		Xshiftx = 0;
	}
}

static XPoint Xmap (t)
XPoint t;
{
	t.x = (long) t.x * Xmapx / Xc->maxx + Xshiftx;
	t.y = (long) (Xc->maxy - t.y) * Xmapy / Xc->maxy + Xshifty;
	return (t);
}

static Xdraw ()
{
	register i, c;
	register XPoint *p;
	XPoint polyline [XLINE];

	Xsetmap (Xc->wid, Xc->hgh);
	i = 0;
	c = 1;
	XClearWindow (Xdpy, Xc->win);
	for (p=Xc->line; p<Xc->line+Xc->next; ++p) {
		if (p->x < 0) {
			if (i > 1)
				/* draw polyline */
				XDrawLines (Xdpy, Xc->win, Xgc [c],
					polyline, i, CoordModeOrigin);
			i = 0;
			if (p->y > 0)
				c = p->y;
		} else {
			if (i >= XLINE) {
				XDrawLines (Xdpy, Xc->win, Xgc [c],
					polyline, i, CoordModeOrigin);
				polyline[0] = polyline[i-1];
				i = 1;
			}
			polyline[i++] = Xmap (*p);
		}
	}
}
# else /* XWINDOWS */
/* VARARGS */   static Xopen () { return (0); }
/* VARARGS */   static Xcolor () { return (0); }
/* VARARGS */   static Xmove () {}
/* VARARGS */   static Xcont () {}
		static Xclose () {}
		static Xexit () {}
# endif /* XWINDOWS */
