#include <stdlib.h>
#include "cube.h"

void effect_wormsqueeze(int size, int axis, int direction, int seconds, int delay)
{
    int x, y, i, j, k, dx, dy;
    int cube_size;
    int origin = 0;
    int iterations = seconds * 1000 / delay;

    if (direction == -1)
        origin = 7;

    cube_size = 8-(size-1);

    x = rand()%cube_size;
    y = rand()%cube_size;

    for (i=0; i<iterations; i++) {
        dx = ((rand()%3)-1);
        dy = ((rand()%3)-1);

        if ((x+dx) > 0 && (x+dx) < cube_size)
            x += dx;

        if ((y+dy) > 0 && (y+dy) < cube_size)
            y += dy;

        shift(axis, direction);

        for (j=0; j<size; j++) {
            for (k=0; k<size; k++) {
                if (axis == AXIS_Z)
                    setvoxel(x+j, y+k, origin);

                if (axis == AXIS_Y)
                    setvoxel(x+j, origin, y+k);

                if (axis == AXIS_X)
                    setvoxel(origin, y+j, x+k);
            }
        }

        delay_ms(delay);
    }
}
