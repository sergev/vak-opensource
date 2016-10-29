
getint (f, i)
register f;
register *i;
{
	char c, buf[6];

	if (read (f, &c, 1) != 1) return (0);
	*i = c&0377;
	if (read (f, &c, 1) != 1) return (0);
	*i |= (c&0377) << 8;
	if (read (f, buf, 6) != 6) return (0);
	if (buf[0] || buf[1] || buf[2] || buf[3] || buf[4] || buf[5])
		return (0);
	return (1);
}
