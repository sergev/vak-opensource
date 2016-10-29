#include <stdlib.h>
#include "cube.h"

void effect_rain(int seconds, int delay)
{
    int i, ii;
    int rnd_x;
    int rnd_y;
    int rnd_num;
    int iterations = seconds * 1000 / delay;

    for (ii=0; ii<iterations; ii++) {
        rnd_num = rand()%4;

        for (i=0; i < rnd_num; i++) {
            rnd_x = rand()%8;
            rnd_y = rand()%8;
            setvoxel(rnd_x, rnd_y, 7);
        }

        delay_ms(delay);
        shift(AXIS_Z, -1);
    }
}
