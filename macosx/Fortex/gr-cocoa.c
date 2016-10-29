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
#include "forth.h"

extern void cocoa_grinit (int *maxx, int *maxy);
extern void cocoa_grclear (void);
extern void cocoa_grline (int xfrom, int yfrom, int xto, int yto, int c);

/*
 *+ grinit ( -- maxx maxy maxc )
 *+	Initialize graphics mode.
 *+	Resolution is maxx*maxy, maxc+1 colors available.
 */
void Fgrinit ()
{
	integer_t maxx, maxy, maxc;

	cocoa_grinit (&maxx, &maxy);
	maxc = 7;
	push (maxx);
	push (maxy);
	push (maxc);
}

/*
 *+ grclear ( -- )
 *+	Clear graphics screen.
 */
void Fgrclear ()
{
	cocoa_grclear ();
}

/*
 *+ grline ( xto yto xfrom yfrom c -- xto yto rc )
 *+	Draw line from (xfrom,yfrom) to (xto,yto), color c.
 *+	Leave in stack coordinates of the end point and
 *+	resulting color.  This allows drawing multilines:
 *+	1 1 2 5 3 2 4 4 1 grline grline grline drop 2drop
 */
void Fgrline ()
{
	integer_t c = pop ();
	integer_t yfrom = pop ();
	integer_t xfrom = pop ();
	integer_t yto = pop ();
	integer_t xto = pop ();
	cocoa_grline (xfrom, yfrom, xto, yto, c);
	push (xto);
	push (yto);
	push (c);
}

/*
 *+ grend ( -- )
 *+	Restore text mode.
 */
void Fgrend ()
{
}

/*
 *+ getch ( -- ch )
 *+	Get character from the console.
 *+	Keypad and function keys have 0400 bit set.
 */
void Fgetch ()
{
	push ('\n');
}

struct table gr_table [] = {
	"grinit",       Fgrinit,        FHARDWARE,      0,
	"grclear",      Fgrclear,       FHARDWARE,      0,
	"grline",       Fgrline,        FHARDWARE,      0,
	"grend",        Fgrend,         FHARDWARE,      0,
	"getch",        Fgetch,         FHARDWARE,      0,

	0,              0,              0,              0,
};
