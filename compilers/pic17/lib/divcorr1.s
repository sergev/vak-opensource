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
 * Divide by constant correction routine.
 * Parameters:
 *   A0 - divider
 *   A1 - dividend
 *   PRODH - approximation
 * Return:
 *   A0 - quotient
 *   A1 - remainder
 */
#include "pic17c43.inc"

divcorr1:
	atx	A3		/* A3 - divider */
	xtr	PRODH,A2	/* A2 - approximation */
	a*x	A2
	xta	PRODL		/* divider * approximation */
	x-a	A1		/* remainder = divider * approximation */

	nb?			/* if remainder >= 0... */
	goto	1f		/* ...fine */

	x--	A2		/* else decrement approximation */
	xta	A3
	x+a	A1		/* remainder += divider */
1:
	xta	A2
	ret
