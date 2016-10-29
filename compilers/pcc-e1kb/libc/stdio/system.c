/*	@(#)system.c	1.2	*/
/*	3.0 SID #	1.4	*/
/*LINTLIBRARY*/
#include "signal.h"

extern int fork(), execl(), wait();

int
system(s)
char	*s;
{
	int	status, pid, w;
	register int (*istat)(), (*qstat)();

	if((pid = fork()) == 0) {
		(void) execl("/bin/sh", "sh", "-c", s, 0);
		_exit(127);
	}
	istat = signal(SIGINT, SIG_IGN);
	qstat = signal(SIGQUIT, SIG_IGN);
	while((w = wait(&status)) != pid && w != -1)
		;
	(void) signal(SIGINT, istat);
	(void) signal(SIGQUIT, qstat);
	return((w == -1)? w: status);
}
