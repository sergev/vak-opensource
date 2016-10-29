# include <time.h>
# include "jlib.h"

# define Jvideomem ((short far *) 0xb8000000L)

# define SCRSZ (80*25)

# define DELAY 10
# define FREQ  500
# define FREQSTEP 120

# define CRTCAddress	0x3d4
# define CRTCData	0x3d5

# define CNORM		0x1b	/* light cyan on blue */
# define CBOLD		0x1f	/* light white on blue */
# define CDIM		0x0f	/* light white on black */
# define CRNORM		0x13	/* light cyan on cyan */
# define CRBOLD		0x4e	/* yellow on red */
# define CRDIM		0x70	/* black on white */

# define BWNORM		0x07	/* white on black */
# define BWBOLD		0x0f	/* light white on black */
# define BWDIM		0x05	/* magenta on black */
# define BWRNORM	0x57	/* white on magenta */
# define BWRBOLD	0x5f	/* light white on magenta */
# define BWRDIM		0x70	/* black on white */

int Jx, Jy;
int Jattr = CNORM;		/* light cyan on blue */

static norm = CNORM;
static bold = CBOLD;
static dim = CDIM;
static rnorm = CRNORM;
static rbold = CRBOLD;
static rdim = CRDIM;

static blank = CNORM<<8|' ';	/* light cyan space on blue */
static lblank = CNORM<<8|' ';	/* light cyan space on blue */

static short oldscreen [SCRSZ];
static oldcursor;

static char linedraw [11] = {
	205,    /* 0    horisontal line */
	186,    /* 1    vertical line */
	200,    /* 2    lower left corner */
	202,    /* 3    lower center */
	188,    /* 4    lower right corner */
	204,    /* 5    left center */
	206,    /* 6    center cross */
	185,    /* 7    right center */
	201,    /* 8    upper left corner */
	203,    /* 9    upper center */
	187,    /* 10   upper right corner */
};

extern char *malloc ();

char *mallo (n)
{
	register char *p;

	p = malloc (n);
	if (! p) {
		printf ("Out of memory !");
		exit (-1);
	}
	return (p);
}

mfree (p, n)
char *p;
{
	if (n)
		free (p);
}

Jclear ()
{
	struct z80 { short i [80]; } z80;
	register i;

	for (i=0; i<80; ++i)
		z80.i[i] = blank;
	for (i=0; i<25; ++i)
		((struct z80 far *) Jvideomem) [i] = z80;
	Jx = Jy = 0;
}

Jscrool (r1, r2, n)
{
	struct z80 { short i [80]; } z80;
	register r;

	for (r=0; r<80; ++r)
		z80.i[r] = blank;
	if (n > 0) {
		for (r=r2-n; r>=r1; --r)
			((struct z80 far *) Jvideomem) [r+n] =
				((struct z80 far *) Jvideomem) [r];
		for (r=r1+n-1; r>=r1; --r)
			((struct z80 far *) Jvideomem) [r] = z80;
	} else {
		for (r=r1-n; r<=r2; ++r)
			((struct z80 far *) Jvideomem) [r+n] =
				((struct z80 far *) Jvideomem) [r];
		for (r=r2+n+1; r<=r2; ++r)
			((struct z80 far *) Jvideomem) [r] = z80;
	}
}

Jputch (c)
{
	register short far *p;

	c &= 0xff;
	switch (c) {
	case '\b':
		if (Jx)
			--Jx;
		return;
	case '\r':
		Jx = 0;
		return;
	case '\n':
		if (Jy < 24)
			++Jy;
		return;
	case '\t':
		Jx = (Jx + 8) / 8 * 8;
		if (Jx >= 80)
			Jx = 79;
		return;
	}
	p = Jvideomem + (Jy*80 + Jx);
	*p = c | Jattr << 8;
	if (Jx < 79)
		++Jx;
}

Jmove (r, c)
{
	if (r>=0 && r<25)
		Jy = r;
	if (c>=0 && c<80)
		Jx = c;
}

Jprintf (s, a, b, c, d)
char *s;
long a, b, c, d;
{
	char buf [90];

	sprintf (buf, s, a, b, c, d);
	Jputstr (buf);
}

Jmprintf (row, col, s, a, b, c, d)
char *s;
long a, b, c, d;
{
	char buf [90];

	Jmove (row, col);
	sprintf (buf, s, a, b, c, d);
	Jputstr (buf);
}

Jputstr (s)
register char *s;
{
	while (*s)
		Jputch (*s++);
}

Jopen ()
{
	oldcursor = Jgetcursor ();
	Jsetcursor (-1);
	savescreen ();
}

Jclose ()
{
	restorescreen ();
	Jsetcursor (oldcursor);
}

Jnorm ()	{ Jattr = norm; }
Jbold ()	{ Jattr = bold; }
Jdim ()		{ Jattr = dim; }
Jrnorm ()	{ Jattr = rnorm; }
Jrbold ()	{ Jattr = rbold; }
Jrdim ()	{ Jattr = rdim; }

