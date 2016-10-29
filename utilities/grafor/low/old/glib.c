/*
 *      GOpen (scrflag, maxx, maxy)
 *              - open graph library. Set color number 1, set current
 *              x and y positions to 0.
 *              If scrflag is 0, draw to metafile.
 *              If scrflag is 1, try to use screen, else draw to metafile.
 *              If scrflag is 2, draw to stdout as a printer.
 *              If scrflag is 3, draw to stdout as epson.
 *              If scrflag is 4, draw to stdout as laserjet.
 *              Return 1 on success, else 0.
 *
 *      GClose ()
 *              - close graph library. Don't close metafile,
 *              next drawes will append it.
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

# include <stdio.h>
# include <signal.h>

# define LASERJET
# define EPSON

# define SLMETA 0
# define SLCGI  1
# define SLTEK  2
# define SLWYSE 3
# define SLJET  4
# define SLEPS  5

# define DMAXX  32767
# define DMAXY  32767

# define DCOLOR 1       /* default color */

# define DMAP   10000

struct swtch {
	int tty;
	int (*open) ();
	int (*close) ();
	int (*move) ();
	int (*cont) ();
	int (*color) ();
	int (*paint) ();
	int (*sync) ();
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
static (*sigsave [NSIG]) ();

static Mopen (), Mclose (), Mmove (), Mcont (), Mcolor (), Mpaint ();
static Copen (), Cclose (), Cmove (), Ccont (), Ccolor (), Csync ();
static Topen (), Tclose (), Tmove (), Tcont ();
static Wopen (), Wclose ();
static Hopen (), Hclose (), Hmove (), Hcont ();
static Eopen (), Eclose (), Emove (), Econt ();

static struct swtch selector [] = {
    0,  Mopen,  Mclose, Mmove,  Mcont,  Mcolor, Mpaint, 0,      /* metafile */
    1,  Copen,  Cclose, Cmove,  Ccont,  Ccolor, 0,      Csync,  /* CGI */
    1,  Topen,  Tclose, Tmove,  Tcont,  0,      0,      0,      /* Tek 4014 */
    1,  Wopen,  Wclose, Tmove,  Tcont,  0,      0,      0,      /* Wyse 99 */
    0,  Hopen,  Hclose, Hmove,  Hcont,  0,      0,      0,      /* HP LJet */
    0,  Eopen,  Eclose, Emove,  Econt,  0,      0,      0,      /* Epson 80 */
};

extern char *getenv (), *calloc ();

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
	case 4:
		sw = selector + SLJET;
		break;
	case 3:
		sw = selector + SLEPS;
		break;
	case 2:
		/* detect type of screen graphics */
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
		if (tektronix ())
			sw = selector + SLTEK;
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
	if (! (*sw->open) (&limx, &limy))
		return (0);
	if (sw->color)
		(*sw->color) (color);
	curx = cury = 0;        /* reset current point */
	lastx = lasty = -1;     /* forget last plotted point */
	mapx = mapy = DMAP;
	shiftx = shifty = 0;
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
	if (curx != lastx || cury != lasty)
		(*sw->move) (curx, cury);
	(*sw->cont) (x, y);
	curx = lastx = x;
	cury = lasty = y;
}

GColor (n)
{
	if (color == n)
		return;
	color = n;
	if (openflag && sw->color)
		(*sw->color) (n);
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
	if (! openflag || ! sw->paint)
		return;
	map (&x, &y);
	(*sw->paint) (n, x, y);
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

static hitkey ()
{
	register d;
	char c;

	if (isatty (0))
		fgetc (stdin);
	else if ((d = open ("/dev/tty", 0)) >= 0) {
		read (d, &c, 1);
		close (d);
	}
}

static sigcatch ()
{
	sigignore ();
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
		sigsave [i] = (int (*) ()) signal (i, sigcatch);
}

static sigignore ()
{
	register i;

	for (i=1; i<NSIG; ++i)
		signal (i, SIG_IGN);
}

static sigrestore ()
{
	register i;

	for (i=1; i<NSIG; ++i)
		signal (i, sigsave [i]);
}

/*********************** CGI ROUTINES *************************/

# ifdef CGI
# define LINESZ 40      /* number of points in line */

static short devhandle, savary [66];
static short line [2*LINESZ];
static next;

static cgilibrary ()
{
	/* ckeck if cgi library present */
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

static Topen (lx, ly)
int *lx, *ly;
{
	*lx = TEKMAXX;
	*ly = TEKMAXY;
	Tclear ();
	return (1);
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
		! strncmp (term, "wy99", 4) ||
		! strncmp (term, "xterm", 4));
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
