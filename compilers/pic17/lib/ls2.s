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
 * Left unsigned short shift.
 * Shift (A1,A2) by the counter in WREG.
 * Leave the result in (A1,WREG).
 */
#include "pic17c43.inc"

ls2:
	a&c     15
	a?
	goto	1f
	xta	A2
	ret
1:      bz      ALUSTA,C
	xc<<x   A2
	xc<<x   A1
	a--?
	goto    1b
	xta     A2
	ret
