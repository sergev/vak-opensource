/*
 * Print a table of ascii characters.
 *
 * Copyright (GPL) 1993-2008 Serge Vakulenko, <vak@cronyx.ru>
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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <iconv.h>

/* Table of options. */
struct option longopts[] = {
	/* option	     has arg		integer code */
	{ "help",		0,	0,	'h'	},
	{ "version",		0,	0,	'V'	},
	{ "octal",		0,	0,	'o'	},
	{ "hex",		0,	0,	'x'	},
	{ "decimal",		0,	0,	'd'	},
	{ 0,			0,	0,	0	},
};

void usage ()
{
	fprintf (stderr, "ascii utility, version 2.0, Copyright (GPL) Serge Vakulenko\n");
	fprintf (stderr, "\n");
	fprintf (stderr, "Usage:\n");
	fprintf (stderr, "  ascii [options] [encoding]\n");
	fprintf (stderr, "Options:\n");
	fprintf (stderr, "  -d, --decimal          show decimal character codes (default)\n");
	fprintf (stderr, "  -o, --octal            show octal character codes\n");
	fprintf (stderr, "  -x, --hex              show hexadecimal character codes\n");
	fprintf (stderr, "\n");
	fprintf (stderr, "For list of available encodings, run `iconv -l'\n");
	exit (1);
}

int main (int argc, char **argv)
{
	int i, j, n, base;
	char *encoding, *local, *p, *outp, buf [32];
	unsigned char c;
	size_t out_bytes_left, in_bytes_left;
	iconv_t cv;

	base = 10;
	for (;;) {
		i = getopt_long (argc, argv, "hVdox", longopts, 0);
		if (i < 0)
			break;
		switch (i) {
		case 'h':		/* help */
			usage ();
			break;
		case 'V':		/* version */
			printf ("Version: 2.0\n");
			return 0;
		case 'd':		/* decimal */
			base = 10;
			break;
		case 'o':		/* octal */
			base = 8;
			break;
		case 'x':		/* hex */
			base = 16;
			break;
		}
	}
	if (optind < argc-1 || optind > argc)
		usage ();

	if (optind == argc-1)
		encoding = argv[optind];
	else
		encoding = "ascii";

	/* Get local encoding. */
	local = getenv ("LANG");
	if (! local)
#ifdef __CYGWIN__
		local = "cp1251";
#else
		local = "utf-8";
#endif
	/* Strip optional modifier. */
	p = strchr (local, '@');
	if (p)
		*p = 0;

	/* Get codeset. */
	p = strchr (local, '.');
	if (p)
		local = p+1;

	cv = iconv_open (local, encoding);
	if (cv < 0) {
		perror ("iconv_open");
		exit (1);
	}

	/* Print table of characters. */
	for (i=' '; i<256; i+=8) {
		for (j=0; j<8; ++j) {
			c = i + j;

			/* Convert a symbol to local encoding. */
			p = (char*) &c;
			in_bytes_left = 1;
			outp = buf;
			out_bytes_left = sizeof (buf);
			n = iconv (cv, &p, &in_bytes_left,
				&outp, &out_bytes_left);
			if (n != 0) {
				if (c == ' ') {
					fprintf (stderr, "%s: unknown encoding\n",
						encoding);
					exit (1);
				}
				if (j == 0) {
					/* Invalid first symbol.
					 * Ignore this line. */
					break;
				}
				buf[0] = '?';
				n = 1;
			} else
				n = sizeof (buf) - out_bytes_left;

			switch (base) {
			default:
			case 10: printf (  " %3d", c); break;
			case 8:  printf ( " %03o", c); break;
			case 16: printf ("0x%02x", c); break;
			}
			printf (" %.*s  ", n, buf);
		}
		if (j != 0)
			printf ("\r\n");
	}
	iconv_close (cv);
	return 0;
}
