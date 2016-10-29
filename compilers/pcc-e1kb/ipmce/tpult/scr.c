/*
 *      Visual screen packadge.
 *      -----------------------
 *
 *      int VInit ()
 *              - initialise packadge. Returns 1 if ok else 0.
 *
 *      VOpen ()
 *              - enters video mode.
 *
 *      VClose ()
 *              - closes video mode.
 *
 *      VSetPalette (n, nb, nr, nrb, b, bb, br, brb, d, db, dr, drb)
 *              - set screen palette. Sets normal, normal background,
 *              normal reverse, normal reverse background, bold,
 *              bold background, bold reverse, bold reverse background,
 *              dim, dim background, dim reverse, dim reverse background.
 *              Default are 10, 0, 0, 10, 15, 0, 15, 12, 7, 0, 0, 6.
 *
 *      VIClear ()
 *              - immediately clears screen.
 *
 *      VIPrompt (s)
 *              - immediately outputs string with bold standout attribute.
 *
 *      VRedraw ()
 *              - redraws screen.
 *
 *      VSync ()
 *              - refresh screen.
 *
 *      VFlush ()
 *              - flush terminal output buffer. Called just before
 *              getting input from terminal.
 *
 *      VBeep ()
 *              - bell.
 *
 *      VSyncLine (wy)
 *              - refresh line number wy.
 *
 *      VDelLine (n)
 *              - delete line.
 *
 *      VInsLine (n)
 *              - insert line.
 *
 *      VDelChar ()
 *              - delete char at current position, shift line left.
 *
 *      VInsChar ()
 *              - insert space at current position.
 *
 *      VMove (y, x)
 *              - set current position.
 *
 *      VClearLine ()
 *              - erase to end of line.
 *
 *      VClear ()
 *              - clear screen.
 *
 *      VPutChar (c)
 *              - put character to screen. Special characters are:
 *                      '\1'    - set dim
 *                      '\2'    - set normal
 *                      '\3'    - set bold
 *                      '\16'   - set reverse
 *                      '\17'   - unset reverse
 *                      '\t'    - next tab stop
 *                      '\r'    - return
 *                      '\n'    - new line
 *                      '\b'    - back space
 *
 *      VPutString (str)
 *              - print string to screen.
 *
 *      int VStandOut ()
 *              - set reverse attribute. Returns 1 if terminal
 *              has reverse, else 0.
 *
 *      VStandEnd ()
 *              - unset reverse attribute.
 *
 *      VSetNormal ()
 *              - set normal attribute.
 *
 *      VSetPrev ()
 *              - set attribute before last change.
 *
 *      VSetDim ()
 *              - set dim attribute.
 *
 *      VSetBold ()
 *		- set bold attribute.
 *
 *	short VGetAttr ()
 *		- get previous attributes
 *
 *	void VSetAttr (attr)
 *		- set stored attributes
 *
 *      CURSOR VGetCursor ()
 *              - get current cursor position.
 *
 *      VSetCursor (c)
 *              - set stored cursor position.
 *
 *      BOX *VGetBox (y, x, ny, nx)
 *              - get rectangular area of screen, called "box".
 *              (y, x) specifies upper left corner, ny, nx -
 *              vertical and horisontal sizes.
 *
 *      VUngetBox (box)
 *              - restore saved box.
 *
 *      VPrintBox (box)
 *              - print saved box with current attribute.
 *
 *      VFreeBox (box)
 *              - free box structure.
 *
 *      VExpandString (s, d)
 *              - expand string "s", which contains
 *              attribute switching escapes '\1, '\2', '\3', '\16', '\17'.
 *              Store expanded string in "d".
 */

# include <setjmp.h>
# include <signal.h>
# ifdef MSDOS
#    include <conio.h>

#if     !defined(__COLORS)
#define __COLORS

enum COLORS {
/*  0 */BLACK,                  /* dark colors */
/*  1 */BLUE,
/*  2 */GREEN,
/*  3 */CYAN,
/*  4 */RED,
/*  5 */MAGENTA,
/*  6 */BROWN,
/*  7 */LIGHTGRAY,
/*  8 */DARKGRAY,               /* light colors */
/*  9 */LIGHTBLUE,
/* 10 */LIGHTGREEN,
/* 11 */LIGHTCYAN,
/* 12 */LIGHTRED,
/* 13 */LIGHTMAGENTA,
/* 14 */YELLOW,
/* 15 */WHITE
};
#endif

#    include <dos.h>
# endif
# include "scr.h"
# include "key.h"
# include "ut.h"
# ifndef MSDOS
#    include "cap.h"
# endif

# ifdef MSDOS
# define DIRECT
# endif

# define STANDOUT       0400
# define BOLD           01000
# define DIM            02000
# define GRAPH          04000
# define NOCHANGE       -1
# define OUTBUFSZ       256

# ifdef MSDOS
# define resetattr      setattr
union REGS inregs, outregs;
# define BIOSVIDEO(n, l) { inregs.h.al = (char) (l); inregs.h.ah = (char) (n);\
int86 (0x10, &inregs, &outregs); }
# define qputch(c)	DOSputch (c)
# define putch(c)       qputch (c)
# else
# define qputch(c)      (outptr>=outbuf+OUTBUFSZ?VFlush(),*outptr++=(c):(*outptr++=(c)))
# define putch(c)       qputch (ucase(c))
# endif

# ifdef MSDOS
int BlackWhite;
# ifdef DIRECT
# define DOSgotoxy(x,y) (DOSx=x-1,DOSy=y-1)

typedef struct { char c, a; } DOSsymbol;
DOSsymbol far *DOSvideomem;
int DOSx, DOSy, NoWait, Mode;
# endif
# endif

WINDOW VScreen;
int TtyUpperCase;

static WINDOW curscr;
static scrool, rscrool;
static beepflag;
static prevattr;

# ifdef MSDOS
int LINES, COLS;
static DOSstdattr, DOSattr;
static char *CS;
int DOScols, DOSrows;
# else
static short ctab [16], btab [16];
static char *colorbuf, *colorp;
static char outbuf [OUTBUFSZ], *outptr = outbuf;

static char *BS, *BR, *DS, *DR, *NS, *NR;
static char *GS, *GE, *G1, *G2, *GT;
static char *CS, *SF, *SR;
static void tputs ();

# ifdef KEYPAD
static char *KS, *KE;
# endif

static char *CL, *CM, *SE, *SO, *TE, *TI, *VE, *VS,
	*AL, *DL, *IS, *IF, *FS, *MD, *MH, *ME, *MR,
	*CF, *CB, *MF, *MB;
static NF, NB;
static char MS, C2;

