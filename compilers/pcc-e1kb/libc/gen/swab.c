/*	@(#)swab.c	1.2	*/
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
/*
 * Swap bytes in 16-bit [half-]words
 * for going between the 11 and the interdata
 */

void
swab(pf, pt, n)
register short *pf, *pt;
register int n;
{
	n /= 2;
	while(--n >= 0) {
		*pt++ = (*pf << 8) + ((*pf >> 8) & 0377);
		pf++;
	}
}
