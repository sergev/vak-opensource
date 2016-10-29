/*
 * Copyright (C) 2005 Serge Vakulenko, <vak@cronyx.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING" for more details.
 */
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
/*#include <unistd.h>*/
#include "cyrillic.h"

const char version[] = "1.0";
const char copyright[] = "Copyright (C) 2006 Serge Vakulenko";

char *progname;

void putc_volapuk (int c, FILE *fd)
{
	switch (c) {
	default:					fputs ("?", fd); break;
	case CYRILLIC_CAPITAL_LETTER_A:			fputs ("A", fd); break;
	case CYRILLIC_CAPITAL_LETTER_BE:		fputs ("B", fd); break;
	case CYRILLIC_CAPITAL_LETTER_VE:		fputs ("V", fd); break;
	case CYRILLIC_CAPITAL_LETTER_GHE:		fputs ("G", fd); break;
	case CYRILLIC_CAPITAL_LETTER_DE:		fputs ("D", fd); break;
	case CYRILLIC_CAPITAL_LETTER_IE:		fputs ("E", fd); break;
	case CYRILLIC_CAPITAL_LETTER_IO:		fputs ("E", fd); break;
	case CYRILLIC_CAPITAL_LETTER_ZHE:		fputs ("Zh", fd); break;
	case CYRILLIC_CAPITAL_LETTER_ZE:		fputs ("Z", fd); break;
	case CYRILLIC_CAPITAL_LETTER_I:			fputs ("I", fd); break;
	case CYRILLIC_CAPITAL_LETTER_SHORT_I:		fputs ("J", fd); break;
	case CYRILLIC_CAPITAL_LETTER_KA:		fputs ("K", fd); break;
	case CYRILLIC_CAPITAL_LETTER_EL:		fputs ("L", fd); break;
	case CYRILLIC_CAPITAL_LETTER_EM:		fputs ("M", fd); break;
	case CYRILLIC_CAPITAL_LETTER_EN:		fputs ("N", fd); break;
	case CYRILLIC_CAPITAL_LETTER_O:			fputs ("O", fd); break;
	case CYRILLIC_CAPITAL_LETTER_PE:		fputs ("P", fd); break;
	case CYRILLIC_CAPITAL_LETTER_ER:		fputs ("R", fd); break;
	case CYRILLIC_CAPITAL_LETTER_ES:		fputs ("S", fd); break;
	case CYRILLIC_CAPITAL_LETTER_TE:		fputs ("T", fd); break;
	case CYRILLIC_CAPITAL_LETTER_U:			fputs ("U", fd); break;
	case CYRILLIC_CAPITAL_LETTER_EF:		fputs ("F", fd); break;
	case CYRILLIC_CAPITAL_LETTER_HA:		fputs ("H", fd); break;
	case CYRILLIC_CAPITAL_LETTER_TSE:		fputs ("C", fd); break;
	case CYRILLIC_CAPITAL_LETTER_CHE:		fputs ("Ch", fd); break;
	case CYRILLIC_CAPITAL_LETTER_SHA:		fputs ("Sh", fd); break;
	case CYRILLIC_CAPITAL_LETTER_SHCHA:		fputs ("Sch", fd); break;
	case CYRILLIC_CAPITAL_LETTER_HARD_SIGN:		fputs ("'", fd); break;
	case CYRILLIC_CAPITAL_LETTER_YERU:		fputs ("Y", fd); break;
	case CYRILLIC_CAPITAL_LETTER_SOFT_SIGN:		fputs ("'", fd); break;
	case CYRILLIC_CAPITAL_LETTER_E:			fputs ("E", fd); break;
	case CYRILLIC_CAPITAL_LETTER_YU:		fputs ("Yu", fd); break;
	case CYRILLIC_CAPITAL_LETTER_YA:		fputs ("Ya", fd); break;

	case CYRILLIC_SMALL_LETTER_A:			fputs ("a", fd); break;
	case CYRILLIC_SMALL_LETTER_BE:			fputs ("b", fd); break;
	case CYRILLIC_SMALL_LETTER_VE:			fputs ("v", fd); break;
	case CYRILLIC_SMALL_LETTER_GHE:			fputs ("g", fd); break;
	case CYRILLIC_SMALL_LETTER_DE:			fputs ("d", fd); break;
	case CYRILLIC_SMALL_LETTER_IE:			fputs ("e", fd); break;
	case CYRILLIC_SMALL_LETTER_IO:			fputs ("e", fd); break;
	case CYRILLIC_SMALL_LETTER_ZHE:			fputs ("zh", fd); break;
	case CYRILLIC_SMALL_LETTER_ZE:			fputs ("z", fd); break;
	case CYRILLIC_SMALL_LETTER_I:			fputs ("i", fd); break;
	case CYRILLIC_SMALL_LETTER_SHORT_I:		fputs ("j", fd); break;
	case CYRILLIC_SMALL_LETTER_KA:			fputs ("k", fd); break;
	case CYRILLIC_SMALL_LETTER_EL:			fputs ("l", fd); break;
	case CYRILLIC_SMALL_LETTER_EM:			fputs ("m", fd); break;
	case CYRILLIC_SMALL_LETTER_EN:			fputs ("n", fd); break;
	case CYRILLIC_SMALL_LETTER_O:			fputs ("o", fd); break;
	case CYRILLIC_SMALL_LETTER_PE:			fputs ("p", fd); break;
	case CYRILLIC_SMALL_LETTER_ER:			fputs ("r", fd); break;
	case CYRILLIC_SMALL_LETTER_ES:			fputs ("s", fd); break;
	case CYRILLIC_SMALL_LETTER_TE:			fputs ("t", fd); break;
	case CYRILLIC_SMALL_LETTER_U:			fputs ("u", fd); break;
	case CYRILLIC_SMALL_LETTER_EF:			fputs ("f", fd); break;
	case CYRILLIC_SMALL_LETTER_HA:			fputs ("h", fd); break;
	case CYRILLIC_SMALL_LETTER_TSE:			fputs ("c", fd); break;
	case CYRILLIC_SMALL_LETTER_CHE:			fputs ("ch", fd); break;
	case CYRILLIC_SMALL_LETTER_SHA:			fputs ("sh", fd); break;
	case CYRILLIC_SMALL_LETTER_SHCHA:		fputs ("sch", fd); break;
	case CYRILLIC_SMALL_LETTER_HARD_SIGN:		fputs ("'", fd); break;
	case CYRILLIC_SMALL_LETTER_YERU:		fputs ("y", fd); break;
	case CYRILLIC_SMALL_LETTER_SOFT_SIGN:		fputs ("'", fd); break;
	case CYRILLIC_SMALL_LETTER_E:			fputs ("e", fd); break;
	case CYRILLIC_SMALL_LETTER_YU:			fputs ("yu", fd); break;
	case CYRILLIC_SMALL_LETTER_YA:			fputs ("ya", fd); break;
	}
}

