#include <stdio.h>

#define LINESZ          512             /* maximum input line length */
#define ISSPACE(c)	((c) == ' ' || (c) == '\t')
#define ISDIGIT(c)	((c) >= '0' && (c) <= '9')

unsigned char tab[256];

unsigned long getval (char *nptr, char **endptr)
{
	char *s = nptr;
	int base, any, c;
	unsigned long acc = 0;

	do c = *s++;
	while (ISSPACE (c));
	if (c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	} else if (c == '0')
		base = 8;
	else
		base = 10;
	for (any=0; ; ++any) {
		if (ISDIGIT (c))
			c -= '0';
		else if (c>='a' && c<='f')
			c -= 'a' - 10;
		else if (c>='A' && c<='F')
			c -= 'A' - 10;
		else
			break;
		if (c >= base)
			break;
		acc *= base;
		acc += c;
		c = *s++;
	}
	if (endptr)
		*endptr = any ? s-1 : nptr;
	return (acc);
}

int getvalues (char *line, int *o, int *oe, int *n, int *ne)
{
	char *p;

#define SKIPSPACES while (*p==' ' || *p=='\t') ++p

	p = line;
	SKIPSPACES;
	if (*p<'0' || *p>'9')
		return (0);
	line = p;
	*o = getval (line, &p);
	if (p == line)
		return (0);
	SKIPSPACES;
	if (*p == '\n' || *p == '#' || *p == 0) { /* 123 */
		*oe = *n = *o;
		return (1);
	}

	if (*p == '>')
		*oe = *o;
	else if (*p == '-') {
		line = ++p;
		*oe = getval (line, &p);
		if (p == line)
			return (0);
		SKIPSPACES;
		if (*p == '\n' || *p == '#' || *p == 0) { /* 123 - 145 */
			*n = *o;
			return (1);
		}
		if (*p != '>')
			return (0);
	} else
		return (0);

	line = ++p;
	*n = getval (line, &p);
	if (p == line)
		return (0);
	SKIPSPACES;
	if (*p == '\n' || *p == '#' || *p == 0) { /* 123 - 145 > 85 */
		*ne = *n;
		return (1);
	}
	if (*p != '-')
		return (0);

	line = ++p;
	*ne = getval (line, &p);
	if (p == line)
		return (0);
	SKIPSPACES;
	if (*p == '\n' || *p == '#' || *p == 0) /* 123 - 145 > 85 - 107 */
		return (*oe - *o == *ne - *n);
	return (0);
}

void gettab (char *name)
{
	char line [LINESZ];
	FILE *fd;
	int i, o, oe, n, ne;

	fd = fopen (name, "r");
	if (! fd) {
		fprintf (stderr, "cannot open %s\n", name);
		exit (-1);
	}
	for (i=0; i<=255; ++i)
		tab[i] = i;
	while (fgets (line, sizeof (line), fd)) {
		if (! getvalues (line, &o, &oe, &n, &ne))
			continue;
		if (ne == n)
			for (i=o; i<=oe; ++i)
				tab[i] = n;
		else
			for (i=o; i<=oe; ++i)
				tab[i] = n++;
	}
	fclose (fd);
}

int main (int argc, char **argv)
{
	int i;

	if (argc != 3) {
		fprintf (stderr, "Usage: mktab filename tabname\n");
		exit (-1);
	}
	gettab (argv[1]);
	printf ("unsigned char %s [128] = {", argv[2]);
	for (i=128; i<256; ++i) {
		if (i % 8 == 0)
			printf ("\n\t");
		printf ("0x%02x, ", tab[i]);
	}
	printf ("\n};\n");
	return (0);
}
