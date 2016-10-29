/*	<@(#)tmpfile.c	1.3>	*/
/*LINTLIBRARY*/
/*
 *	tmpfile - return a pointer to an update file that can be
 *		used for scratch. The file will automatically
 *		go away if the program using it terminates.
 */
#include "stdio.h"

extern FILE *fopen();
extern int unlink();
extern char *tmpnam();
extern void perror();

FILE *
tmpfile()
{
	char	tfname[L_tmpnam];
	register FILE	*p;

	(void) tmpnam(tfname);
	if((p = fopen(tfname, "w+")) == NULL)
		return NULL;
	else
		(void) unlink(tfname);
	return(p);
}
