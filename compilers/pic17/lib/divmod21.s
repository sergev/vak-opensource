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
 * Unsigned short-by-char divide/remainder.
 * Divide (A1,A2) by A3.
 * Leave the quotient in (A1,A2),
 * the remainder in A4.
 */
#include "pic17c43.inc"

#define Alow    A2
#define Ahigh   A1
#define B       A3
#define REM     A4
#define TEMP    A5

#define DIVHIGH \
	xc<<a   Ahigh; \
	xc<<x   REM; \
	xta     B; \
	x-a     REM; \
	nb?; \
	goto    1f; \
	x+a     REM; \
	bz      ALUSTA,C; \
1:      xc<<x   Ahigh

#define DIVLOW \
	xc<<a   Alow; \
	xc<<x   REM; \
	xc<<x   TEMP; \
	xta     B; \
	x-a     REM; \
	az; \
	x-ba    TEMP; \
	nb?; \
	goto 1f; \
	xta     B; \
	x+a     REM; \
	az; \
	x+ca    TEMP; \
	bz      ALUSTA,C; \
1:      xc<<x   Alow

divmod21:
	xz      REM
	DIVHIGH
	DIVHIGH
	DIVHIGH
	DIVHIGH
	DIVHIGH
	DIVHIGH
	DIVHIGH
	DIVHIGH

	xz      TEMP
	DIVLOW
	DIVLOW
	DIVLOW
	DIVLOW
	DIVLOW
	DIVLOW
	DIVLOW
	DIVLOW

	ret
