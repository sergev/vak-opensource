# include <stdio.h>

# define COMMENT 40     /* position of comment */

main ()
{
	char line [100];

	while (gets (line))
		frm (line);
	return (0);
}

frm (s)
char *s;
{
	char *label, *cmd, *arg, *commnt;
	int column;
	char buf [40];

	if (! *s)
		return;
	if (*s=='*' || *s=='!') {
		puts (s);
		return;
	}
	parse (s, &label, &column, &cmd, &arg, &commnt);
	sprintf (buf, "%s%c", label, column ? ':' : ' ');
	printf ("%-16s%-8s%-24s", buf, cmd, arg);
	if (commnt)
		printf ("; %s\n", commnt);
	else
		putchar ('\n');
}

parse (s, label, column, cmd, arg, commnt)
char *s;
char **label, **cmd, **arg, **commnt;
int *column;
{
	int n;          /* number of spaces seen */
	char *b;        /* begin of line */
	int len;

	*label = *cmd = *arg = "";
	*column = 0;
	*commnt = 0;
	b = s;
	if (*s != ' ') {
		*label = s;
		while (*s && *s!=' ' && *s!=':' && *s!=';' && *s!='\t')
			++s;
		if (*s == ':') {
			*column = 1;
			*s++ = 0;
		} else if (*s == ';') {
			*s++ = 0;
			while (*s && (*s=='\t' || *s==' '))
				++s;
			*commnt = s;
			return;
		} else if (*s)
			*s++ = 0;
		else
			return;
	}
	while (*s && (*s=='\t' || *s==' '))
		++s;
	if (! *s)
		return;

	*cmd = s;
	while (*s && *s!=' ' && *s!=';' && *s!='\t')
		++s;
	if (*s == ';') {
		*s++ = 0;
		while (*s && (*s=='\t' || *s==' '))
			++s;
		*commnt = s;
		return;
	} else if (*s)
		*s++ = 0;
	else
		return;
	while (*s && (*s=='\t' || *s==' '))
		++s;
	if (! *s)
		return;

	*arg = s;
	while (*s && *s!=';')
		++s;
	if (*s == ';') {
		*s++ = 0;
		while (*s && (*s=='\t' || *s==' '))
			++s;
		*commnt = s;
		return;
	}
}

lenof (s)
register char *s;
{
	/* compute length of string with tabs and backspaces */
	register len;

	for (len=0; *s; ++s)
		switch (*s) {
		case '\b':
			if (len > 0)
				--len;
			break;
		case '\t':
			len = (len + 8) / 8 * 8;
			break;
		default:
			++len;
		}
	return (len);
}

off (l, o)
{
	int newlen;

	if (l >= o)
		return;
	while ((newlen = (l + 8) / 8 * 8) <= o) {
		putchar ('\t');
		l = newlen;
	}
	while (++l <= o)
		putchar (' ');
}

getstr (s)
register char *s;
{
	register c;

	for (;;) {
		c = getchar () & 0377;
		if (feof (stdin))
			return (0);
		if (c == '\n') {
			*s = 0;
			return (1);
		}
		*s++ = c;
	}
}
