/*	@(#)strrchr.c	1.2	*/

/*
 * Return the ptr in sp at which the character c last
 * appears; NULL if not found
 */
#define EOF     (-1)
#define NULL (char *)0

char *
strrchr(sp, c)
register char *sp, c;
{
	register char *r;

	r = NULL;
	do {
		if(*sp == c)
			r = sp;
	} while(*sp++);
	return(r);
}