Jrev ()
{
	if (norm == BWNORM)
		switch (Jattr) {
		case BWNORM:	Jattr = BWRNORM;	break;
		case BWBOLD:	Jattr = BWRBOLD;	break;
		case BWDIM:	Jattr = BWRDIM;		break;
		}
	else
		switch (Jattr) {
		case CNORM:	Jattr = CRNORM;		break;
		case CBOLD:	Jattr = CRBOLD;		break;
		case CDIM:	Jattr = CRDIM;		break;
		}
}

Junrev ()
{
	if (norm == BWNORM)
		switch (Jattr) {
		case BWRNORM:	Jattr = BWNORM;		break;
		case BWRBOLD:	Jattr = BWBOLD;		break;
		case BWRDIM:	Jattr = BWDIM;		break;
		}
	else
		switch (Jattr) {
		case CRNORM:	Jattr = CNORM;		break;
		case CRBOLD:	Jattr = CBOLD;		break;
		case CRDIM:	Jattr = CDIM;		break;
		}
}

Jswitchcolor ()
{
	if (norm == BWNORM) {
		norm = CNORM;
		bold = CBOLD;
		dim = CDIM;
		rnorm = CRNORM;
		rbold = CRBOLD;
		rdim = CRDIM;
		lblank = CNORM << 8 | ' ';
	} else {
		norm = BWNORM;
		bold = BWBOLD;
		dim = BWDIM;
		rnorm = BWRNORM;
		rbold = BWRBOLD;
		rdim = BWRDIM;
		lblank = BWRBOLD << 8 | ' ';
	}
	blank = norm << 8 | ' ';
}

Jattrib (c)
{
	Jattr = c & 0xff;
}

Jcolor (c)
{
	Jattr &= ~0xf;
	Jattr |= c & 0xf;
}

Jbackground (c)
{
	Jattr &= 0xf;
	Jattr |= (c & 0xf) << 4;
}

# ifdef JDEBUG
main ()
{
	register c;

	printf ("before Jopen ...");
	getch ();
	Jopen ();
	Jclear ();
	Jmove (12, 40);
	Jprintf ("Hello, world !");
	getch ();
	Jclose ();
	printf ("after Jopen ...");
	getch ();
	return (0);
}
# endif /* JDEBUG */

Jgetcursor ()
{
	register c;

	outportb (CRTCAddress, 0xe);	/* curcor location high */
	c = inportb (CRTCData) << 8;
	outportb (CRTCAddress, 0xf);	/* curcor location low */
	c |= inportb (CRTCData) & 0xff;
	return (c);
}

Jsetcursor (c)
register c;
{
	outportb (CRTCAddress, 0xe);	/* curcor location high */
	outportb (CRTCData, c >> 8);
	outportb (CRTCAddress, 0xf);	/* curcor location low */
	outportb (CRTCData, c);
}

static savescreen ()
{
	register short far *s, far *p;
	register i, hz;

	s = (short far *) oldscreen + SCRSZ-1;
	p = Jvideomem + SCRSZ-1;
	while (s >= oldscreen)
		*s-- = *p--;

	s = (short far *) oldscreen;
	p = Jvideomem;
	hz = FREQ;
	/* center points */
	p [SCRSZ/2-1] = lblank;
	p [SCRSZ/2] = lblank;

	for (i=1; i<=12; ++i) {
		sound (hz);
		hz = (long) hz * FREQSTEP / 100;
		drawrect (p, i);
		mydelay (DELAY);
	}
	nosound ();
	for (i=0; i<3; ++i) {
		vertline (p, i, 0, 24);
		vertline (p, i+77, 0, 24);
	}
}

static restorescreen ()
{
	register short far *s, far *p;
	register i, hz;

	s = (short far *) oldscreen;
	p = Jvideomem;

	hz = FREQ;
	for (i=11; i>0; --i)
		hz = (long) hz * FREQSTEP / 100;
	for (i=0; i<3; ++i) {
		restvertline (p, s, i, 0, 24);
		restvertline (p, s, i+77, 0, 24);
	}
	for (i=12; i>0; --i) {
		sound (hz);
		hz = (long) hz * 100 / FREQSTEP;
		mydelay (DELAY);
		restrect (p, s, i);
	}
	nosound ();
	/* center points */
	p [SCRSZ/2-1] = s [SCRSZ/2-1];
	p [SCRSZ/2] = s [SCRSZ/2];
}

static vertline (p, c, r, rf)
register short far *p;
register r;
{
	register i;

	for (i=r*80+c; r<=rf; ++r, i+=80)
		p [i] = lblank;
}

static horline (p, r, c, cf)
register short far *p;
register c;
{
	for (p+=r*80+c; c<=cf; ++c, ++p)
		*p = lblank;
}

static resthorline (p, s, r, c, cf)
register short far *p, far *s;
register c;
{
	for (p+=r*80+c, s+=r*80+c; c<=cf; ++c, ++p, ++s)
		*p = *s;
}

