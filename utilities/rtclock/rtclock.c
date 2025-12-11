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
 * Register addresses of MM58167 chip on PC XT bus
 */
#define MM58167_MSEC   0x2c0 /* Milliseconds */
#define MM58167_CSEC   0x2c1 /* Hundredths/Tenths of Seconds */
#define MM58167_SEC    0x2c2 /* Seconds */
#define MM58167_MIN    0x2c3 /* Minutes */
#define MM58167_HOUR   0x2c4 /* Hours */
#define MM58167_WDAY   0x2c5 /* Day of Week */
#define MM58167_DAY    0x2c6 /* Day of Month */
#define MM58167_MON    0x2c7 /* Month */
#define MM58167_STATUS 0x2d4 /* Status Bit */
#define MM58167_GO     0x2d5 /* GO Command */

/*
 * Read byte from port and convert from BCD to decimal.
 */
static int inbyte_bcd(int port)
{
    unsigned char byte = inportb(port);

    return (byte & 0x0F) + 10 * ((byte >> 4) & 0x0F);
}

/*
 * Convert value from decimal to BCD and write byte to port.
 */
static void outbyte_bcd(int port, unsigned char val)
{
    outportb(port, ((val / 10) << 4) | (val % 10));
}

/*
 * Get time from RTC chip
 */
static int rtc_gettime(int *month, int *day, int *hour, int *min, int *sec, int *hund)
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
        *month = inbyte_bcd(MM58167_MON);
        *day   = inbyte_bcd(MM58167_DAY);
        *hour  = inbyte_bcd(MM58167_HOUR);
        *min   = inbyte_bcd(MM58167_MIN);
        *sec   = inbyte_bcd(MM58167_SEC);
        *hund  = inbyte_bcd(MM58167_CSEC);

        /* Check status bit */
        status = inportb(MM58167_STATUS);

        /* Limit retries to avoid infinite loop */
        if (--retries <= 0) {
            return -1; /* Timeout */
        }
    } while ((status & 1) == 0);

    /* Validate ranges */
    if (*month < 1 || *month > 12 || *day < 1 || *day > 31 || *hour > 23 ||
        *min > 59 || *sec > 59 || *hund > 99) {
        return -1;
    }
    return 0;
}

/*
 * Set time to RTC chip
 */
static void rtc_settime(int month, int day, int hour, int min, int sec, int hund)
{
    /*
     * Issue a GO command to reset everything less significant
     * than the minutes to zero.
     */
    outportb(MM58167_GO, 0xFF);

    /* Load everything. Values are in BCD format. */
    outbyte_bcd(MM58167_MON, month);
    outbyte_bcd(MM58167_DAY, day);
    //TODO: WDAY
    outbyte_bcd(MM58167_HOUR, hour);
    outbyte_bcd(MM58167_MIN, min);
    outbyte_bcd(MM58167_SEC, sec);
    outbyte_bcd(MM58167_CSEC, hund);
}

/*
 * Print usage message
 */
static void usage()
{
    printf("Usage:\n");
    printf("    rtclock get YYYY\n");
    printf("    rtclock set\n");
    printf("    rtclock show\n");
    printf("Options:\n");
    printf("    get YYYY  - Read time from RTC and set DOS date/time\n");
    printf("                Here YYYY is the current year\n");
    printf("                (required, not stored in RTC)\n");
    printf("    set       - Write current DOS date/time to RTC\n");
    printf("    show      - Compare time from RTC and from DOS\n");
}

int main(int argc, char **argv)
{
    struct date dos_date;
    struct time dos_time;
    int month, day, hour, min, sec, hund;
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
        if (rtc_gettime(&month, &day, &hour, &min, &sec, &hund) != 0) {
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
        dos_time.ti_hund = hund;

        /* Set DOS date and time */
        setdate(&dos_date);
        settime(&dos_time);

        printf("Get time from RTC: %04d-%02d-%02d %02d:%02d:%02d.%02d\n",
               year, month, day, hour, min, sec, hund);

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
        rtc_settime(dos_date.da_mon, dos_date.da_day, dos_time.ti_hour,
                        dos_time.ti_min, dos_time.ti_sec, dos_time.ti_hund);

        printf("Set RTC time from DOS: %04d-%02d-%02d %02d:%02d:%02d.%02d\n",
               dos_date.da_year, dos_date.da_mon, dos_date.da_day,
               dos_time.ti_hour, dos_time.ti_min, dos_time.ti_sec, dos_time.ti_hund);

    } else if (strcmp(argv[1], "show") == 0) {
        /* Show command: print RTC time versus DOS time */
        if (argc != 2) {
            usage();
            return 1;
        }

        /* Read time from RTC */
        if (rtc_gettime(&month, &day, &hour, &min, &sec, &hund) != 0) {
            fprintf(stderr, "Error: Failed to read time from RTC\n");
            return 1;
        }

        /* Get current DOS date and time */
        getdate(&dos_date);
        gettime(&dos_time);

        printf("Time from RTC: ????-%02d-%02d %02d:%02d:%02d.%02d\n",
               month, day, hour, min, sec, hund);
        printf("Time from DOS: %04d-%02d-%02d %02d:%02d:%02d.%02d\n",
               dos_date.da_year, dos_date.da_mon, dos_date.da_day,
               dos_time.ti_hour, dos_time.ti_min, dos_time.ti_sec, dos_time.ti_hund);
    } else {
        fprintf(stderr, "Error: Unknown command '%s'\n", argv[1]);
        usage();
        return 1;
    }
    return 0;
}
