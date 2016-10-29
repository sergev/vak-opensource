/*	@(#)execvp.c	1.2	*/
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
/*
 *	execlp(name, arg,...,0)	(like execl, but does path search)
 *	execvp(name, argv)	(like execv, but does path search)
 */
#include <sys/errno.h>
#define	NULL	0

static char *execat(), shell[] = "/bin/sh";
extern char *getenv(), *strchr();
extern unsigned sleep();
extern int errno, execv();

/*VARARGS1*/
int
execlp(name, argv)
char	*name, *argv;
{
	return(execvp(name, &argv));
}

int
execvp(name, argv)
char	*name, **argv;
{
	char	*pathstr;
	char	fname[128];
	char	*newargs[256];
	int	i;
	register char	*cp;
	register unsigned etxtbsy=1;
	register int eacces=0;

	if((pathstr = getenv("PATH")) == NULL)
		pathstr = ":/bin:/usr/bin";
	cp = strchr(name, '/')? "": pathstr;

	do {
		cp = execat(cp, name, fname);
	retry:
		(void) execv(fname, argv);
		switch(errno) {
		case ENOEXEC:
			newargs[0] = "sh";
			newargs[1] = fname;
			for(i=1; newargs[i+1]=argv[i]; ++i) {
				if(i >= 254) {
					errno = E2BIG;
					return(-1);
				}
			}
			(void) execv(shell, newargs);
			return(-1);
		case ETXTBSY:
			if(++etxtbsy > 5)
				return(-1);
			(void) sleep(etxtbsy);
			goto retry;
		case EACCES:
			++eacces;
			break;
		case ENOMEM:
		case E2BIG:
			return(-1);
		}
	} while(cp);
	if(eacces)
		errno = EACCES;
	return(-1);
}

static char *
execat(s1, s2, si)
register char *s1, *s2;
char	*si;
{
	register char	*s;

	s = si;
	while(*s1 && *s1 != ':')
		*s++ = *s1++;
	if(si != s)
		*s++ = '/';
	while(*s2)
		*s++ = *s2++;
	*s = '\0';
	return(*s1? ++s1: 0);
}
