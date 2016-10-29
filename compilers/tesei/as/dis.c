/*
 * Disassembler for Angstrem KP1878BE1 processor.
 *
 * Copyright (C) 2002 Serge Vakulenko <vak@cronyx.ru>
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
#include <argp.h>

const char *argp_program_version =
	"Tesei Disassembler 1.0\n"
	"Copyright (C) 2002 Serge Vakulenko\n"
	"This program is free software; it comes with ABSOLUTELY NO WARRANTY;\n"
	"see the GNU General Public License for more details.";

const char *argp_program_bug_address = "<vak@cronyx.ru>";

#define TXTSIZE         1024	/* command segment size */
#define DATSIZE         128	/* data segment size */

int debug;
char *infile;

unsigned short text [TXTSIZE];
unsigned char tbusy [TXTSIZE];

struct argp_option argp_options[] = {
	{"debug",	'D', 0,		0,	"Print debugging information" },
	{ 0 }
};

/*
 * Parse a single option.
 */
int parse_option (int key, char *arg, struct argp_state *state)
{
	switch (key) {
	case 'D':
		debug = 1;
		break;
	case ARGP_KEY_END:
		if (state->arg_num != 1)	/* Not enough arguments. */
			argp_usage (state);
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

/*
 * Our argp parser.
 */
struct argp argp_parser = {
	/* The options we understand. */
	argp_options,

	/* Function to parse a single option. */
	parse_option,

	/* A description of the arguments we accept. */
	"infile.hex",

	/* Program documentation. */
	"\nDisassembler for Angstrem Tesei (KP1878BE1) processor"
};

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

void print_reg (FILE *out, unsigned char reg)
{
	fprintf (out, "%%%c%d", (reg >> 3) + 'a', reg & 7);
}

void print_command (FILE *out, int line, int addr, int cmd)
{
	char *m;
	int arg;

	if (line > 0)
		fprintf (out, " %4d", line);
	else
		fprintf (out, "     ");
	fprintf (out, " %04x %04x\t\t\t", addr, cmd);

	switch (cmd) {
        case 0x0000: fprintf (out, "nop");	goto done;
        case 0x0001: fprintf (out, "wait");	goto done;
        case 0x0002: fprintf (out, "rst");	goto done;
        case 0x0003: fprintf (out, "ijmp");	goto done;
        case 0x0004: fprintf (out, "tof");	goto done;
        case 0x0005: fprintf (out, "tdc");	goto done;
        case 0x0006: fprintf (out, "sksp");	goto done;
        case 0x0007: fprintf (out, "ijsr");	goto done;
        case 0x0008: fprintf (out, "slp");	goto done;
        case 0x000c: fprintf (out, "rts");	goto done;
        case 0x000d: fprintf (out, "rti");	goto done;
        case 0x000e: fprintf (out, "rtsc\t0"); goto done;
        case 0x000f: fprintf (out, "rtsc\t1"); goto done;
        case 0x0181: fprintf (out, "stc");	goto done;
        case 0x0182: fprintf (out, "stz");	goto done;
        case 0x0184: fprintf (out, "stn");	goto done;
        case 0x0188: fprintf (out, "stie");	goto done;
        case 0x01c1: fprintf (out, "clc");	goto done;
        case 0x01c2: fprintf (out, "clz");	goto done;
        case 0x01c4: fprintf (out, "cln");	goto done;
        case 0x01c8: fprintf (out, "clie");	goto done;
	}

	switch (cmd >> 4) {
	case 0x001:
		fprintf (out, "%s\t#%c", (cmd & 8) ? "pop" : "push",
			(cmd & 7) + 'a');
		goto done;

        case 0x002: case 0x003: m = "swap"; goto arg1op;
        case 0x004: case 0x005: m = "neg"; goto arg1op;
        case 0x006: case 0x007: m = "not"; goto arg1op;
        case 0x008: case 0x009: m = "shl"; goto arg1op;
        case 0x00a: case 0x00b: m = "shr"; goto arg1op;
        case 0x00c: case 0x00d: m = "shra"; goto arg1op;
        case 0x00e: case 0x00f: m = "rlc"; goto arg1op;
        case 0x010: case 0x011: m = "rrc"; goto arg1op;
        case 0x012: case 0x013: m = "adc"; goto arg1op;
        case 0x014: case 0x015: m = "sbc"; goto arg1op;
        case 0x2c2: case 0x2c3: m = "dec"; goto arg1op;
        case 0x302: case 0x303: m = "inc"; goto arg1op;
	case 0x400: case 0x401: m = "clr"; goto arg1op;
arg1op:		fprintf (out, "%s\t", m);
		print_reg (out, cmd & 0x1f);
		goto done;

	case 0x018: m = "sst"; goto statusop;
	case 0x01c: m = "cst";
statusop:	fprintf (out, "%s\t0%xh", m, cmd & 0xf);
		goto done;
	}

	switch (cmd >> 12) {
        case 0x4:
	case 0x5: m = "movl"; goto moveop;
        case 0x6:
	case 0x7: m = "cmpl";
moveop:		fprintf (out, "%s\t", m);
		print_reg (out, cmd & 0x1f);
		arg = (cmd >> 5) & 0xff;
		if (arg > 9)
			fprintf (out, ", 0%xh", arg);
		else
			fprintf (out, ", %d", arg);
		goto done;

        case 0x8: m = "jmp"; goto jumpop;
        case 0x9: m = "jsr"; goto jumpop;
        case 0xa: m = "jz";  goto jumpop;
        case 0xb: m = "jnz"; goto jumpop;
        case 0xc: m = "jns"; goto jumpop;
        case 0xd: m = "js";  goto jumpop;
        case 0xe: m = "jnc"; goto jumpop;
        case 0xf: m = "jc";
jumpop:		fprintf (out, "%s", m);
		arg = cmd & 0xfff;
		if (arg > 9)
			fprintf (out, "\t0%xh", arg);
		else if (arg)
			fprintf (out, "\t%d", arg);
		goto done;
	}

	switch (cmd >> 8) {
        case 0x04: case 0x05: case 0x06: case 0x07: m = "mov"; goto movop;
        case 0x08: case 0x09: case 0x0a: case 0x0b: m = "cmp"; goto movop;
        case 0x0c: case 0x0d: case 0x0e: case 0x0f: m = "sub"; goto movop;
        case 0x10: case 0x11: case 0x12: case 0x13: m = "add"; goto movop;
        case 0x14: case 0x15: case 0x16: case 0x17: m = "and"; goto movop;
        case 0x18: case 0x19: case 0x1a: case 0x1b: m = "or"; goto movop;
        case 0x1c: case 0x1d: case 0x1e: case 0x1f: m = "xor";
movop:		fprintf (out, "%s\t", m);
		print_reg (out, cmd & 0x1f);
		fprintf (out, ", ");
		print_reg (out, cmd >> 5 & 0x1f);
		goto done;

        case 0x28: case 0x29: m = "bicl"; goto bitop;
        case 0x2a: case 0x2b: m = "bich"; goto bitop;
        case 0x34: case 0x35: m = "bttl"; goto bitop;
        case 0x36: case 0x37: m = "btth"; goto bitop;
        case 0x38: case 0x39: m = "bisl"; goto bitop;
        case 0x3a: case 0x3b: m = "bish"; goto bitop;
        case 0x3c: case 0x3d: m = "btgl"; goto bitop;
        case 0x3e: case 0x3f: m = "btgh";
bitop:		fprintf (out, "%s\t", m);
		print_reg (out, cmd & 0x1f);
		fprintf (out, ", 0%xh", cmd >> 5 & 0xf);
		goto done;

        case 0x2c: case 0x2d:
	case 0x2e: case 0x2f: m = "subl"; goto bit5op;
        case 0x30: case 0x31:
	case 0x32: case 0x33: m = "addl";
bit5op:		fprintf (out, "%s\t", m);
		print_reg (out, cmd & 0x1f);
		fprintf (out, ", 0%xh", cmd >> 5 & 0x1f);
		goto done;

        case 0x20: case 0x21: case 0x22: case 0x23:
	case 0x24: case 0x25: case 0x26: case 0x27:
		fprintf (out, "ldr\t#%c, ", (cmd & 7) + 'a');
		arg = (cmd >> 3) & 0xff;
		if (arg > 9)
			fprintf (out, "0%xh", arg);
		else
			fprintf (out, "%d", arg);
		goto done;
        case 0x02:
		fprintf (out, "mtpr\t#%c, ", ((cmd >> 5) & 7) + 'a');
		print_reg (out, cmd & 0x1f);
		goto done;
        case 0x03:
		fprintf (out, "mfpr\t");
		print_reg (out, cmd & 0x1f);
		fprintf (out, ", #%c", ((cmd >> 5) & 7) + 'a');
		goto done;
	}
	fprintf (out, "?");
	if (cmd >= ' ' && cmd <= '~')
		fprintf (out, "\t\t; `%c'", cmd);
done:
	fprintf (out, "\n");
}

void disasm ()
{
	int i;

	for (i=0; i<TXTSIZE; ++i)
		if (tbusy [i])
			print_command (stdout, 0, i, text [i]);
}

int main (int argc, char **argv)
{
	int i;

	argp_parse (&argp_parser, argc, argv, 0, &i, 0);
	if (! freopen (argv[i], "r", stdin))
		uerror ("cannot open");

	readimage ();
	disasm ();
	return 0;
}
