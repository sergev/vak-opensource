
# include <stdio.h>

# ifdef CROSS
#    include "../h/a.out.h"
# else
#    include <a.out.h>
# endif

extern fputh ();

fputsym (s, file)
register struct nlist *s;
register FILE *file;
{
	register i;

	putc (s->n_len, file);
	putc (s->n_type, file);
	fputh (s->n_value, file);
	for (i=0; i<s->n_len; i++)
		putc (s->n_name[i], file);
}