int LINES;                      /* число строк на экране терминала */
int COLS;                       /* число символов в строке */

struct CapTab outtab [] = {
	{ "ms", CAPFLG, 0, &MS, 0, 0, },
	{ "C2", CAPFLG, 0, &C2, 0, 0, },
	{ "li", CAPNUM, 0, 0, &LINES, 0, },
	{ "co", CAPNUM, 0, 0, &COLS, 0, },
	{ "Nf", CAPNUM, 0, 0, &NF, 0, },
	{ "Nb", CAPNUM, 0, 0, &NB, 0, },
	{ "cl", CAPSTR, 0, 0, 0, &CL, },
	{ "cm", CAPSTR, 0, 0, 0, &CM, },
	{ "se", CAPSTR, 0, 0, 0, &SE, },
	{ "so", CAPSTR, 0, 0, 0, &SO, },
	{ "Cf", CAPSTR, 0, 0, 0, &CF, },
	{ "Cb", CAPSTR, 0, 0, 0, &CB, },
	{ "Mf", CAPSTR, 0, 0, 0, &MF, },
	{ "Mb", CAPSTR, 0, 0, 0, &MB, },
	{ "md", CAPSTR, 0, 0, 0, &MD, },
	{ "mh", CAPSTR, 0, 0, 0, &MH, },
	{ "mr", CAPSTR, 0, 0, 0, &MR, },
	{ "me", CAPSTR, 0, 0, 0, &ME, },
	{ "te", CAPSTR, 0, 0, 0, &TE, },
	{ "ti", CAPSTR, 0, 0, 0, &TI, },
	{ "vs", CAPSTR, 0, 0, 0, &VS, },
	{ "ve", CAPSTR, 0, 0, 0, &VE, },
# ifdef KEYPAD
	{ "ks", CAPSTR, 0, 0, 0, &KS, },
	{ "ke", CAPSTR, 0, 0, 0, &KE, },
# endif
	{ "al", CAPSTR, 0, 0, 0, &AL, },
	{ "dl", CAPSTR, 0, 0, 0, &DL, },
	{ "is", CAPSTR, 0, 0, 0, &IS, },
	{ "if", CAPSTR, 0, 0, 0, &IF, },
	{ "fs", CAPSTR, 0, 0, 0, &FS, },
	{ "gs", CAPSTR, 0, 0, 0, &GS, },
	{ "ge", CAPSTR, 0, 0, 0, &GE, },
	{ "g1", CAPSTR, 0, 0, 0, &G1, },
	{ "g2", CAPSTR, 0, 0, 0, &G2, },
	{ "gt", CAPSTR, 0, 0, 0, &GT, },
	{ "cs", CAPSTR, 0, 0, 0, &CS, },
	{ "sf", CAPSTR, 0, 0, 0, &SF, },
	{ "sr", CAPSTR, 0, 0, 0, &SR, },
	{ { 0, 0, }, 0, 0, 0, 0, 0, },
};

static char *skipdelay ();

# endif /* MSDOS */

static char linedraw [11] = {
# ifdef MSDOS
	196,	/* 0	horisontal line */
	179,    /* 1    vertical line */
	192,    /* 2    lower left corner */
	193,    /* 3    lower center */
	217,    /* 4    lower right corner */
	195,    /* 5    left center */
	197,    /* 6    center cross */
	180,    /* 7    right center */
	218,    /* 8    upper left corner */
	194,    /* 9    upper center */
	191,    /* 10   upper right corner */
# else
	'-',    /* 0    horisontal line */
	'|',    /* 1    vertical line */
	'+',    /* 2    lower left corner */
	'-',    /* 3    lower center */
	'+',    /* 4    lower right corner */
	'|',    /* 5    left center */
	'+',    /* 6    center cross */
	'|',    /* 7    right center */
	'+',    /* 8    upper left corner */
	'-',    /* 9    upper center */
	'+',    /* 10   upper right corner */
# endif
};

extern char *getenv (), *mallo (), *reallo (), *CapGoto (), *strcat (), *strcpy ();
static newwin(), makenew();
static void delwin(), makerch(), makech(), doscrool(), sclln();
static void domvcur(), screrase(), setattr(), pokechr(), DOSinsline(), DOSdelline();

# ifdef SIGTSTP
static (*redraw) ();
static tstp ();
# endif

VInit ()
{
# ifdef MSDOS
	DOSgetinfo (&LINES, &COLS, &DOSstdattr);
	scrool = 1;
# else
	CapGet (outtab);
	if (LINES <= 6 || COLS <= 30)
		return (0);
	if (! CM)
		return (0);
	scrool = AL && DL;
	if (! (rscrool = SF && SR))
		SF = SR = 0;
	if (NF)
		initcolor ();
	else if (ME)
		initbold ();
	if (G1 || G2 || GT)
		initgraph ();
# endif
	if (curscr.y)
		delwin (&curscr);
	if (VScreen.y)
		delwin (&VScreen);
	if (! newwin (&curscr) || ! newwin (&VScreen)) {
		VClose ();
		return (0);
	}
	curscr.flgs = curscr.clear = 1;
	resetattr (0);
	return (1);
}

void VOpen ()
{
# ifndef MSDOS
	TtySet ();
# ifdef INITFILE
	if (IF)
		typefile (IF);
# endif
	if (IS)
		tputs (IS);
	if (TI)
		tputs (TI);
	if (VS)
		tputs (VS);
# ifdef KEYPAD
	if (KS)
		tputs (KS);
# endif
# ifdef SIGTSTP
	signal (SIGTSTP, tstp);
# endif
# endif /* MSDOS */
}

void VReopen ()
{
# ifndef MSDOS
	TtySet ();
# ifdef STRINIT
	if (IS)
		tputs (IS);
# endif
	if (TI)
		tputs (TI);
	if (VS)
		tputs (VS);
# ifdef KEYPAD
	if (KS)
		tputs (KS);
# endif
# ifdef SIGTSTP
	signal (SIGTSTP, tstp);
# endif
# endif /* MSDOS */
}

void VClose ()
{
	if (curscr.y)
		setattr (0);
# ifdef MSDOS
	DOSsetattr (DOSstdattr);
# else
	if (FS)
		tputs (FS);
	if (VE)
		tputs (VE);
	if (TE)
		tputs (TE);
# ifdef KEYPAD
	if (KE)
		tputs (KE);
# endif
	VFlush ();
	TtyReset ();
# ifdef SIGTSTP
	signal (SIGTSTP, SIG_DFL);
# endif
# endif /* MSDOS */
}

