#include <sys/types.h>
#include <string.h>

#define KEYSIZE(tab) (sizeof (tab) / sizeof (struct keytab))

static struct keytab {          /* table of keywords */
	char *name;
	int value;
} monthtab [] = {               /* month names */
	{ "apr",        4  },   { "april",      4  },
	{ "aug",        8  },   { "august",     8  },
	{ "dec",        12 },   { "december",   12 },
	{ "feb",        2  },   { "february",   2  },
	{ "jan",        1  },   { "january",    1  },
	{ "jul",        7  },   { "july",       7  },
	{ "jun",        6  },   { "june",       6  },
	{ "mar",        3  },   { "march",      3  },
	{ "may",        5  },
	{ "nov",        11 },   { "november",   11 },
	{ "oct",        10 },   { "october",    10 },
	{ "sep",        9  },   { "sept",       9  },   { "september",  9  },
};

/*
 * Binary search of the keyword in the table.
 */
static struct keytab *binsearch (char *name, struct keytab *tab, int tabsz)
{
	int min, max, middle, cmp;

	min = 0;
	max = tabsz - 1;
	while (max >= min) {
		middle = (min + max) / 2;
		cmp = strcmp (name, tab[middle].name);
		if (! cmp)
			return (tab + middle);
		if (cmp < 0)
			max = middle - 1;
		else
			min = middle + 1;
	}
	return (0);
}

/*
 * Read the integer number.
 * Return -1 on error.
 */
static int getint (char **p)
{
	register n;

	if (**p<'0' || **p>'9')
		return (-1);
	n = 0;
	while (**p>='0' && **p<='9')
		n = n*10 + *(*p)++ - '0';
	return (n);
}

/*
 * Read the keyword from the table.
 * Return the keyword value, or -1 if not found.
 */
static int getkeyword (char **p, struct keytab *tab, int tabsz)
{
	register n;
	char key [20];

	if (! ((**p>='A' && **p<='Z') || (**p>='a' && **p<='z')))
		return (-1);
	n = 0;
	while (n<sizeof(key) && ((**p>='A' && **p<='Z') ||
	    (**p>='a' && **p<='z') || **p=='.'))
		key [n++] = *(*p)++;
	if (n >= sizeof (key))
		return (-1);
	key [n] = 0;
	for (n=0; key[n]; ++n)
		if (key[n]>='A' && key[n]<='Z')
			key [n] += 'a' - 'A';
	tab = binsearch (key, tab, tabsz);
	if (tab)
		return (tab->value);
	return (-1);
}

/*
 * Read the date in RFC1036/RFC822/Unix format.
 */
time_t getdate (char *ctim)
{
#define skip(p) while (*(p)==' ' || *(p)=='\t') ++(p)
	static dmsize [12] = {0,31,59,90,120,151,181,212,243,273,304,334};
	int day, mon, year, h, m, s;
	long nday;

	skip (ctim);
	if (ctim[3]==' ' && ctim[7]==' ' && ctim[10]==' ' &&
	    ctim[13]==':' && ctim[16]==':' && ctim[19]==' ' &&
	    (ctim[20]=='1' || ctim[20]=='2' || ctim[24]=='1' || ctim[24]=='2')) {
		/*
		 * ANSI C's asctime() format:
		 * Wdy Mon DD HH:MM:SS YYYY
		 */
		char *p;

		p = ctim + 8;
		skip (p);
		day = getint (&p);

		p = ctim + 4;
		mon = getkeyword (&p, monthtab, KEYSIZE (monthtab));

		p = ctim + 20;
		if (*p != '1' && *p != '2')
			p += 4;
		year = getint (&p);

		ctim += 11;
	} else {
		/*
		 * RFC 1036 format
		 * [Weekday,] DD-Mon-YY HH:MM:SS
		 * RFC 822 format
		 * [Wdy,] DD Mon YYYY HH:MM:SS
		 */
		if ((*ctim>='A' && *ctim<='Z') || (*ctim>='a' && *ctim<='z')) {
			ctim = strchr (ctim, ' ');
			if (! ctim)
				return (0);
		}
		skip (ctim);
		day = getint (&ctim);
		if (*ctim == '-')
			++ctim;
		skip (ctim);
		mon = getkeyword (&ctim, monthtab, KEYSIZE (monthtab));
		if (*ctim == '-')
			++ctim;
		skip (ctim);
		year = getint (&ctim);
	}

	if (day < 0 || mon < 0 || year < 0)
		return (0);

	if (year < 70)
		year += 2000;
	else if (year < 100)
		year += 1900;
	if (year < 1970)                /* ancient date */
		return (0);

	skip (ctim);
	if (*ctim>='0' && *ctim<='9') {
		h = getint (&ctim);
		if (h<0 || *ctim!=':')
			return (0);
		++ctim;

		skip (ctim);
		m = getint (&ctim);
		if (m < 0)
			return (0);

		if (*ctim == ':') {
			++ctim;
			skip (ctim);
			s = getint (&ctim);
			if (s < 0)
				return (0);
		} else
			s = 0;
	} else {
		h = 12;
		m = s = 0;
	}

	nday = day-1 + dmsize[mon-1] + (year-1970) * 365L + (year-1969) / 4;
	if (year%4 == 0 && mon > 2)
		nday += 1;
	return (nday * 60 * 60 * 24 + (h * 60L + m) * 60 + s);
}

#ifdef DEBUG_DATE
#include <stdio.h>
#include <time.h>

int main ()
{
	char *tab[] = {
		"Thursday, 11-Jan-96 21:24:51 GMT; length=660",
		"Sun, 06 Nov 1994 08:49:37 GMT",
		"Sunday, 06-Nov-94 08:49:37 GMT",
		"Sun Nov  6 08:49:37 1994",
		0,
	};
	char **p;
	long t;

	for (p=tab; *p; ++p) {
		t = getdate (*p);
		printf ("%08ld - %s", t, ctime (&t));
	}
	return (0);
}
#endif
