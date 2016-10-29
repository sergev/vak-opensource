/*
 * Disassembler for PIC17C4x processors.
 *
 * Copyright (C) 1997-2002 Serge Vakulenko <vak@cronyx.ru>
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
#include <stdarg.h>

#define TXTSIZE         8196    /* command segment size */
#define DATSIZE         256     /* data segment size */

/*
 * Processor configuration.
 */
#define CFGADDR         0xfe00

#define O_FOSC0         0x0001
#define O_FOSC1         0x0002
#define O_WDTPS0        0x0004
#define O_WDTPS1        0x0008
#define O_PM0           0x0010
#define O_PM1           0x0040
#define O_PM2           0x8000

int debug;
int option;
char *infile;

unsigned char data [DATSIZE];
unsigned short text [TXTSIZE];
unsigned char tbusy [TXTSIZE];

void uerror (char *s, ...)
{
	va_list ap;

	va_start (ap, s);
	if (infile)
		fprintf (stderr, "%s: ", infile);
	vfprintf (stderr, s, ap);
	va_end (ap);
	fprintf (stderr, "\n");
	exit (1);
}

int hexdig (char *p)
{
	unsigned char val = 0;

	if      (*p >= '0' && *p <= '9') val = (*p - '0') << 4;
	else if (*p >= 'A' && *p <= 'F') val = (*p - 'A' + 10) << 4;
	else uerror ("bad hex digit");
	++p;
	if      (*p >= '0' && *p <= '9') val += *p - '0';
	else if (*p >= 'A' && *p <= 'F') val += *p - 'A' + 10;
	else uerror ("bad hex digit");
	return val;
}

int gethex (int *len, long *addr, unsigned char *line)
{
	char buf [80];
	unsigned char sum;
	int i, eof;
	static int high;
again:
	if (! fgets (buf, sizeof (buf), stdin))
		uerror ("unexpected EOF");
	if (buf[0] != ':')
		goto again;
	*len = hexdig (buf+1);
	if (strlen (buf) < *len * 2 + 11)
		uerror ("too short hex line");
	*addr = (long) high << 16 | hexdig (buf+3) << 8 | hexdig (buf+5);
	eof = hexdig (buf+7);

	sum = 0;
	for (i=0; i<*len; ++i) {
		line [i] = hexdig (buf+9 + i + i);
		sum += line [i];
	}

	sum += eof + *len + (*addr & 0xff) + (*addr >> 8 & 0xff);
	if (sum != (unsigned char) - hexdig (buf+9 + *len + *len))
		uerror ("bad hex checksum");

	if (eof == 4) {
		if (*len != 2)
			uerror ("invalid hex linear address record length");
		high = line[0] << 8 | line[1];
		goto again;
	}
	if (eof == 1)
		return 0;
	if (eof)
		uerror ("unknown hex record");
	return 1;
}

void readimage ()
{
	int len, i;
	long addr;
	unsigned char line [16];

	while (gethex (&len, &addr, line)) {
		if (addr == CFGADDR*2) {
			option = line [0] | line [1] << 8;
			continue;
		}
		if (addr & 1)
			uerror ("odd address");
		if (len & 1)
			uerror ("odd length");
		addr /= 2;
		if (addr > TXTSIZE)
			uerror ("invalid hex address");
		for (i=0; i<len; i+=2) {
			text [addr] = line [i] | line [i+1] << 8;
			tbusy [addr] = 1;
			++addr;
		}
	}
}

