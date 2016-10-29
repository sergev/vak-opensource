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
 * Divide INDF0 by WREG.
 * Leave the quotient in INDF0,
 * the remainder in A3.
 * (INDF0, A3) := (INDF0/WREG, INDF0%WREG)
 */
#include "pic17c43.inc"

#define LA      INDF0
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

divmod11p:
	atx     A2
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
