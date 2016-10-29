/*	@(#)ctermid.c	1.3	*/
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
#include "stdio.h"

extern char *strcpy();
static char res[L_ctermid];

char *
ctermid(s)
register char *s;
{
	return (strcpy(s != NULL ? s : res, "/dev/tty"));
}
