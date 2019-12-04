/*
 * Demo of fonts on OLED display.
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
#include "oled.h"

#define PIN_LED     2   // TTGO LoRa32 board
#define PIN_BUTTON  0

/*
 * Screen size.
 */
int xsize, ysize;

extern oled_font_t font_lucidasans7;
extern oled_font_t font_lucidasans9;
extern oled_font_t font_lucidasans11;
extern oled_font_t font_lucidasans15;
extern oled_font_t font_lucidasans28;
extern oled_font_t font_verdana7;
extern oled_font_t font_5x7;
extern oled_font_t font_6x9;
extern oled_font_t font_digits20;
extern oled_font_t font_digits32;

void setup_pins()
{
    // LED pin: output.
    gpio_pad_select_gpio(PIN_LED);
    gpio_set_direction(PIN_LED, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_LED, 0);

    // User button pin: input.
    gpio_pad_select_gpio(PIN_BUTTON);
    gpio_set_direction(PIN_BUTTON, GPIO_MODE_INPUT);
    gpio_set_pull_mode(PIN_BUTTON, GPIO_PULLUP_ONLY);
}

void show(const oled_font_t *font, const char *title, int digits_only)
{
    int x = 0, y = 0, i;
    const char *phrase = digits_only ? "0123456789" :
                         "The quick brown fox jumps over the lazy dog.";

    oled_clear(0);
    oled_text(&font_lucidasans11, 1, 0, x, y, title);
    y += font_lucidasans11.height;

    for (i=0; y<ysize; i++) {
        oled_text(font, 1, 0, x, y, phrase);
        y += font->height;
    }
    oled_update();

    // Wait for user button pressed (active low).
    while (gpio_get_level(PIN_BUTTON))
        vTaskDelay(1);
    gpio_set_level(PIN_LED, 1);

    // Wait until user button released.
    while (!gpio_get_level(PIN_BUTTON))
        vTaskDelay(1);
    gpio_set_level(PIN_LED, 0);
}

void app_main()
{
    printf("Draw fonts.\n");
    setup_pins();

    if (oled_init(0, 0, &xsize, &ysize) < 0)
        return;

    for (;;) {
        printf("Screen size %u x %u.\n", xsize, ysize);

        show(&font_lucidasans7, "Lucida Sans 7", 0);
        show(&font_lucidasans9, "Lucida Sans 9", 0);
        show(&font_lucidasans11, "Lucida Sans 11", 0);
        show(&font_lucidasans15, "Lucida Sans 15", 0);
        show(&font_lucidasans28, "Lucida Sans 28", 0);
        show(&font_verdana7, "Verdana 7", 0);
        show(&font_5x7, "5x7", 0);
        show(&font_6x9, "6x9", 0);
        show(&font_digits20, "Digits 20", 1);
        show(&font_digits32, "Digits 32", 1);
    }
}
