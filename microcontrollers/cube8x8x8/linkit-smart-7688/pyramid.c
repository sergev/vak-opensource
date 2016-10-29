#include "cube.h"

static void zoom_pyramid(int delay)
{
    fill(0);
    box_walls(0, 0, 0, 7, 0, 7);
    delay_ms(delay);

    fill(0);
    box_wireframe(0, 0, 0, 7, 0, 1);
    box_walls(0, 1, 0, 7, 1, 7);
    delay_ms(delay);

    fill(0);
    box_wireframe(0, 0, 0, 7, 1, 1);
    box_wireframe(1, 1, 2, 6, 1, 3);
    box_walls(0, 2, 0, 7, 2, 7);
    delay_ms(delay);

    fill(0);
    box_wireframe(0, 0, 0, 7, 2, 1);
    box_wireframe(1, 1, 2, 6, 2, 3);
    box_wireframe(2, 2, 4, 5, 2, 5);
    box_walls(0, 3, 0, 7, 3, 7);
    delay_ms(delay);

    fill(0);
    box_wireframe(0, 0, 0, 7, 3, 1);
    box_wireframe(1, 1, 2, 6, 3, 3);
    box_wireframe(2, 2, 4, 5, 3, 5);
    box_wireframe(3, 3, 6, 4, 3, 7);
    box_walls(0, 4, 0, 7, 4, 7);
    delay_ms(delay);

    fill(0);
    box_wireframe(0, 0, 0, 7, 4, 1);
    box_wireframe(1, 1, 2, 6, 4, 3);
    box_wireframe(2, 2, 4, 5, 4, 5);
    box_wireframe(3, 3, 6, 4, 4, 7);
    box_walls(0, 5, 0, 7, 5, 7);
    delay_ms(delay);

    fill(0);
    box_wireframe(0, 0, 0, 7, 5, 1);
    box_wireframe(1, 1, 2, 6, 5, 3);
    box_wireframe(2, 2, 4, 5, 5, 5);
    box_wireframe(3, 3, 6, 4, 4, 7);
    box_walls(0, 6, 0, 7, 6, 7);
    delay_ms(delay);

    fill(0);
    box_wireframe(0, 0, 0, 7, 6, 1);
    box_wireframe(1, 1, 2, 6, 6, 3);
    box_wireframe(2, 2, 4, 5, 5, 5);
    box_wireframe(3, 3, 6, 4, 4, 7);
    box_walls(0, 7, 0, 7, 7, 7);
    delay_ms(delay);

    fill(0);
    box_wireframe(0, 0, 0, 7, 7, 1);
    box_wireframe(1, 1, 2, 6, 6, 3);
    box_wireframe(2, 2, 4, 5, 5, 5);
    box_wireframe(3, 3, 6, 4, 4, 7);
}

static void zoom_pyramid_clear(int delay)
{
    box_walls(0, 0, 0, 7, 0, 7);
    delay_ms(delay);

    box_wireframe(0, 0, 0, 7, 0, 1);
    clrplane(AXIS_Y, 0);
    delay_ms(delay);

    clrplane(AXIS_Y, 1);
    box_walls(0, 2, 0, 7, 2, 7);
    delay_ms(delay);

    clrplane(AXIS_Y, 2);
    box_walls(0, 3, 0, 7, 3, 7);
    delay_ms(delay);

    clrplane(AXIS_Y, 3);
    box_walls(0, 4, 0, 7, 4, 7);
    delay_ms(delay);

    clrplane(AXIS_Y, 4);
    box_walls(0, 5, 0, 7, 5, 7);
    delay_ms(delay);

    clrplane(AXIS_Y, 5);
    box_walls(0, 6, 0, 7, 6, 7);
    delay_ms(delay);

    clrplane(AXIS_Y, 6);
    box_walls(0, 7, 0, 7, 7, 7);
    delay_ms(delay);

    clrplane(AXIS_Y, 7);
}

void effect_zoom_pyramid(int delay)
{
    zoom_pyramid(delay);
    delay_ms(10*delay);
    zoom_pyramid_clear(delay);
}
