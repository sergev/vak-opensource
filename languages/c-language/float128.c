/*
 * Convert float128 number from hex to decimal.
 *
 * Copyright (C) 2019 Serge Vakulenko, <serge@vak.ru>
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
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <getopt.h>
#include <quadmath.h>

char *progname;

extern char *optarg;
extern int optind;
extern float strtof(const char *nptr, char **endptr);

void usage()
{
    fprintf(stderr, "Convert float128 number from hex to decimal\n");
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "    %s float-number\n", progname);
    fprintf(stderr, "or:\n");
    fprintf(stderr, "    %s -h hex-number\n", progname);
    fprintf(stderr, "\nExamples:\n");
    fprintf(stderr, "    %s 1.2\n", progname);
    fprintf(stderr, "    %s -h 12345678abcdef015555aaaacccceeee\n", progname);
    exit(-1);
}

void check_endptr(char *endptr, char *input)
{
    if (errno != 0) {
        perror(input);
        exit(-1);
    }
    if (*endptr != 0) {
        fprintf(stderr, "%s: bad format\n", input);
        exit(-1);
    }
}

int main(int argc, char **argv)
{
    int from_hex = 0;
    char *endptr = "";
    union {
        __float128 float128;
        struct {
            uint64_t lo64;
            uint64_t hi64;
        };
    } val;
    unsigned short *sp;

    progname = strrchr(*argv, '/');
    if (progname)
        progname++;
    else
        progname = *argv;

    for (;;) {
        switch (getopt(argc, argv, "h")) {
        case EOF:
            break;
        case 'h':
            ++from_hex;
            continue;
        default:
            usage();
        }
        break;
    }
    argc -= optind;
    argv += optind;

    if (argc != 1)
        usage();

    errno = 0;
    if (from_hex) {
        int nchars = strlen(argv[0]);
        if (nchars > 16) {
            val.lo64 = strtoull(argv[0] + nchars-16, &endptr, 16);
            check_endptr(endptr, argv[0] + nchars-16);
            argv[0][nchars-16] = 0;
            val.hi64 = strtoull(argv[0], &endptr, 16);
        } else {
            val.lo64 = strtoull(argv[0], &endptr, 16);
            val.hi64 = 0;
        }
    } else {
        val.float128 = strtoflt128(argv[0], &endptr);
    }
    sp = (unsigned short*) &val;

    check_endptr(endptr, argv[0]);

    char buf_g[128], buf_a[128];
    quadmath_snprintf(buf_g, sizeof buf_g, "%.34Qg", val.float128);
    quadmath_snprintf(buf_a, sizeof buf_a, "%Qa", val.float128);
    printf("%s = %s = <%04x %04x %04x %04x %04x %04x %04x %04x>\n",
        buf_g, buf_a, sp[7], sp[6], sp[5], sp[4], sp[3], sp[2], sp[1], sp[0]);
    return 0;
}
