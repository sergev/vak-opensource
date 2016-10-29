#include "cube.h"

/*
 * Draw a plane on one axis and send it back and forth once.
 */
void effect_plane(int plane, int delay)
{
    int i;

    for (i=0; i<8; i++) {
        fill(0);
        setplane(plane, i);
        delay_ms(delay);
    }

    for (i=7; i>=0; i--) {
        fill(0);
        setplane(plane, i);
        delay_ms(delay);
    }
}
