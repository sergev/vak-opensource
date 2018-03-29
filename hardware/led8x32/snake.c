/*
 * Snake demo.
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

#define LENGTH  8

int x[LENGTH], y[LENGTH];
int ptr, nextPtr;

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

int equal(int ptrA, int ptrB)
{
    return x[ptrA] == x[ptrB] && y[ptrA] == y[ptrB];
}

int occupied(int ptrA)
{
    for (int ptrB = 0 ; ptrB < LENGTH; ptrB++) {
        if (ptrA != ptrB && equal(ptrA, ptrB)) {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (led_init() < 0) {
        goto out;
    }

    // Reset all variables
    for (ptr = 0; ptr < LENGTH; ptr++) {
        x[ptr] = WIDTH / 2;
        y[ptr] = HEIGHT / 2;
    }
    nextPtr = 0;

    srandom(time(0));
    for (;;) {
        // Shift pointer to the next segment
        ptr = nextPtr;
        nextPtr = (ptr + 1) % LENGTH;

        // Draw the head of the snake
        draw_pixel(x[ptr], y[ptr], 1);
        if (write_bitmap() < 0)
            break;

        // Sleep for 100 msec.
        usleep(100000);

        if (! occupied(nextPtr)) {
            // Remove the tail of the snake
            draw_pixel(x[nextPtr], y[nextPtr], 0);
        }

        for (int attempt = 0; attempt < 10; attempt++) {

            // Jump at random one step up, down, left, or right
            switch (random() % 4) {
            case 0: x[nextPtr] = constrain(x[ptr] + 1, 0, WIDTH-1); y[nextPtr] = y[ptr]; break;
            case 1: x[nextPtr] = constrain(x[ptr] - 1, 0, WIDTH-1); y[nextPtr] = y[ptr]; break;
            case 2: y[nextPtr] = constrain(y[ptr] + 1, 0, HEIGHT-1); x[nextPtr] = x[ptr]; break;
            case 3: y[nextPtr] = constrain(y[ptr] - 1, 0, HEIGHT-1); x[nextPtr] = x[ptr]; break;
            }

            if (! occupied(nextPtr)) {
                break; // The spot is empty, break out the for loop
            }
        }
    }

out:
    led_close();
    return 0;
}
