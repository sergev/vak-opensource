/*	@(#)putchar.c	1.2	*/
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
/*
 * A subroutine version of the macro putchar
 */
#include "stdio.h"
#undef putchar

int
putchar(c)
register char c;
{
	return(putc(c, stdout));
}
