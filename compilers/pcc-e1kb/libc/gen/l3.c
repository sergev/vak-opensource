/*	@(#)l3.c	1.3	*/
/*LINTLIBRARY*/
/*
 * Convert longs to and from 3-byte disk addresses
 */
void
ltol3(cp, lp, n)
char	*cp;
long	*lp;
int	n;
{
	register i;
	register char *a, *b;

	a = cp;
	b = (char *)lp;
	for(i=0; i < n; ++i) {
#if besm
		*a++ = *b++;
		*a++ = *b++;
		*a++ = *b++;
		b += 3;
#endif
#if interdata || u370 || u3b || u3b5
		b++;
		*a++ = *b++;
		*a++ = *b++;
		*a++ = *b++;
#endif
#if vax
		*a++ = *b++;
		*a++ = *b++;
		*a++ = *b++;
		b++;
#endif
#if pdp11
		*a++ = *b++;
		b++;
		*a++ = *b++;
		*a++ = *b++;
#endif
	}
}

void
l3tol(lp, cp, n)
long	*lp;
char	*cp;
int	n;
{
	register i;
	register char *a, *b;

	a = (char *)lp;
	b = cp;
	for(i=0; i < n; ++i) {
#if besm
		*a++ = *b++;
		*a++ = *b++;
		*a++ = *b++;
		*a++ = 0;
		*a++ = 0;
		*a++ = 0;
#endif
#if interdata || u370 || u3b || u3b5
		*a++ = 0;
		*a++ = *b++;
		*a++ = *b++;
		*a++ = *b++;
#endif
#if vax
		*a++ = *b++;
		*a++ = *b++;
		*a++ = *b++;
		*a++ = 0;
#endif
#if pdp11
		*a++ = *b++;
		*a++ = 0;
		*a++ = *b++;
		*a++ = *b++;
#endif
	}
}
