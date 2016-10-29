#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char *
expand_string (char **dst, int *len, char *tail, int n)
{
	/* Make enough space. */
	if (tail + n >= *dst + *len) {
		/* Allocate by kilobytes. */
		*len = (tail + n - *dst + 1024) / 1024 * 1024;
		*dst = realloc (*dst, *len);
		if (! *dst)
			return 0;
		tail = *dst + strlen (*dst);
	}
	return tail;
}

/*
 * Append file contents to the string.
 * Rellocate the string when needed.
 * 1) Remove comments "# test", ignore blank lines.
 * 2) Remove leading and trailing spaces.
 * 4) Insert '|' between lines.
 * 5) Mask dots - replace by '\.'
 * 6) Replace quastion marks by dots.
 */
static void
append_file (char **dst, int *len, char *filename)
{
	FILE *fd;
	char line [256], *p, *tail;

	/*printf ("%d - %s\n", *len, filename);*/

	fd = fopen (filename, "r");
	if (! fd) {
		perror (filename);
		return;
	}
	/* Add '('. */
	tail = expand_string (dst, len, *dst + strlen (*dst), 1);
	if (! tail)
		return;
	*tail++ = '(';

	while (fgets (line, sizeof(line), fd)) {
		/* Remove trailing comment or newline. */
		p = line;
		strsep (&p, "#\n");

		/* Remove trailing spaces. */
		p = line + strlen (line) - 1;
		while (p>=line && (*p==' ' || *p == '\t' || *p == '\r'))
			*p-- = 0;
		if (p < line)
			continue;

		/* Remove spaces at line beginning. */
		p = line;
		while (*p==' ' || *p == '\t' || *p == '\r' || *p == '\n')
			++p;
		if (*p == 0)
			continue;

		/* Make enough space. */
		tail = expand_string (dst, len, tail, 2 * strlen (p) + 1);
		if (! tail)
			return;
		/*printf ("%d/%d>\t[%d] `%s'\n", tail - *dst, *len, strlen (p), p);*/

		/* Copy line. Escape dots - replace by '\.'
		 * Question marks replace by dots. */
		for (; *p; ++p) {
			if (*p == '.') {
				*tail++ = '\\';
				*tail++ = '.';
			} else if (*p == '?')
				*tail++ = '.';
			else
				*tail++ = *p;
		}
		/* Add '|'. */
		*tail++ = '|';
		*tail = 0;
	}
	/* Remove trailing '('. */
	if (*(tail-1) == '(')
		*--tail = 0;
	/* Replace trailing '|' by ')'. */
	else if (*(tail-1) == '|')
		*(tail-1) = ')';

	fclose (fd);
}

/*
 * Scan string, searching for file name. Replace the file name by
 * file contents. Return the copy of string, allocated by malloc().
 */
static char *
scan_file_ref (char *str)
{
	char *dst, *p, *q;
	int len;

	len = strlen (str) + 1;
	dst = malloc (len);
	if (! dst)
		return 0;

	for (p=dst; p<dst+len-1; ) {
		if (*str != '=' || str[1] != '(') {
			if (*str == '\\' && str[1] == '=') {
				/* Translate '\=' to '='. */
				++str;
			}
			*p++ = *str++;
			continue;
		}
		/* Got '=(', search for ')'. */
		q = strchr (str, ')');
		if (! q)
			continue;
		*q = 0;

		/* Got file name, append file contents. */
		*p = 0;
		append_file (&dst, &len, str+2);
		if (! dst)
			return 0;

		/* Scan the rest of string. */
		p = dst + strlen (dst);
		str = q+1;
	}
	*p = 0;
	return dst;
}

int main (int argc, char **argv)
{
	int i;
	char *p;

	for (i=1; i<argc; ++i) {
		printf ("%s:\n", argv[i]);
		p = scan_file_ref (argv[i]);
		printf ("%s\n", p);
	}
	return 0;
}
