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
 * Compare ">" unsigned long.
 * Leave the result in WREG.
 * Call (X>Y):
 *      xtr X1, A1
 *      xtr X2, A2
 *      xtr X3, A3
 *      xtr X0, A4
 *      xtr Y0, A5
 *      xtr Y1, A6
 *      xtr Y2, A7
 *      xta Y3
 *      call gt44
 */
#include "pic17c43.inc"

#define X1 A1
#define X2 A2
#define X3 A3
#define X0 A4
#define Y0 A5
#define Y1 A6
#define Y2 A7

gt44:
	a-x     X3      /* WREG=X3-Y3, C=(X3>=Y3) */
	IF_LT           /* if X3<Y3... */
	retc    0       /* ...return false */
	nz?
	retc    1       /* if X3>Y3 return true */

	xta     Y2
	a-x     X2      /* WREG=X2-Y2, C=(X2>=Y2) */
	IF_LT           /* if X2<Y2... */
	retc    0       /* ...return false */
	nz?
	retc    1       /* if X2>Y2 return true */

	xta     Y1
	a-x     X1      /* WREG=X1-Y1, C=(X1>=Y1) */
	IF_LT           /* if X1<Y1... */
	retc    0       /* ...return false */
	nz?
	retc    1       /* if X1>Y1 return true */

	xta     Y0
	a-x     X0      /* WREG=X0-Y0, C=(X0>=Y0) */
	IF_LE_GOTO 1f   /* if X0<=Y0 goto false */
	retc    1       /* true */

1:	retc    0       /* false */
