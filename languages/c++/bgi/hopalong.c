/* hopalong.c  -*- C -*-
 *
 * To compile:
 * gcc -o hopalong hopalong.c -lSDL_bgi -lSDL2
 *
 * An implementation of Barry Martin's algorithm.
 * By Guido Gonzato, May 2015.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <graphics.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// -----

#define sgn(n) ((n < 0) ? -1 : 1)

// -----

int main(int argc, char *argv[])
{
    int gd, gm, stop;
    unsigned int seed;
    unsigned long int counter;
    float j, k, x, y, xx, xp, yp, r, xoffs, yoffs;

    if (argc == 2)
        seed = atoi(argv[1]); // no checks!
    else {
        printf("Seed: ");
        j = scanf("%d", &seed);
    }
    srand(seed);

    initwindow(800, 800);

    setbkcolor(BLACK);
    cleardevice();
    setcolor(YELLOW);
    outtextxy(0, 0, "Press a key or click to exit: ");

    xoffs = getmaxx() / 2;
    yoffs = getmaxy() / 2;
    j = random(100);
    k = random(100);
    x = y = xx = xp = yp = r = 0.0;
    stop = counter = 0;
    setcolor(COLOR(random(256), random(256), random(256)));

    while (!stop) {
        xx = y - sgn(x) * sqrt(fabs(k * x - 1));
        y = j - x;
        x = xx;
        xp = x * 2 + xoffs;
        yp = y * 2 + yoffs;
        _putpixel(xp, yp);
        if (++counter == 50000) {
            refresh();
            counter = 0;
            setcolor(COLOR(random(256), random(256), random(256)));
            refresh();
            if (ismouseclick(WM_LBUTTONDOWN) || xkbhit())
                stop = 1;
        }
    }

    closegraph();
    if (SDL_KEYDOWN == eventtype())
        puts("Key pressed.");
    else
        puts("Mouse clicked.");

    return 0;
}

// ----- end of file hopalong.c
