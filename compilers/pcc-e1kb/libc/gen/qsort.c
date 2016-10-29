/*	@(#)qsort.c	1.4	*/
/*	3.0 SID #	1.2	*/
/*LINTLIBRARY*/

#ifndef pdp11
#ifdef vax         /* number is determined experimentally on vax-11/780 */
#define MINCPY	24 /* minimum number of characters worth using memcpy for */
#else              /* number is determined experimentally on 3b20s */
#define MINCPY	8  /* minimum number of characters worth using memcpy for */
#endif
#define NULL	0
#define CPY(i, j) ((void) memcpy(i, j, n))
extern char *malloc(), *realloc(), *memcpy();
static char *qsbuf = NULL;
#endif

static 	qses, (*qscmp)();

void
qsort(a, n, es, fc)
char	*a;
unsigned n, es;
int	(*fc)();
{
	void qs1();

#ifndef pdp11
	{
		static unsigned qsbufsize;

		if (es >= MINCPY)
			if (qsbuf == NULL)
				qsbuf = malloc(qsbufsize = es);
			else if (qsbufsize < es)
				qsbuf = realloc(qsbuf, qsbufsize = es);
	}
#endif
	qscmp = fc;
	qses = es;
	qs1(a, a+n*es);
}

static void
qs1(a, l)
char	*a, *l;
{
	register char *i, *j;
	register int es;
	char	*lp, *hp;
	int	c;
	void	qsexc(), qstexc();
	unsigned n;

	es = qses;
start:
	if((n=l-a) <= es)
		return;
	n = es * (n / (2*es));
	hp = lp = a+n;
	i = a;
	j = l-es;
	while(1) {
		if(i < lp) {
			if((c = (*qscmp)(i, lp)) == 0) {
				qsexc(i, lp -= es);
				continue;
			}
			if(c < 0) {
				i += es;
				continue;
			}
		}

loop:
		if(j > hp) {
			if((c = (*qscmp)(hp, j)) == 0) {
				qsexc(hp += es, j);
				goto loop;
			}
			if(c > 0) {
				if(i == lp) {
					qstexc(i, hp += es, j);
					i = lp += es;
					goto loop;
				}
				qsexc(i, j);
				j -= es;
				i += es;
				continue;
			}
			j -= es;
			goto loop;
		}

		if(i == lp) {
			if(lp-a >= l-hp) {
				qs1(hp+es, l);
				l = lp;
			} else {
				qs1(a, lp);
				a = hp+es;
			}
			goto start;
		}

		qstexc(j, lp -= es, i);
		j = hp -= es;
	}
}

static void
qsexc(ri, rj)
register char *ri, *rj;
{
	register int n = qses;

#ifndef pdp11
	if (n >= MINCPY && qsbuf != NULL) {
		CPY(qsbuf, ri);
		CPY(ri, rj);
		CPY(rj, qsbuf);
		return;
	}
#endif
	do {
		register char c = *ri;
		*ri++ = *rj;
		*rj++ = c;
	} while(--n);
}

static void
qstexc(ri, rj, rk)
register char *ri, *rj, *rk;
{
	register int n = qses;

#ifndef pdp11
	if (n >= MINCPY && qsbuf != NULL) {
		CPY(qsbuf, ri);
		CPY(ri, rk);
		CPY(rk, rj);
		CPY(rj, qsbuf);
		return;
	}
#endif
	do {
		register char c = *ri;
		*ri++ = *rk;
		*rk++ = *rj;
		*rj++ = c;
	} while(--n);
}
