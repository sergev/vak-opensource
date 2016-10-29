bcopy (from, to, count)
char *from, *to;
{
	register *f, *t;

	f = (int *) from;
	t = (int *) to;
	if ((char *) f == from && (char *) t == to && count >= 8) {
		wcopy (f, t, count/8);
		if (count & 7) {
			from = (char *) (f + count/8);
			to = (char *) (t + count/8);
			count &= 7;
		} else
			return;
	}
	while (--count >= 0)
		*to++ = *from++;
}

bzero (from, count)
char *from;
{
	register *f;

	f = (int *) from;
	if ((char *) f == from && count >= 8) {
		wzero (f, count/8);
		if (count & 7) {
			from = (char *) (f + count/8);
			count &= 7;
		} else
			return;
	}
	while (--count >= 0)
		*from++ = 0;
}

bcmp (from, to, count)
char *from, *to;
{
	register *f, *t;

	f = (int *) from;
	t = (int *) to;
	if ((char *) f == from && (char *) t == to && count >= 8) {
		if (wcmp (f, t, count/8))
			return (1);
		if (count & 7) {
			from = (char *) (f + count/8);
			to = (char *) (t + count/8);
			count &= 7;
		} else
			return (0);
	}
	while (--count >= 0)
		if (*to++ != *from++)
			return (1);
	return (0);
}

inverbuf (p, n)
register *p;
{
	for (; --n >= 0; ++p)
		*p = inverword (*p);
}

inverhbuf (p, n)
register *p;
{
	for (; --n >= 0; ++p)
		*p = inverhword (*p);
}

meminit ()
{
	/* set core page mapping for entire memory
	 * page size = 256 Kwords
	 */
	int i, pagsiz, zero;
	struct {
		int math:32;
		int phys:32;
	} p;

	p.phys = 0xff;
	for (i=0; i<32; ++i) {
		_out_ (0x40+i, p);
		p.phys += 0x100;
		p.math += 0x100;
	}
	zero = 0;
	pagsiz = 0xf;                   /* 256 Kw */
	_out_ (0x22, pagsiz<<32);       /* page size */
	_out_ (0x29, ~zero);            /* all pages accessible */
	_out_ (0x2a, zero);             /* read/write */
	_out_ (0x2b, zero);             /* clear RFO */
}

long microtime ()
{
	return (_in_ (0x1d));
}

long time ()
{
	return ((_in_ (0x1d) << 16 >> 16) / 0xf22a1);
}
