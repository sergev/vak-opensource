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

//
// Font collection.
//
extern oled_font_t font_5x7;
extern oled_font_t font_6x9;
extern oled_font_t font_digits20;
extern oled_font_t font_digits32;

extern oled_font_t font_verdana8;
extern oled_font_t font_verdana10;
extern oled_font_t font_verdana12;
extern oled_font_t font_verdana14;
extern oled_font_t font_verdana18;

extern oled_font_t font_tahoma8;
extern oled_font_t font_tahoma10;
extern oled_font_t font_tahoma12;
extern oled_font_t font_tahoma14;
extern oled_font_t font_tahoma18;

extern oled_font_t font_georgia8;
extern oled_font_t font_georgia10;
extern oled_font_t font_georgia12;
extern oled_font_t font_georgia14;
extern oled_font_t font_georgia18;

extern oled_font_t font_dejavusans8;
extern oled_font_t font_dejavusans10;
extern oled_font_t font_dejavusans12;
extern oled_font_t font_dejavusans14;
extern oled_font_t font_dejavusans18;

extern oled_font_t font_dejavusanscond8;
extern oled_font_t font_dejavusanscond10;
extern oled_font_t font_dejavusanscond12;
extern oled_font_t font_dejavusanscond14;
extern oled_font_t font_dejavusanscond18;

extern oled_font_t font_freesans8;
extern oled_font_t font_freesans10;
extern oled_font_t font_freesans12;
extern oled_font_t font_freesans14;
extern oled_font_t font_freesans18;

extern oled_font_t font_liberationsans8;
extern oled_font_t font_liberationsans10;
extern oled_font_t font_liberationsans12;
extern oled_font_t font_liberationsans14;
extern oled_font_t font_liberationsans18;

extern oled_font_t font_liberationserif8;
extern oled_font_t font_liberationserif10;
extern oled_font_t font_liberationserif12;
extern oled_font_t font_liberationserif14;
extern oled_font_t font_liberationserif18;

extern oled_font_t font_ubuntu8;
extern oled_font_t font_ubuntu10;
extern oled_font_t font_ubuntu12;
extern oled_font_t font_ubuntu14;
extern oled_font_t font_ubuntu18;

extern oled_font_t font_helv11;
extern oled_font_t font_helv13;
extern oled_font_t font_helv16;
extern oled_font_t font_helv19;
extern oled_font_t font_helv24;
extern oled_font_t font_helv32;

extern oled_font_t font_times11;
extern oled_font_t font_times13;
extern oled_font_t font_times16;
extern oled_font_t font_times19;
extern oled_font_t font_times24;
extern oled_font_t font_times32;

extern oled_font_t font_lucidasans8;
extern oled_font_t font_lucidasans10;
extern oled_font_t font_lucidasans12;
extern oled_font_t font_lucidasans14;
extern oled_font_t font_lucidasans18;
extern oled_font_t font_lucidasans19;
extern oled_font_t font_lucidasans24;
extern oled_font_t font_lucidasans28;

extern oled_font_t font_lucidabright8;
extern oled_font_t font_lucidabright10;
extern oled_font_t font_lucidabright12;
extern oled_font_t font_lucidabright14;
extern oled_font_t font_lucidabright18;
extern oled_font_t font_lucidabright19;
extern oled_font_t font_lucidabright24;

extern oled_font_t font_courier8;
extern oled_font_t font_courier10;
extern oled_font_t font_courier12;
extern oled_font_t font_courier14;
extern oled_font_t font_courier18;
extern oled_font_t font_courier24;

extern oled_font_t font_helvetica8;
extern oled_font_t font_helvetica10;
extern oled_font_t font_helvetica12;
extern oled_font_t font_helvetica14;
extern oled_font_t font_helvetica18;
extern oled_font_t font_helvetica24;

extern oled_font_t font_times8;
extern oled_font_t font_times10;
extern oled_font_t font_times12;
extern oled_font_t font_times14;
extern oled_font_t font_times18;
extern oled_font_t font_times24;

