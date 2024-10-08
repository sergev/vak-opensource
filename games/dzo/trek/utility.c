/*
**  ASSORTED UTILITY ROUTINES
*/

/*
**  BLOCK MOVE
**
**      Moves a block of storage of length `l' bytes from the data
**      area pointed to by `a' to the area pointed to by `b'.
**      Returns the address of the byte following the `b' field.
**      Overflow of `b' is not tested.
*/

#include "trek.h"
#include <stdlib.h>

char *bmove(a, b, l)
char    *a, *b;
int     l;
{
	register int            n;
	register char           *p, *q;

	p = a;
	q = b;
	n = l;
	while (n--)
		*q++ = *p++;
	return (q);
}


/*
**  STRING EQUALITY TEST
**      null-terminated strings `a' and `b' are tested for
**      absolute equality.
**      returns one if equal, zero otherwise.
*/

sequal(a, b)
char    *a, *b;
{
	register char           *p, *q;

	p = a;
	q = b;
	while (*p || *q)
		if (*p++ != *q++)
			return(0);
	return(1);
}


/*
**  STRING CONCATENATE
**
**      The strings `s1' and `s2' are concatenated and stored into
**      `s3'.  It is ok for `s1' to equal `s3', but terrible things
**      will happen if `s2' equals `s3'.  The return value is is a
**      pointer to the end of `s3' field.
*/

char *concat(s1, s2, s3)
char    *s1, *s2, *s3;
{
	register char           *p;
	register char           *q;

	p = s3;
	q = s1;
	while (*q)
		*p++ = *q++;
	q = s2;
	while (*q)
		*p++ = *q++;
	*p = 0;
	return (p);
}


/*
**  FIND STRING LENGTH
**
**      The length of string `s' (excluding the null byte which
**              terminates the string) is returned.
*/

length(s)
char    *s;
{
	register int    l;
	register char   *p;

	l = 0;
	p = s;
	while (*p++)
		l++;
	return(l);
}


/*
**  SYSTEM ERROR
*/

#include <errno.h>

void tsyserr(const char *s, ...)
{
	va_list p;

	printf("\n\07TREK SYSERR: ");
	va_start(p, s);
	vprintf(s, p);
	va_end(p);
	printf("\n");
	if (errno)
		printf("\tsystem error %d\n", errno);
	exit(-1);
}
