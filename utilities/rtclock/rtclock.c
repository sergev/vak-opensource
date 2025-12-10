/*
 * rtcclock.exe - get/set DOS time and date from MM58167 RTC chip.
 * Copyright (c) 2025 Serge Vakulenko
 *
 * Based on NetBSD mm58167 driver code, adapted for Turbo C++ 3.0 on MS-DOS.
 * This code is derived from software contributed to The NetBSD Foundation
 * by Matthew Fredette.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Register addresses (A4-A0 bits) - these form lower 5 bits of I/O port
 */
#define MM58167_MSEC   0x2c0 /* Milliseconds (port 0x2C0) */
#define MM58167_CSEC   0x2c1 /* Hundredths/Tenths of Seconds (port 0x2C1) */
#define MM58167_SEC    0x2c2 /* Seconds (port 0x2C2) */
#define MM58167_MIN    0x2c3 /* Minutes (port 0x2C3) */
#define MM58167_HOUR   0x2c4 /* Hours (port 0x2C4) */
#define MM58167_WDAY   0x2c5 /* Day of Week (port 0x2C5) */
#define MM58167_DAY    0x2c6 /* Day of Month (port 0x2C6) */
#define MM58167_MON    0x2c7 /* Month (port 0x2C7) */
#define MM58167_STATUS 0x2d4 /* Status Bit (port 0x2D4) */
#define MM58167_GO     0x2d5 /* GO Command (port 0x2D5) */

/*
 * BCD conversion macros
 */
#define FROMBCD(x) (((x) & 0x0F) + ((((x) >> 4) & 0x0F) * 10))
#define TOBCD(x)   ((((x) / 10) << 4) | ((x) % 10))

/*
 * Get time from RTC chip
 */
static int mm58167_gettime(int *month, int *day, int *hour, int *min, int *sec)
{
    unsigned char status;
    int retries = 100;

    /*
     * Read the date values until we get a coherent read (one
     * where the status stays zero, indicating no increment was
     * rippling through while we were reading).
     * Following NetBSD logic: loop while status bit is 0 (which seems
     * inverted from comment, but matches NetBSD code behavior).
     */
    do {
        /* Reset status bit by reading it first */
        (void)inportb(MM58167_STATUS);

        /* Read registers in order */
        *month = FROMBCD(inportb(MM58167_MON) & 0x1F); /* bits D4-D0, max 1 */
        *day   = FROMBCD(inportb(MM58167_DAY) & 0x3F); /* bits D5-D0, max 3 */
        *hour  = FROMBCD(inportb(MM58167_HOUR) & 0x3F); /* bits D5-D0, max 2 */
        *min   = FROMBCD(inportb(MM58167_MIN) & 0x7F); /* bits D6-D0, max 5 */
        *sec   = FROMBCD(inportb(MM58167_SEC) & 0x7F); /* bits D6-D0, max 5 */

        /* Check status bit - NetBSD code loops while status & 1 == 0 */
        status = inportb(MM58167_STATUS);

        /* Limit retries to avoid infinite loop */
        if (--retries <= 0) {
            return -1; /* Timeout */
        }
    } while ((status & 1) == 0);

    /* Validate ranges */
    if (*month < 1 || *month > 12 || *day < 1 || *day > 31 || *hour > 23 ||
        *min > 59 || *sec > 59) {
        return -1;
    }

    return 0;
}

/*
 * Set time to RTC chip
 */
static void mm58167_settime(int month, int day, int hour, int min, int sec)
{
    /*
     * Issue a GO command to reset everything less significant
     * than the minutes to zero.
     */
    outportb(MM58167_GO, 0xFF);

    /* Load everything. Values are in BCD format. */
    outportb(MM58167_MON, TOBCD(month) & 0x1F);
    outportb(MM58167_DAY, TOBCD(day) & 0x3F);
    outportb(MM58167_HOUR, TOBCD(hour) & 0x3F);
    outportb(MM58167_MIN, TOBCD(min) & 0x7F);
    outportb(MM58167_SEC, TOBCD(sec) & 0x7F);
}

/*
 * Print usage message
 */
static void usage()
{
    printf("Usage:\n");
    printf("    rtclock get YYYY\n");
    printf("    rtclock set\n");
    printf("Options:\n");
    printf("    get YYYY  - Read time from RTC and set DOS date/time\n");
    printf("                Here YYYY is the current year\n");
    printf("                (required, not stored in RTC)\n");
    printf("    set       - Write current DOS date/time to RTC\n");
}

int main(int argc, char **argv)
{
    struct date dos_date;
    struct time dos_time;
    int month, day, hour, min, sec;
    int year;

    if (argc < 2) {
        usage();
        return 1;
    }

    if (strcmp(argv[1], "get") == 0) {
        /* Get command: read RTC and set DOS date/time */
        if (argc != 3) {
            fprintf(stderr, "Error: Year required for 'get' command\n");
            usage();
            return 1;
        }

        year = atoi(argv[2]);
        if (year < 1980 || year > 2099) {
            fprintf(stderr, "Error: Year must be between 1980 and 2099\n");
            return 1;
        }

        /* Read time from RTC */
        if (mm58167_gettime(&month, &day, &hour, &min, &sec) != 0) {
            fprintf(stderr, "Error: Failed to read time from RTC\n");
            return 1;
        }

        /* Convert to DOS date structure */
        dos_date.da_year = year;
        dos_date.da_mon  = month;
        dos_date.da_day  = day;

        /* Convert to DOS time structure */
        dos_time.ti_hour = hour;
        dos_time.ti_min  = min;
        dos_time.ti_sec  = sec;
        dos_time.ti_hund = 0; /* Hundredths not available from RTC */

        /* Set DOS date and time */
        setdate(&dos_date);
        settime(&dos_time);

        printf("Get time from RTC: %04d-%02d-%02d %02d:%02d:%02d\n",
               year, month, day, hour, min, sec);

    } else if (strcmp(argv[1], "set") == 0) {
        /* Set command: write date/time to RTC */
        if (argc != 2) {
            usage();
            return 1;
        }

        /* Get current DOS date and time */
        getdate(&dos_date);
        gettime(&dos_time);

        /* Write to RTC */
        mm58167_settime(dos_date.da_mon, dos_date.da_day, dos_time.ti_hour,
                        dos_time.ti_min, dos_time.ti_sec);

        printf("Set RTC time from DOS: %04d-%02d-%02d %02d:%02d:%02d\n",
               dos_date.da_year, dos_date.da_mon, dos_date.da_day,
               dos_time.ti_hour, dos_time.ti_min, dos_time.ti_sec);
    } else {
        fprintf(stderr, "Error: Unknown command '%s'\n", argv[1]);
        usage();
        return 1;
    }
    return 0;
}