void VRestore ()
{
# ifndef MSDOS
# ifdef STRINIT
	if (FS)
		tputs (FS);
# endif
	if (VE)
		tputs (VE);
	if (TE)
		tputs (TE);
# ifdef KEYPAD
	if (KE)
		tputs (KE);
# endif
	VFlush ();
	TtyReset ();
# ifdef SIGTSTP
	signal (SIGTSTP, SIG_DFL);
# endif
# endif /* MSDOS */
}

# ifdef INITFILE
static typefile (s)
char *s;
{
	register d, n;
	char buf [64];

	if ((d = open (s, 0)) < 0)
		return;
	VFlush ();
	while ((n = read (d, buf, sizeof (buf))) > 0)
		write (1, buf, (unsigned) n);
	close (d);
}
# endif

static void delwin (win)
register WINDOW *win;
{
	register i;

	for (i=0; i < LINES && win->y[i]; i++)
		mfree ((char *) win->y[i], COLS * sizeof (short));
	mfree ((char *) win->y, LINES * sizeof (short));
	mfree ((char *) win->firstch, LINES * sizeof (short));
	mfree ((char *) win->lastch, LINES * sizeof (short));
	mfree ((char *) win->lnum, LINES * sizeof (short));
	win->y = 0;
}

static newwin (win)
register WINDOW *win;
{
	register short *sp;
	register i;

	if (! makenew (win))
		return (0);
	for (i=0; i<LINES; i++) {
		win->y[i] = (short *) mallo ((int) (COLS * sizeof (short)));
		if (! win->y[i]) {
			register j;

			for (j=0; j<i; j++)
				mfree ((char *) win->y[i], COLS * sizeof (short));
			mfree ((char *) win->y, LINES * sizeof (short));
			mfree ((char *) win->firstch, LINES * sizeof (short));
			mfree ((char *) win->lastch, LINES * sizeof (short));
			mfree ((char *) win->lnum, LINES * sizeof (short));
			win->y = 0;
			return (0);
		}
		for (sp=win->y[i]; sp < win->y[i]+COLS;)
			*sp++ = ' ';
	}
	return (1);
}

static makenew (win)
register WINDOW *win;
{
	register i;

	if (! (win->y = (short **) mallo ((int) (LINES * sizeof (short *)))))
		return (0);
	if (! (win->firstch = (short *) mallo ((int) (LINES * sizeof (short)))))
		goto b;
	if (! (win->lastch = (short *) mallo ((int) (LINES * sizeof (short)))))
		goto c;
	if (! (win->lnum = (short *) mallo ((int) (LINES * sizeof (short))))) {
		mfree ((char *) win->lastch, LINES * sizeof (short));
c:              mfree ((char *) win->firstch, LINES * sizeof (short));
b:              mfree ((char *) win->y, LINES * sizeof (short));
		return (0);
	}
	win->cury = win->curx = 0;
	win->clear = 1;
	win->flgs = 0;
	for (i=0; i<LINES; i++) {
		win->firstch[i] = win->lastch[i] = NOCHANGE;
		win->lnum[i] = i;
	}
	return (1);
}

void VIClear ()
{
# ifndef MSDOS
	tputs (CL);
# else
	DOSclrscr ();
# endif
}

# ifndef MSDOS
void VIPrompt (s)
register char *s;
{
	if (BR)
		tputs (BR);
	while (*s)
		putch (*s++);
	if (NS)
		tputs (NS);
}
# endif

void VRedraw ()
{
	register short wy;
	register y, x;

# ifdef MSDOS
	DOSclrscr ();
# else
	tputs (CL);
# endif
	y = curscr.cury;
	x = curscr.curx;
	curscr.cury = 0;
	curscr.curx = 0;
	for (wy=0; wy<LINES; wy++)
		makerch (wy);
	domvcur (y, x);
	setattr (VScreen.flgs);
	VFlush ();
}

static void makerch (y)
register y;
{
	register short *new;
	register short x;

	new = &curscr.y [y] [0];
	for (x=0; x<COLS; ++new, ++x) {
		if (*new == ' ')
			continue;
		if (x >= COLS-1 && y >= LINES-1)
			return;
		domvcur (y, x);
		setattr (*new);
		putch (*new);
# ifdef MSDOS
		curscr.curx = x;
# else
		curscr.curx = x + 1;
# endif
	}
}

void VSyncLine (wy)
register wy;
{
	if (VScreen.firstch[wy] != NOCHANGE) {
		makech (wy);
		VScreen.firstch[wy] = NOCHANGE;
	}
}

void VSync ()
{
	register short wy;

	if (VScreen.clear || curscr.clear) {
# ifdef MSDOS
		DOSclrscr ();
# else
		setattr (0);
		tputs (CL);
# endif
		VScreen.clear = 0;
		curscr.clear = 0;
		curscr.cury = 0;
		curscr.curx = 0;
		for (wy=0; wy<LINES; wy++) {
			register short *sp, *end;

			end = &curscr.y[wy][COLS];
			for (sp=curscr.y[wy]; sp<end; sp++)
				*sp = ' ';
		}
		for (wy=0; wy<LINES; wy++) {
			VScreen.firstch[wy] = 0;
			VScreen.lastch[wy] = COLS-1;
			VScreen.lnum[wy] = wy;
		}
	} else if (rscrool || scrool)
		doscrool ();
	for (wy=0; wy<LINES; wy++) {
		VSyncLine (wy);
		VScreen.lnum[wy] = wy;
	}
	domvcur (VScreen.cury, VScreen.curx);
	curscr.cury = VScreen.cury;
	curscr.curx = VScreen.curx;
	if (beepflag) {
# ifdef MSDOS
/*
		delay (0);
		sound (800);
		delay (100);
		sound (600);
		delay (100);
		nosound ();
*/
# undef         putch
		putch  ('\007');
# define        putch(c)        qputch (c)
# else
		qputch ('\007');
# endif
		beepflag = 0;
	}
	setattr (VScreen.flgs);
	VFlush ();
}

static void makech (y)
register y;
{
	register short *new, *old;
	register short x, lastch;

	x = VScreen.firstch[y];
	lastch = VScreen.lastch[y];
	old = &curscr.y [y] [x];
	new = &VScreen.y [y] [x];
	for (; x<=lastch; ++new, ++old, ++x) {
		if (*new == *old)
			continue;
# ifndef MSDOS
		if (x >= COLS-1 && y >= LINES-1)
			return;
# endif /* MSDOS */
		domvcur (y, x);
		setattr (*new);
		putch (*old = *new);
# ifdef MSDOS
		curscr.curx = x;
# else
		curscr.curx = x + 1;
# endif
	}
}

