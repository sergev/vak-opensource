/*	@(#)vfprintf.c	1.1	*/
/*LINTLIBRARY*/
#include "stdio.h"
#include "varargs.h"

extern int _doprnt();

/*VARARGS2*/
int
vfprintf(iop, format, ap)
FILE *iop;
char *format;
va_list ap;
{
	register int count;

	if (!(iop->_flag | _IOWRT)) {
		/* if no write flag */
		if (iop->_flag | _IORW) {
			/* if ok, cause read-write */
			iop->_flag |= _IOWRT;
		} else {
			/* else error */
			return EOF;
		}
	}
	count = _doprnt(format, ap, iop);
	return(ferror(iop)? EOF: count);
}
