/*
 * Print decimal floating-point values.
 *
 * Using boost::decimal library from https://github.com/cppalliance/decimal.
 *
 * Copyright (C) 2024 Serge Vakulenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <boost/decimal.hpp>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    MODE_FLOAT,
    MODE_HEXINT,
} float_mode_t;

float_mode_t mode;

void usage()
{
    fprintf(stderr, "Show a decimal floating-point number in different formats\n");
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "    decimal32 number\n");
    fprintf(stderr, "    decimal64 number\n");
    fprintf(stderr, "    decimal128 number\n");
    fprintf(stderr, "\nNumbers:\n");
    fprintf(stderr, "    [+-]DDD.DDDe[+-]DD    - decimal floating-point literal\n");
    fprintf(stderr, "    0xNNNN                - hexadecimal image\n");
    fprintf(stderr, "\nExamples:\n");
    fprintf(stderr, "    decimal32 1.2\n");
    fprintf(stderr, "    decimal64 3.14\n");
    fprintf(stderr, "    decimal32 0x55555555\n");
    fprintf(stderr, "    decimal64 0x5555555555555555\n");
    exit(-1);
}

__uint128_t strtou128(const char *str, char **endptr)
{
    __uint128_t val = 0;

    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        str += 2;
    }

    // Convert digits
    while (*str) {
        int digit;

        // Handle decimal digits (0-9)
        if (*str >= '0' && *str <= '9') {
            digit = *str - '0';
        } else {
            // Handle letters based on base (a-z or A-Z)
            if (*str >= 'A' && *str <= 'Z') {
                digit = *str - 'A' + 10;
            } else {
                digit = *str - 'a' + 10;
            }
            if (digit >= 16) {
                break; // Invalid digit
            }
        }

        val = val * 16 + digit;
        str++;
    }

    // Set endptr if provided
    if (endptr) {
        *endptr = (char*) str;
    }
    return val;
}

//
// Prefix 0x, but no pN suffix - mode HEXINT.
//
float_mode_t detect_mode(const char *input)
{
    if (strcasestr(input, "0x") != NULL && strcasestr(input, "p") == NULL) {
        return MODE_HEXINT;
    }
    return MODE_FLOAT;
}

void print_decimal64(const char *input)
{
    union {
        uint64_t uns64;
        boost::decimal::decimal64 float64{};
        uint16_t uns16[4];
    } d;
    char *endptr = NULL;

    errno = 0;
    switch (mode) {
    case MODE_HEXINT:
        d.uns64 = strtoull(input, &endptr, 16);
        break;
    default:
        d.float64 = boost::decimal::strtod64(input, &endptr);
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

    boost::decimal::printf("%Dg = <%04x %04x %04x %04x>\n", d.float64, d.uns16[3], d.uns16[2],
           d.uns16[1], d.uns16[0]);
}

void print_decimal128(const char *input)
{
    union {
        __uint128_t uns128;
        boost::decimal::decimal128 float128{};
        uint16_t uns16[8];
    } d;
    char *endptr = NULL;

    errno = 0;
    switch (mode) {
    case MODE_HEXINT:
        d.uns128 = strtou128(input, &endptr);
        break;
    default:
        d.float128 = boost::decimal::strtod128(input, &endptr);
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

    boost::decimal::printf("%DDg = <%04x %04x %04x %04x %04x %04x %04x %04x>\n", d.float128,
           d.uns16[7], d.uns16[6], d.uns16[5], d.uns16[4], d.uns16[3], d.uns16[2], d.uns16[1],
           d.uns16[0]);
}

void print_decimal32(const char *input)
{
    union {
        uint32_t uns32;
        boost::decimal::decimal32 float32{};
        uint16_t uns16[2];
    } f;
    char *endptr = NULL;

    errno = 0;
    switch (mode) {
    case MODE_HEXINT:
        f.uns32 = strtoul(input, &endptr, 16);
        break;
    default:
        f.float32 = boost::decimal::strtod32(input, &endptr);
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

    boost::decimal::printf("%Hg = <%04x %04x>\n", f.float32, f.uns16[1], f.uns16[0]);
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
    if (strcmp("decimal32", progname) == 0) {
        print_decimal32(argv[0]);

    } else if (strcmp("decimal64", progname) == 0) {
        print_decimal64(argv[0]);

    } else if (strcmp("decimal128", progname) == 0) {
        print_decimal128(argv[0]);

    } else {
        fprintf(stderr, "%s: bad program name\n", progname);
        exit(-1);
    }
    return 0;
}
