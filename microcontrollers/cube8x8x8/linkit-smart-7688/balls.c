#include "cube.h"

static const unsigned char dat3[24]= {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x16,
    0x26, 0x36, 0x46, 0x56, 0x66, 0x65, 0x64, 0x63,
    0x62, 0x61, 0x60, 0x50, 0x40, 0x30, 0x20, 0x10
};

/*
 * Reorder two values to make first one smaller than the second one.
 */
static void reorder(int *a, int *b)
{
    int t;

    if (*a > *b) {
        t = *a;
        *a = *b;
        *b = t;
    }
}

static void box(int x1, int y1, int z1, int x2, int y2, int z2, int fill, int le)
{
    int i, j, t = 0;

    reorder(&x1, &x2);
    reorder(&y1, &y2);
    reorder(&z1, &z2);
    for (i=x1; i<=x2; i++)
        t |= 1<<i;
    if (!le)
        t = ~t;
    if (fill) {
        if (le) {
            for (i=z1; i<=z2; i++) {
                for (j=y1; j<=y2; j++)
                    cube[j][i] |= t;
            }
        } else {
            for (i=z1; i<=z2; i++) {
                for (j=y1; j<=y2; j++)
                    cube[j][i] &= t;
            }
        }
    } else {
        if (le) {
            cube[y1][z1] |= t;
            cube[y2][z1] |= t;
            cube[y1][z2] |= t;
            cube[y2][z2] |= t;
        } else {
            cube[y1][z1] &= t;
            cube[y2][z1] &= t;
            cube[y1][z2] &= t;
            cube[y2][z2] &= t;
        }
        t = (1 << x1) | (1 << x2);
        if (!le)
            t = ~t;
        if (le) {
            for (j=z1; j<=z2; j+=(z2-z1)) {
                for (i=y1; i<=y2; i++)
                    cube[i][j] |= t;
            }
            for (j=y1; j<=y2; j+=(y2-y1)) {
                for (i=z1; i<=z2; i++)
                    cube[j][i] |= t;
            }
        } else {
            for (j=z1; j<=z2; j+=(z2-z1)) {
                for (i=y1; i<=y2; i++) {
                    cube[i][j] &= t;
                }
            }
            for (j=y1; j<=y2; j+=(y2-y1)) {
                for (i=z1; i<=z2; i++) {
                    cube[j][i] &= t;
                }
            }
        }
    }
}

void effect_rotating_balls(int delay)
{
    int i, j, an[4], x, y, t;

    for (i=1; i<7; i++) {
        fill(0);
        box(0, 6,   6,   1,   7,   7,   1, 1);
        box(i, 6,   6-i, i+1, 7,   7-i, 1, 1);
        box(i, 6,   6,   i+1, 7,   7,   1, 1);
        box(0, 6,   6-i, 1,   7,   7-i, 1, 1);
        box(0, 6-i, 6,   1,   7-i, 7,   1, 1);
        box(i, 6-i, 6-i, i+1, 7-i, 7-i, 1, 1);
        box(i, 6-i, 6,   i+1, 7-i, 7,   1, 1);
        box(0, 6-i, 6-i, 1,   7-i, 7-i, 1, 1);
        delay_ms(delay*3);
    }
    for (i=0; i<4; i++) {
        an[i] = 6*i;
    }
    for (i=0; i<35; i++) {
        fill(0);
        for (j=0; j<4; j++) {
            t = an[j] % 24;
            x = dat3[t] >> 4;
            y = dat3[t] & 0x0f;
            box(x, y, 0, x+1, y+1, 1, 1, 1);
            box(x, y, 6, x+1, y+1, 7, 1, 1);
        }
        for (j=0; j<4; j++)
            an[j]++;
        delay_ms(delay);
    }
    for (i=0; i<35; i++) {
        fill(0);
        for (j=0; j<4; j++) {
            t = an[j] % 24;
            x = dat3[t] >> 4;
            y = dat3[t] & 0x0f;
            box(x, y, 0, x+1, y+1, 1, 1, 1);
            box(x, y, 6, x+1, y+1, 7, 1, 1);
        }
        for (j=0; j<4; j++)
            an[j]--;
        delay_ms(delay);
    }
    for (i=0; i<35; i++) {
        fill(0);
        for (j=0; j<4; j++) {
            t = an[j] % 24;
            x = dat3[t] >> 4;
            y = dat3[t] & 0x0f;
            box(x, 0, y, x+1, 1, y+1, 1, 1);
            box(x, 6, y, x+1, 7, y+1, 1, 1);
        }
        for (j=0; j<4; j++)
            an[j]++;
        delay_ms(delay);
    }
    for (i=0; i<36; i++) {
        fill(0);
        for (j=0; j<4; j++) {
            t = an[j] % 24;
            x = dat3[t] >> 4;
            y = dat3[t] & 0x0f;
            box(x, 0, y, x+1, 1, y+1, 1, 1);
            box(x, 6, y, x+1, 7, y+1, 1, 1);
        }
        for (j=0; j<4; j++)
            an[j]--;
        delay_ms(delay);
    }
    for (i=6; i>0; i--) {
        fill(0);
        box(0, 6,   6,   1,   7,   7,   1, 1);
        box(i, 6,   6-i, i+1, 7,   7-i, 1, 1);
        box(i, 6,   6,   i+1, 7,   7,   1, 1);
        box(0, 6,   6-i, 1,   7,   7-i, 1, 1);
        box(0, 6-i, 6,   1,   7-i, 7,   1, 1);
        box(i, 6-i, 6-i, i+1, 7-i, 7-i, 1, 1);
        box(i, 6-i, 6,   i+1, 7-i, 7,   1, 1);
        box(0, 6-i, 6-i, 1,   7-i, 7-i, 1, 1);
        delay_ms(delay*3);
    }
}
