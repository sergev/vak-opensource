/*
 *+	Portable Forth interpreter.
 *+
 *+	Copyright (C) 1990-92 Serge Vakulenko, <vak@kiae.su>.
 *+
 */

# include <stdio.h>
# include <bios.h>
# include <graphics.h>
# include "forth.h"

/*
 *+     grinit ( -- maxx maxy maxc )
 *+             Initialize graphics mode.
 *+		Resolution is maxx*maxy, maxc+1 colors available.
 */

Fgrinit ()
{
	integer maxx, maxy, maxc;
	int drv, mode, xasp, yasp;

	registerbgidriver (EGAVGA_driver);
	/* registerbgidriver (CGA_driver); */
	/* registerbgidriver (Herc_driver); */
	detectgraph (&drv, &mode);
	initgraph (&drv, &mode, (char *) 0);
	maxx = getmaxx ();
	maxy = getmaxy ();
	maxc = getmaxcolor();
	getaspectratio (&xasp, &yasp);
	push (maxx);
	push (maxy);
	push (maxc);
}

/*
 *+     grclear ( -- )
 *+		Clear graphics screen.
 */

Fgrclear ()
{
	cleardevice ();
}

/*
 *+     grline ( xto yto xfrom yfrom c -- xto yto rc )
 *+             Draw line from (xfrom,yfrom) to (xto,yto), color c.
 *+		Leave in stack coordinates of the end point and
 *+		resulting color.  This allows drawing multilines:
 *+		1 1 2 5 3 2 4 4 1 grline grline grline drop 2drop
 */

Fgrline ()
{
	integer c = pop ();
	integer yfrom = pop ();
	integer xfrom = pop ();
	integer yto = pop ();
	integer xto = pop ();
	setcolor (c);
	c = getcolor ();
	line (xfrom, yfrom, xto, yto);
	push (xto);
	push (yto);
	push (c);
}

/*
 *+     grend ( -- )
 *+		Restore text mode.
 */

Fgrend ()
{
	closegraph ();
}

/*
 *+     getch ( -- ch )
 *+		Get character from the console.
 *+		Keypad and function keys have 0400 bit set.
 */

Fgetch ()
{
	register integer c = bioskey (0) & 0377;
	if (! c)
		c = bioskey (0) & 0377 | 0400;
	push (c);
}

/*
 *+     kbhit ( -- hit )
 *+		Check if the key was hit.  If keystroke is available,
 *+		return 1 else 0.
 */

Fkbhit ()
{
	register integer hit = bioskey (1) ? 1 : 0;
	push (hit);
}

struct table grtable [] = {
	"grinit",       Fgrinit,        FHARDWARE,      0,
	"grclear",      Fgrclear,       FHARDWARE,      0,
	"grline",       Fgrline,        FHARDWARE,      0,
	"grend",        Fgrend,         FHARDWARE,      0,
	"getch",        Fgetch,         FHARDWARE,      0,
	"kbhit",        Fkbhit,         FHARDWARE,      0,

	0,              0,              0,              0,
};
