/*
 * Disassembler for PIC16Cxx processors.
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

#define TXTSIZE         256	/* command segment size */
#define DATSIZE         32	/* data segment size */

/*
 * Processor configuration.
 */
#define CFGADDR         0xfff

#define CFG_LP_OSC      0x00	/* low power crystal */
#define CFG_XT_OSC      0x01	/* crystal/resonator */
#define CFG_IRC_OSC     0x02	/* internal 4 MHz RC oscillator */
#define CFG_XRC_OSC     0x03	/* external resistor/capacitor */
#define CFG_WDTE        0x04	/* watchdog enable */
#define CFG_PROTECT     0x08	/* code protection enable */
#define CFG_MCLRE       0x10	/* /MCLR pin enable */
#if 0
#define O_FOSC0         0x0001
#define O_FOSC1         0x0002
#define O_WDTPS0        0x0004
#define O_WDTPS1        0x0008
#define O_PM0           0x0010
#define O_PM1           0x0040
#define O_PM2           0x8000
#endif

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
	case 0x000: printf ("nop");   goto done;
	case 0x002: printf ("option");goto done;
	case 0x003: printf ("sleep"); goto done;
	case 0x004: printf ("awake"); goto done;
	case 0x006: printf ("tris");  goto done;
	case 0x007: printf ("dump");  goto done;
	case 0x008: printf ("halt");  goto done;
	case 0x040: printf ("az");    goto done;
	case 0x643: printf ("z?");    goto done;
	case 0x743: printf ("nz?");   goto done;
	case 0x603: printf ("c?");    goto done;
	case 0x703: printf ("nc?");   goto done;
	}

	switch (cmd >> 8) {
	case 0x8: m = "ret";  goto byteop;
	case 0x9: m = "call"; goto byteop;
	case 0xc: m = "cta";  goto byteop;
	case 0xd: m = "a|c";  goto byteop;
	case 0xe: m = "a&c";  goto byteop;
	case 0xf: m = "a^c";
byteop:		printf ("%s", m);
		arg = cmd & 0xff;
		if (arg > 9)
			printf ("\t0x%x", arg);
		else if (arg)
			printf ("\t%d", arg);
		goto done;

	case 0x4: m = "bz";  goto bitop;
	case 0x5: m = "bs";  goto bitop;
	case 0x6: m = "bs?"; goto bitop;
	case 0x7: m = "bz?"; goto bitop;
bitop:          printf ("%s\t", m);
		arg = cmd & 0x1f;
		if (arg > 9)
			printf ("0x%x", arg);
		else
			printf ("%d", arg);
		printf (", %d", cmd >> 5 & 7);
		goto done;
	}

	switch (cmd >> 4 & 0xfe) {
	case 0x02: m = "atx";   goto shortop;
	case 0x06: m = "xz";    goto shortop;
	case 0x08: m = "a-x";   goto shortop;
	case 0x0a: m = "x-a";   goto shortop;
	case 0x0c: m = "x--a";  goto shortop;
	case 0x0e: m = "x--";   goto shortop;
	case 0x10: m = "a|x";   goto shortop;
	case 0x12: m = "x|a";   goto shortop;
	case 0x14: m = "a&x";   goto shortop;
	case 0x16: m = "x&a";   goto shortop;
	case 0x18: m = "a^x";   goto shortop;
	case 0x1a: m = "x^a";   goto shortop;
	case 0x1c: m = "a+x";   goto shortop;
	case 0x1e: m = "x+a";   goto shortop;
	case 0x20: m = "xta";   goto shortop;
	case 0x22: m = "tst";   goto shortop;
	case 0x24: m = "xca";   goto shortop;
	case 0x26: m = "xc";    goto shortop;
	case 0x28: m = "x++a";  goto shortop;
	case 0x2a: m = "x++";   goto shortop;
	case 0x2c: m = "x--a?"; goto shortop;
	case 0x2e: m = "x--?";  goto shortop;
	case 0x30: m = "xc>>a"; goto shortop;
	case 0x32: m = "xc>>x"; goto shortop;
	case 0x34: m = "xc<<a"; goto shortop;
	case 0x36: m = "xc<<x"; goto shortop;
	case 0x38: m = "xwa";   goto shortop;
	case 0x3a: m = "xw";    goto shortop;
	case 0x3c: m = "x++a?"; goto shortop;
	case 0x3e: m = "x++?";
shortop:	printf ("%s", m);
		arg = cmd & 0x1f;
		if (arg > 9)
			printf ("\t0x%x", arg);
		else if (arg)
			printf ("\t%d", arg);
		break;

	case 0xa0: case 0xa2: case 0xa4: case 0xa6:
	case 0xa8: case 0xaa: case 0xac: case 0xae:
	case 0xb0: case 0xb2: case 0xb4: case 0xb6:
	case 0xb8: case 0xba: case 0xbc: case 0xbe:
		printf ("goto");
		arg = cmd & 0x1ff;
		if (arg > 9)
			printf ("\t0x%x", arg);
		else if (arg)
			printf ("\t%d", arg);
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
		if (option & CFG_PROTECT)    printf ("protected ");
		if (option & CFG_WDTE)       printf ("watchdog ");
		if (option & CFG_MCLRE)      printf ("mclr ");
		switch (option & 3) {
		case CFG_LP_OSC:  printf ("lowcrystal "); break;
		case CFG_XT_OSC:  printf ("crystal ");    break;
		case CFG_IRC_OSC: printf ("intclock ");   break;
		case CFG_XRC_OSC: printf ("rc ");         break;
		}
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
usage:          printf ("PIC12 Disassembler, by Serge V.Vakulenko\n\n");
		printf ("Usage:\n\tdis12 infile.hex\n\n");
		return -1;
	}

	readimage ();
	disasm ();
	return 0;
}
