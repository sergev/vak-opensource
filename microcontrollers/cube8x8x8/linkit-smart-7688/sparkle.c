#include <stdlib.h>
#include "cube.h"

static void sparkle_flash(int iterations, int voxels, int delay)
{
    int i, v;

    for (i = 0; i < iterations; i++) {
        for (v=0; v<=voxels; v++)
            setvoxel(rand()%8, rand()%8, rand()%8);

        delay_ms(delay);
        fill(0);
    }
}

/*
 * blink 1 random voxel, blink 2 random voxels..... blink 20 random voxels
 * and back to 1 again.
 */
void effect_random_sparkle(int seconds, int delay)
{
    int i;
    int iterations = seconds * 1000 / delay / 40;

    for (i=1; i<20; i++) {
        sparkle_flash(iterations, i, delay);
    }

    for (i=20; i>=1; i--) {
        sparkle_flash(iterations, i, delay);
    }
}
