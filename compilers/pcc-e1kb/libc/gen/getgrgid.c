/*	@(#)getgrgid.c	1.3	*/
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
#include <grp.h>

extern struct group *getgrent();
extern void setgrent(), endgrent();

struct group *
getgrgid(gid)
register int gid;
{
	register struct group *p;

	setgrent();
	while((p = getgrent()) && p->gr_gid != gid)
		;
	endgrent();
	return(p);
}
