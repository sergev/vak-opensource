/*
 * GPIO interface for PIC32.
 *
 * Copyright (C) 2019 Serge Vakulenko
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. The name of the author may not be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <stddef.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "gpio.h"

struct gpioreg {
    volatile unsigned ansel;        // Analog select
    volatile unsigned anselclr;
    volatile unsigned anselset;
    volatile unsigned anselinv;
    volatile unsigned tris;         // Mask of inputs
    volatile unsigned trisclr;
    volatile unsigned trisset;
    volatile unsigned trisinv;
    volatile unsigned port;         // Read inputs, write outputs
    volatile unsigned portclr;
    volatile unsigned portset;
    volatile unsigned portinv;
    volatile unsigned lat;          // Read/write outputs
    volatile unsigned latclr;
    volatile unsigned latset;
    volatile unsigned latinv;
    volatile unsigned odc;          // Open drain configuration
    volatile unsigned odcclr;
    volatile unsigned odcset;
    volatile unsigned odcinv;
    volatile unsigned cnpu;         // Input pin pull-up enable
    volatile unsigned cnpuclr;
    volatile unsigned cnpuset;
    volatile unsigned cnpuinv;
    volatile unsigned cnpd;         // Input pin pull-down enable
    volatile unsigned cnpdclr;
    volatile unsigned cnpdset;
    volatile unsigned cnpdinv;
    volatile unsigned cncon;        // Interrupt-on-change control
    volatile unsigned cnconclr;
    volatile unsigned cnconset;
    volatile unsigned cnconinv;
    volatile unsigned cnen;         // Input change interrupt enable
    volatile unsigned cnenclr;
    volatile unsigned cnenset;
    volatile unsigned cneninv;
    volatile unsigned cnstat;       // Change notification status
    volatile unsigned cnstatclr;
    volatile unsigned cnstatset;
    volatile unsigned cnstatinv;
    volatile unsigned unused[6*4];
};

int gpio_debug;                     // Debug output
int gpio_mem_fd;                    // Access to /dev/mem
static ptrdiff_t gpio_base;         // GPIO registers mapped here

//
// Get access to GPIO control registers.
// Set gpio_base to a base address of the appropriate page.
//
static void gpio_init()
{
    const int GPIO_ADDR = 0x1f860000;

    // Obtain handle to physical memory
    gpio_mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (gpio_mem_fd < 0) {
        printf("Unable to open /dev/mem: %s\n", strerror(errno));
        exit(-1);
    }

    // Map a page of memory to gpio address
    gpio_base = (ptrdiff_t) mmap(0, 4096, PROT_READ|PROT_WRITE, MAP_SHARED,
        gpio_mem_fd, GPIO_ADDR);
    if (gpio_base < 0) {
        printf("Mmap failed: %s\n", strerror(errno));
        exit(-1);
    }
}

//
// Get pin direction or alternative function.
//
gpio_mode_t gpio_get_mode(int pin)
{
    if (!gpio_base)
        gpio_init();

    // Check output mapping.
    gpio_mode_t mode = gpio_get_output_mapping(pin);
    if (mode)
        return mode;

    // Check input mapping.
    mode = gpio_get_input_mapping(pin);
    if (mode)
        return mode;

    struct gpioreg *reg = (struct gpioreg*) (gpio_base + (pin >> 16));
    uint16_t mask = (uint16_t) pin;

    if (reg->ansel & mask)
        return MODE_ANALOG;

    if (reg->tris & mask)
        return MODE_INPUT;

    return MODE_OUTPUT;
}

//
// Set pin direction or alternative function.
//
int gpio_set_mode(int pin, gpio_mode_t mode)
{
    if (!gpio_base)
        gpio_init();

    struct gpioreg *reg = (struct gpioreg*) (gpio_base + (pin >> 16));
    uint16_t mask = (uint16_t) pin;

    gpio_clear_mapping(pin);
    switch (mode) {
    case MODE_ANALOG:
        // Analog input.
        reg->trisset = mask;
        reg->anselset = mask;
        break;

    case MODE_INPUT:
        // Digital input.
        reg->anselclr = mask;
        reg->trisset = mask;
        break;

    case MODE_OUTPUT:
        // Digital output.
        reg->anselclr = mask;
        reg->trisclr = mask;
        break;

    default:
        // Alternative function.
        reg->trisset = mask;
        reg->anselclr = mask;
        gpio_set_mapping(pin, mode);
        break;
    }
    return 0;
}

//
// Set pull up/down resistors.
//
int gpio_set_pull(int pin, gpio_pull_t pull)
{
    if (!gpio_base)
        gpio_init();

    struct gpioreg *reg = (struct gpioreg*) (gpio_base + (pin >> 16));
    uint16_t mask = (uint16_t) pin;

    switch (pull) {
    case PULL_OFF:
        reg->cnpuclr = mask;
        reg->cnpdclr = mask;
        break;

    case PULL_UP:
        reg->cnpdclr = mask;
        reg->cnpuset = mask;
        break;

    case PULL_DOWN:
        reg->cnpuclr = mask;
        reg->cnpdset = mask;
        break;
    }
    return 0;
}

//
// Read the input value. This can be 0 or 1.
// Return -1 in case of error.
//
int gpio_read(int pin)
{
    if (!gpio_base)
        gpio_init();

    struct gpioreg *reg = (struct gpioreg*) (gpio_base + (pin >> 16));
    uint16_t mask = (uint16_t) pin;

    if (reg->port & mask)
        return 1;

    return 0;
}

//
// Write the output value.
//
int gpio_write(int pin, int value)
{
    if (!gpio_base)
        gpio_init();

    struct gpioreg *reg = (struct gpioreg*) (gpio_base + (pin >> 16));
    uint16_t mask = (uint16_t) pin;

    if (value & 1)
        reg->latset = mask;
    else
        reg->latclr = mask;

    return 0;
}

//
// Toggle the output value.
//
int gpio_toggle(int pin)
{
    if (!gpio_base)
        gpio_init();

    struct gpioreg *reg = (struct gpioreg*) (gpio_base + (pin >> 16));
    uint16_t mask = (uint16_t) pin;

    reg->latinv = mask;

    return 0;
}
