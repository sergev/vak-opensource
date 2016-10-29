
double atof (p)
register char *p;
{
	static double twoe37 = 137438953472.; /*2^37*/
	static double exp10[] = {
		10.,
		100.,
		10000.,
		100000000.,
		10000000000000000.,
	};
	register c, exp = 0, eexp = 0;
	double fl = 0, flexp = 1.0;
	int bexp, neg = 1, negexp = 1;

	while ((c = *p++) == ' ');
	if (c == '-') neg = -1;
	else if (c == '+');
	else --p;
	while ((c = *p++), c>='0' && c<='9')
		if (fl < twoe37) fl = 10*fl + (c-'0');
		else exp++;
	if (c == '.') while ((c = *p++), c>='0' && c<='9')
		if (fl < twoe37) {
			fl = 10*fl + (c-'0');
			exp--;
		}
	if ((c == 'E') || (c == 'e')) {
		if ((c = *p++) == '+');
		else if (c=='-') negexp = -1;
		else --p;
		while ((c = *p++), c>='0' && c<='9')
			eexp = 10*eexp + (c-'0');
		if (negexp < 0) eexp = -eexp;
		exp += eexp;
	}
	bexp = exp;
	if (exp < 0) exp = -exp;
	for (c = 0; c < 5; c++) {
		if (exp & 01) flexp *= exp10[c];
		exp >>= 1;
		if (exp == 0) break;
	}
	if (bexp < 0) fl /= flexp;
	else fl *= flexp;
	if (neg < 0) return(-fl);
	else return(fl);
}