void print (int addr, int cmd)
{
	char *m;
	int arg;

	printf ("%04x: %02x %02x\t", addr, cmd >> 8 & 0xff, cmd & 0xff);

	switch (cmd) {
	case 0x0000: printf ("nop");   goto done;
	case 0x0002: printf ("ret");   goto done;
	case 0x0003: printf ("sleep"); goto done;
	case 0x0004: printf ("awake"); goto done;
	case 0x0005: printf ("reti");  goto done;
	case 0x0006: printf ("dump");  goto done;
	case 0x0007: printf ("halt");  goto done;
	case 0x170a: printf ("a--?");  goto done;
	case 0x1d0a: printf ("aw");    goto done;
	case 0x1f0a: printf ("a++?");  goto done;
	case 0x250a: printf ("a++z?"); goto done;
	case 0x270a: printf ("a--z?"); goto done;
	case 0x290a: printf ("az");    goto done;
	case 0x2b0a: printf ("as");    goto done;
	case 0x330a: printf ("a?");    goto done;
	case 0x9a04: printf ("z?");    goto done;
	case 0x9204: printf ("nz?");   goto done;
	case 0x9804: printf ("c?");    goto done;
	case 0x9004: printf ("b?");    goto done;
	}

	switch (cmd >> 8) {
	case 0x01: m = "atx";    goto shortop;
	case 0x02: m = "a-bx";   goto shortop;
	case 0x03: m = "x-ba";   goto shortop;
	case 0x04: m = "a-x";    goto shortop;
	case 0x05: m = "x-a";    goto shortop;
	case 0x06: m = "x--a";   goto shortop;
	case 0x07: m = "x--";    goto shortop;
	case 0x08: m = "a|x";    goto shortop;
	case 0x09: m = "x|a";    goto shortop;
	case 0x0a: m = "a&x";    goto shortop;
	case 0x0b: m = "x&a";    goto shortop;
	case 0x0c: m = "a^x";    goto shortop;
	case 0x0d: m = "x^a";    goto shortop;
	case 0x0e: m = "a+x";    goto shortop;
	case 0x0f: m = "x+a";    goto shortop;
	case 0x10: m = "a+cx";   goto shortop;
	case 0x11: m = "x+ca";   goto shortop;
	case 0x12: m = "xca";    goto shortop;
	case 0x13: m = "xc";     goto shortop;
	case 0x14: m = "x++a";   goto shortop;
	case 0x15: m = "x++";    goto shortop;
	case 0x16: m = "x--a?";  goto shortop;
	case 0x17: m = "x--?";   goto shortop;
	case 0x18: m = "xc>>a";  goto shortop;
	case 0x19: m = "xc>>x";  goto shortop;
	case 0x1a: m = "xc<<a";  goto shortop;
	case 0x1b: m = "xc<<x";  goto shortop;
	case 0x1c: m = "xwa";    goto shortop;
	case 0x1d: m = "xw";     goto shortop;
	case 0x1e: m = "x++a?";  goto shortop;
	case 0x1f: m = "x++?";   goto shortop;
	case 0x20: m = "x>>a";   goto shortop;
	case 0x21: m = "x>>x";   goto shortop;
	case 0x22: m = "x<<a";   goto shortop;
	case 0x23: m = "x<<x";   goto shortop;
	case 0x24: m = "x++az?"; goto shortop;
	case 0x25: m = "x++z?";  goto shortop;
	case 0x26: m = "x--az?"; goto shortop;
	case 0x27: m = "x--z?";  goto shortop;
	case 0x28: m = "xza";    goto shortop;
	case 0x29: m = "xz";     goto shortop;
	case 0x2a: m = "xsa";    goto shortop;
	case 0x2b: m = "xs";     goto shortop;
	case 0x2c: m = "anax";   goto shortop;
	case 0x2d: m = "anx";    goto shortop;
	case 0x2e: m = "adax";   goto shortop;
	case 0x2f: m = "adx";    goto shortop;
	case 0x30: m = "x>=a?";  goto shortop;
	case 0x31: m = "x!=a?";  goto shortop;
	case 0x32: m = "x<=a?";  goto shortop;
	case 0x33: m = "x?";     goto shortop;
	case 0x34: m = "a*x";    goto shortop;
	case 0x6a: m = "xta";    goto shortop;
	case 0xa0: m = "llx";    goto shortop;
	case 0xa2: m = "lhx";    goto shortop;
	case 0xa4: m = "xll";    goto shortop;
	case 0xa6: m = "xhl";    goto shortop;
	case 0xa8: m = "plx";    goto shortop;
	case 0xa9: m = "pl++x";  goto shortop;
	case 0xae: m = "xhp";    goto shortop;
	case 0xaf: m = "xhp++";  goto shortop;
	case 0xb0: m = "cta";    goto shortop;
	case 0xb1: m = "a+c";    goto shortop;
	case 0xb2: m = "c-a";    goto shortop;
	case 0xb3: m = "a|c";    goto shortop;
	case 0xb4: m = "a^c";    goto shortop;
	case 0xb5: m = "a&c";    goto shortop;
	case 0xb6: m = "retc";   goto shortop;
	case 0xb7: m = "lcall";  goto shortop;
	case 0xb8: m = "reg";    goto shortop;
	case 0xba: m = "dat";    goto shortop;
	case 0xbc: m = "a*c";
shortop:        printf ("%s", m);
		arg = cmd & 0xff;
		if (arg > 9)
			printf ("\t0x%x", arg);
		else if (arg)
			printf ("\t%d", arg);
		goto done;
	}

	switch (cmd >> 8 & 0xf8) {
	case 0x38: m = "bt";  goto bitop;
	case 0x80: m = "bs";  goto bitop;
	case 0x88: m = "bz";  goto bitop;
	case 0x90: m = "bz?"; goto bitop;
	case 0x98: m = "bs?";
bitop:          printf ("%s\t", m);
		arg = cmd & 0xff;
		if (arg > 9)
			printf ("0x%x", arg);
		else
			printf ("%d", arg);
		printf (", %d", cmd >> 8 & 7);
		break;

	case 0xc0: case 0xc8: case 0xd0: case 0xd8: m = "goto"; goto longop;
	case 0xe0: case 0xe8: case 0xf0: case 0xf8: m = "call";
longop:         printf ("%s", m);
		arg = cmd & 0x1fff;
		if (arg > 9)
			printf ("\t0x%x", arg);
		else if (arg)
			printf ("\t%d", arg);
		break;

	case 0x60: case 0x68: case 0x70: case 0x78:
		printf ("xtr\t");
		arg = cmd & 0xff;
		if (arg > 9)
			printf ("0x%x", arg);
		else
			printf ("%d", arg);
		arg = cmd >> 8 & 0x1f;
		if (arg > 9)
			printf (", 0x%x", arg);
		else
			printf (", %d", arg);
		break;

	case 0x40: case 0x48: case 0x50: case 0x58:
		printf ("rtx\t");
		arg = cmd >> 8 & 0x1f;
		if (arg > 9)
			printf ("0x%x", arg);
		else
			printf ("%d", arg);
		arg = cmd & 0xff;
		if (arg > 9)
			printf (", 0x%x", arg);
		else
			printf (", %d", arg);
		break;

	default:
		printf ("?");
		if (cmd >= ' ' && cmd <= '~')
			printf ("\t\t; `%c'", cmd);
		break;
	}

done:   printf ("\n");
}

