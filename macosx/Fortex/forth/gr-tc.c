/*
 *+ Portable Forth interpreter - graphics commands.
 *+
 *+ Copyright (C) 1990-2006 Serge Vakulenko, <vak@cronyx.ru>.
 *+
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 */
#include <stdio.h>
#include <bios.h>
#include <graphics.h>
#include "forth.h"

/*
 *+ grinit ( -- maxx maxy maxc )
 *+	Initialize graphics mode.
 *+	Resolution is maxx*maxy, maxc+1 colors available.
 */
Fgrinit ()
{
	integer_t maxx, maxy, maxc;
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
 *+ grclear ( -- )
 *+	Clear graphics screen.
 */
Fgrclear ()
{
	cleardevice ();
}

/*
 *+ grline ( xto yto xfrom yfrom c -- xto yto rc )
 *+	Draw line from (xfrom,yfrom) to (xto,yto), color c.
 *+	Leave in stack coordinates of the end point and
 *+	resulting color.  This allows drawing multilines:
 *+	1 1 2 5 3 2 4 4 1 grline grline grline drop 2drop
 */
Fgrline ()
{
	integer_t c = pop ();
	integer_t yfrom = pop ();
	integer_t xfrom = pop ();
	integer_t yto = pop ();
	integer_t xto = pop ();
	setcolor (c);
	c = getcolor ();
	line (xfrom, yfrom, xto, yto);
	push (xto);
	push (yto);
	push (c);
}

/*
 *+ grend ( -- )
 *+	Restore text mode.
 */
Fgrend ()
{
	closegraph ();
}

/*
 *+ getch ( -- ch )
 *+	Get character from the console.
 *+	Keypad and function keys have 0400 bit set.
 */
Fgetch ()
{
	integer_t c;

	c = bioskey (0) & 0377;
	if (! c)
		c = bioskey (0) & 0377 | 0400;
	push (c);
}

/*
 *+ kbhit ( -- hit )
 *+	Check if the key was hit.  If keystroke is available,
 *+	return 1 else 0.
 */
Fkbhit ()
{
	integer_t hit;

	hit = bioskey (1) ? 1 : 0;
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
