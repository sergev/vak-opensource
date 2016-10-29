strcmp(s1, s2)
register char *s1, *s2;
{
	while (*s1 == *s2++)
		if (*s1++=='\0')
			return(0);
	return((unsigned) (*s1) - (unsigned) (*--s2));
}
