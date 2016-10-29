/*
 * Utility to disassemble MIPS HEX files.
 *
 * Copyright (C) 2014 Serge Vakulenko, <serge@vak.ru>
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is hereby
 * granted, provided that the above copyright notice appear in all
 * copies and that both that the copyright notice and this
 * permission notice and warranty disclaimer appear in supporting
 * documentation, and that the name of the author not be used in
 * advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * The author disclaim all warranties with regard to this
 * software, including all implied warranties of merchantability
 * and fitness.  In no event shall the author be liable for any
 * special, indirect or consequential damages or any damages
 * whatsoever resulting from loss of use, data or profits, whether
 * in an action of contract, negligence or other tortious action,
 * arising out of or in connection with the use or performance of
 * this software.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

int dflag;

#define FLASHV_BASE     0x9d000000
#define BOOTV_BASE      0x9fc00000
#define FLASHP_BASE     0x1d000000
#define BOOTP_BASE      0x1fc00000
#define FLASH_BYTES     (2048 * 1024)
#define BOOT_BYTES      (80 * 1024)

unsigned char boot_data [BOOT_BYTES];
unsigned char flash_data [FLASH_BYTES];
unsigned char boot_dirty [BOOT_BYTES / 4];
unsigned char flash_dirty [FLASH_BYTES / 4];
unsigned boot_bytes;
unsigned flash_bytes;
unsigned total_bytes;

extern int print_insn_mips (unsigned memaddr,
    unsigned long int word, FILE *stream);

/* Macros for converting between hex and binary. */
#define NIBBLE(x)       (isdigit(x) ? (x)-'0' : tolower(x)+10-'a')
#define HEX(buffer)     ((NIBBLE((buffer)[0])<<4) + NIBBLE((buffer)[1]))

void store_data (unsigned address, unsigned byte)
{
    unsigned offset;

    if (address >= BOOTV_BASE && address < BOOTV_BASE + BOOT_BYTES) {
        /* Boot code, virtual. */
        offset = address - BOOTV_BASE;
        boot_data [offset] = byte;
        boot_dirty [offset/4] = 1;
        boot_bytes++;

    } else if (address >= BOOTP_BASE && address < BOOTP_BASE + BOOT_BYTES) {
        /* Boot code, physical. */
        offset = address - BOOTP_BASE;
        boot_data [offset] = byte;
        boot_dirty [offset/4] = 1;
        boot_bytes++;

    } else if (address >= FLASHV_BASE && address < FLASHV_BASE + FLASH_BYTES) {
        /* Main flash memory, virtual. */
        offset = address - FLASHV_BASE;
        flash_data [offset] = byte;
        flash_dirty [offset/4] = 1;
        flash_bytes++;

    } else if (address >= FLASHP_BASE && address < FLASHP_BASE + FLASH_BYTES) {
        /* Main flash memory, physical. */
        offset = address - FLASHP_BASE;
        flash_data [offset] = byte;
        flash_dirty [offset/4] = 1;
        flash_bytes++;
    } else {
        /* Ignore incorrect data. */
        fprintf (stderr, "%08X: address out of flash memory\n", address);
        return;
    }
    total_bytes++;
}

/*
 * Read HEX file.
 */
