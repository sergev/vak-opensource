/*	@(#)getgrnam.c	1.3	*/
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
#include <grp.h>

extern struct group *getgrent();
extern void setgrent(), endgrent();
extern int strcmp();

struct group *
getgrnam(name)
register char *name;
{
	register struct group *p;

	setgrent();
	while((p = getgrent()) && strcmp(p->gr_name, name))
		;
	endgrent();
	return(p);
}
