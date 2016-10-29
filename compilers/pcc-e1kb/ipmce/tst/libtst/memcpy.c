memcpy (to, from, count)
char *from, *to;
{
	register *f, *t;

	f = (int *) from;
	t = (int *) to;
	if ((char *) f == from && (char *) t == to) {
		while (count >= 8)
			*t++ = *f++, count -= 8;
		if (! count)
			return;
	}
	while (--count >= 0)
		*to++ = *from++;
}
