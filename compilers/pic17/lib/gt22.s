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
 * Compare ">" unsigned short.
 * Leave the result in WREG.
 * Call (A>B):
 *      xtr Ahigh, A1
 *      xtr Alow, A2
 *      xtr Blow, A3
 *      xta Bhigh
 *      call gt22
 */
#include "pic17c43.inc"

#define Ahigh A1
#define Alow  A2
#define Blow  A3

gt22:
	a-x     Ahigh   /* WREG=Ahigh-Bhigh, C=(Ahigh>=Bhigh) */
	IF_LT           /* if Ahigh<Bhigh... */
	retc    0       /* ...return false */

	nz?
	retc    1       /* if Ahigh>Bhigh return true */

	xta     Blow
	a-x     Alow    /* WREG=Alow-Blow, C=(Alow>=Blow) */
	IF_LE_GOTO 1f   /* if Alow<=Blow goto false */
	retc    1       /* true */

1:	retc    0       /* false */
