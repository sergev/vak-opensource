/*	@(#)setvbuf.c	1.2			*/
/*LINTLIBRARY*/
#include "stdio.h"

extern void free();
extern int isatty();

int
setvbuf(iop, type, buf, size)
register FILE *iop;
register int type;
register char	*buf;
register int size;
{
	char *malloc();

	if(iop->_base != NULL && iop->_flag & _IOMYBUF)
		free((char*)iop->_base);
	iop->_flag &= ~(_IOMYBUF | _IONBF | _IOLBF);
	switch (type)  {
	    /*note that the flags are the same as the possible values for type*/
	    case _IONBF:
		/* file is unbuffered */
		iop->_flag |= _IONBF;
		_bufend(iop) = iop->_base = NULL;
		break;
	    case _IOLBF:
	    case _IOFBF:
		iop->_flag |= type;
		size = (size == 0) ? BUFSIZ : size;
		if ((size > BUFSIZ) || (iop->_base == NULL))
			iop->_base = (unsigned char *)
				     malloc((unsigned)(size + 8));
		_bufend(iop) = iop->_base + size;
		break;
	    default:
		return -1;
	}
	iop->_ptr = iop->_base;
	iop->_cnt = 0;
	return 0;
}