static void doscrool ()
{
	register line, n, topline, botline;
# ifndef MSDOS
	int mustreset = 0;
# endif

	for (line=0; line<LINES; ++line) {
		/* find next range to scrool */

		/* skip fresh lines */
		while (line < LINES && VScreen.lnum [line] < 0)
			++line;

		/* last line reached - no range to scrool */
		if (line >= LINES)
			break;

		/* top line found */
		topline = line;

		/* skip range of old lines */
		while (line < LINES-1 && VScreen.lnum [line] + 1 == VScreen.lnum [line+1])
			++line;

		/* bottom line found */
		botline = line;

		/* compute number of scrools, >0 - forward */
		n = topline - VScreen.lnum [topline];

		if (n == 0)
			continue;
		else if (n > 0)
			topline = VScreen.lnum [topline];
		else if (n < 0)
			botline = VScreen.lnum [botline];

		/* do scrool */

		if (rscrool && !scrool && !CS) {
			/* cannot scrool small regions if no scrool region */
			if (2 * (botline - topline) < LINES-2) {
				for (line=topline; line<=botline; ++line) {
					VScreen.firstch [line] = 0;
					VScreen.lastch [line] = COLS-1;
				}
				return;
			}
			if (topline > 0) {
				for (line=0; line<topline; ++line) {
					VScreen.firstch [line] = 0;
					VScreen.lastch [line] = COLS-1;
				}
				topline = 0;
			}
			if (botline < LINES-1) {
				for (line=botline+1; line<LINES; ++line) {
					VScreen.firstch [line] = 0;
					VScreen.lastch [line] = COLS-1;
				}
				botline = LINES-1;
			}
		}

		/* update curscr */
		sclln (topline, botline, n);

# ifndef MSDOS
		/* set scrool region */
		if (CS) {
			tputs (CapGoto (CS, botline, topline));
			mustreset = 1;
		}
# endif

		/* do scrool n lines forward or backward */
		if (n > 0) {
			if (CS || !scrool) {
# ifndef MSDOS
				tputs (CapGoto (CM, 0, CS ? topline : 0));
				while (--n >= 0)
					tputs (SR);
# endif
			} else {
				while (--n >= 0) {
# ifdef MSDOS
					DOSdelline (botline, 1);
					DOSinsline (topline, 1);
# else
					tputs (CapGoto (CM, 0, botline));
					tputs (DL);
					tputs (CapGoto (CM, 0, topline));
					tputs (AL);
# endif
				}
			}
		} else {
			if (CS || !scrool) {
# ifndef MSDOS
				tputs (CapGoto (CM, 0, CS ? botline : LINES-1));
				while (++n <= 0)
					tputs (SF);
# endif
			} else {
				while (++n <= 0) {
# ifdef MSDOS
					DOSdelline (topline, 1);
					DOSinsline (botline, 1);
# else
					tputs (CapGoto (CM, 0, topline));
					tputs (DL);
					tputs (CapGoto (CM, 0, botline));
					tputs (AL);
# endif
				}
			}
		}
	}
# ifndef MSDOS
	if (mustreset)
		/* unset scrool region */
		tputs (CapGoto (CS, LINES-1, 0));
# endif
}

static void sclln (y1, y2, n)
{
	register short *end, *temp;
	register y;

	if (n > 0) {
		for (y=y2-n+1; y<=y2; ++y) {
			temp = curscr.y [y];
			for (end = &temp[COLS]; temp<end; *--end = ' ');
		}
		while (--n >= 0) {
			temp = curscr.y [y2];
			for (y=y2; y>y1; --y)
				curscr.y [y] = curscr.y [y-1];
			curscr.y [y1] = temp;
		}
	} else {
		for (y=y1; y<y1-n; ++y) {
			temp = curscr.y [y];
			for (end = &temp[COLS]; temp<end; *--end = ' ');
		}
		while (++n <= 0) {
			temp = curscr.y [y1];
			for (y=y1; y<y2; ++y)
				curscr.y [y] = curscr.y [y+1];
			curscr.y [y2] = temp;
		}
	}
}

void VDelLine (n)
{
	register short *temp;
	register y;
	register short *end;

	if (n<0 || n>=LINES)
		return;
	temp = VScreen.y [n];
	for (y=n; y < LINES-1; y++) {
		VScreen.y [y] = VScreen.y [y+1];
		VScreen.lnum [y] = VScreen.lnum [y+1];
		if (scrool || rscrool) {
			VScreen.firstch [y] = VScreen.firstch [y+1];
			VScreen.lastch [y] = VScreen.lastch [y+1];
		} else {
			VScreen.firstch [y] = 0;
			VScreen.lastch [y] = COLS-1;
		}
	}
	VScreen.y [LINES-1] = temp;
	VScreen.lnum [LINES-1] = -1;
	VScreen.firstch [LINES-1] = 0;
	VScreen.lastch [LINES-1] = COLS-1;
	for (end = &temp[COLS]; temp<end; *temp++ = ' ');
}

void VInsLine (n)
{
	register short *temp;
	register y;
	register short *end;

	if (n<0 || n>=LINES)
		return;
	temp = VScreen.y [LINES-1];
	for (y=LINES-1; y>n; --y) {
		VScreen.y [y] = VScreen.y [y-1];
		VScreen.lnum [y] = VScreen.lnum [y-1];
		if (scrool || rscrool) {
			VScreen.firstch [y] = VScreen.firstch [y-1];
			VScreen.lastch [y] = VScreen.lastch [y-1];
		} else {
			VScreen.firstch [y] = 0;
			VScreen.lastch [y] = COLS-1;
		}
	}
	VScreen.lnum [n] = -1;
	VScreen.y [n] = temp;
	VScreen.firstch [n] = 0;
	VScreen.lastch [n] = COLS-1;
	for (end = &temp[COLS]; temp<end; *temp++ = ' ');
}

static void domvcur (y, x)
register y, x;
{
	if (curscr.curx==x && curscr.cury==y)
		return;
# ifdef MSDOS
	DOSgotoxy (x+1, y+1);
# else
	if (curscr.cury==y && x-curscr.curx > 0 && x-curscr.curx < 7) {
		register short i;

		while (curscr.curx < x) {
			i = curscr.y [curscr.cury] [curscr.curx];
			if ((i & ~0377) == curscr.flgs)
				putch ((int) i);
			else break;
			++curscr.curx;
		}
		if (curscr.curx == x)
			return;
	}
	if (!MS && curscr.flgs)
		setattr (0);
	tputs (CapGoto (CM, x, y));
# endif
	curscr.curx = x;
	curscr.cury = y;
}

void VMove (y, x)
register y, x;
{
	if (x>=0 && x<COLS)
		VScreen.curx = x;
	if (y>=0 && y<LINES)
		VScreen.cury = y;
}

