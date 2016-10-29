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
 * Unsigned short-by-short divide/remainder.
 * Divide (A1,A2) by (A3,A4).
 * Leave the quotient in (A1,A2),
 * the remainder in (A5,A6).
 */
#include "pic17c43.inc"

#define A_l     A2
#define A_h     A1
#define B_l     A3
#define B_h     A4
#define REM_l   A5
#define REM_h   A6

#define DIVHIGH \
	xc<<a   A_h; \
	xc<<x   REM_l; \
	xc<<x   REM_h; \
	xta     B_l; \
	x-a     REM_l; \
	xta     B_h; \
	x-ba    REM_h; \
	nb?; \
	goto    1f; \
	xta     B_l; \
	x+a     REM_l; \
	xta     B_h; \
	x+ca    REM_h; \
	bz      ALUSTA,C; \
1:      xc<<x   A_h

#define DIVLOW \
	xc<<a   A_l; \
	xc<<x   REM_l; \
	xc<<x   REM_h; \
	xta     B_l; \
	x-a     REM_l; \
	xta     B_h; \
	x-ba    REM_h; \
	nb?; \
	goto    1f; \
	xta     B_l; \
	x+a     REM_l; \
	xta     B_h; \
	x+ca    REM_h; \
	bz      ALUSTA,C; \
1:      xc<<x   A_l

divmod22:
	xz      REM_l
	xz      REM_h

	DIVHIGH
	DIVHIGH
	DIVHIGH
	DIVHIGH
	DIVHIGH
	DIVHIGH
	DIVHIGH
	DIVHIGH

	DIVLOW
	DIVLOW
	DIVLOW
	DIVLOW
	DIVLOW
	DIVLOW
	DIVLOW
	DIVLOW

	ret
