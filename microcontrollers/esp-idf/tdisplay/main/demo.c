/*
 * Demo of fonts on TFT display.
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
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "tft.h"
#include "parrot-image.h"

//
// TTGO T-Display board
//
#define PIN_BACKLIGHT   4   // io4 - backlight
#define PIN_BUTTON1     0   // io0 - button S1
#define PIN_BUTTON2     35  // io35 - button S2

//
// Screen size.
//
int xsize, ysize;

void setup_pins()
{
    // Backlight pin: output.
    gpio_pad_select_gpio(PIN_BACKLIGHT);
    gpio_set_direction(PIN_BACKLIGHT, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_BACKLIGHT, 1);

    // User button pins: input.
    gpio_pad_select_gpio(PIN_BUTTON1);
    gpio_set_direction(PIN_BUTTON1, GPIO_MODE_INPUT);
    gpio_set_pull_mode(PIN_BUTTON1, GPIO_PULLUP_ONLY);

    gpio_pad_select_gpio(PIN_BUTTON2);
    gpio_set_direction(PIN_BUTTON2, GPIO_MODE_INPUT);
    gpio_set_pull_mode(PIN_BUTTON2, GPIO_PULLUP_ONLY);
}

void wait_button(int pin)
{
    // Wait for user button pressed (active low).
    while (gpio_get_level(pin))
        vTaskDelay(1);
    gpio_set_level(PIN_BACKLIGHT, 0);

    // Wait until user button released.
    while (!gpio_get_level(pin))
        vTaskDelay(1);
    gpio_set_level(PIN_BACKLIGHT, 1);
}

void app_main()
{
    printf("Draw fonts.\n");
    setup_pins();

    if (tft_init(0, 0, &xsize, &ysize) < 0)
        return;


    for (;;) {
        printf("Screen size %u x %u.\n", xsize, ysize);

        tft_image(0, 0, parrot_image.width, parrot_image.height,
            parrot_image.data);
        wait_button(PIN_BUTTON1);
    }
}
