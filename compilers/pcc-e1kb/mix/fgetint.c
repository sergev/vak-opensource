
# include <stdio.h>

extern long fgeth ();

fgetint (f, i)
register FILE * f;
register *i;
{
	*i = fgeth (f);
	fgeth (f);
	return (1);
}

