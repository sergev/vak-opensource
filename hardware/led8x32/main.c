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

    led_write(0, 0b00111000000100000111110001111100);
    led_write(1, 0b01000100001100001000001010000010);
    led_write(2, 0b10100010010100000000001000000010);
    led_write(3, 0b10010010000100000011110000111100);
    led_write(4, 0b10001010000100000100000000000010);
    led_write(5, 0b10000010000100001000000010000010);
    led_write(6, 0b01000100000100001000000010000010);
    led_write(7, 0b00111000011111001111111001111100);

out:
    led_close();
    return 0;
}
