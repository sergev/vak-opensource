/*
 * Copyright (C) 1997-2002 Serge Vakulenko <vak@cronyx.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 */
/*
 * Multiply unsigned short by short.
 * Left arg is (A1,A4), right is (A6,A5).
 * Leave the result in (A3,A2,A1,WREG).
 * Leave A6,A5 untouched.
 */
#include "pic17c43.inc"

#define X0   A4
#define X1   A1
#define X2   A2
#define X3   A3
#define Y0   A5
#define Y1   A6
#define TEMP A7

mul22:
	xta     X1
	atx     TEMP            /* temp := x1 */

	a*x     Y1
	rtx     PRODH,X3        /* x3x2 := x1 * y1 */
	rtx     PRODL,X2

	xta     X0
	a*x     Y0
	rtx     PRODH,X1        /* x1x0 := x0 * y0 */
	rtx     PRODL,X0

	a*x     Y1              /* x0 * y1 */
	xta     PRODL
	x+a     X1
	xta     PRODH
	x+ca    X2
	az
	x+ca    X3

	xta     TEMP
	a*x     Y0              /* x1 * y0 */
	xta     PRODL
	x+a     X1
	xta     PRODH
	x+ca    X2
	az
	x+ca    X3

	xta     X0
	ret
