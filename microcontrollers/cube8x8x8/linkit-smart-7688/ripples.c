#include "cube.h"

static const unsigned char LUT[65] = {
    0, 8, 17, 26, 35, 43, 52, 60, 69, 77,
    85, 93, 100, 107, 114, 121, 127, 134, 139, 145,
    150, 155, 159, 163, 167, 170, 173, 175, 177, 179,
    180, 180, 181, 180, 180, 179, 177, 175, 173, 170,
    167, 163, 159, 155, 150, 145, 139, 134, 127, 121,
    114, 107, 100, 93, 85, 77, 69, 60, 52, 43,
    35, 26, 17, 8, 0,
};

static int totty_sin(int sin_of)
{
    unsigned char inv = 0;
    if (sin_of < 0) {
        sin_of = -sin_of;
        inv = 1;
    }
    sin_of &= 0x7f;             /* 127 */
    if (sin_of > 64) {
        sin_of -= 64;
        inv = 1-inv;
    }
    if (inv)
        return -LUT[sin_of];
    else
        return LUT[sin_of];
}

#if 0
static int totty_cos(int cos_of)
{
    unsigned char inv = 0;

    cos_of += 32;               /* Simply rotate by 90 degrees for COS */
    cos_of &= 0x7f;             /* 127 */
    if (cos_of > 64) {
        cos_of -= 64;
        inv = 1;
    }
    if (inv)
        return -LUT[cos_of];
    else
        return LUT[cos_of];
}
#endif

void effect_quad_ripples(int seconds, int delay)
{
    /* 16 values for square root of a^2+b^2.
     * index a*4+b = 10*sqrt
     * This gives the distance to 3.5,3.5 from the point */
    unsigned char sqrt_LUT[] = {
        49, 43, 38, 35, 43, 35, 29, 26, 38, 29, 21, 16, 35, 25, 16, 7
    };
    unsigned char x, y, height, distance;
    int i;
    int iterations = seconds * 1000 / delay;

    for (i=0; i<iterations*4; i+=4) {
        fill(0);
        for (x=0; x<4; x++) {
            for (y=0; y<4; y++) {
                /* x+y*4 gives no. from 0-15 for sqrt_LUT */
                distance = sqrt_LUT[x + y*4];   /* distance is 0-50 roughly */

                /* height is sin of distance + iteration*4 */
                height = (196 + totty_sin(distance+i)) / 49;

                /* Use 4-way mirroring to save on calculations */
                setvoxel(x,   y,        height);
                setvoxel(7-x, y,        height);
                setvoxel(x,   7-y,      height);
                setvoxel(7-x, 7-y,      height);
                setvoxel(x,   y,        7-height);
                setvoxel(7-x, y,        7-height);
                setvoxel(x,   7-y,      7-height);
                setvoxel(7-x, 7-y,      7-height);
                setvoxel(x,   height,   y);
                setvoxel(7-x, height,   y);
                setvoxel(x,   height,   7-y);
                setvoxel(7-x, height,   7-y);
                setvoxel(x,   7-height, y);
                setvoxel(7-x, 7-height, y);
                setvoxel(x,   7-height, 7-y);
                setvoxel(7-x, 7-height, 7-y);
            }
        }
        delay_ms(delay);
    }
}

void effect_int_ripples(int seconds, int delay)
{
    /* 16 values for square root of a^2+b^2.
     * index a*4+b = 10*sqrt
     * This gives the distance to 3.5,3.5 from the point */
    unsigned char sqrt_LUT[]={
        49, 43, 38, 35, 43, 35, 29, 26, 38, 29, 21, 16, 35, 25, 16, 7
    };
    unsigned char x, y, height, distance;
    int i;
    int iterations = seconds * 1000 / delay;

    for (i=0; i<iterations*4; i+=4) {
        fill(0);
        for (x=0; x<4; x++) {
            for (y=0; y<4; y++) {
                /* x+y*4 gives no. from 0-15 for sqrt_LUT */
                distance = sqrt_LUT[x+y*4];     /* distance is 0-50 roughly */

                /* height is sin of distance + iteration*4 */
                height = (196 + totty_sin(distance+i)) / 49;

                /* Use 4-way mirroring to save on calculations */
                setvoxel(x,   y,   height);
                setvoxel(7-x, y,   height);
                setvoxel(x,   7-y, height);
                setvoxel(7-x, 7-y, height);
            }
        }
        delay_ms(delay);
    }
}

void effect_side_ripples(int seconds, int delay)
{
    /* 16 values for square root of a^2+b^2.
     * index a*4+b = 10*sqrt
     * This gives the distance to 3.5,3.5 from the point */
    unsigned char sqrt_LUT[]={
        49, 43, 38, 35, 43, 35, 29, 26, 38, 29, 21, 16, 35, 25, 16, 7
    };
    unsigned char x, y, height, distance;
    int i;
    int iterations = seconds * 1000 / delay;

    for (i=0; i<iterations*4; i+=4) {
        fill(0);
        for (x=0; x<4; x++) {
            for (y=0; y<4; y++) {
                /* x+y*4 gives no. from 0-15 for sqrt_LUT */
                distance = sqrt_LUT[x+y*4];     /* distance is 0-50 roughly */

                /* height is sin of distance + iteration*4 */
                height = (196 + totty_sin(distance+i)) / 49;

                /* Use 4-way mirroring to save on calculations */
                setvoxel(x,   height,   y);
                setvoxel(7-x, height,   y);
                setvoxel(x,   height,   7-y);
                setvoxel(7-x, height,   7-y);
                setvoxel(x,   7-height, y);
                setvoxel(7-x, 7-height, y);
                setvoxel(x,   7-height, 7-y);
                setvoxel(7-x, 7-height, 7-y);
            }
        }
        delay_ms(delay);
    }
}

void effect_mirror_ripples(int seconds, int delay)
{
    /* 16 values for square root of a^2+b^2.
     * index a*4+b = 10*sqrt
     * This gives the distance to 3.5,3.5 from the point */
    unsigned char sqrt_LUT[]={
        49, 43, 38, 35, 43, 35, 29, 26, 38, 29, 21, 16, 35, 25, 16, 7
    };
    unsigned char x, y, height, distance;
    int i;
    int iterations = seconds * 1000 / delay;

    for (i=0; i<iterations*4; i+=4) {
        fill(0);
        for (x=0; x<4; x++) {
            for (y=0; y<4; y++) {
                /* x+y*4 gives no. from 0-15 for sqrt_LUT */
                distance=sqrt_LUT[x+y*4];   /* distance is 0-50 roughly */

                /* height is sin of distance + iteration*4 */
                height = (196 + totty_sin(distance+i)) / 49;

                /* Use 4-way mirroring to save on calculations */
                setvoxel(x,   y,   height);
                setvoxel(7-x, y,   height);
                setvoxel(x,   7-y, height);
                setvoxel(7-x, 7-y, height);
                setvoxel(x,   y,   7-height);
                setvoxel(7-x, y,   7-height);
                setvoxel(x,   7-y, 7-height);
                setvoxel(7-x, 7-y, 7-height);
            }
        }
        delay_ms(delay);
    }
}
