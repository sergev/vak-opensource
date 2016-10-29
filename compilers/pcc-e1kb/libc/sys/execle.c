/*	@(#)execle.c	1.1	*/
/*
 *	execle(file, arg1, arg2, ..., 0, envp)
 */

execle(file, args)
	char	*file;
	char	*args;			/* first arg */
{
	register  char  **p;

	p = &args;
	while(*p++);
	return(execve(file, &args, *p));
}
