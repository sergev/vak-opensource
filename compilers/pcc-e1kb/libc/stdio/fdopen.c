/*	@(#)fdopen.c	1.4	*/
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
/*
 * Unix routine to do an "fopen" on file descriptor
 * The mode has to be repeated because you can't query its
 * status
 */

#include "stdio.h"
#include "sys/errno.h"

extern long lseek();
extern FILE *_findiop();

FILE *
fdopen(fd, mode)
int	fd;
register char *mode;
{
	register FILE *iop;

	if((iop = _findiop()) == NULL)
		return(NULL);

	iop->_cnt = 0;
	iop->_flag = 0;
	iop->_file = fd;
	_bufend(iop) = iop->_base = iop->_ptr = NULL;
	switch(*mode) {

		case 'r':
			iop->_flag |= _IOREAD;
			break;
		case 'a':
			(void) lseek(fd, 0L, 2);
			/* No break */
		case 'w':
			iop->_flag |= _IOWRT;
			break;
		default:
			return(NULL);
	}

	if(mode[1] == '+') {
		iop->_flag &= ~(_IOREAD | _IOWRT);
		iop->_flag |= _IORW;
	}

	return(iop);
}
