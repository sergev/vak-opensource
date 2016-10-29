#include <stdlib.h>
#include "cube.h"

/*
 * Set or clear exactly 512 voxels in a random order.
 */
void effect_random_filler(int delay, int state)
{
    int x, y, z;
    int loop = 0;

    if (state == 1) {
        fill(0);
    } else {
        fill(0xff);
    }

    while (loop < 511) {
        x = rand()%8;
        y = rand()%8;
        z = rand()%8;

        if ((state == 0 && getvoxel(x, y, z) == 1) ||
            (state == 1 && getvoxel(x, y, z) == 0))
        {
            altervoxel(x, y, z, state);
            delay_ms(delay);
            loop++;
        }
    }
}
