/*	@(#)getlogin.c	1.4	*/
/*	@(#)getlogin.c	1.2	*/
/*LINTLIBRARY*/
#include <sys/types.h>
#include "utmp.h"

#define NULL 0

extern long lseek();
extern int open(), read(), close(), ttyslot();

char *
getlogin()
{
	register me, uf;
	struct utmp ubuf ;
	static char answer[sizeof(ubuf.ut_user)+1] ;

	if((me = ttyslot()) < 0)
		return(NULL);
	if((uf = open(UTMP_FILE, 0)) < 0)
		return(NULL);
	(void) lseek(uf, (long)(me * sizeof(ubuf)), 0);
	if(read(uf, (char*)&ubuf, sizeof(ubuf)) != sizeof(ubuf)) {
		(void) close(uf);
		return(NULL);
	}
	(void) close(uf);
	if(ubuf.ut_user[0] == '\0')
		return(NULL);
	strncpy(&answer[0],&ubuf.ut_user[0],sizeof(ubuf.ut_user)) ;
	answer[sizeof(ubuf.ut_user)] = '\0' ;
	return(&answer[0]);
}