static restvertline (p, s, c, r, rf)
register short far *p, far *s;
register r;
{
	for (p+=r*80+c, s+=r*80+c; r<=rf; ++r, p+=80, s+=80)
		*p = *s;
}

static drawrect (p, i)
register short far *p;
register i;
{
	register n;

	horline (p, 12-i, 42-3*i, 37+3*i);
	horline (p, 12+i, 42-3*i, 37+3*i);
	for (n=0; n<3; ++n) {
		vertline (p, 41-3*i-n, 12-i, 12+i);
		vertline (p, 38+3*i+n, 12-i, 12+i);
	}
}

static restrect (p, s, i)
register short far *p, far *s;
register i;
{
	register n;

	resthorline (p, s, 12-i, 42-3*i, 37+3*i);
	resthorline (p, s, 12+i, 42-3*i, 37+3*i);
	for (n=0; n<3; ++n) {
		restvertline (p, s, 41-3*i-n, 12-i, 12+i);
		restvertline (p, s, 38+3*i+n, 12-i, 12+i);
	}
}

mydelay (n)
register n;
{
	register i, z;

	while (--n >= 0)
		for (z=i=200; --i>=0; z*=z);
}

JBOX *Jgetbox (y, x, ny, nx)
{
	register xx, yy;
	register short far *p;
	register short *q;
	JBOX *box;

	box = (JBOX *) mallo ((int) (sizeof (JBOX)));
	box->y = y;
	box->x = x;
	box->ny = ny;
	box->nx = nx;
	box->mem = (short *) mallo ((int) (ny * nx * sizeof (short)));

	for (yy=0; yy<ny; ++yy) {
		p = Jvideomem + ((yy+y)*80 + x);
		q = & box->mem [yy*nx];
		for (xx=0; xx<nx; ++xx)
			*q++ = *p++;
	}
	return (box);
}

Jungetbox (box)
JBOX *box;
{
	register xx, yy;
	register short far *p;
	register short *q;

	for (yy=0; yy<box->ny; ++yy) {
		q = & box->mem [yy * box->nx];
		p = Jvideomem + ((yy+box->y)*80 + box->x);
		for (xx=0; xx<box->nx; ++xx)
			*p++ = *q++;
	}
}

Jprintbox (box)
JBOX *box;
{
	register xx, yy;
	register short far *p;
	register short *q;

	for (yy=0; yy<box->ny; ++yy) {
		q = & box->mem [yy * box->nx];
		p = Jvideomem + ((yy+box->y)*80 + box->x);
		for (xx=0; xx<box->nx; ++xx)
			*p++ = *q++ & 0xff | Jattr << 8;
	}
}

Jfreebox (box)
JBOX *box;
{
	mfree ((char *) box->mem, box->ny * box->nx * sizeof (short));
	mfree ((char *) box, sizeof (JBOX));
}

Jclearbox (r, c, nr, nc)
{
	register i;
	register short far *p;

	for (; --nr>=0; ++r) {
		p = Jvideomem + (r*80 + c);
		for (i=nc; --i>=0;)
			*p++ = blank;
	}
}

Jfillbox (r, c, nr, nc, sym)
{
	register i;
	register short far *p;

	for (; --nr>=0; ++r) {
		p = Jvideomem + (r*80 + c);
		for (i=nc; --i>=0;)
			*p++ = sym | Jattr << 8;
	}
}

Jhorline (r, c, nc)
register c, nc;
{
	register sym;
	register short far *p;

	sym = linedraw [0] & 0377 | Jattr << 8;
	p = Jvideomem + (r*80 + c);
	while (--nc >= 0)
		*p++ = sym;
}

Jvertline (c, r, nr)
register r, nr;
{
	register sym;
	register short far *p;

	sym = linedraw [1] & 0377 | Jattr << 8;
	p = Jvideomem + (r * 80 + c);
	while (--nr >= 0) {
		*p = sym;
		p += 80;
	}
}

Jcorner (r, c, n)
register n;
{
	static char map [9] = { 8, 9, 10, 5, 6, 7, 2, 3, 4 };
	register short far *p;

	switch (n) {
	case 1: case 2: case 3: case 4:
	case 5: case 6: case 7: case 8:
	case 9:
		n = linedraw [map [n-1]] & 0377;
		break;
	default:
		n = '?';
		break;
	}
	p = Jvideomem + (r * 80 + c);
	*p = n | Jattr << 8;
}

Jdrawbox (r, c, nr, nc)
register r, c, nr, nc;
{
	Jhorline (r, c+1, nc-2);
	Jhorline (r+nr-1, c+1, nc-2);
	Jvertline (c, r+1, nr-2);
	Jvertline (c+nc-1, r+1, nr-2);
	Jcorner (r, c, 1) ;
	Jcorner (r, c+nc-1, 3);
	Jcorner (r+nr-1, c, 7) ;
	Jcorner (r+nr-1, c+nc-1, 9);
}
