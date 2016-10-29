/*	@(#)execl.c	1.1	*/
/*
 *	execl(name, arg0, arg1, ..., argn, 0)
 *	environment automatically passed.
 */

execl(name, args)
char *name, *args;
{
	extern char **environ;

	return (execve(name, &args, environ));
}