void VClearLine ()
{
	register short *sp, *end;
	register y, x;
	register short *maxx;
	register minx;

	y = VScreen.cury;
	x = VScreen.curx;
	end = &VScreen.y[y][COLS];
	minx = NOCHANGE;
	maxx = &VScreen.y[y][x];
	for (sp=maxx; sp<end; sp++)
		if (*sp != ' ') {
			maxx = sp;
			if (minx == NOCHANGE)
				minx = sp - VScreen.y[y];
			*sp = ' ';
		}
	if (minx != NOCHANGE) {
		if (VScreen.firstch[y] > minx || VScreen.firstch[y] == NOCHANGE)
			VScreen.firstch[y] = minx;
		if (VScreen.lastch[y] < maxx - VScreen.y[y])
			VScreen.lastch[y] = maxx - VScreen.y[y];
	}
}

void VClear ()
{
	screrase ();
	VScreen.clear = 1;
}

static void screrase ()
{
	register y;
	register short *sp, *end, *maxx;
	register minx;

	for (y=0; y<LINES; y++) {
		minx = NOCHANGE;
		end = &VScreen.y[y][COLS];
		for (sp=VScreen.y[y]; sp<end; sp++)
			if (*sp != ' ') {
				maxx = sp;
				if (minx == NOCHANGE)
					minx = sp - VScreen.y[y];
				*sp = ' ';
			}
		if (minx != NOCHANGE) {
			if (VScreen.firstch[y] > minx
					|| VScreen.firstch[y] == NOCHANGE)
				VScreen.firstch[y] = minx;
			if (VScreen.lastch[y] < maxx - VScreen.y[y])
				VScreen.lastch[y] = maxx - VScreen.y[y];
		}
		VScreen.lnum[y] = y;
	}
	VScreen.curx = VScreen.cury = 0;
}

void VPutString (str)
register char *str;
{
	while (*str)
		VPutChar (*str++);
}

# ifndef MSDOS
static char *makecolor (c, b)
{
	register char *p = colorp;

	if (C2) {
		strcpy (colorp, CapGoto (CF, b, c));
		while (*colorp++);
	} else {
		strcpy (colorp, CapGoto (CF, 0, c));
		while (*colorp++);
		strcpy (--colorp, CapGoto (CB, 0, b));
		while (*colorp++);
	}
	return (p);
}

static initcolor ()     /* will it work for not the IBM PC ? */
{
	register i;

	if (NF<=0 || NB<=0 || !CF || !CB && !C2)
		return;
	if (NF > 16)
		NF = 16;
	if (NB > 16)
		NB = 16;
	if (! MF)
		MF = "0123456789ABCDEF";
	if (! MB)
		MB = "0123456789ABCDEF";
	for (i=0; i<16; ++i)
		ctab [i] = btab [i] = -1;
	for (i=0; i<16 && i<NF; ++i)
		if (! MF [i])
			break;
		else if (MF[i]>='0' && MF[i]<='9')
			ctab [MF [i] - '0'] = i;
		else if (MF[i]>='A' && MF[i]<='F')
			ctab [MF [i] - 'A' + 10] = i;
	for (i=0; i<16 && i<NB; ++i)
		if (! MB [i])
			break;
		else if (MB[i]>='0' && MB[i]<='9')
			btab [MB [i] - '0'] = i;
		else if (MF[i]>='A' && MF[i]<='F')
			btab [MB [i] - 'A' + 10] = i;
	for (i=1; i<8; ++i) {
		if (ctab[i] >= 0 && ctab[i+8] < 0)
			ctab [i+8] = ctab [i];
		if (ctab[i+8] >= 0 && ctab[i] < 0)
			ctab [i] = ctab [i+8];
		if (btab[i] >= 0 && btab[i+8] < 0)
			btab [i+8] = btab [i];
		if (btab[i+8] >= 0 && btab[i] < 0)
			btab [i] = btab [i+8];
	}
	VSetPalette (10, 0, 0, 7, 15, 0, 15, 7, 14, 0, 15, 1);
}

void VSetPalette (n, nb, nr, nrb, b, bb, br, brb, d, db, dr, drb)
{
	if (! NF || ctab[n]<0 || ctab[nr]<0 || btab[nb]<0 || btab[nrb]<0)
		return;
	NS = NR = BS = BR = DS = DR = 0;
	if (colorp)
		mfree (colorbuf);
	colorp = colorbuf = mallo (1024);;
	NS = makecolor (ctab [n], btab [nb]);   /* NORMAL - bright green on black */
	NR = makecolor (ctab [nr], btab [nrb]); /* REVERSE NORMAL - black on bright green */
	if (ctab[b]<0 || ctab[br]<0 || btab[bb]<0 || btab[brb]<0)
		goto ret;
	BS = makecolor (ctab [b], btab [bb]);   /* BOLD - bright white on black */
	BR = makecolor (ctab [br], btab [brb]); /* REVERSE BOLD - bright white on bright red */
	if (ctab[d]<0 || ctab[dr]<0 || btab[db]<0 || btab[drb]<0)
		goto ret;
	DS = makecolor (ctab [d], btab [db]);   /* DIM - white on black */
	DR = makecolor (ctab [dr], btab [drb]); /* REVERSE DIM - bright yellow on brown */
ret:
	if (colorp > colorbuf)
		reallo (colorbuf, colorp - colorbuf);
	else
		mfree (colorbuf);
}

static initbold ()
{
	if (ME)
		NS = skipdelay (ME);
	if (MD)
		BS = skipdelay (MD);
	if (MH)
		DS = skipdelay (MH);
	if (SO)
		SO = skipdelay (SO);
	else if (MR)
		SO = skipdelay (MR);
}

initgraph ()
{
	register i;
	register char *g = 0;

	if (GT) {
		GT [1] = GT [0];
		g = GT+1;
	}
	if (G2)
		g = G2;
	if (G1)
		g = G1;
	if (! g)
		return;
	for (i=0; i<11 && *g; ++i, ++g)
		linedraw [i] = *g;
}
# endif

# ifdef MSDOS

/* ARGSUSED */
void VSetPalette ()
{
}

# define SETATTR(a,b) DOSattr = ((a) | (b)<<4) & 0177; break

