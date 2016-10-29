/*	@(#)calloc.c	1.3	*/
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/
/*	calloc - allocate and clear memory block
*/
#define NULL 0

extern char *malloc(), *memset();
extern void free();

char *
calloc(num, size)
unsigned num, size;
{
	register char *mp;

	if((mp = malloc(num *= size)) != NULL)
		(void)memset(mp, 0, num);
	return(mp);
}

/*ARGSUSED*/
void
cfree(p, num, size)
char *p;
unsigned num, size;
{
	free(p);
}
