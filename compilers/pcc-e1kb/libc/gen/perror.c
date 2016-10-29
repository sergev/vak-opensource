/*	@(#)perror.c	1.2	*/
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
/*
 * Print the error indicated
 * in the cerror cell.
 */

extern int errno, sys_nerr, strlen(), write();
extern char *sys_errlist[];

void
perror(s)
char	*s;
{
	register char *c;
	register int n;

	c = "Unknown error";
	if(errno < sys_nerr)
		c = sys_errlist[errno];
	n = strlen(s);
	if(n) {
		(void) write(2, s, (unsigned)n);
		(void) write(2, ": ", 2);
	}
	(void) write(2, c, (unsigned)strlen(c));
	(void) write(2, "\n", 1);
}
