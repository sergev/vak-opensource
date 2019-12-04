/**
 * Copyright (c) 2017 Tara Keeling
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include "oled.h"

void oled_draw_pixel(int x, int y, bool color)
{
    uint32_t y_bit = (y & 7);

    /*
     * We only need to modify the Y coordinate since the pitch
     * of the screen is the same as the width.
     * Dividing y by 8 gives us which row the pixel is in but not
     * the bit position.
     */
    y >>= 3;

    uint8_t *ptr = oled.Framebuffer + (y * oled.width) + x;
    if (color)
        *ptr |= BIT(y_bit);
    else
        *ptr &= ~BIT(y_bit);
}

void oled_draw_h_line(int x, int y, int x2, bool Color)
{
    NullCheck(oled.Framebuffer, return);

    CheckBounds(x >= oled.width, return);
    CheckBounds((x2 + x) >= oled.width, return);
    CheckBounds(y >= oled.height, return);

    for (; x <= x2; x++) {
        oled_draw_pixel(x, y, Color);
    }
}

void oled_draw_v_line(int x, int y, int y2, bool Color)
{
    NullCheck(oled.Framebuffer, return);

    CheckBounds(x >= oled.width, return);
    CheckBounds(y >= oled.height, return);
    CheckBounds((y2 + y) >= oled.height, return);

    for (; y <= y2; y++) {
        oled_draw_pixel(x, y, Color);
    }
}

void oled_draw_line(int x0, int y0, int x1, int y1, bool Color)
{
    NullCheck(oled.Framebuffer, return);

    if (x0 == x1) {
        /* Vertical line */
        oled_draw_v_line(x0, y0, y1, Color);
    } else if (y0 == y1) {
        /* Horizontal line */
        oled_draw_h_line(x0, y0, x1, Color);
    } else {
        /* Diagonal line */
        /* TODO: This */

        CheckBounds((x0 < 0) || (x0 >= oled.width), return);
        CheckBounds((x1 < 0) || (x1 >= oled.width), return);

        CheckBounds((y0 < 0) || (y0 >= oled.height), return);
        CheckBounds((y1 < 0) || (y1 >= oled.height), return);
    }
}

void oled_draw_rect(int x, int y, int x2, int y2, bool Color)
{
    NullCheck(oled.Framebuffer, return);

    CheckBounds(x >= oled.width, return);
    CheckBounds((x2 + x) >= oled.width, return);
    CheckBounds(y >= oled.height, return);
    CheckBounds((y2 + y) >= oled.height, return);

    for (; y <= y2; y++) {
        oled_draw_h_line(x, y, x2, Color);
    }
}

void oled_clear(bool Color)
{
    NullCheck(oled.Framebuffer, return);

    memset(oled.Framebuffer, Color, oled.FramebufferSize);
}
