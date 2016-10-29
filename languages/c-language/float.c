/*
 * Convert float number from hex to decimal.
 *
 * Copyright (C) 2011 Serge Vakulenko, <serge@vak.ru>
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
#include <errno.h>
#include <getopt.h>

char *progname;

extern char *optarg;
extern int optind;
extern float strtof (const char *nptr, char **endptr);

void usage ()
{
    fprintf (stderr, "Convert float number from hex to decimal\n");
    fprintf (stderr, "Usage:\n");
    fprintf (stderr, "    %s [-d] float-number\n", progname);
    fprintf (stderr, "or:\n");
    fprintf (stderr, "    %s [-d] -h hex-number\n", progname);
    fprintf (stderr, "or:\n");
    fprintf (stderr, "    %s [-d] -t testfloat-hex-number\n", progname);
    fprintf (stderr, "\nExamples:\n");
    fprintf (stderr, "    %s 1.2\n", progname);
    fprintf (stderr, "    %s -d 3.14\n", progname);
    fprintf (stderr, "    %s -h 55555555\n", progname);
    fprintf (stderr, "    %s -d -h 5555555555555555\n", progname);
    fprintf (stderr, "    %s -t 012.345678\n", progname);
    fprintf (stderr, "    %s -d -t 123.456789abcdef0\n", progname);
    exit (-1);
}

float get_testfloat (const char *str)
{
    char *endptr;
    unsigned val, exp;
    float f;

    exp = strtoul (str, &endptr, 16);
    if (*endptr != '.') {
        fprintf (stderr, "%s: bad format\n", endptr);
        exit (-1);
    }
    val = strtoul (endptr+1, 0, 16);
    val |= exp << 23;
    f = *(float*) &val;
    return (exp & 0x800) ? -f : f;
}

double get_testfloat_double (const char *str)
{
    char *endptr;
    unsigned exp;
    unsigned long long val;

    exp = strtoul (str, &endptr, 16);
    if (*endptr != '.') {
        fprintf (stderr, "%s: bad format\n", str);
        exit (-1);
    }
    val = strtoull (endptr+1, 0, 16);
    val |= (unsigned long long) exp << 52;
    return *(double*) &val;
}

int main (int argc, char **argv)
{
    int use_double = 0;
    int from_hex = 0;
    int testfloat_format = 0;
    char *endptr = "";
    float f;
    double d;
    unsigned short *sp;

    progname = strrchr (*argv, '/');
    if (progname)
        progname++;
    else
        progname = *argv;

    for (;;) {
        switch (getopt (argc, argv, "hdt")) {
        case EOF:
            break;
        case 'h':
            ++from_hex;
            continue;
        case 'd':
            ++use_double;
            continue;
        case 't':
            ++testfloat_format;
            continue;
        default:
            usage ();
        }
        break;
    }
    argc -= optind;
    argv += optind;

    if (argc != 1)
        usage ();

    errno = 0;
    if (use_double) {
        if (from_hex)
            *(long long*) &d = strtoull (argv[0], &endptr, 16);
        else if (testfloat_format)
            d = get_testfloat_double (argv[0]);
        else
            d = strtod (argv[0], &endptr);
        sp = (unsigned short*) &d;
    } else {
        if (from_hex)
            *(int*) &f = strtoul (argv[0], &endptr, 16);
        else if (testfloat_format)
            f = get_testfloat (argv[0]);
        else
            f = strtof (argv[0], &endptr);
        sp = (unsigned short*) &f;
    }

    if (errno != 0) {
        perror (argv[0]);
        return -1;
    }
    if (*endptr != 0) {
        fprintf (stderr, "%s: bad format\n", argv[0]);
        return -1;
    }

    if (use_double) {
        printf ("%.17g = %a = <%04x %04x %04x %04x>\n", d, d, sp[3], sp[2], sp[1], sp[0]);
    } else {
        printf ("%.8g = %a = <%04x %04x>\n", f, f, sp[1], sp[0]);
    }
    return (0);
}
