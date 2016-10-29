
# include <stdio.h>

# ifdef CROSS
#    include "../h/ar.h"
# else
#    include <ar.h>
# endif

extern long fgeth ();

fgetarhdr (f, h)
register FILE *f;
register struct ar_hdr *h;
{
	register int i, c;

	for (i=0; i<14; i++) {
		if ((c = getc (f)) == EOF) return (0);
		h->ar_name[i] = c;
	}
	if (getc (f)) return (0);
	if (getc (f)) return (0);

	h->ar_date = fgeth (f);
	h->ar_date |= fgeth (f) << 32;

	h->ar_uid = fgeth (f);
	fgeth (f);

	h->ar_gid = fgeth (f);
	fgeth (f);

	h->ar_mode = fgeth (f);
	fgeth (f);

	h->ar_size = fgeth (f);
	h->ar_size |= fgeth (f) << 32;
	return (1);
}
