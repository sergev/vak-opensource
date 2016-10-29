/*	@(#)strncmp.c	1.2	*/
/*LINTLIBRARY*/
/*
 * Compare strings (at most n bytes)
 *	returns: s1>s2; >0  s1==s2; 0  s1<s2; <0
 */

int
strncmp(s1, s2, n)
register char *s1, *s2;
register n;
{
	if(s1 == s2)
		return(0);
	while(--n >= 0 && *s1 == *s2++)
		if(*s1++ == '\0')
			return(0);
	return((n < 0)? 0: (*s1 - *--s2));
}
