/*	@(#)getpwuid.c	1.3	*/
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
#include <pwd.h>

extern struct passwd *getpwent();
extern void setpwent(), endpwent();

struct passwd *
getpwuid(uid)
register int uid;
{
	register struct passwd *p;

	setpwent();
	while((p = getpwent()) && p->pw_uid != uid)
		;
	endpwent();
	return(p);
}
