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
#define IIR_MAXPOLES	20

typedef struct {
	int npoles;
	double a [IIR_MAXPOLES + 1];
	double b [IIR_MAXPOLES + 1];
	double x [IIR_MAXPOLES + 1];
	double y [IIR_MAXPOLES + 1];
} iir_t;

void iir_init (iir_t *c, int n, ...);
double iir_filter (iir_t *c, double x);
