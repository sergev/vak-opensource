/*
 * MadFly demo.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "max7219.h"

#define WIDTH   32
#define HEIGHT  8

long bitmap[HEIGHT];

int constrain(int x, int a, int b)
{
    if (x < a)
        return a;

    if (b < x)
        return b;

    return x;
}

//
// Send bitmap to display.
//
int write_bitmap()
{
    int i;

    for (i=0; i<HEIGHT; i++) {
        if (led_write(i, bitmap[i]) < 0)
            return -1;
    }
    return 0;
}

void draw_pixel(int x, int y, int on)
{
    if (on)
        bitmap[y] |= 1 << x;
    else
        bitmap[y] &= ~(1 << x);
}

int main(int argc, char *argv[])
{
    int x, y, xNext, yNext, xLast, yLast;

    if (led_init() < 0) {
        goto out;
    }

    x = WIDTH / 2;
    y = HEIGHT / 2;
    xLast = x;
    yLast = y;
    srandom(time(0));
    for (;;) {
        draw_pixel(x, y, 1);
        if (write_bitmap() < 0)
            break;

        // Sleep for 20 msec.
        usleep(20000);

        // Erase the old position of our dot.
        draw_pixel(x, y, 0);

        do {
            switch (random() % 4) {
            default:
            case 0: xNext = constrain(x + 1, 0, WIDTH-1);  yNext = y; break;
            case 1: xNext = constrain(x - 1, 0, WIDTH-1);  yNext = y; break;
            case 2: yNext = constrain(y + 1, 0, HEIGHT-1); xNext = x; break;
            case 3: yNext = constrain(y - 1, 0, HEIGHT-1); xNext = x; break;
            }
            // Repeat until we find a new coordinate
        } while ((x == xNext && y == yNext) ||
                 (xNext == xLast && yNext == yLast));

        xLast = x;
        yLast = y;
        x = xNext;
        y = yNext;
    }

out:
    led_close();
    return 0;
}
