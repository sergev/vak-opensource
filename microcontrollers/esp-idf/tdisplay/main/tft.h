/*
 * Interface to a color TFT display.
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
#ifndef _TFT_H_
#define _TFT_H_

typedef struct {
    int width, height;      // Screen size
    int col, row;           // Current position for text
    int offsetx, offsety;
    void *spidev;
} tft_t;

extern tft_t tft;

//
// Font descriptor.
//
typedef struct tft_font_t {
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
} tft_font_t;

//
// Initialize the display.
// Set either horizontal or vertical mode.
// Fill the screen with given color.
// Return a size of the screen in pixels.
//
int tft_init(int rotate, int color, int *xsize, int *ysize);

//
// Enable or disable the display.
//
void tft_enable(int on);

//
// Enable or disable the backlight.
//
void tft_backlight(int on);

//
// Invert the display.
//
void tft_invert(int on);

//
// Send updated image to the screen.
//
void tft_update(void);

//
// Clear screen.
//
void tft_clear(int color);

//
// Draw single pixel.
//
void tft_pixel(int color, int x, int y);

//
// Draw an arbitrary line.
//
void tft_line(int color, int x0, int y0, int x1, int y1);

//
// Fill rectangle.
//
void tft_fill(int color, int x0, int y0, int x1, int y1);

//
// Draw a rectangular frame.
//
void tft_rect(int color, int x0, int y0, int x1, int y1);

//
// Draw a rounded rectangular frame.
//
void tft_round_rect(int color, int x0, int y0, int x1, int y1, int radius);

//
// Fill a triangle.
//
void tft_fill_triangle(int color, int x0, int y0, int x1, int y1, int x2, int y2);

//
// Draw a circle.
//
void tft_circle(int color, int x, int y, int radius);

//
// Draw an image.
//
void tft_image(int x, int y, int width, int height, const void *data);

//
// Draw one character from the specified font.
//
void tft_char(const struct tft_font_t *font, int color, int background, int x, int y, int sym);

//
// Draw a string of ASCII characters.
//
void tft_text(const struct tft_font_t *font, int color, int background, int x, int y, const char *text);

//
// Compute a width in pixels of a text string.
//
int tft_text_width(const struct tft_font_t *font, const char *text, int nchars);

//
// Draw a glyph of one symbol.
//
void tft_glyph(const tft_font_t *font,
    int color, int background, int x, int y, int width,
    const unsigned short *bits);

//
// RGB565 conversion.
// RGB565 is 16bit color format: "RRRRRGGGGGGBBBBB"
//
#define COLOR_RGB(r, g, b) (((r) >> 3 << 11) | ((g) >> 2 << 5) | ((b) >> 3))

#define COLOR_BLACK     0x0000
#define COLOR_WHITE     0xffff
#define COLOR_GRAY      0x8c51
#define COLOR_RED       0xf800
#define COLOR_GREEN     0x07e0
#define COLOR_BLUE      0x001f
#define COLOR_YELLOW    0xffe0
#define COLOR_CYAN      0x07ff
#define COLOR_MAGENTA   0xf81f

#endif /* _TFT_H_ */
