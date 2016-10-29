/*	@(#)bsearch.c	1.5	*/
/*LINTLIBRARY*/
/*
 * Binary search algorithm, generalized from Knuth (6.2.1) Algorithm B.
 *
 */

typedef char *POINTER;

POINTER
bsearch(key, base, nel, width, compar)
POINTER	key;			/* Key to be located */
POINTER	base;			/* Beginning of table */
unsigned nel;			/* Number of elements in the table */
unsigned width;			/* Width of an element (bytes) */
int	(*compar)();		/* Comparison function */
{
	int two_width = width + width;
	POINTER last = base + width * (nel - 1); /* Last element in table */

	while (last >= base) {

		register POINTER p = base + width * ((last - base)/two_width);
		register int res = (*compar)(key, p);

		if (res == 0)
			return (p);	/* Key found */
		if (res < 0)
			last = p - width;
		else
			base = p + width;
	}
	return ((POINTER) 0);		/* Key not found */
}
