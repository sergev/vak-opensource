#ifndef _FONT_H_
#define _FONT_H_

#include <stdbool.h>

typedef struct {
    const uint8_t *Data;
    int width;      /* Width in bytes */
    int height;     /* Height in bytes */
    int StartChar;  /* Which ascii # the font starts with (usually a space or !) */
    int EndChar;    /* Ending ascii # the font ends with */
} font_t;

typedef enum {
    TextAnchor_East = 0,
    TextAnchor_West,
    TextAnchor_North,
    TextAnchor_South,
    TextAnchor_NorthEast,
    TextAnchor_NorthWest,
    TextAnchor_SouthEast,
    TextAnchor_SouthWest,
    TextAnchor_Center
} TextAnchor;

int font_get_char_height(font_t *font, char c);
int font_measure_string(font_t *font, const char *text);
int font_get_char_width(font_t *font, char c);

void font_draw_char(font_t *font, char c, int x, int y, bool color);
void font_draw_string(font_t *font, const char *text, int x, int y, bool color);

void font_draw_char_unaligned(font_t *font, char c, int x, int y, bool color);
void font_draw_string_unaligned(font_t *font, const char *text, int x, int y, bool color);
void font_draw_anchored_string(font_t *font, const char *text, TextAnchor anchor , bool color);

#endif