static void setattr (c)
register c;
{
	c &= DIM | BOLD | STANDOUT;
	if (c == curscr.flgs)
		return;
	if (BlackWhite)
		switch (c & (DIM|BOLD|STANDOUT)) {
		case 0:                 SETATTR (LIGHTGRAY, BLACK);
		case STANDOUT:          SETATTR (BLACK, LIGHTGRAY);
		case DIM:               SETATTR (LIGHTGRAY, BLACK);
		case DIM|STANDOUT:      SETATTR (DARKGRAY, WHITE);
		case BOLD:              SETATTR (WHITE, BLACK);
		case BOLD|STANDOUT:     SETATTR (WHITE, BLACK);
		}
	else
		switch (c & (DIM|BOLD|STANDOUT)) {
		case 0:                 SETATTR (LIGHTGREEN, BLACK);
		case STANDOUT:          SETATTR (BLACK, LIGHTGRAY);
		case DIM:               SETATTR (YELLOW, BLACK);
		case DIM|STANDOUT:      SETATTR (WHITE, BLUE);
		case BOLD:              SETATTR (WHITE, BLACK);
		case BOLD|STANDOUT:     SETATTR (WHITE, LIGHTGRAY);
		}
	curscr.flgs = c;
}
# else
# define RESETATTR(a,b,c,f)\
	if (a)\
		tputs (b);\
	else {\
		tputs (c);\
		tputs (f);\
	}\
	break

resetattr (c)
{
	switch (c & (DIM | BOLD | STANDOUT)) {
	case 0:                 RESETATTR (NR,NS,NS,SE);
	case STANDOUT:          RESETATTR (NR,NR,NS,SO);
	case DIM:               RESETATTR (DR,DS,DS,SE);
	case DIM|STANDOUT:      RESETATTR (DR,DR,DS,SO);
	case BOLD:              RESETATTR (BR,BS,BS,SE);
	case BOLD|STANDOUT:     RESETATTR (BR,BR,BS,SO);
	}
	curscr.flgs = c;
}

# define SETATTR(a,b,c)\
	if (a)\
		tputs (b);\
	else {\
		if (curscr.flgs & STANDOUT) {\
			tputs (SE);\
			tputs (b);\
		} else if ((curscr.flgs & (DIM|BOLD)) != (c))\
			tputs (b);\
	}\
	break

# define SETREVA(a,b,c)\
	if (a)\
		tputs (a);\
	else {\
		if ((curscr.flgs & (DIM|BOLD)) != (c))\
			tputs (b);\
		if (! (curscr.flgs & STANDOUT))\
			tputs (SO);\
	}\
	break

static void setattr (c)
register c;
{
	if ((c & GRAPH) != (curscr.flgs & GRAPH))
		if (c & GRAPH) {
			tputs (GS);
			resetattr (c);
			return;
		} else
			tputs (GE);
	if ((c & (DIM | BOLD | STANDOUT)) != (curscr.flgs & (DIM | BOLD | STANDOUT)))
		switch (c & (DIM | BOLD | STANDOUT)) {
		case 0:                 SETATTR (NR,NS,0);
		case STANDOUT:          SETREVA (NR,NS,0);
		case DIM:               SETATTR (DR,DS,DIM);
		case DIM|STANDOUT:      SETREVA (DR,DS,DIM);
		case BOLD:              SETATTR (BR,BS,BOLD);
		case BOLD|STANDOUT:     SETREVA (BR,BS,BOLD);
		}
	curscr.flgs = c;
}
# endif

VStandOut ()
{
# ifndef MSDOS
	if (!SO && !NR)
		return (0);
# endif
	VScreen.flgs |= STANDOUT;
	return (1);
}

void VStandEnd ()
{
	VScreen.flgs &= ~STANDOUT;
}

void VSetNormal ()
{
	prevattr = VScreen.flgs;
	VScreen.flgs &= ~(BOLD | DIM);
}

void VSetPrev ()
{
	VScreen.flgs &= ~(BOLD | DIM);
	VScreen.flgs |= prevattr & (BOLD | DIM);
}

VSetDim ()
{
	prevattr = VScreen.flgs & (BOLD | DIM);
	VScreen.flgs &= ~(BOLD | DIM);
# ifndef MSDOS
	if (!DS)
		return (0);
# endif
# ifdef UNDEFINED
	if (BlackWhite)
		return (0);
# endif
	VScreen.flgs |= DIM;
	return (1);
}

VSetBold ()
{
	prevattr = VScreen.flgs & (BOLD | DIM);
	VScreen.flgs &= ~(BOLD | DIM);
# ifndef MSDOS
	if (!BS)
		return (0);
# endif
# ifdef UNDEFINED
	if (BlackWhite)
		return (0);
# endif
	VScreen.flgs |= BOLD;
	return (1);
}

/* Сохранение предыдущих аттрибутов экрана */

short VGetAttr ()
{
	return VScreen.flgs ;
}

/* Восстановление сохраненных аттрибутов экрана */

void VSetAttr (attr)
short attr;
{
	VScreen.flgs = attr;
}

void VPutChar (c)
register c;
{
	register x, y;

	x = VScreen.curx;
	y = VScreen.cury;
	switch (c &= 0377) {
	case '\16':
		VStandOut ();
		return;
	case '\17':
		VStandEnd ();
		return;
	case '\1':
		VSetDim ();
		return;
	case '\2':
		VSetNormal ();
		return;
	case '\3':
		VSetBold ();
		return;
	case '\7':
		VBeep ();
		return;
	case '\t':
		x += (8 - (x & 07));
		break;
	default:
		pokechr (y, x++, c | VScreen.flgs & ~0377);
		break;
	case '\n':
		++y;
	case '\r':
		x = 0;
		break;
	case '\b':
		if (x == 0)
			return;
		--x;
		break;
	}
	if (x >= COLS) {
		x = 0;
		if (++y >= LINES)
			y = 0;
	}
	VScreen.curx = x;
	VScreen.cury = y;
}

static void pokechr (y, x, c)
register y, x, c;
{
	if (VScreen.y[y][x] == c)
		return;
	if (VScreen.firstch[y] == NOCHANGE)
		VScreen.firstch[y] = VScreen.lastch[y] = x;
	else if (x < VScreen.firstch[y])
		VScreen.firstch[y] = x;
	else if (x > VScreen.lastch[y])
		VScreen.lastch[y] = x;
	VScreen.y[y][x] = c;
}

# ifndef MSDOS
static char *skipdelay (cp)
register char *cp;
{
	while (*cp>='0' && *cp<='9')
		++cp;
	if (*cp == '.') {
		++cp;
		while (*cp>='0' && *cp<='9')
			++cp;
	}
	if (*cp == '*')
		++cp;
	return (cp);
}

static void tputs (cp)
register char *cp;
{
	register c;

	if (! cp)
		return;
	cp = skipdelay (cp);
	while (c = *cp++)
		qputch (c);
}

