/*
 * $Log:	strlen.c,v $
 * Revision 1.1  86/04/21  20:06:54  root
 * Initial revision
 * 
 */

/*
 * Returns the number of
 * non-NULL bytes in string argument.
 */

strlen(s)
register char *s;
{
	register n;

	n = 0;
	while (*s++)
		n++;
	return(n);
}
