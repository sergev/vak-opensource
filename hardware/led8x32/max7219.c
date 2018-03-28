/*
 * Interface to LED 8x32 matrix with MAX7219 controller.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include <stdint.h>
#include "max7219.h"
#include "ch341a.h"

//
// Opcodes for MAX7219
//
#define OP_NOOP         0   // No-Op register
#define OP_DIGIT0       1   // Digit 0
#define OP_DIGIT1       2   // Digit 1
#define OP_DIGIT2       3   // Digit 2
#define OP_DIGIT3       4   // Digit 3
#define OP_DIGIT4       5   // Digit 4
#define OP_DIGIT5       6   // Digit 5
#define OP_DIGIT6       7   // Digit 6
#define OP_DIGIT7       8   // Digit 7
#define OP_DECODEMODE   9   // Decode-Mode register
#define OP_INTENSITY    10  // Intensity control
#define OP_SCANLIMIT    11  // Scan-Limit register
#define OP_SHUTDOWN     12  // Shutdown register
#define OP_DISPLAYTEST  15  // Display-Test register

//
// Four max7219 devices in SPI chain.
//
static int led_op(int op, int val)
{
    uint8_t out[8] = { op, val, op, val, op, val, op, val, };

    return spi_send(out, 8);
}

int led_write_row(int row, uint8_t data[4])
{
    uint8_t out[8] = { row, data[0], row, data[1], row, data[2], row, data[3], };

    return spi_send(out, 8);
}

int led_clear()
{
    uint8_t zero[] = { 0, 0, 0, 0 };

    if (led_write_row(0, zero) < 0)
        return -1;
    if (led_write_row(1, zero) < 0)
        return -1;
    if (led_write_row(2, zero) < 0)
        return -1;
    if (led_write_row(3, zero) < 0)
        return -1;
    if (led_write_row(4, zero) < 0)
        return -1;
    if (led_write_row(5, zero) < 0)
        return -1;
    if (led_write_row(6, zero) < 0)
        return -1;
    if (led_write_row(7, zero) < 0)
        return -1;
    return 0;
}

//
// Initialize the display devices.
//
int led_init()
{
    int status;

    status = spi_open();
    if (status < 0)
        return -1;

    status = spi_set_speed(0);
    if (status < 0)
        return -1;

    status = led_op(OP_DISPLAYTEST, 0);     // no test
    if (status < 0)
        return -1;

    status = led_op(OP_SCANLIMIT, 7);       // scan limit is set to max
    if (status < 0)
        return -1;

    status = led_op(OP_INTENSITY, 31);      // set max intensity
    if (status < 0)
        return -1;

    status = led_op(OP_DECODEMODE, 0);      // ensure no decoding
    if (status < 0)
        return -1;

    status = led_clear();
    if (status < 0)
        return -1;

    status = led_op(OP_SHUTDOWN, 1);        // disable shutdown mode
    if (status < 0)
        return -1;

    return 0;
}

//
// Close the display connection.
//
int led_close()
{
    int status;

    status = spi_close();
    if (status < 0)
        return -1;

    return 0;
}
