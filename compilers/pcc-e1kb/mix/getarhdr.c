
# ifdef CROSS
#    include "../h/ar.h"
# else
#    include <ar.h>
# endif

# define skip(n) \
	for (i=0; i<n; i++) if (read (f, &c, 1) != 1) return (0)

getarhdr (f, h)
register f;
register struct ar_hdr * h;
{
	register i;
	char c;

	for (i=0; i<14; i++)
		if (read (f, (char *) &h->ar_name[i], 1) != 1)
			return (0);
	skip (2);

	h->ar_date = 0;
	for (i=0; i<=24; i+=8) {
		if (read (f, &c, 1) != 1) return (0);
		h->ar_date |= ((long) c & 0377) << i;
	}
	skip (4);

	if (read (f, &c, 1) != 1) return (0);
	h->ar_uid = c & 0377;
	skip (7);
	if (read (f, &c, 1) != 1) return (0);
	h->ar_gid = c & 0377;
	skip (7);

	if (read (f, &c, 1) != 1) return (0);
	h->ar_mode = c & 0377;
	if (read (f, &c, 1) != 1) return (0);
	h->ar_mode |= (c&0377) << 8;
	skip (6);

	h->ar_size = 0;
	for (i=0; i<=24; i+=8) {
		if (read (f, &c, 1) != 1) return (0);
		h->ar_size |= ((long) c & 0377) << i;
	}
	skip (4);
	return (1);
}
