/*	@(#)putpwent.c	1.2	*/
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
/*
 * format a password file entry
 */
#include <stdio.h>
#include <pwd.h>

extern int fprintf();

int
putpwent(p, f)
register struct passwd *p;
register FILE *f;
{
	(void) fprintf(f, "%s:%s", p->pw_name, p->pw_passwd);
	if((*p->pw_age) != '\0')
		(void) fprintf(f, ",%s", p->pw_age);
	(void) fprintf(f, ":%u:%u:%s:%s:%s",
		p->pw_uid,
		p->pw_gid,
		p->pw_gecos,
		p->pw_dir,
		p->pw_shell);
	(void) putc('\n', f);
	return(ferror(f));
}