static ucase (c)
register c;
{
	if (TtyUpperCase) {
		c &= 0377;
		if (c>='a' && c<='z')
			c += 'A' - 'a';
		else if (c>=0300 && c<=0336)
			c += 040;
		else if (c == 0337)
			c = '\'';
	}
	return (c);
}
# endif

# ifdef UNDEFINED
void VDelChar ()
{
	register short *temp1, *temp2;
	register short *end;

	end = &VScreen.y [VScreen.cury] [COLS - 1];
	temp1 = &VScreen.y [VScreen.cury] [VScreen.curx];
	temp2 = temp1 + 1;
	while (temp1 < end)
		*temp1++ = *temp2++;
	*temp1 = ' ';
	VScreen.lastch [VScreen.cury] = COLS - 1;
	if (VScreen.firstch [VScreen.cury] == NOCHANGE ||
	    VScreen.firstch [VScreen.cury] > VScreen.curx)
		VScreen.firstch [VScreen.cury] = VScreen.curx;
}

void VInsChar ()
{
	register short *temp1, *temp2;
	register short *end;

	end = &VScreen.y [VScreen.cury] [VScreen.curx];
	temp1 = &VScreen.y [VScreen.cury] [COLS - 1];
	temp2 = temp1 - 1;
	while (temp1 > end)
		*temp1-- = *temp2--;
	*temp1 = ' ';
	VScreen.lastch [VScreen.cury] = COLS - 1;
	if (VScreen.firstch [VScreen.cury] == NOCHANGE ||
	    VScreen.firstch [VScreen.cury] > VScreen.curx)
		VScreen.firstch [VScreen.cury] = VScreen.curx;
}
# endif

static void dogotoxy ();

void VFlush ()
{
# ifndef MSDOS
	if (outptr > outbuf)
		write (1, outbuf, (unsigned) (outptr-outbuf));
	outptr = outbuf;
# else

# ifdef DIRECT
	dogotoxy ();
# endif

# endif
}

void VBeep ()
{
	beepflag = 1;
}

CURSOR VGetCursor ()
{
	return ((CURSOR) ((long) VScreen.cury<<16 | VScreen.curx));
}

void VSetCursor (c)
CURSOR c;
{
	VMove ((int) (c >> 16), (int) c & 0xffff);
}

BOX *VGetBox (y, x, ny, nx)
{
	register xx, yy;
	register short *p, *q;
	BOX *box;

	box = (BOX *) mallo ((int) (sizeof (BOX)));
	box->y = y;
	box->x = x;
	box->ny = ny;
	box->nx = nx;
	box->mem = (short *) mallo ((int) (ny * nx * sizeof (short)));

	for (yy=0; yy<ny; ++yy) {
		p = & VScreen.y [yy+y] [x];
		q = & box->mem [yy*nx];
		for (xx=0; xx<nx; ++xx)
			*q++ = *p++;
	}
	return (box);
}

void VUngetBox (box)
BOX *box;
{
	register xx, yy;
	register short *q;

	for (yy=0; yy<box->ny; ++yy) {
		q = & box->mem [yy * box->nx];
		for (xx=0; xx<box->nx; ++xx)
			pokechr (box->y+yy, box->x+xx, *q++);
	}
}

void VPrintBox (box)
BOX *box;
{
	register xx, yy;
	register short *q;

	for (yy=0; yy<box->ny; ++yy) {
		q = & box->mem [yy * box->nx];
		for (xx=0; xx<box->nx; ++xx)
			pokechr (box->y+yy, box->x+xx,
				*q++ & 0377 | VScreen.flgs & ~0377);
	}
}

void VFreeBox (box)
BOX *box;
{
	mfree ((char *) box->mem, box->ny * box->nx * sizeof (short));
	mfree ((char *) box, sizeof (BOX));
}

void VClearBox (r, c, nr, nc)
{
	register i;

	for (; --nr>=0; ++r)
		for (i=nc; --i>=0;)
			pokechr (r, c+i, ' ');
}

void VFillBox (r, c, nr, nc, sym)
{
	register i;

	for (; --nr>=0; ++r)
		for (i=nc; --i>=0;)
			pokechr (r, c+i, sym | VScreen.flgs & ~0377);
}

void VHorLine (r, c, nc)
register c, nc;
{
	register sym;

	sym = linedraw [0] & 0377 | GRAPH | VScreen.flgs & ~0377;
	while (--nc >= 0)
		pokechr (r, c++, sym);
}

void VVertLine (c, r, nr)
register r, nr;
{
	register sym;

	sym = linedraw [1] & 0377 | GRAPH | VScreen.flgs & ~0377;
	while (--nr >= 0)
		pokechr (r++, c, sym);
}

void VCorner (r, c, n)
register n;
{
	static char map [9] = { 8, 9, 10, 5, 6, 7, 2, 3, 4 };

	switch (n) {
	case 1: case 2: case 3: case 4:
	case 5: case 6: case 7: case 8:
	case 9:
		n = linedraw [map [n-1]] & 0377 | GRAPH;
		break;
	default:
		n = '?';
		break;
	}
	pokechr (r, c, n | VScreen.flgs & ~0377);
}

void VDrawBox (r, c, nr, nc)
register r, c, nr, nc;
{
	VHorLine (r, c+1, nc-2);
	VHorLine (r+nr-1, c+1, nc-2);
	VVertLine (c, r+1, nr-2);
	VVertLine (c+nc-1, r+1, nr-2);
	VCorner (r, c, 1) ;
	VCorner (r, c+nc-1, 3);
	VCorner (r+nr-1, c, 7) ;
	VCorner (r+nr-1, c+nc-1, 9);
}

# define ADDSTR(a)\
	if (a) {\
		strcpy (d, skipdelay (a));\
		while (*d) ++d;\
	}

# define ADDREVERSE(a,b)\
	if (reverse == (a))\
		continue;\
	ADDSTR (b)\
	reverse = (a);

# define ADDCOLOR(a,b,c)\
	if (bright == (a))\
		continue;\
	ADDSTR (reverse ? (b) : (c))\
	bright = (a);

# ifndef MSDOS
void VExpandString (s, d)
register char *s, *d;
{
	/* expand string s to d substituting \1, \2, \3, \16, \17 */
	register c;
	int bright;             /* 1=bold, 0=normal, 2=dim */
	int reverse;

	bright = 0;
	reverse = 0;
	while (c = *s++)
		switch (c) {
		case '\17':     ADDREVERSE (0, SE);     break;
		case '\16':     ADDREVERSE (1, SO);     break;
		case '\1':      ADDCOLOR (2, DR, DS);   break;
		case '\2':      ADDCOLOR (0, NR, NS);   break;
		case '\3':      ADDCOLOR (1, BR, BS);   break;
		default:        *d++ = c;               break;
		}
	*d = 0;
}

