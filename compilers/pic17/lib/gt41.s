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
 * Compare unsigned long ">" unsigned char.
 * Leave the result in WREG.
 * Call (X>Y):
 *      xtr X1, A1
 *      xtr X2, A2
 *      xtr X3, A3
 *      xtr X0, A4
 *      xta Y
 *      call gt41
 */
#include "pic17c43.inc"

#define X1      A1
#define X2      A2
#define X3      A3
#define X0      A4

gt41:
	x?      X1
	retc    1       /* true */
	x?      X2
	retc    1       /* true */
	x?      X3
	retc    1       /* true */

	a-x     X0      /* WREG=X0-Y, C=(X0>=Y) */
	IF_LE_GOTO 1f   /* if X0<=Y goto false */
	retc    1       /* true */

1:      retc    0       /* false */
