/*
 * Generate fragments of Intel HEX file.
 * Every line has the following format:
 *
 *  : nn aaaa tt dd...dd ss
 *
 *  nn      - number of data bytes
 *
 *  aaaa    - low 16 bits of address
 *
 *  tt      - record type
 *              00 - data
 *              01 - end of file
 *              04 - high 16 bits of address
 *              05 - start address, 32-bit
 *
 *  dd...dd - data, optional
 *
 *  ss      - checksum
 *
 * Copyright (C) 2014 Serge Vakulenko, <serge@vak.ru>
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
const char copyright[] = "Copyright (C) 2014 Serge Vakulenko";

char *progname;
int verbose;
int count = 1;

#define TYPE_DATA   0   // Data record
#define TYPE_EOF    1   // End of file
#define TYPE_ADDR   4   // High 16 bits of address
#define TYPE_START  5   // start address, 32-bit

void usage ()
{
    fprintf (stderr, "HEX generator, Version %s, %s\n", version, copyright);
    fprintf (stderr, "Usage:\n\t%s [-v] [-r count] address value...\n", progname);
    fprintf (stderr, "Options:\n");
    fprintf (stderr, "\t-v\tverbose mode\n");
    fprintf (stderr, "\t-r #\trepeat count\n");
    exit (-1);
}

/*
 * Print one line in Intel HEX format.
 */
void print_hex (unsigned char *data)
{
    unsigned bytes = data[0] + 4;
    unsigned char sum = 0;
    unsigned i;

    printf (":");
    for (i=0; i<bytes; ++i) {
        printf ("%02x", data[i]);
        sum -= data[i];
    }
    printf ("%02x\n", sum);
}

int main (int argc, char **argv)
{
    unsigned char data[128];
    unsigned high;

    progname = *argv;
    for (;;) {
        switch (getopt (argc, argv, "vr:")) {
        case EOF:
            break;
        case 'v':
            ++verbose;
            continue;
        case 'r':
            count = strtol (optarg, 0, 0);
            continue;
        default:
            usage ();
        }
        break;
    }
    argc -= optind;
    argv += optind;

    if (argc < 2 || argc % 2)
        usage ();

    high = 0;
    for (; argc > 0; argc -= 2, argv += 2) {
        unsigned addr = strtoul (argv[0], 0, 16);
        unsigned value = strtoul (argv[1], 0, 16);
        unsigned i;

        if (verbose)
            printf ("\n--- Address = %08x, value = %08x\n",
                addr, value);

        /* High part of address. */
        if ((addr >> 16) != high) {
            data[0] = 2;
            data[1] = 0;
            data[2] = 0;
            data[3] = TYPE_ADDR;
            data[4] = addr >> 24;
            data[5] = addr >> 16;
            print_hex (data);
            high = addr >> 16;
        }
        for (i=0; i<count; i++) {
            /* Data record. */
            data[0] = 4;
            data[1] = addr >> 8;
            data[2] = addr;
            data[3] = TYPE_DATA;
            data[4] = value;
            data[5] = value >> 8;
            data[6] = value >> 16;
            data[7] = value >> 24;
            print_hex (data);
        }
    }

    if (verbose)
        printf ("\n--- Done.\n");

    /* End of file. */
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = TYPE_EOF;
    print_hex (data);

    return (0);
}