void disasm ()
{
	int i;

	if (option) {
		printf ("\t\t.config\t");
		if (option & O_FOSC0)  printf ("FOSC0 ");
		if (option & O_FOSC1)  printf ("FOSC1 ");
		if (option & O_WDTPS0) printf ("WDTPS0 ");
		if (option & O_WDTPS1) printf ("WDTPS1 ");
		if (option & O_PM0)    printf ("PM0 ");
		if (option & O_PM1)    printf ("PM1 ");
		if (option & O_PM2)    printf ("PM2 ");
		printf ("\n");
	}
	for (i=0; i<TXTSIZE; ++i)
		if (tbusy [i])
			print (i, text [i]);
}

int main (int argc, char **argv)
{
	int i;
	char *cp;

	for (i=1; i<argc; i++)
		switch (argv[i][0]) {
		case '-':
			for (cp=argv[i]; *cp; cp++) switch (*cp) {
			case 'd':
				debug++;
				break;
			}
			break;
		default:
			if (infile)
				goto usage;
			infile = argv[i];
			if (! freopen (infile, "r", stdin))
				uerror ("cannot open");
			break;
		}

	if (! infile) {
usage:          printf ("PIC 17c4x Disassembler, by Serge V.Vakulenko\n");
		printf ("Copyright (C) 1997 Cronyx Engineering Ltd.\n\n");
		printf ("Usage:\n\tdis17 [-o outfile.hex] infile.s ...\n\n");
		return -1;
	}

	readimage ();
	disasm ();
	return 0;
}
