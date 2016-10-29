/*	@(#)printf.c	1.5	*/
/*LINTLIBRARY*/
#include "stdio.h"
#include "varargs.h"

extern int _doprnt();

/*VARARGS1*/
int
printf(format, va_alist)
char *format;
va_dcl
{
	register int count;
	va_list ap;

	va_start(ap);
	if (!(stdout->_flag | _IOWRT)) {
		/* if no write flag */
		if (stdout->_flag | _IORW) {
			/* if ok, cause read-write */
			stdout->_flag |= _IOWRT;
		} else {
			/* else error */
			return EOF;
		}
	}
	count = _doprnt(format, ap, stdout);
	va_end(ap);
	return(ferror(stdout)? EOF: count);
}