//
// utf-8
// 00000000.0xxxxxxx -> 0xxxxxxx
// 00000xxx.xxyyyyyy -> 110xxxxx, 10yyyyyy
// xxxxyyyy.yyzzzzzz -> 1110xxxx, 10yyyyyy, 10zzzzzz
//
int utf8_getc (FILE *fd)
{
	int c1, c2, c3;

	c1 = getc (fd);
	if (c1 < 0 || ! (c1 & 0x80))
		return c1;
	c2 = getc (fd);
	if (! (c1 & 0x20))
		return (c1 & 0x1f) << 6 | (c2 & 0x3f);
	c3 = getc (fd);
	return (c1 & 0x0f) << 12 | (c2 & 0x3f) << 6 | (c3 & 0x3f);
}

void utf8_putc (unsigned short c, FILE *fd)
{
	if (c < 0x80) {
		putc (c, fd);
		return;
	}
	if (c < 0x800) {
		putc (c >> 6 | 0xc0, fd);
		putc ((c & 0x3f) | 0x80, fd);
		return;
	}
	putc (c >> 12 | 0xe0, fd);
	putc (((c >> 6) & 0x3f) | 0x80, fd);
	putc ((c & 0x3f) | 0x80, fd);
}

void process (FILE *fd)
{
	int c;

	for (;;) {
		c = utf8_getc (fd);
		if (c < 0)
			break;
		if (c >= 0x400 && c < 0x4ff)
			putc_volapuk (c, stdout);
		else
			putchar (c & 0xff);
	}
}

void usage ()
{
	fprintf (stderr, "Volapuk encoder, Version %s, %s\n", version, copyright);
	fprintf (stderr, "Usage:\n");
	fprintf (stderr, "\t%s [-v] [-d] [file]\n",
		progname);
	fprintf (stderr, "Options:\n");
	fprintf (stderr, "\t-v\t\tverbose\n");
	fprintf (stderr, "\t-d\t\tdecode\n");
	exit (-1);
}

int main (int argc, char **argv)
{
	int verbose = 0, decode_flag = 0;

	progname = *argv;
	for (;;) {
		switch (getopt (argc, argv, "vd")) {
		case EOF:
			break;
		case 'd':
			decode_flag = 1;
			continue;
		case 'v':
			verbose = 1;
			continue;
		default:
			usage ();
		}
		break;
	}
	argc -= optind;
	argv += optind;

	if (argc > 1)
		usage ();

	if (argc == 1) {
		FILE *fd;

		fd = fopen (argv[0], "r");
		if (! fd) {
			perror (argv[0]);
			exit (-1);
		}
		process (fd);
	} else
		process (stdin);

	return (0);
}