/* ARGSUSED */

void VSetRedraw (f)
int (*f) ();
{
# ifdef SIGTSTP
	redraw = f;
# endif /* SIGTSTP */
}

# ifdef SIGTSTP
static void tstp ()
{
	tputs (CapGoto (CM, 0, LINES-1));
	VRestore ();
	kill (0, SIGSTOP);
	VReopen ();
	if (redraw)
		(*redraw) ();
	else
		VRedraw ();
}
# endif /* SIGTSTP */
# endif /* ! MSDOS */

# ifdef MSDOS
# ifdef DIRECT		/* use direct video memory access */

void DOSputch (c)
{
	DOSsymbol far *p;
	union {
		unsigned char c[2];
		unsigned      i;
	} i;

# pragma check_pointer (off)

	i.c[0] = (unsigned char) c;
	i.c[1] = (unsigned char) DOSattr;

	p = & DOSvideomem [DOSy*DOScols + DOSx];
	_asm {
		push	di
		sti
		mov	dx,3dah
		les	di,p
		mov	bx,i.i
		mov	ax,NoWait
		cmp	ax,1
		je	write
		cli
	on:
		in	al,dx
		test	al,1
		jnz	on
	off:
		in	al,dx
		test	al,1
		jz	off
	write:
		mov	ax,bx
	;|***	p->c = c;
	;|***	p->a = DOSattr;
		stosw
		sti
		pop	di
	}

# pragma check_pointer ()

}

static void dogotoxy () {
	inregs.h.dh = (char) DOSy;
	inregs.h.dl = (char) DOSx;
	inregs.h.bh = 0;
	BIOSVIDEO (2, 0);       /* set cursor position */
}

# else /* DIRECT */

void DOSgotoxy (x, y)
{
	--x;
	--y;
	inregs.h.dh = (char) y;
	inregs.h.dl = (char) x;
	inregs.h.bh = 0;
	BIOSVIDEO (2, 0);	/* set cursor position */
}

void DOSputch (c)
{
	c &= 0377;
	inregs.x.cx = 1;
	inregs.h.bh = 0;
	inregs.h.bl = (char) DOSattr;
	BIOSVIDEO (9, c);		/* write character/attribute */
}

# endif /* DIRECT */

void DOSgetinfo (lin, col, attr)
register *lin, *col, *attr;
{
	BIOSVIDEO (5, 0);	/* set active display page = 0 */
	BIOSVIDEO (0xf, 0);	/* read video mode */
	DOScols = *col = outregs.h.ah;
	DOSrows = *lin = 25;

# ifdef DIRECT

	Mode  = outregs.h.al;
	DOSvideomem = (Mode == 7) ? (DOSsymbol far *) 0xb0000000l:
				    (DOSsymbol far *) 0xb8000000l;
	switch (Mode) {
	case 0:
	case 2:
	case 7:
		BlackWhite = 1;
		break;
	default:
		BlackWhite = 0;
	}

	inregs.x.bx = 0xff10;
	inregs.x.cx = 0x000f;
	BIOSVIDEO (0x12, 0);

	if (outregs.h.cl >= 12 || outregs.h.bh > 1 || outregs.h.bl > 3)
		NoWait = Mode == 7;     /* CGA or MDA */
	else
		NoWait = 1;             /* EGA or VGA is present */
# endif

	inregs.x = outregs.x;

	inregs.h.bh = 0;
	BIOSVIDEO (8, 0);		/* read char/attribute */
	*attr = outregs.h.ah;
}

void DOSsetattr (a)
{
	inregs.x.cx = 1;
	inregs.h.bh = 0;
	inregs.h.bl = (char) a;
	BIOSVIDEO (9, 0);		/* write character/attribute */
}

void DOSclrscr ()
{
	inregs.x.cx = 0;
	inregs.h.dh = 24;
	inregs.h.dl = 79;
	inregs.h.bh = (char) DOSstdattr;
	BIOSVIDEO (6, 0);		/* clear screen */

	DOSgotoxy (1, 1);
}

void DOSdelline (l, n)
{
	inregs.h.ch = (char) l;
	inregs.h.cl = 0;
	inregs.h.dh = 24;
	inregs.h.dl = 79;
	inregs.h.bh = 0;
	BIOSVIDEO (6, n);	/* scrool up */
}

void DOSinsline (l, n)
{
	inregs.h.ch = (char) l;
	inregs.h.cl = 0;
	inregs.h.dh = 24;
	inregs.h.dl = 79;
	inregs.h.bh = 0;
	BIOSVIDEO (7, n);	/* scrool down */
}

# endif /* MSDOS */

# ifdef DOC
# include <stdio.h>

_prscreen ()
{
	static char filename [] = "screenX",fname[128];
	static count = 0;
	register FILE *fd;
	register short y, x, c, a, m;
	char *toname;

	filename [6] = 'A' + count;
	toname = getenv ("DECOSCREEN");
	if (! toname)
		toname = ".";
	strcpy (fname, toname);
	strcat (fname, "/");
	strcat (fname, filename);
	fd = fopen (fname, "w");
	if (! fd)
		return;
	++count;
	a = 0;
	m = BOLD | DIM;
	for (y=0; y<LINES; ++y) {
		for (x=0; x<COLS; ++x) {
			c = curscr.y [y] [x];
			if ((a & GRAPH) != (c & GRAPH))
				putc (c & GRAPH ? cntrl ('[') : cntrl ('\\'), fd);
			if ((a & STANDOUT) != (c & STANDOUT))
				putc (c & STANDOUT ? cntrl ('n') : cntrl ('o'), fd);
			if ((a & m) != (c & m))
				switch (c & m) {
				case BOLD:      putc (3, fd);   break;
				case DIM:       putc (1, fd);   break;
				case 0:         putc (2, fd);   break;
				}
			putc (c & GRAPH ? _graphsym (c) : c, fd);
			a = c;
		}
		putc ('\n', fd);
	}
	fclose (fd);
}

_graphsym (c)
register c;
{
	register i;

	c &= 0xff;
	for (i=0; i<11; ++i)
		if (c == linedraw [i])
			switch (i) {
			case 0:         /* horisontal line */
				return ('-');
			case 1:         /* vertical line */
				return ('|');
			case 2:         /* lower left corner */
			case 3:         /* lower center */
			case 4:         /* lower right corner */
			case 5:         /* left center */
			case 6:         /* center cross */
			case 7:         /* right center */
			case 8:         /* upper left corner */
			case 9:         /* upper center */
			case 10:        /* upper right corner */
				return ('0' + i - 1);
			}
	return ('?');
}
# endif
