
# include <stdio.h>

# ifdef CROSS
#    include "../h/ranlib.h"
# else
#    include <ranlib.h>
# endif

extern fputh ();

fputran (s, file)
register struct ranlib *s;
register FILE *file;
{
	register i;

	putc (s->ran_len, file);
	fputh (s->ran_off, file);
	for (i=0; i<s->ran_len; i++)
		putc (s->ran_name[i], file);
}
