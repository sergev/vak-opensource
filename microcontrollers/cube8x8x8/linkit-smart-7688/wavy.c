#include "cube.h"

static void transss()
{
    int i, j;

    for (i=0; i<8; i++) {
        for (j=0; j<8; j++) {
            cube[i][j] <<= 1;
        }
    }
}

void effect_wavy_plane(int seconds, int delay)
{
    int iterations = seconds * 1000 / delay / 13;
    int i, j, t;
    static const unsigned char daa[13] = {
        0, 1, 2, 0x23, 5, 6, 7, 6, 5, 0x23, 2, 1, 0
    };

    /* Wavy plane */
    fill(0);
    for (j=0; j<iterations; j++) {
        for (i=0; i<13; i++) {
            if (daa[i]>>4) {
                t = daa[i] & 0x0f;
                line(0, 0, t+1, 0, 7, t+1, 1);
            } else
                t = daa[i];
            line(0, 0, t, 0, 7, t, 1);
            transss();
            delay_ms(delay);
        }
    }

#if 0
    /* Thick snake */
    for (j=1; j<8; j++) {
        if (j > 3)
            t = 4;
        else
            t = j;
        for (i=0; i<24; i+=j) {
            int x = dat3[i] >> 4;
            int y = dat3[i] & 0x0f;
            box_apeak_xy(0, x, y, 0, x+1, y+1, 1, 1);
            transss();
            delay_ms(delay);
        }
    }

    /* Thin snake */
    for (j=1; j<8; j++) {
        if (j>3)
            t = 4;
        else
            t = j;
        for (i=0; i<24; i+=j) {
            int x = dat3[i] >> 4;
            int y = dat3[i] & 0x0f;
            point(0, x, y, 1);
            transss();
            delay_ms(delay);
        }
    }
#endif
    fill(0);
}
