
# include <stdio.h>

# ifdef CROSS
#    include "../h/ranlib.h"
# else
#    include <ranlib.h>
# endif

extern long fgeth ();
extern char *malloc ();

fgetran (text, sym)
register FILE *text;
register struct ranlib *sym;
{
	register c;

	/* read struct ranlib from file */
	/* 1 byte - length of name */
	/* 4 bytes - seek in archive */
	/* 'len' bytes - symbol name */
	/* if len == 0 then eof */
	/* return 1 if ok, 0 if eof, -1 if out of memory */

	if ((sym->ran_len = getc (text)) <= 0)
		return (0);
	if (! (sym->ran_name = malloc (sym->ran_len+1)))
		return (-1);
	sym->ran_off = fgeth (text);
	for (c=0; c<sym->ran_len; c++)
		sym->ran_name [c] = getc (text);
	sym->ran_name [sym->ran_len] = '\0';
	return (1);
}