void read_hex (char *filename)
{
    FILE *fd;
    unsigned char buf [256], data[16], record_type, sum;
    unsigned address, high;
    int bytes, i;

    fd = fopen (filename, "r");
    if (! fd) {
        perror (filename);
        exit (1);
    }
    memset (boot_data, ~0, sizeof(boot_data));
    memset (flash_data, ~0, sizeof(flash_data));
    memset (boot_dirty, 0, sizeof(boot_dirty));
    memset (flash_dirty, 0, sizeof(flash_dirty));
    flash_bytes = 0;
    boot_bytes = 0;
    total_bytes = 0;

    high = 0;
    while (fgets ((char*) buf, sizeof(buf), fd)) {
        if (buf[0] == '\n')
            continue;
        if (buf[0] != ':') {
            fclose (fd);
            fprintf (stderr, "%s: not a HEX format\n",
                filename);
            exit (1);
        }
        if (! isxdigit (buf[1]) || ! isxdigit (buf[2]) ||
            ! isxdigit (buf[3]) || ! isxdigit (buf[4]) ||
            ! isxdigit (buf[5]) || ! isxdigit (buf[6]) ||
            ! isxdigit (buf[7]) || ! isxdigit (buf[8])) {
            fprintf (stderr, "%s: bad HEX record: %s\n", filename, buf);
            exit (1);
        }
	record_type = HEX (buf+7);
	if (record_type == 1) {
	    /* End of file. */
            break;
        }
	if (record_type == 5) {
	    /* Start address, ignore. */
	    continue;
	}

	bytes = HEX (buf+1);
	if (strlen ((char*) buf) < bytes * 2 + 11) {
            fprintf (stderr, "%s: too short hex line\n", filename);
            exit (1);
        }
	address = high << 16 | HEX (buf+3) << 8 | HEX (buf+5);
        if (address & 3) {
            fprintf (stderr, "%s: odd address\n", filename);
            exit (1);
        }

	sum = 0;
	for (i=0; i<bytes; ++i) {
            data [i] = HEX (buf+9 + i + i);
	    sum += data [i];
	}
	sum += record_type + bytes + (address & 0xff) + (address >> 8 & 0xff);
	if (sum != (unsigned char) - HEX (buf+9 + bytes + bytes)) {
            fprintf (stderr, "%s: bad HEX checksum\n", filename);
            exit (1);
        }

	if (record_type == 4) {
	    /* Extended address. */
            if (bytes != 2) {
                fprintf (stderr, "%s: invalid HEX linear address record length\n",
                    filename);
                exit (1);
            }
	    high = data[0] << 8 | data[1];
	    continue;
	}
	if (record_type != 0) {
            fprintf (stderr, "%s: unknown HEX record type: %d\n",
                filename, record_type);
            exit (1);
        }
        //printf ("%08x: %u bytes\n", address, bytes);
        for (i=0; i<bytes; i++) {
            store_data (address++, data [i]);
        }
    }
    fclose (fd);
}

void disasm (const char *title, unsigned char data[],
    unsigned char dirty[], unsigned base, unsigned nbytes)
{
    unsigned offset;
    int need_separator = 0;
    int words_printed = 0;

    printf ("%s memory:\n", title);
    for (offset=0; offset<nbytes; offset+=4) {
        if (! dirty[offset/4]) {
            need_separator = (words_printed > 0);
            continue;
        }
        if (need_separator) {
            printf ("    ...\t\t...\n");
            need_separator = 0;
        }

        unsigned addr = base + offset;
        unsigned opcode = *(uint32_t*) &data[offset];

        printf ("    %8x:\t%08x", addr, opcode);
        if (! dflag) {
            putchar ('\t');
            print_insn_mips (addr, opcode, stdout);
        }
        putchar ('\n');
        words_printed++;
    }
}

int main (int argc, char **argv)
{
    int ch;

    while ((ch = getopt (argc, argv, "d")) != EOF)
        switch (ch) {
        case 'd':       /* do not disassemble */
            dflag++;
            break;
        default:
usage:      fprintf (stderr, "Usage:\n");
            fprintf (stderr, "  aout [-rd] file...\n");
            fprintf (stderr, "Options:\n");
            fprintf (stderr, "  -d      Do not disassemble\n");
            return (1);
        }
    argc -= optind;
    argv += optind;
    if (argc <= 0)
        goto usage;

    for (; argc-- > 0; argv++) {
        read_hex (*argv);
        printf ("File %s: %u bytes\n", *argv, total_bytes);

        if (boot_bytes > 0)
            disasm ("Boot", boot_data, boot_dirty, BOOTV_BASE, BOOT_BYTES);
        if (flash_bytes > 0)
            disasm ("Flash", flash_data, flash_dirty, FLASHV_BASE, FLASH_BYTES);
    }
    return (0);
}
