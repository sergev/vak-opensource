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
#include <ctype.h>
#include <getopt.h>
#include "disassembler.h"

/* Macros for converting between hex and binary. */
#define NIBBLE(x)       (isdigit(x) ? (x)-'0' : tolower(x)+10-'a')
#define HEX(buffer)     ((NIBBLE((buffer)[0])<<4) + NIBBLE((buffer)[1]))

int debug;
char *infile;

unsigned char memory_data [0x20000];   /* Code - up to 128 kbytes */
int memory_len;
unsigned memory_base;

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

/*
 * Read the S record file.
 */
int read_srec (char *filename, unsigned char *output)
{
	FILE *fd;
	unsigned char buf [256];
	unsigned char *data;
	unsigned address;
	int bytes, output_len;

	fd = fopen (filename, "r");
	if (! fd) {
		perror (filename);
		exit (1);
	}
	output_len = 0;
	while (fgets ((char*) buf, sizeof(buf), fd)) {
		if (buf[0] == '\n')
			continue;
		if (buf[0] != 'S') {
			fprintf (stderr, "%s: bad file format\n", filename);
			exit (1);
		}
		if (buf[1] == '7' || buf[1] == '8' || buf[1] == '9')
			break;

		/* Starting an S-record.  */
		if (! isxdigit (buf[2]) || ! isxdigit (buf[3])) {
			fprintf (stderr, "%s: bad record: %s\n", filename, buf);
			exit (1);
		}
		bytes = HEX (buf + 2);

		/* Ignore the checksum byte.  */
		--bytes;

		address = 0;
		data = buf + 4;
		switch (buf[1]) {
		case '3':
			address = HEX (data);
			data += 2;
			--bytes;
			/* Fall through.  */
		case '2':
			address = (address << 8) | HEX (data);
			data += 2;
			--bytes;
			/* Fall through.  */
		case '1':
			address = (address << 8) | HEX (data);
			data += 2;
			address = (address << 8) | HEX (data);
			data += 2;
			bytes -= 2;

			if (! memory_base) {
				/* Автоматическое определение базового адреса. */
				memory_base = address;
			}
			if (address < memory_base) {
				fprintf (stderr, "%s: incorrect address %08X, must be %08X or greater\n",
					filename, address, memory_base);
				exit (1);
			}
			address -= memory_base;
			if (address+bytes > sizeof (memory_data)) {
				fprintf (stderr, "%s: address too large: %08X + %08X\n",
					filename, address + memory_base, bytes);
				exit (1);
			}
			while (bytes-- > 0) {
				output[address++] = HEX (data);
				data += 2;
			}
			if (output_len < (int) address)
				output_len = address;
			break;
		}
	}
	fclose (fd);
	return output_len;
}

unsigned short read_short (unsigned address)
{
//fprintf (stderr, "read_short (%08x)\n", address);
	if (address < memory_base || address > memory_base + memory_len - 2)
		uerror ("address out of range: %08x", address);
	return *(unsigned short*) (memory_data + address - memory_base);
}

int main (int argc, char **argv)
{
	for (;;) {
		switch (getopt (argc, argv, "d")) {
		case EOF:
			break;
		case 'd':
			++debug;
			continue;
		default:
usage:			printf ("ARM Cortex-M3 Disassembler, by Serge V.Vakulenko\n");
			printf ("Usage:\n");
			printf ("        mildis file.srec\n");
			return -1;
		}
		break;
	}
	argc -= optind;
	argv += optind;
	if (argc != 1)
		goto usage;

	infile = argv[0];
        memory_len = read_srec (infile, memory_data);
	if (memory_len < 192)
		uerror ("not enough data");
	if (memory_base != 0x08000000)
		uerror ("invalid base address %08x, expected 08000000", memory_base);

	/* Vectors. */
	unsigned address = memory_base;
	unsigned min_addr = memory_base + 0xc0;
	while (address < min_addr) {
		unsigned word = *(unsigned*) (memory_data + address - memory_base);
		if (word) {
			printf ("0x%08x  0x%08x\n", address, word);
			if (address > 0 && (min_addr == 0 || word < min_addr))
				min_addr = word & ~3;
		} else {
			printf ("0x%08x  --\n", address);
		}
		address += 4;
	}
	printf ("----------\n");

	/* Code. */
	if (min_addr)
		address = min_addr;
	while (address < memory_base + memory_len) {
		struct arm_instruction instr;
		if (thumb2_opcode (address, &instr, read_short) != 0)
			break;

		printf ("%s\n", instr.text);
		address += instr.instruction_size;
	}
	return 0;
}