//
// Screen size.
//
int xsize, ysize;

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
    oled_text(&font_tahoma10, 1, 0, x, y, title);
    y += font_tahoma10.height;

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

        show(&font_lucidasans8, "Lucida Sans 8", 0);
        show(&font_lucidasans10, "Lucida Sans 10", 0);
        show(&font_lucidasans12, "Lucida Sans 12", 0);
        show(&font_lucidasans14, "Lucida Sans 14", 0);
        show(&font_lucidasans18, "Lucida Sans 18", 0);
        show(&font_lucidasans19, "Lucida Sans 19", 0);
        show(&font_lucidasans24, "Lucida Sans 24", 0);
        show(&font_lucidasans28, "Lucida Sans 28", 0);

        show(&font_lucidabright8, "Lucida Bright 8", 0);
        show(&font_lucidabright10, "Lucida Bright 10", 0);
        show(&font_lucidabright12, "Lucida Bright 12", 0);
        show(&font_lucidabright14, "Lucida Bright 14", 0);
        show(&font_lucidabright18, "Lucida Bright 18", 0);
        show(&font_lucidabright19, "Lucida Bright 19", 0);
        show(&font_lucidabright24, "Lucida Bright 24", 0);

        show(&font_tahoma8, "Tahoma 8", 0);
        show(&font_tahoma10, "Tahoma 10", 0);
        show(&font_tahoma12, "Tahoma 12", 0);
        show(&font_tahoma14, "Tahoma 14", 0);
        show(&font_tahoma18, "Tahoma 18", 0);

        show(&font_verdana8, "Verdana 8", 0);
        show(&font_verdana10, "Verdana 10", 0);
        show(&font_verdana12, "Verdana 12", 0);
        show(&font_verdana14, "Verdana 14", 0);
        show(&font_verdana18, "Verdana 18", 0);

        show(&font_georgia8, "Georgia 8", 0);
        show(&font_georgia10, "Georgia 10", 0);
        show(&font_georgia12, "Georgia 12", 0);
        show(&font_georgia14, "Georgia 14", 0);
        show(&font_georgia18, "Georgia 18", 0);

        show(&font_dejavusans8, "DejaVu Sans 8", 0);
        show(&font_dejavusans10, "DejaVu Sans 10", 0);
        show(&font_dejavusans12, "DejaVu Sans 12", 0);
        show(&font_dejavusans14, "DejaVu Sans 14", 0);
        show(&font_dejavusans18, "DejaVu Sans 18", 0);

        show(&font_dejavusanscond8, "DejaVu Sans C 8", 0);
        show(&font_dejavusanscond10, "DejaVu Sans C 10", 0);
        show(&font_dejavusanscond12, "DejaVu Sans C 12", 0);
        show(&font_dejavusanscond14, "DejaVu Sans C 14", 0);
        show(&font_dejavusanscond18, "DejaVu Sans C 18", 0);

        show(&font_freesans8, "FreeSans 8", 0);
        show(&font_freesans10, "FreeSans 10", 0);
        show(&font_freesans12, "FreeSans 12", 0);
        show(&font_freesans14, "FreeSans 14", 0);
        show(&font_freesans18, "FreeSans 18", 0);

        show(&font_liberationsans8, "Liberation Sans 8", 0);
        show(&font_liberationsans10, "Liberation Sans 10", 0);
        show(&font_liberationsans12, "Liberation Sans 12", 0);
        show(&font_liberationsans14, "Liberation Sans 14", 0);
        show(&font_liberationsans18, "Liberation Sans 18", 0);

        show(&font_liberationserif8, "Liberation Serif 8", 0);
        show(&font_liberationserif10, "Liberation Serif 10", 0);
        show(&font_liberationserif12, "Liberation Serif 12", 0);
        show(&font_liberationserif14, "Liberation Serif 14", 0);
        show(&font_liberationserif18, "Liberation Serif 18", 0);

        show(&font_ubuntu8, "Ubuntu 8", 0);
        show(&font_ubuntu10, "Ubuntu 10", 0);
        show(&font_ubuntu12, "Ubuntu 12", 0);
        show(&font_ubuntu14, "Ubuntu 14", 0);
        show(&font_ubuntu18, "Ubuntu 18", 0);

        show(&font_courier8, "Courier 8", 0);
        show(&font_courier10, "Courier 10", 0);
        show(&font_courier12, "Courier 12", 0);
        show(&font_courier14, "Courier 14", 0);
        show(&font_courier18, "Courier 18", 0);
        show(&font_courier24, "Courier 24", 0);

        show(&font_helvetica8, "Helvetica 8", 0);
        show(&font_helvetica10, "Helvetica 10", 0);
        show(&font_helvetica12, "Helvetica 12", 0);
        show(&font_helvetica14, "Helvetica 14", 0);
        show(&font_helvetica18, "Helvetica 18", 0);
        show(&font_helvetica24, "Helvetica 24", 0);

        show(&font_times8, "Times 8", 0);
        show(&font_times10, "Times 10", 0);
        show(&font_times12, "Times 12", 0);
        show(&font_times14, "Times 14", 0);
        show(&font_times18, "Times 18", 0);
        show(&font_times24, "Times 24", 0);

        show(&font_5x7, "5x7", 0);
        show(&font_6x9, "6x9", 0);

        show(&font_digits20, "Digits 20", 1);
        show(&font_digits32, "Digits 32", 1);
    }
}
