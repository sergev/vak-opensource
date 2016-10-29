# include <stdio.h>
# include "key.h"
# include "jlib.h"

extern char *malloc (), *realloc ();
extern VFlush ();

static struct KeyMap keymap [] = {
	"kl",           0,              meta ('l'),     0,
	"kr",           0,              meta ('r'),     0,
	"ku",           0,              meta ('u'),     0,
	"kd",           0,              meta ('d'),     0,
	"kN",           0,              meta ('n'),     0,
	"kP",           0,              meta ('p'),     0,
	"kh",           0,              meta ('h'),     0,
# ifndef LKEYS
	"k1",           0,              meta ('e'),     0,
	"kH",           0,              meta ('e'),     0,
# endif
	"k0",           0,              cntrl ('T'),    0,
	"kI",           0,              cntrl ('T'),    0,
	"k.",           0,              cntrl ('G'),    0,
	"kD",           0,              cntrl ('G'),    0,
	"kE",           0,              meta ('h'),     0,
	"kS",           0,              meta ('e'),     0,
# ifdef LKEYS
	"k1",           0,              meta ('A'),     0,
	"k2",           0,              meta ('B'),     0,
	"k3",           0,              meta ('C'),     0,
	"k4",           0,              meta ('D'),     0,
	"l5",           0,              meta ('E'),     0,
	"l6",           0,              meta ('F'),     0,
	"l7",           0,              meta ('G'),     0,
	"l8",           0,              meta ('H'),     0,
	"l9",           0,              meta ('I'),     0,
	"l0",           0,              meta ('J'),     0,
# else
	"f1",           0,              meta ('A'),     0,
	"f2",           0,              meta ('B'),     0,
	"f3",           0,              meta ('C'),     0,
	"f4",           0,              meta ('D'),     0,
	"f5",           0,              meta ('E'),     0,
	"f6",           0,              meta ('F'),     0,
	"f7",           0,              meta ('G'),     0,
	"f8",           0,              meta ('H'),     0,
	"f9",           0,              meta ('I'),     0,
	"f0",           0,              meta ('J'),     0,
# endif
	0,              "\0331",        meta ('A'),     0,
	0,              "\0332",        meta ('B'),     0,
	0,              "\0333",        meta ('C'),     0,
	0,              "\0334",        meta ('D'),     0,
	0,              "\0335",        meta ('E'),     0,
	0,              "\0336",        meta ('F'),     0,
	0,              "\0337",        meta ('G'),     0,
	0,              "\0338",        meta ('H'),     0,
	0,              "\0339",        meta ('I'),     0,
	0,              "\0330",        meta ('J'),     0,

	0,              "\033l",        meta ('l'),     0,
	0,              "\033r",        meta ('r'),     0,
	0,              "\033u",        meta ('u'),     0,
	0,              "\033d",        meta ('d'),     0,
	0,              "\033n",        meta ('n'),     0,
	0,              "\033p",        meta ('p'),     0,
	0,              "\033h",        meta ('h'),     0,
	0,              "\033e",        meta ('e'),     0,

	0,              "\177",         cntrl ('H'),    0,
	0,              0,              0,              0,
};

init ()
{
	char buf [2048];

	if (! CapInit (buf)) {
		outerr ("cannot read termcap\n");
		exit (1);
	}
	if (! VInit ()) {
		outerr ("cannot initialize terminal\n");
		exit (1);
	}
	if (COLS < 80) {
		outerr ("must be 80 columns on terminal\n");
		exit (1);
	}
	KeyInit (keymap, VFlush);
	VSetPalette (11, 1, 3, 1, 15, 1, 14, 4, 15, 0, 0, 7);
}

char *mallo (n)
{
	register char *p;

	if (! (p = malloc ((unsigned) n))) {
		VMove (LINES-1, 0);
		VClearLine ();
		VSync ();
		VClose ();
		outerr ("out of memory\n");
		exit (1);
	}
	return (p);
}

char *reallo (p, n)
register char *p;
{
	if (! (p = realloc (p, (unsigned) n))) {
		VMove (LINES-1, 0);
		VClearLine ();
		VSync ();
		VClose ();
		outerr ("out of memory\n");
		exit (1);
	}
	return (p);
}

/* VARARGS 1 */
/* ARGSUSED */

mfree (p, n)
register char *p;
{
	free (p);
}

char *mdup (s)
char *s;
{
	char *p;

	p = mallo (strlen (s) + 1);
	strcpy (p, s);
	return (p);
}

hidecursor ()
{
	VMove (LINES-1, COLS-2);
}

mvcaddstr (r, c, s)
char *s;
{
	VMPutString (r, c - (strlen (s) + 1) / 2, s);
}

/* VARARGS1 */

printw (fmt, a,b,c,d,e,f,g,h)
char *fmt;
{
	char buf [512];

	sprintf (buf, fmt, a,b,c,d,e,f,g,h);
	VPutString (buf);
}

mprintw (row, col, fmt, a,b,c,d,e,f,g,h)
char *fmt;
{
	char buf [512];

	sprintf (buf, fmt, a,b,c,d,e,f,g,h);
	VMPutString (row, col, buf);
}

outerr (s)
char *s;
{
	write (2, s, strlen (s));
}
