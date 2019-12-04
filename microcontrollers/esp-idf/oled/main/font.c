/**
 * Copyright (c) 2017 Tara Keeling
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "oled.h"
#include "font.h"

int font_get_char_height(font_t *font, char c)
{
    NullCheck(font, return 0);
    NullCheck(font->Data, return 0);

    return font->height * 8;
}

int font_measure_string(font_t *font, const char* Text)
{
    int StringWidthInPixels = 0;
    int Length = 0;
    int i = 0;

    NullCheck(font, return 0);
    NullCheck(font->Data, return 0);
    NullCheck(Text, return 0);

    for (i = 0, Length = strlen(Text); i < Length; i++) {
        StringWidthInPixels+= font_get_char_width(font, Text[ i ]);
    }

    return StringWidthInPixels;
}

int font_get_char_width(font_t *font, char c)
{
    const uint8_t* WidthOffset = font->Data;

    NullCheck(font, return 0);
    NullCheck(font->Data, return 0);

    if (c < font->StartChar || c > font->EndChar)
        return 0;

    WidthOffset+= (c - font->StartChar) * ((font->width * font->height) + 1);
    return *WidthOffset;
}

void font_draw_char(font_t *font, char c, int x, int y, bool color)
{
    const uint8_t* FontOffset = NULL;
    int GlyphSizeInBytes = 0;
    int CharHeight = 0;
    int CharWidth = 0;
    int x2 = 0;
    int y2 = 0;

    NullCheck(oled.Framebuffer, return);
    NullCheck(font, return);
    NullCheck(font->Data, return);

    CharWidth = font_get_char_width(font, c);
    CharHeight = font_get_char_height(font, c);

    CheckBounds(x >= oled.width - CharWidth, return);
    CheckBounds(y >= oled.height - CharHeight, return);

    if (c < font->StartChar || c > font->EndChar) {
        return;
    }

    /* Divide y by 8 to get which page the Y coordinate is on */
    y>>= 3;

    /* Height is size in bytes, so a 16px high font would be 2 bytes tall */
    GlyphSizeInBytes = (font->width * font->height) + 1;
    FontOffset = &font->Data[ ((c - font->StartChar) * GlyphSizeInBytes) + 1 ];

    for (x2 = 0; x2 < font_get_char_width(font, c); x2++) {
        for (y2 = 0; y2 < font->height; y2++) {
            oled.Framebuffer[ ((y2 + y) * oled.width) + (x + x2) ] = (color == true) ? *FontOffset : ! *FontOffset;
            FontOffset++;
        }
    }
}

void font_draw_string(font_t *font, const char* Text, int x, int y, bool color)
{
    int Length = 0;
    int i = 0;
    int x2 = 0;
    int y2 = 0;

    NullCheck(oled.Framebuffer, return);
    NullCheck(font, return);
    NullCheck(font->Data, return);
    NullCheck(Text, return);

    CheckBounds(x >= oled.width, return);
    CheckBounds(y >= oled.height, return);

    for (i = 0, x2 = x, y2 = y, Length = strlen(Text); i < Length; i++) {
        font_draw_char(font, Text[ i ], x2, y2, color);
        x2+= font_get_char_width(font, Text[ i ]);
    }
}

void font_draw_char_unaligned(font_t *font, char c, int x, int y, bool color)
{
    const uint8_t* FontOffset = NULL;
    int CharSizeInBytes = 0;
    int CharHeight = 0;
    int CharWidth = 0;
    int x2 = 0;
    int y2 = 0;
    int i = 0;

    NullCheck(oled.Framebuffer, return);
    NullCheck(font, return);
    NullCheck(font->Data, return);

    CharWidth = font_get_char_width(font, c);
    CharHeight = font_get_char_height(font, c);

    CheckBounds(x >= oled.width - CharWidth, return);
    CheckBounds(y >= oled.height - CharHeight, return);

    if (c < font->StartChar || c > font->EndChar) {
        return;
    }

    /* Height is size in bytes, so a 16px high font would be 2 bytes tall */
    CharSizeInBytes = (font->width * font->height) + 1;
    FontOffset = &font->Data[ ((c - font->StartChar) * CharSizeInBytes) + 1 ];

    for (x2 = 0; x2 < CharWidth; x2++) {
        for (y2 = 0; y2 < font->height; y2++) {
            for (i = 7; i >= 0; i--) {
                if (*FontOffset & BIT(i)) {
                    oled_draw_pixel(x + x2,
                                    y + (i + (y2 * 8)),
                                    color);
                }
            }

            FontOffset++;
        }
    }
}

void font_draw_string_unaligned(font_t *font, const char* Text, int x, int y, bool color)
{
    int Length = 0;
    int i = 0;

    NullCheck(oled.Framebuffer, return);
    NullCheck(font, return);
    NullCheck(font->Data, return);
    NullCheck(Text, return);

    CheckBounds(x >= oled.width, return);
    CheckBounds(y >= oled.height, return);

    for (i = 0, Length = strlen(Text); i < Length; i++) {
        font_draw_char_unaligned(font, Text[ i ], x, y, color);
        x+= font_get_char_width(font, Text[ i ]);
    }
}

void font_draw_anchored_string(font_t *font, const char* Text, TextAnchor Anchor , bool color)
{
    int StringLengthInPixels = 0;
    int CharHeight = 0;
    int MidpointX = 0;
    int MidpointY = 0;
    int x = 0;
    int y = 0;

    NullCheck(oled.Framebuffer, return);
    NullCheck(font, return);
    NullCheck(font->Data, return);
    NullCheck(Text, return);

    StringLengthInPixels = font_measure_string(font, Text);
    CharHeight = font_get_char_height(font, ' ');
    MidpointX = (oled.width / 2) - 1;
    MidpointY = (oled.height / 2) - 1;

    switch (Anchor) {
        case TextAnchor_North: {
            x = MidpointX - (StringLengthInPixels / 2);
            y = 0;
            break;
        }
        case TextAnchor_NorthEast: {
            x = oled.width - 1 - StringLengthInPixels;
            y = 0;
            break;
        }
        case TextAnchor_NorthWest: {
            x = 0;
            y = 0;
            break;
        }
        case TextAnchor_East: {
            y = MidpointY - (CharHeight / 2);
            x = oled.width - 1 - StringLengthInPixels;
            break;
        }
        case TextAnchor_West: {
            y = MidpointY - (CharHeight / 2);
            x = 0;
            break;
        }
        case TextAnchor_SouthEast: {
            y = oled.height - 1 - CharHeight;
            x = oled.width - 1 - StringLengthInPixels;
            break;
        }
        case TextAnchor_SouthWest: {
            y = oled.height - 1 - CharHeight;
            x = 0;
            break;
        }
        case TextAnchor_South: {
            x = MidpointX - (StringLengthInPixels / 2);
            y = oled.height - 1 - CharHeight;
            break;
        }
        case TextAnchor_Center: {
            x = MidpointX - (StringLengthInPixels / 2);
            y = MidpointY - (CharHeight / 2);
            break;
        }
        default: {
            return;
        }
    };

    font_draw_string_unaligned(font, Text, x, y, color);
}
