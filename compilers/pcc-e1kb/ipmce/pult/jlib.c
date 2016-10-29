Jscrool (r1, r2, n)
{
	register r;

	if (n > 0) {
		r2 -= n-1;
		for (r=n; --r>=0; --r)
			VDelLine (r2);
		for (r=n; --r>=0; --r)
			VInsLine (r1);
	} else {
		n = -n;
		r2 -= n-1;
		for (r=n; --r>=0; --r)
			VDelLine (r1);
		for (r=n; --r>=0; --r)
			VInsLine (r2);
	}
}

Jsetcursor (c)
register c;
{
	register r;

	r = c/80;
	VMove (r, c - r*80);
}
