/*	<@(#)abort.c	1.4>	*/
/*	3.0 SID #	1.4	*/
/*LINTLIBRARY*/
/*
 *	abort() - terminate current process with dump via SIGIOT
 */

#include <signal.h>

extern int kill(), getpid();
static pass = 0;		/* counts how many times abort has been called*/

int
abort()
{
	/* increment first to avoid any hassle with interupts */
	if (++pass == 1)  {
		_cleanup();
	}
	return(kill(getpid(), SIGIOT));
}
