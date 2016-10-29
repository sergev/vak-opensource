/*	@(#)fgets.c	3.3	*/
/*LINTLIBRARY*/
/*
 * This version reads directly from the buffer rather than looping on getc.
 * Ptr args aren't checked for NULL because the program would be a
 * catastrophic mess anyway.  Better to abort than just to return NULL.
 */
#include "stdio.h"
#include "stdiom.h"

#define MIN(x, y)	(x < y ? x : y)

extern int _filbuf();
extern char *memccpy();

char *
fgets(ptr, size, iop)
char *ptr;
register int size;
register FILE *iop;
{
	char *p, *ptr0 = ptr;
	register int n;

	for (size--; size > 0; size -= n) {
		if (iop->_cnt <= 0) { /* empty buffer */
			if (_filbuf(iop) == EOF) {
				if (ptr0 == ptr)
					return (NULL);
				break; /* no more data */
			}
			iop->_ptr--;
			iop->_cnt++;
		}
		n = MIN(size, iop->_cnt);
		if ((p = memccpy(ptr, (char *) iop->_ptr, '\n', n)) != NULL)
			n = p - ptr;
		ptr += n;
		iop->_cnt -= n;
		iop->_ptr += n;
		_BUFSYNC(iop);
		if (p != NULL)
			break; /* found '\n' in buffer */
	}
	*ptr = '\0';
	return (ptr0);
}
