/*
 * Fast Fourier transform.
 *
 * Copyright (C) 1998 Cronyx Ltd.
 * Author: Serge Vakulenko, <vak@cronyx.ru>
 *
 * This software is distributed with NO WARRANTIES, not even the implied
 * warranties for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Authors grant any other persons or organisations permission to use
 * or modify this software as long as this message is kept with the software,
 * all derivative works or modified versions.
 */
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

typedef float REAL;

typedef struct complex {
	REAL real;
	REAL imag;
} COMPLEX;

static void fft_doit (COMPLEX data[], int n, int inverse)
{
	int a, b, c, d, e;	/* Counter variables */
	COMPLEX temp;		/* Swap variable */
	COMPLEX u, v;		/* Temporary variables */

	/* Calculate Fourier transform */
	for (c=1; c<n; ) {
		d = c;
		c += c;
		u.real = 1.0;
		u.imag = 0.0;
		v.real = cos (M_PI/d);
		v.imag = sin (-M_PI/d);
		if (inverse)
			v.imag = - v.imag;
		for (b=0; b<d; ++b) {
			for (a=b; a<n; a+=c) {
				e = a + d;
				temp.real = data[e].real * u.real -
					    data[e].imag * u.imag;
				temp.imag = data[e].real * u.imag -
					    data[e].imag * u.real;
				data[e].real = data[a].real - temp.real;
				data[e].imag = data[a].imag - temp.imag;
				data[a].real += temp.real;
				data[a].imag += temp.imag;
			}
			temp.real = u.real;
			u.real = u.real    * v.real - u.imag * v.imag;
			u.imag = temp.real * v.imag + u.imag * v.real;
		}
	}

	/* Normalize if inverse transform */
	if (inverse)
		for (a=0; a<n; ++a) {
			data[a].real /= n;
			data[a].imag /= n;
		}
}

/* FFT - Cooley-Tukey Fast Fourier Transform
 *
 * Purpose: To calculate the discrete Fast Fourier Transform (or its
 *          inverse) of a set of complex data elements.
 *
 * Call:    fft_complex (COMPLEX data[n], int n, int inverse)
 *
 * Where:   data is an array of n data elements.
 *          n is the number of valid data elements in "data",
 *              i.e. data[0..n-1]. It MUST be an integer power of two.
 *          inverse is a Boolean flag which indicates that the inverse
 *              Fourier transform is to be calculated;
 *		otherwise the Fourier transform is calculated.
 *
 * Result:  The Fourier transform (or the inverse Fourier transform) is
 *          calculated in place and returned in "data"; the original data
 *          elements are overwritten.
 */
void fft_complex (COMPLEX data[], int n, int inverse)
{
	int a, b, c;
	COMPLEX temp;

	/* Perform bit reversal */
	b = n/2;
	for (a=1; a<n-1; ++a) {
		if (a < b) {
			temp = data[a];
			data[a] = data[b];
			data[b] = temp;
		}
		c = n/2;
		while (c <= b) {
			b -= c;
			c /= 2;
		}
		b += c;
	}

	fft_doit (data, n, inverse);
}

/* FFT - Cooley-Tukey Fast Fourier Transform for real input
 *
 * Purpose: To calculate the discrete Fast Fourier Transform (or its
 *          inverse) of a set of real data elements.
 *
 * Call:    fft_real (COMPLEX output[n], REAL input[n], int n)
 *
 * Where:   output is an output array of n complex data elements.
 *          input is an input array of n real data elements.
 *          n is the number of valid data elements in "output" and "input",
 *              i.e. output[0..n-1]. It MUST be an integer power of two.
 *
 * Result:  The Fourier transform (or the inverse Fourier transform) is
 *          calculated and returned in "output"; the original input data
 *          elements are not touched.
 */
void fft_real (COMPLEX data[], REAL input[], int n)
{
	int a, b, c;
	COMPLEX *p;
	REAL temp, *q;

	for (p=data, q=input; p<data+n; ++p) {
		p->real = *q++;
		p->imag = 0;
	}

	/* Perform bit reversal */
	b = n/2;
	for (a=1; a<n-1; ++a) {
		if (a < b) {
			temp = data[a].real;
			data[a].real = data[b].real;
			data[b].real = temp;
		}
		c = n/2;
		while (c <= b) {
			b -= c;
			c /= 2;
		}
		b += c;
	}

	fft_doit (data, n, 0);
}

void print_time (char *label, int n)
{
	static struct timeval t0;
	struct timeval t;
	int usec;

	if (label) {
		printf ("%s - repeat %d\n", label, n);
		gettimeofday (&t0, 0);
		return;
	}
	gettimeofday (&t, 0);
	usec = (t.tv_sec - t0.tv_sec) * 1000000 +
		(t.tv_usec - t0.tv_usec);
	printf ("- %.3f seconds, %d usec/transform\n",
		usec/1000000.0, usec/n);
}

int main ()
{
	int n, i, k, skip;
	REAL a[256];
	COMPLEX c[256+1];
	COMPLEX av[256];

	while (scanf ("%d", &n) == 1) {
		skip = (n < 0);
		if (skip)
			n = -n;
		if (n < 4 || n > 256) {
			fprintf (stderr, "Vector size N=%d not in 4..256\n", n);
			exit (-1);
		}
		for (i=0; i<n; ++i)
			scanf ("%f", &a[i]);
		if (skip)
			continue;

#if 0
#define ENOUGH 10000
		print_time ("Real version", ENOUGH);
		for (k=0; k<ENOUGH; ++k)
			fft_real (av, a, n);
		print_time (0, ENOUGH);

		print_time ("Complex version", ENOUGH);
		for (k=0; k<ENOUGH; ++k) {
			for (i=0; i<n; ++i) {
				c[i].real = a[i];
				c[i].imag = 0;
			}
			fft_complex (c, n, 0);
		}
		print_time (0, ENOUGH);
#else
		fft_real (av, a, n);
		for (i=0; i<n; ++i)
			printf ("%15g   -- %15g %15g\n", a[i],
				av[i].real, av[i].imag);
#endif
	}
	return 0;
}
