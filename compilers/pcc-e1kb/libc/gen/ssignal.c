/*	@(#)ssignal.c	1.2	*/
/*LINTLIBRARY*/
/*
 *	ssignal, gsignal: software signals
 */
#include <signal.h>

/* Highest allowable user signal number */
#define MAXSIG 16

/* Lowest allowable signal number (lowest user number is always 1) */
#define MINSIG (-4)

/* Table of signal values */
static int (*sigs[MAXSIG-MINSIG+1])();

int
(*ssignal(sig, fn))()
register int sig, (*fn)();
{
	register int (*savefn)();

	if(sig >= MINSIG && sig <= MAXSIG) {
		savefn = sigs[sig-MINSIG];
		sigs[sig-MINSIG] = fn;
	} else
		savefn = SIG_DFL;

	return(savefn);
}

int
gsignal(sig)
register int sig;
{
	register int (*sigfn)();

	if(sig < MINSIG || sig > MAXSIG ||
				(sigfn = sigs[sig-MINSIG]) == SIG_DFL)
		return(0);
	else if(sigfn == SIG_IGN)
		return(1);
	else {
		sigs[sig-MINSIG] = SIG_DFL;
		return((*sigfn)(sig));
	}
}
