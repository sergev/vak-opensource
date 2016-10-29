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
 * Compare "==" unsigned short.
 * Leave the result in WREG.
 * Call (A==B):
 *      xtr Ahigh, A1
 *      xtr Alow, A2
 *      xtr Blow, A3
 *      xta Bhigh
 *      call eq22
 */
#include "pic17c43.inc"

#define Ahigh A1
#define Alow  A2
#define Blow  A3

eq22:
	x!=a?   Ahigh   /* if Ahigh!=Bhigh... */
	retc    0       /* ...return false */

	xta     Blow
	x!=a?   Alow    /* if Alow!=Blow... */
	retc    0       /* ...return false */

	retc    1       /* true */
