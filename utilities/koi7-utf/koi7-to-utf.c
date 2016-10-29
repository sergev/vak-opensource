/*
 * Decode KOI-7 files from BESM-6.
 *
 * Copyright (C) 2009 Serge Vakulenko, <serge@vak.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 */
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

const char version[] = "1.0";
const char copyright[] = "Copyright (C) 2009 Serge Vakulenko";

char *progname;
int verbose;
int trace;
int debug;
int cyrmode;

void put (int lat, const char *rus)
{
	if (cyrmode)
		fputs (rus, stdout);
	else
		fputc (lat, stdout);
}


void decode (char *filename)
{
	FILE *fd;
	int c;

	fd = fopen (filename, "r");
	for (;;) {
		c = getc (fd);
		switch (c) {
		case EOF:
			fclose (fd);
			return;
		case '\n':
		case '\t':
			fputc (c, stdout);
			break;
		case 'N' & 037:
			cyrmode = 1;
			break;
		case 'O' & 037:
			cyrmode = 0;
			break;
                case '@':	put (c, "ю"); break;
                case 'A':       put (c, "а"); break;
                case 'B':       put (c, "б"); break;
                case 'C':       put (c, "ц"); break;
                case 'D':       put (c, "д"); break;
                case 'E':       put (c, "е"); break;
                case 'F':       put (c, "ф"); break;
                case 'G':       put (c, "г"); break;
                case 'H':       put (c, "х"); break;
                case 'I':       put (c, "и"); break;
                case 'J':       put (c, "й"); break;
                case 'K':       put (c, "к"); break;
                case 'L':       put (c, "л"); break;
                case 'M':       put (c, "м"); break;
                case 'N':       put (c, "н"); break;
                case 'O':       put (c, "о"); break;
                case 'P':       put (c, "п"); break;
                case 'Q':       put (c, "я"); break;
                case 'R':       put (c, "р"); break;
                case 'S':       put (c, "с"); break;
                case 'T':       put (c, "т"); break;
                case 'U':       put (c, "у"); break;
                case 'V':       put (c, "ж"); break;
                case 'W':       put (c, "в"); break;
                case 'X':       put (c, "ь"); break;
                case 'Y':       put (c, "ы"); break;
                case 'Z':       put (c, "з"); break;
                case '[':       put (c, "ш"); break;
                case '\\':      put (c, "э"); break;
                case ']':       put (c, "щ"); break;
                case '^':       put (c, "ч"); break;

                case '`':	put (c, "Ю"); break;
                case 'a':       put (c, "А"); break;
                case 'b':       put (c, "Б"); break;
                case 'c':       put (c, "Ц"); break;
                case 'd':       put (c, "Д"); break;
                case 'e':       put (c, "Е"); break;
                case 'f':       put (c, "Ф"); break;
                case 'g':       put (c, "Г"); break;
                case 'h':       put (c, "Х"); break;
                case 'i':       put (c, "И"); break;
                case 'j':       put (c, "Й"); break;
                case 'k':       put (c, "К"); break;
                case 'l':       put (c, "Л"); break;
                case 'm':       put (c, "М"); break;
                case 'n':       put (c, "Н"); break;
                case 'o':       put (c, "О"); break;
                case 'p':       put (c, "П"); break;
                case 'q':       put (c, "Я"); break;
                case 'r':       put (c, "Р"); break;
                case 's':       put (c, "С"); break;
                case 't':       put (c, "Т"); break;
                case 'u':       put (c, "У"); break;
                case 'v':       put (c, "Ж"); break;
                case 'w':       put (c, "В"); break;
                case 'x':       put (c, "Ь"); break;
                case 'y':       put (c, "Ы"); break;
                case 'z':       put (c, "З"); break;
                case '{':       put (c, "Ш"); break;
                case '|':       put (c, "Э"); break;
                case '}':       put (c, "Щ"); break;
                case '~':       put (c, "Ч"); break;
		default:
			if (c >= ' ' && c <= '_') {
				/* Символ ASCII. */
				fputc (c, stdout);

			} else if (c > 0200) {
				/* Пробелы. */
				c -= 0200;
				while (c-- > 0)
					fputc (' ', stdout);
			} else {
				printf ("<%03o>", c);
			}
			break;
		}
	}
}

void usage ()
{
	fprintf (stderr, "Decode KOI7 to UTF8, Version %s, %s\n", version, copyright);
	fprintf (stderr, "Usage:\n\t%s [-vtd] file...\n", progname);
	fprintf (stderr, "Options:\n");
	fprintf (stderr, "\t-v\tverbose mode\n");
	fprintf (stderr, "\t-t\ttrace mode\n");
	fprintf (stderr, "\t-d\tdebug\n");
	exit (-1);
}

int main (int argc, char **argv)
{
	int i;

	progname = *argv;
	for (;;) {
		switch (getopt (argc, argv, "vtd")) {
		case EOF:
			break;
		case 'v':
			++verbose;
			continue;
		case 't':
			++trace;
			continue;
		case 'd':
			++debug;
			continue;
		default:
			usage ();
		}
		break;
	}
	argc -= optind;
	argv += optind;
	if (argc < 1)
		usage ();

	for (i=0; i<argc; ++i)
		decode (argv[i]);

	return (0);
}
