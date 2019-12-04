/*
 * Interface to a b/w OLED display based on SSD1306 controller.
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
#ifndef _SSD1306_H_
#define _SSD1306_H_

typedef struct {
    int width, height;      // Screen size
    int col, row;           // Current position for text
    unsigned char *image;
    int image_size;
} oled_t;

extern oled_t oled;

//
// Font descriptor.
//
typedef struct oled_font_t {
    const char *    name;           /* font name */
    int             maxwidth;       /* max width in pixels */
    unsigned int    height;         /* height in pixels */
    int             ascent;         /* ascent (baseline) height */
    int             firstchar;      /* first character in bitmap */
    int             size;           /* font size in characters */
    const unsigned short *bits;     /* 16-bit right-padded bitmap data */
    const unsigned short *offset;   /* offsets into bitmap data */
    const unsigned char *width;     /* character widths or 0 if fixed */
    int             defaultchar;    /* default char (not glyph index) */
    long            bits_size;      /* # words of bits */
} oled_font_t;

//
// Initialize the display.
// Set either horizontal or vertical mode.
// Fill the screen with given color.
// Return a size of the screen in pixels.
//
int oled_init(int rotate, int color, int *xsize, int *ysize);

//
// Send updated image to the screen.
//
void oled_update(void);

//
// Clear screen.
//
void oled_clear(int color);

//
// Draw single pixel.
//
void oled_pixel(int color, int x, int y);

//
// Draw an arbitrary line.
//
void oled_line(int color, int x0, int y0, int x1, int y1);

//
// Fill rectangle.
//
void oled_fill(int color, int x0, int y0, int x1, int y1);

//
// Draw a rectangular frame.
//
void oled_rect(int color, int x0, int y0, int x1, int y1);

//
// Fill a triangle.
//
void oled_fill_triangle(int color, int x0, int y0, int x1, int y1, int x2, int y2);

//
// Draw a circle.
//
void oled_circle(int color, int x, int y, int radius);

//TODO: void oled_image(int x, int y, int width, int height, const unsigned char *data);

//
// Draw one character from the specified font.
//
void oled_char(const struct oled_font_t *font, int color, int background, int x, int y, int sym);

//
// Draw a string of ASCII characters.
//
void oled_text(const struct oled_font_t *font, int color, int background, int x, int y, const char *text);

//
// Compute a width in pixels of a text string.
//
int oled_text_width(const struct oled_font_t *font, const char *text, int nchars);

#endif
