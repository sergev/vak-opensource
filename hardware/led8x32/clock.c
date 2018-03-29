/*
 * Demo of LED 8x32 matrix (with max7219 controller).
 *
 * Copyright (C) 2018 Serge Vakulenko
 * Based on ch341 sources by Pluto Yang (yangyj.ee@gmail.com).
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "max7219.h"

#define ROW(a,b,c,d) (a<<3 | b<<2 | c<<1 | d)
#define _ 0
#define O 1

const unsigned char digits[] = {
	ROW (_,O,O,_),
	ROW (O,_,_,O),
	ROW (O,_,_,O),
	ROW (O,_,_,O),
	ROW (O,_,_,O),
	ROW (O,_,_,O),
	ROW (O,_,_,O),
	ROW (_,O,O,_),

	ROW (_,_,O,_),
	ROW (_,O,O,_),
	ROW (O,_,O,_),
	ROW (_,_,O,_),
	ROW (_,_,O,_),
	ROW (_,_,O,_),
	ROW (_,_,O,_),
	ROW (_,O,O,O),

	ROW (O,O,O,_),
	ROW (_,_,_,O),
	ROW (_,_,_,O),
	ROW (_,_,_,O),
	ROW (_,_,O,_),
	ROW (_,O,_,_),
	ROW (O,_,_,_),
	ROW (O,O,O,O),

	ROW (O,O,O,_),
	ROW (_,_,_,O),
	ROW (_,_,_,O),
	ROW (_,O,O,_),
	ROW (_,_,_,O),
	ROW (_,_,_,O),
	ROW (O,_,_,O),
	ROW (_,O,O,_),

	ROW (O,_,_,_),
	ROW (O,_,_,O),
	ROW (O,_,_,O),
	ROW (O,_,_,O),
	ROW (O,O,O,O),
	ROW (_,_,_,O),
	ROW (_,_,_,O),
	ROW (_,_,_,O),

	ROW (O,O,O,O),
	ROW (O,_,_,_),
	ROW (O,_,_,_),
	ROW (O,O,O,_),
	ROW (_,_,_,O),
	ROW (_,_,_,O),
	ROW (O,_,_,O),
	ROW (_,O,O,_),

	ROW (_,O,O,O),
	ROW (O,_,_,_),
	ROW (O,_,_,_),
	ROW (O,O,O,_),
	ROW (O,_,_,O),
	ROW (O,_,_,O),
	ROW (O,_,_,O),
	ROW (_,O,O,_),

	ROW (O,O,O,O),
	ROW (O,_,_,O),
	ROW (_,_,_,O),
	ROW (_,_,O,_),
	ROW (_,O,_,_),
	ROW (_,O,_,_),
	ROW (_,O,_,_),
	ROW (_,O,_,_),

	ROW (_,O,O,_),
	ROW (O,_,_,O),
	ROW (O,_,_,O),
	ROW (_,O,O,_),
	ROW (O,_,_,O),
	ROW (O,_,_,O),
	ROW (O,_,_,O),
	ROW (_,O,O,_),

	ROW (_,O,O,_),
	ROW (O,_,_,O),
	ROW (O,_,_,O),
	ROW (O,_,_,O),
	ROW (_,O,O,O),
	ROW (_,_,_,O),
	ROW (_,_,_,O),
	ROW (O,O,O,_),
};

//
// Return a pointer to a glyph of the specified digit.
//
const unsigned char *digit_glyph(int n)
{
    return &digits[n * 8];
}

//
// Copy glyph to the bitmap.
//
void copy_glyph(long bitmap[], const unsigned char *glyph, int xoff)
{
    int i;

    for (i=0; i<8; i++) {
        bitmap[i] |= glyph[i] << (32 - 4 - xoff);
    }
}

int update_time(time_t sec)
{
    struct tm *tm = localtime(&sec);
    long bitmap[8] = {0};
    int i;

    copy_glyph(bitmap, digit_glyph(tm->tm_hour/10), 0);
    copy_glyph(bitmap, digit_glyph(tm->tm_hour%10), 5);
    copy_glyph(bitmap, digit_glyph(tm->tm_min/10), 11);
    copy_glyph(bitmap, digit_glyph(tm->tm_min%10), 16);
    copy_glyph(bitmap, digit_glyph(tm->tm_sec/10), 22);
    copy_glyph(bitmap, digit_glyph(tm->tm_sec%10), 27);

    for (i=0; i<8; i++) {
        if (led_write(i, bitmap[i]) < 0)
            return -1;
    }

    //printf("%02d:%02d:%02d\n", tm->tm_hour, tm->tm_min, tm->tm_sec);
    return 0;
}

int main(int argc, char *argv[])
{
    struct timeval tv;
    time_t last_sec = 0;

    if (led_init() < 0) {
        goto out;
    }

    for (;;) {
        gettimeofday(&tv, 0);
        if (tv.tv_sec != last_sec) {
            last_sec = tv.tv_sec;
            if (update_time(last_sec) < 0)
                break;
        }

        // Sleep for 20 msec.
        usleep(20000);
    }

out:
    led_close();
    return 0;
}
