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
 * Unsigned long-by-long divide/remainder.
 * Divide (A3,A2,A1,A4) by (A8,A7,A6,A5).
 * Leave the quotient in (A3,A2,A1,A4),
 * the remainder in (A12,A11,A10,A9).
 * A13 is used as a temporary variable.
 */
#include "pic17c43.inc"

#define X0     A4
#define X1     A1
#define X2     A2
#define X3     A3
#define Y0     A5
#define Y1     A6
#define Y2     A7
#define Y3     A8
#define REM0   A9
#define REM1   A10
#define REM2   A11
#define REM3   A12

#define TEMP   A13

#define DIV(XN) \
	xc<<a   XN; \
	xc<<x   REM0; \
	xc<<x   REM1; \
	xc<<x   REM2; \
	xc<<x   REM3; \
	xc<<x   TEMP; \
	xta     Y0; \
	bz?     XN,0; \
	goto    1f; \
	x-a     REM0; \
	xta     Y1; \
	x-ba    REM1; \
	xta     Y2; \
	x-ba    REM2; \
	xta     Y3; \
	x-ba    REM3; \
	az; \
	x-ba    TEMP; \
	goto    2f; \
1:      x+a     REM0; \
	xta     Y1; \
	x+ca    REM1; \
	xta     Y2; \
	x+ca    REM2; \
	xta     Y3; \
	x+ca    REM3; \
	az; \
	x+ca    TEMP; \
2:      xc<<x   XN

#define DIV2(XN,XK) \
	xc<<a   XK; \
	xc<<x   REM0; \
	xc<<x   REM1; \
	xc<<x   REM2; \
	xc<<x   REM3; \
	xc<<x   TEMP; \
	xta     Y0; \
	bz?     XN,0; \
	goto    3f; \
	x-a     REM0; \
	xta     Y1; \
	x-ba    REM1; \
	xta     Y2; \
	x-ba    REM2; \
	xta     Y3; \
	x-ba    REM3; \
	az; \
	x-ba    TEMP; \
	goto    4f; \
3:      x+a     REM0; \
	xta     Y1; \
	x+ca    REM1; \
	xta     Y2; \
	x+ca    REM2; \
	xta     Y3; \
	x+ca    REM3; \
	az; \
	x+ca    TEMP; \
4:      xc<<x   XK

divmod44:
	xz      REM0
	xz      REM1
	xz      REM2
	xz      REM3

	xc<<a   X3
	xc<<x   REM0
	xc<<x   REM1
	xc<<x   REM2
	xc<<x   REM3
	xta     Y0
	x-a     REM0
	xta     Y1
	x-ba    REM1
	xta     Y2
	x-ba    REM2
	xta     Y3
	x-ba    REM3
	xza     TEMP
	x-ba    TEMP
	xc<<x   X3

	DIV (X3)
	DIV (X3)
	DIV (X3)
	DIV (X3)
	DIV (X3)
	DIV (X3)
	DIV (X3)
	DIV2 (X3,X2)

	DIV (X2)
	DIV (X2)
	DIV (X2)
	DIV (X2)
	DIV (X2)
	DIV (X2)
	DIV (X2)
	DIV2 (X2,X1)

	DIV (X1)
	DIV (X1)
	DIV (X1)
	DIV (X1)
	DIV (X1)
	DIV (X1)
	DIV (X1)
	DIV2 (X1,X0)

	DIV (X0)
	DIV (X0)
	DIV (X0)
	DIV (X0)
	DIV (X0)
	DIV (X0)
	DIV (X0)

	bs?     X0,0
	ret

	xta     Y0
	x+a     REM0
	xta     Y1
	x+ca    REM1
	xta     Y2
	x+ca    REM2
	xta     Y3
	x+ca    REM3
	ret
