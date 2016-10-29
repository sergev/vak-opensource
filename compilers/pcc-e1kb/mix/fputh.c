
# include <stdio.h>

fputh (h, f)
register long h;
register FILE *f;
{
	putc ((int) h, f);
	putc ((int) (h >> 8), f);
	putc ((int) (h >> 16), f);
	putc ((int) (h >> 24), f);
}
