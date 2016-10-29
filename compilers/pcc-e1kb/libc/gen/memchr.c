/*	@(#)memchr.c	1.1	*/
/*LINTLIBRARY*/
/*
 * Return the ptr in sp at which the character c appears;
 *   NULL if not found in n chars; don't stop at \0.
 */
char *
memchr(sp, c, n)
register char *sp, c;
register int n;
{
	while (--n >= 0)
		if (*sp++ == c)
			return (--sp);
	return (0);
}
