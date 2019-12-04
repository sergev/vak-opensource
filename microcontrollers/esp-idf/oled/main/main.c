/**
 * Copyright (c) 2017 Tara Keeling
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <stdio.h>
#include <string.h>
#include <esp_timer.h>

#include "oled.h"
#include "font.h"

/*
 * Screen size.
 */
int xsize, ysize;

extern font_t Font_Comic_Neue_25x28;

void app_main()
{
    printf("Draw fonts.\n");

    if (oled_init(0, 0, &xsize, &ysize) >= 0) {
        printf("Screen size %u x %u.\n", xsize, ysize);

        //show(&font_lucidasans28, "Lucida Sans 28", 0);
        //show(&font_lucidasans15, "Lucida Sans 15", 0);
        //show(&font_lucidasans11, "Lucida Sans 11", 0);
        //show(&font_digits32, "Digits 32", 1);
        //show(&font_digits20, "Digits 20", 1);
        font_draw_anchored_string(&Font_Comic_Neue_25x28, "Smile!", TextAnchor_Center, true);
        oled_update();
    }
}
