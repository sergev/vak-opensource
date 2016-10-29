/*	@(#)findiop.c	1.2	*/
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
#include "stdio.h"

extern FILE *_lastbuf;

FILE *
_findiop()
{
	register FILE *iop;

	for(iop = _iob; iop->_flag & (_IOREAD | _IOWRT | _IORW); iop++)
		if(iop >= _lastbuf)
			return(NULL);
	return(iop);
}
