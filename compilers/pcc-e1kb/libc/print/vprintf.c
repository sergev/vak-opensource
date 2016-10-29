/*	@(#)vprintf.c	1.1	*/
/*LINTLIBRARY*/
#include "stdio.h"
#include "varargs.h"

extern int _doprnt();

/*VARARGS1*/
int
vprintf(format, ap)
char *format;
va_list ap;
{
	register int count;

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
	return(ferror(stdout)? EOF: count);
}
