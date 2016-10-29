#include <stdio.h>
#include <stdlib.h>

int main ()
{
	double s;
	long long i, m;

	s = 0;
	m = 1;
	for (i=1; ; ++i) {
		s += (double) 1.0 / i;
		if (i >= m) {
			printf ("%lld: %.3f\n", i, s);
			if (m >= 100000000000LL)
				exit (0);
			m *= 10;
		}
	}
}
