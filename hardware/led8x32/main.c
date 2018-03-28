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
#include "max7219.h"

int verbose;

int main(int argc, char* argv[])
{
    if (led_init() < 0) {
        goto out;
    }

    uint8_t data[] = { 0b01010101, 0b10101010, 0b11110000, 0b00001111, };
    if (led_write_row(0, data) < 0) {
        goto out;
    }

    printf("Press ^C to stop\n");
    pause();

out:
    led_close();
    return 0;
}
