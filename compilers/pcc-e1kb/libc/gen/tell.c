/*	@(#)tell.c	1.2	*/
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
/*
 * return offset in file.
 */

extern long lseek();

long
tell(f)
int	f;
{
	return(lseek(f, 0L, 1));
}
