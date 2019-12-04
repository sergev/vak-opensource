#ifndef _SSD1306_H_
#define _SSD1306_H_

#include <stdint.h>
#include <stdbool.h>

#ifndef BIT
#define BIT(n) (1 << n)
#endif

#define CheckBounds(expr, retexpr) { \
    if (expr) { \
        printf("[%s:%d] %s\n", __FUNCTION__, __LINE__, #expr); \
        retexpr; \
    } \
}

#define NullCheck(ptr, retexpr) { \
    if (ptr == NULL) { \
        printf("%s: %s == NULL\n", __FUNCTION__, #ptr); \
        retexpr; \
    }; \
}

typedef struct {
    int width;
    int height;

    uint8_t *Framebuffer;
    int FramebufferSize;
} oled_t;

extern oled_t oled;

//
// Initialize the display.
// Set either horizontal or vertical mode.
// Fill the screen with given color.
// Return a size of the screen in pixels.
//
int oled_init(int rotate, int color, int *xsize, int *ysize);

void oled_update(void);
void oled_clear(bool color);
void oled_draw_pixel(int x, int y, bool color);
void oled_draw_h_line(int x, int y0, int x1, bool color);
void oled_draw_v_line(int x, int y0, int y1, bool color);
void oled_draw_line(int x0, int y0, int x1, int y1, bool color);
void oled_draw_rect(int x0, int y0, int x1, int y1, bool color);

#endif
