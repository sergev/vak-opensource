
# ifdef CROSS
#    include "../h/ar.h"
# else
#    include <ar.h>
# endif

# define skip(n) \
	for (c=i=0; i<n; i++) if (write (f, &c, 1) != 1) return (0)

putarhdr (f, h)
register f;
register struct ar_hdr *h;
{
	register i;
	char c;

	for (i=0; i<14; i++)
		if (write (f, (char *) &h->ar_name[i], 1) != 1)
			return (0);
	skip (2);

	for (i=0; i<=24; i+=8) {
		c = (h->ar_date >> i) & 0377;
		if (write (f, &c, 1) != 1) return (0);
	}
	skip (4);

	c = h->ar_uid;
	if (write (f, &c, 1) != 1) return (0);
	skip (7);
	c = h->ar_gid;
	if (write (f, &c, 1) != 1) return (0);
	skip (7);

	c = h->ar_mode & 0377;
	if (write (f, &c, 1) != 1) return (0);
	c = (h->ar_mode >> 8) & 0377;
	if (write (f, &c, 1) != 1) return (0);
	skip (6);

	for (i=0; i<=24; i+=8) {
		c = (h->ar_size >> i) & 0377;
		if (write (f, &c, 1) != 1) return (0);
	}
	skip (4);
	return (1);
}
