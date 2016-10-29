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
 * Left unsigned long shift.
 * Shift (A3,A2,A1,A4) by the counter in WREG.
 * Leave the result in (A3,A2,A1,WREG).
 */
#include "pic17c43.inc"

ls4:
	a&c     31
	a?
	goto	1f
	xta	A4
	ret
1:      bz      ALUSTA,C
	xc<<x   A4
	xc<<x   A1
	xc<<x   A2
	xc<<x   A3
	a--?
	goto    1b
	xta     A4
	ret
