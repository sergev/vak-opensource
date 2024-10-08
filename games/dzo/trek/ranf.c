# include       <stdio.h>
# define FLOAT  float

ranf(max)
int     max;
{
	register int    t;

	if (max <= 0)
		return (0);
	t = rand() >> 5;
	return (t % max);
}


FLOAT  franf()
{
	FLOAT           t;
	t = rand() & 077777;
	return (t / 32767.0);
}
