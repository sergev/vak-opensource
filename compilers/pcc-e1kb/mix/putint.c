
putint (f, i)
register f;
{
	char c;
	static char buf[6];

	c = i & 0377;
	if (write (f, &c, 1) != 1) return (0);
	c = (i >> 8) & 0377;
	if (write (f, &c, 1) != 1) return (0);
	if (write (f, buf, 6) != 6) return (0);
	return (1);
}
