/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"

//
// Set LED color.
//
static inline void led_write(unsigned color)
{
    pio_sm_put_blocking(pio0, 0, color << 8);
}

//
// Create color value from r/g/b components.
//
static inline unsigned color_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return (r << 8) | (g << 16) | b;
}

int main()
{
    // WS2812 LED on rp2040-zero connected to GP16 pin.
    const unsigned WS2812_PIN = 16;
    const bool IS_RGBW = false;
    const unsigned SM = 0;
    const float FREQ = 800000;
    const unsigned offset = pio_add_program(pio0, &ws2812_program);

    ws2812_program_init(pio0, SM, offset, WS2812_PIN, FREQ, IS_RGBW);

    for (;;) {
        led_write(color_rgb(0xff, 0, 0)); // red
        sleep_ms(250);
        led_write(0);
        sleep_ms(250);

        led_write(color_rgb(0, 0xff, 0)); // green
        sleep_ms(250);
        led_write(0);
        sleep_ms(250);

        led_write(color_rgb(0, 0, 0xff)); // blue
        sleep_ms(250);
        led_write(0);
        sleep_ms(250);
    }
}
