/*
 * SPI interface via ch341 chip.
 *
 * Copyright (C) 2018 Serge Vakulenko
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
#ifndef __CH341_H__
#define __CH341_H__

int spi_open(void);
int spi_close(void);
int spi_set_speed(int speed);
int spi_send_receive(uint8_t *out, uint8_t *in, int len);
int spi_send(uint8_t *out, int len);

#endif
