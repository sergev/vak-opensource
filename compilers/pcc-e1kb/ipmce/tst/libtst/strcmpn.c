strcmpn(s1, s2, n)
register char *s1, *s2;
register n;
{

	while (--n >= 0 && *s1 == *s2++)
		if (*s1++ == '\0')
			return(0);
	return(n < 0 ? 0 : (unsigned) (*s1) - (unsigned) (*--s2));
}
