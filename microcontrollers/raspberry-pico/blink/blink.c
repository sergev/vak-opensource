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
    return (r << 16) | (g << 8) | b;
}

void blink(unsigned color, unsigned msec)
{
    led_write(color);
    sleep_ms(msec);
    led_write(0);
    sleep_ms(msec);
}

int main()
{
    // WS2812 LED on rp2040-zero connected to GP16 pin.
//  const unsigned WS2812_PIN = 16; // Waveshare RP2040-Zero board
    const unsigned WS2812_PIN = 23; // vcc-gnd YD-RP2040 board
    const bool IS_RGBW = false;
    const unsigned SM = 0;
    const float FREQ = 800000;
    const unsigned offset = pio_add_program(pio0, &ws2812_program);

    ws2812_program_init(pio0, SM, offset, WS2812_PIN, FREQ, IS_RGBW);

    for (;;) {
        // red
        blink(color_rgb(0xff, 0, 0), 250);
        blink(color_rgb(0xff, 0, 0), 250);

        // green
        blink(color_rgb(0, 0xff, 0), 250);
        blink(color_rgb(0, 0xff, 0), 250);
        blink(color_rgb(0, 0xff, 0), 250);

        // blue
        blink(color_rgb(0, 0, 0xff), 250);
        blink(color_rgb(0, 0, 0xff), 250);
        blink(color_rgb(0, 0, 0xff), 250);
        blink(color_rgb(0, 0, 0xff), 250);
    }
}
