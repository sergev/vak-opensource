# include <stdio.h>
# include "jlib.h"

/*
 * TTYADDR lower byte - tty output buffer
 * TTYADDR+1 lower byte - tty input buffer
 */
# define TTYADDR	0x3fff0L
# define TTYBUFSZ	2048

int (*funchome) ();
int (*funcend) ();
int (*funcpgup) ();
int (*funcpgdn) ();
int (*funcup) ();
int (*funcdown) ();
int (*funcleft) ();
int (*funcright) ();

extern win2 (), win3 (), win4 (), win5 (), win6 (), win7 (), win8 (), win0 ();

extern ctrl1 (), ctrl2 (), ctrl3 (), ctrl4 (), ctrl5 (), ctrl6 (), ctrl7 (), ctrl8 ();
extern bc0 (), bc1 (), bc2 (), bc3 (), bc4 (), bc5 (), bc6 (), bc7 ();
extern mem0 (), mem1 (), mem2 (), mem3 (), mem4 (), mem5 (), mem6 (), mem7 (), mem8 ();
extern help ();

static (*wfunc [10] [10]) () = {
win0,	win0,	win2,	win3,	win4,	win5,	win6,	win7,	win8,	help,
win0,	ctrl1,	ctrl2,	ctrl3,	ctrl4,	ctrl5,	ctrl6,	ctrl7,	ctrl8,	help,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
win0,	bc0,	bc1,	bc2,	bc3,	bc4,	bc5,	bc6,	bc7,	help,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
mem0,	mem1,	mem2,	mem3,	mem4,	mem5,	mem6,	mem7,	mem8,	help,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
};

static char *fkeyname [10] [10] = {
	" Рег  ", " Брс  ", " Брз  ", "МП СК ", " Прер ",
	"МП БК ", "Прип1 ", "Прип2 ", "Справ ", " Общ ",

	" У0ио ", " У0   ", " Ор   ", " Ар   ", "ПктАр ",
	"Ркп:= ", " Заг  ", "      ", "Справ ", " Общ ",

	"      ", "      ", "      ", "      ", "      ",
	"      ", "      ", "      ", "      ", "     ",

	" БК 0 ", " БК 1 ", " БК 2 ", " БК 3 ", " БК 4 ",
	" БК 5 ", " БК 6 ", " БК 7 ", "Справ ", " Общ ",

	"      ", "      ", "      ", "      ", "      ",
	"      ", "      ", "      ", "      ", "     ",

	"      ", "      ", "      ", "      ", "      ",
	"      ", "      ", "      ", "      ", "     ",

	"Адрес1", "Адрес2", " 1/2  ", " Ред  ", " Д/К  ",
	"Формат", " Мнем ", "Кодир ", "Справ ", " Общ ",

	"      ", "      ", "      ", "      ", "      ",
	"      ", "      ", "      ", "      ", "     ",

	"      ", "      ", "      ", "      ", "      ",
	"      ", "      ", "      ", "      ", "     ",

	"      ", "      ", "      ", "      ", "      ",
	"      ", "      ", "      ", "      ", "     ",
};

static char *nkeyname [10] = {
	" Управ ", "       ", "БайтКан", "       ", "       ",
	"Память ", "       ", "       ", " Справ ", " Инфор ",
};

static char ttybuf [TTYBUFSZ];
int tboff, tbbeg;

int curgroup = 0;
int curwin = 0;
int oldgroup = 0;
int oldwin = 0;

extern halting;
extern daemonflag;

dowin ()
{
	register c;
loop:
	Jmove (LINES-1, COLS-1);
	switch (c = getch ()) {
	default:
		goto loop;
	case cntrl (']'):
		VRedraw ();
		goto loop;
	case cntrl ('_'):
		if (! daemonflag)
			return;
		goto loop;
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		curgroup = c - '0';
		curwin = 0;
		break;
	case '*':					/* step */
		step ();
		break;
	case meta ('h'): if (funchome)  (*funchome) (); goto loop;
	case meta ('e'): if (funcend)   (*funcend) ();  goto loop;
	case meta ('p'): if (funcpgup)  (*funcpgup) (); goto loop;
	case meta ('n'): if (funcpgdn)  (*funcpgdn) (); goto loop;
	case meta ('u'): if (funcup)    (*funcup) ();   goto loop;
	case meta ('d'): if (funcdown)  (*funcdown) (); goto loop;
	case meta ('l'): if (funcleft)  (*funcleft) (); goto loop;
	case meta ('r'): if (funcright) (*funcright) (); goto loop;
	case meta ('J'):
		curwin = 0;
		break;
	case meta ('A'): case meta ('B'):
	case meta ('C'): case meta ('D'):
	case meta ('E'): case meta ('F'):
	case meta ('G'): case meta ('H'):
	case meta ('I'):
		curwin = c - meta ('A') + 1;
		break;
	}
	funcclear ();
	pultwin (curgroup, curwin);
	goto loop;
}

pultwin (g, w)
{
	register (*f) ();

	f = wfunc [g] [w];
	if (oldgroup != g || oldwin != w) {
		Jclear ();
		oldgroup = g;
		oldwin = w;
	}
	prmenu ();
	if (! f) {
		Jmove (12, 40-10);
		Jprintf ("Мы вам покажем ! %d%d", g, w);
		return;
	}
	(*f) ();
}

prmenu ()
{
	register i;

	for (i=0; i<10; ++i) {
		Jdim ();
		Jmove (0, i*8);
		Jprintf ("F%d", i+1);
		if ((i+1)%10 == curwin)
			Jrbold ();
		else
			Jrdim ();
		Jputstr (fkeyname [curgroup] [i]);
		Jmove (24, i*8);
		Jdim ();
		Jprintf ("%d", i<9 ? i+1 : 0);
		if ((i+1)%10 == curgroup)
			Jrbold ();
		else
			Jrdim ();
		Jputstr (nkeyname [i]);
	}
	Jnorm (0x1b);
}

static funcclear ()
{
	funchome = funcend = funcpgup = funcpgdn = funcup = funcdown = funcleft = funcright = 0;
}
