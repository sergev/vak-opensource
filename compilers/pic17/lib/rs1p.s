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
 * Right unsigned byte shift by pointer.
 * Shift INDF0 by the counter in WREG.
 */
#include "pic17c43.inc"

rs1p:
	a&c     7
	z?
	ret
1:      bz      ALUSTA,C
	xc>>x   INDF0
	a--?
	goto    1b
	ret
