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
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <math.h>

typedef enum {
    MODE_FLOAT,
    MODE_HEXINT,
} float_mode_t;

float_mode_t mode;

void usage()
{
    fprintf(stderr, "Show a floating-point number in different formats\n");
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "    float number\n");
    fprintf(stderr, "    double number\n");
    fprintf(stderr, "    bfloat16 number\n");
    fprintf(stderr, "\nNumbers:\n");
    fprintf(stderr, "    [+-]DDD.DDDe[+-]DD    - decimal floating-point literal\n");
    fprintf(stderr, "    0xNNNN                - hexadecimal image\n");
    fprintf(stderr, "    [+-]0xHHH.HHHp[+-]DD  - hexfloat literal, as in C++17\n");
    fprintf(stderr, "\nExamples:\n");
    fprintf(stderr, "    float 1.2\n");
    fprintf(stderr, "    double 3.14\n");
    fprintf(stderr, "    bfloat16 -3.14\n");
    fprintf(stderr, "    float 0x55555555\n");
    fprintf(stderr, "    double 0x5555555555555555\n");
    fprintf(stderr, "    bfloat16 0x5555\n");
    fprintf(stderr, "    float 0x1.fp127\n");
    fprintf(stderr, "    double 0xa.bp-1000\n");
    fprintf(stderr, "    bfloat16 -0xffp120\n");
    exit(-1);
}

//
// Prefix 0x, but no pN suffix - mode HEXINT.
//
float_mode_t detect_mode(const char *input)
{
    if (strcasestr(input, "0x") != NULL &&
        strcasestr(input, "p") == NULL) {
        return MODE_HEXINT;
    }
    return MODE_FLOAT;
}

void print_double(const char *input)
{
    union {
        uint64_t uns64;
        double float64;
        uint16_t uns16[4];
    } d;
    char *endptr = "";

    errno = 0;
    switch (mode) {
    case MODE_HEXINT:
        d.uns64 = strtoull(input, &endptr, 16);
        break;
    default:
        d.float64 = strtod(input, &endptr);
        break;
    }

    if (errno != 0) {
        perror(input);
        exit(-1);
    }
    if (*endptr != 0) {
        fprintf(stderr, "%s: bad format\n", input);
        exit(-1);
    }

    printf("%.17g = %a = <%04x %04x %04x %04x>\n", d.float64, d.float64, d.uns16[3], d.uns16[2], d.uns16[1], d.uns16[0]);
}

void print_float(const char *input)
{
    union {
        uint32_t uns32;
        float float32;
        uint16_t uns16[2];
    } f;
    char *endptr = "";

    errno = 0;
    switch (mode) {
    case MODE_HEXINT:
        f.uns32 = strtoul(input, &endptr, 16);
        break;
    default:
        f.float32 = strtof(input, &endptr);
        break;
    }

    if (errno != 0) {
        perror(input);
        exit(-1);
    }
    if (*endptr != 0) {
        fprintf(stderr, "%s: bad format\n", input);
        exit(-1);
    }

    printf("%.8g = %a = <%04x %04x>\n", f.float32, f.float32, f.uns16[1], f.uns16[0]);
}

//
// Round value to a valid bfloat16 value.
//
float round_to_bfloat16(float input)
{
    int exponent;
    float mantissa = frexpf(input, &exponent);

    if (exponent < -125) {
        fprintf(stderr, "%a: denormal\n", input);
        exit(-1);
    }

    // Round half away from zero.
    if (mantissa > 0) {
        mantissa += 0x0.008p0;
    } else if (mantissa < 0) {
        mantissa -= 0x0.008p0;
    }

    return ldexpf(mantissa, exponent);
}

void print_bfloat16(const char *input)
{
    union {
        uint32_t uns32;
        float float32;
        uint16_t uns16[2];
    } f;
    char *endptr = "";

    errno = 0;
    switch (mode) {
    case MODE_HEXINT:
        f.uns32 = strtoul(input, &endptr, 16) << 16;
        break;
    default:
        f.float32 = round_to_bfloat16(strtof(input, &endptr));
        break;
    }

    if (errno != 0) {
        perror(input);
        exit(-1);
    }
    if (*endptr != 0) {
        fprintf(stderr, "%s: bad format\n", input);
        exit(-1);
    }

    f.uns16[0] = 0;
    printf("%.8g = %a = <%04x>\n", f.float32, f.float32, f.uns16[1]);
}

int main(int argc, char **argv)
{
    //
    // Get program name.
    //
    char *progname = strrchr(*argv, '/');
    if (progname) {
        progname++;
    } else {
        progname = *argv;
    }
    argc--;
    argv++;

    //
    // Get options.
    //
    if (argc != 1) {
        usage();
    }

    mode = detect_mode(argv[0]);

    //
    // Switch based on program name.
    //
    if (strcmp("float", progname) == 0) {
        print_float(argv[0]);

    } else if (strcmp("double", progname) == 0) {
        print_double(argv[0]);

    } else if (strcmp("bfloat16", progname) == 0) {
        print_bfloat16(argv[0]);

    } else {
        fprintf(stderr, "%s: bad program name\n", progname);
        exit(-1);
    }
    return 0;
}
