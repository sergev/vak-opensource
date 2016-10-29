/*	@(#)clrerr.c	1.3	*/
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
#include "stdio.h"
#undef clearerr

void
clearerr(iop)
register FILE *iop;
{
	iop->_flag &= ~(_IOERR | _IOEOF);
}
