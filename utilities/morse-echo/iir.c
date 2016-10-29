/*
 * Generic infinite impulse response filter.
 * Copyright (C) 2005 Serge Vakulenko, <vak@cronyx.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING" for more details.
 */
#include <string.h>
#include <stdarg.h>
#include "iir.h"

/*
 * Initialize IIR filter with N poles.
 * Parameters are N+1 coefficients A(0)..A(N)
 * and N coefficients B(1)..B(N).
 */
void iir_init (iir_t *c, int n, ...)
{
	va_list args;
	int i;

	memset (c, 0, sizeof (*c));
	c->npoles = n;

	va_start (args, n);

	/* N+1 coefficients A(0)..A(N) */
	for (i=0; i<=n; ++i)
		c->a[i] = va_arg (args, double);

	/* N coefficients B(1)..B(N) */
	c->b[0] = 1;
	for (i=1; i<=n; ++i)
		c->b[i] = va_arg (args, double);

	va_end (args);
}

/*
 * Pass a sample through filter.
 */
double iir_filter (iir_t *c, double x)
{
	int i;
	double y;

	/* Time shift */
	for (i=c->npoles; i>0; --i) {
		c->x[i] = c->x[i-1];
		c->y[i] = c->y[i-1];
	}
	c->x[0] = x;

	/* Compute output */
	y = x * c->a[0];
	for (i=1; i<=c->npoles; ++i) {
		y += c->x[i] * c->a[i] + c->y[i] * c->b[i];
	}
	c->y[0] = y;

	return y;
}
