/*
 * Author: Serge Vakulenko <vak@besm6.org>
 * Copyright (c) 2017 Serge Vakulenko.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>

#include "mraa.h"

//
// RGB LED at Omega2 Expansion Dock
//
#define DEFAULT_PIN_RED     4
#define DEFAULT_PIN_GREEN   5
#define DEFAULT_PIN_BLUE    6

int running = 0;

void
sig_handler(int signo)
{
    if (signo == SIGINT) {
        printf("\nClosing IO nicely.\n");
        running = -1;
    }
}

void
blink(mraa_gpio_context gpio)
{
    mraa_result_t r = mraa_gpio_write(gpio, 0);
    if (r != MRAA_SUCCESS) {
        mraa_result_print(r);
    }

    usleep(233000);

    r = mraa_gpio_write(gpio, 1);
    if (r != MRAA_SUCCESS) {
        mraa_result_print(r);
    }

    usleep(100000);
}

int
main(int argc, char** argv)
{
    mraa_result_t r = MRAA_SUCCESS;
    int pin_red   = DEFAULT_PIN_RED;
    int pin_green = DEFAULT_PIN_GREEN;
    int pin_blue  = DEFAULT_PIN_BLUE;

    if (argc == 4) {
        pin_red   = strtol(argv[1], NULL, 10);
        pin_green = strtol(argv[1], NULL, 10);
        pin_blue  = strtol(argv[1], NULL, 10);
    } else if (argc == 1) {
        printf("Provide three int args if you want to flash on something other than %d %d %d\n",
            DEFAULT_PIN_RED, DEFAULT_PIN_GREEN, DEFAULT_PIN_BLUE);
    } else {
        printf("Usage:\n");
        printf("    %s [ red green blue ]\n");
        exit(1);
    }

    mraa_init();
    fprintf(stdout, "MRAA Version: %s\nStarting Blinking on pins %d, %d, %d\n",
        mraa_get_version(), pin_red, pin_green, pin_blue);

    mraa_gpio_context gpio_red   = mraa_gpio_init(pin_red);
    mraa_gpio_context gpio_green = mraa_gpio_init(pin_green);
    mraa_gpio_context gpio_blue  = mraa_gpio_init(pin_blue);
    if (gpio_red == NULL) {
        fprintf(stderr, "Are you sure that pin%d you requested is valid on your platform?", pin_red);
        exit(1);
    }
    if (gpio_green == NULL) {
        fprintf(stderr, "Are you sure that pin%d you requested is valid on your platform?", pin_green);
        exit(1);
    }
    if (gpio_blue == NULL) {
        fprintf(stderr, "Are you sure that pin%d you requested is valid on your platform?", pin_blue);
        exit(1);
    }
    printf("Initialised pins %d, %d, %d\n", pin_red, pin_green, pin_blue);

    // set direction to OUT
    r = mraa_gpio_dir(gpio_red, MRAA_GPIO_OUT);
    if (r != MRAA_SUCCESS) {
        mraa_result_print(r);
    }
    r = mraa_gpio_dir(gpio_green, MRAA_GPIO_OUT);
    if (r != MRAA_SUCCESS) {
        mraa_result_print(r);
    }
    r = mraa_gpio_dir(gpio_blue, MRAA_GPIO_OUT);
    if (r != MRAA_SUCCESS) {
        mraa_result_print(r);
    }

    signal(SIGINT, sig_handler);

    while (running == 0) {
        blink(gpio_red);
        blink(gpio_green);
        blink(gpio_blue);
        printf("Blink...\n");
    }

    r = mraa_gpio_close(gpio_red);
    if (r != MRAA_SUCCESS) {
        mraa_result_print(r);
    }
    r = mraa_gpio_close(gpio_green);
    if (r != MRAA_SUCCESS) {
        mraa_result_print(r);
    }
    r = mraa_gpio_close(gpio_blue);
    if (r != MRAA_SUCCESS) {
        mraa_result_print(r);
    }

    return r;
}
