#include "cube.h"

static const unsigned char dat2[28]= {
    0x0,  0x20, 0x40, 0x60, 0x80, 0xa0, 0xc0, 0xe0, 0xe4, 0xe8,
    0xec, 0xf0, 0xf4, 0xf8, 0xfc, 0xdc, 0xbc, 0x9c, 0x7c, 0x5c,
    0x3c, 0x1c, 0x18, 0x14, 0x10, 0xc,  0x8,  0x4
};

void effect_curved_plane(int seconds, int delay)
{
    int iterations = seconds * 1000 / delay;
    int i, j, an[8], x, y, t, x1, y1;

    for (i=0; i<8; i++)
        an[i] = 14;

    /* Rotate curved plane clockwise */
    for (i=0; i<iterations; i++) {
        fill(0);
        for (j=0; j<8; j++) {
            t = an[j] % 28;
            x = dat2[t] >> 5;
            y = (dat2[t] >> 2) & 7;
            t = (an[j] - 14) % 28;
            x1 = dat2[t] >> 5;
            y1 = (dat2[t] >> 2) & 7;
            line(x, y, j, x1, y1, j, 1);
        }
        for (j=0; j<8; j++) {
            if (i > j && j > i+14-iterations)
                an[j]++;
        }
        delay_ms(delay);
    }

    /* Rotate counter-clockwise */
    for (i=0; i<iterations; i++) {
        fill(0);
        for (j=0; j<8; j++) {
            t = an[j] % 28;
            x = dat2[t] >> 5;
            y = (dat2[t] >> 2) & 7;
            t = (an[j] - 14) % 28;
            x1 = dat2[t] >> 5;
            y1 = (dat2[t] >> 2) & 7;
            line(x, y, j, x1, y1, j, 1);
        }
        for (j=0; j<8; j++) {
            if (i > j && j > i+14-iterations)
                an[j]--;
        }
        delay_ms(delay);
    }
}
