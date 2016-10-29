
# include <stdio.h>

# ifdef CROSS
#    include "../h/a.out.h"
# else
#    include <a.out.h>
# endif

extern long fgeth ();
extern char *malloc ();

fgetsym (text, sym)
register FILE *text;
register struct nlist *sym;
{
	register c;

	if ((sym->n_len = getc (text)) <= 0)
		return (1);
	if (! (sym->n_name = malloc (sym->n_len+1)))
		return (0);
	sym->n_type = getc (text);
	sym->n_value = fgeth (text);
	for (c=0; c<sym->n_len; c++)
		sym->n_name [c] = getc (text);
	sym->n_name [sym->n_len] = '\0';
	return (sym->n_len + 6);
}
