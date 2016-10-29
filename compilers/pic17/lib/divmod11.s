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
 * Unsigned divide/remainder.
 * Divide A1 by A2.
 * Leave the quotient in A1,
 * the remainder in A3.
 * (A1, A3) := (A1/A2, A1%A2)
 */
#include "pic17c43.inc"

#define LA      A1
#define LDIV    A2
#define LREM    A3

#define DIVBIT \
	xc<<a   LA; \
	xc<<x   LREM; \
	xta     LDIV; \
	x-a     LREM; \
	nb?; \
	goto    1f; \
	x+a     LREM; \
	bz      ALUSTA,C; \
1:      xc<<x   LA

divmod11:
	xz      LREM
	DIVBIT
	DIVBIT
	DIVBIT
	DIVBIT
	DIVBIT
	DIVBIT
	DIVBIT
	DIVBIT
	ret
