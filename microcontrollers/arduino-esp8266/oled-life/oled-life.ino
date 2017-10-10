/*
 * Convey's Life on 128x32 display.
 *
 * Copyright (C) 2017 Serge Vakulenko, <vak@besm6.org>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 */
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//
// On W8266 module, OLED reset signal is connected to pin D0.
//
Adafruit_SSD1306 display(D0);

#define MX      128     // x-side length
#define MY      32      // y-side length

char mem[MX*MY], mem2[MX*MY];
char *pm = mem, *pm2 = mem2;

void setup()
{
    // Initialize with the I2C addr 0x3C (for the 128x32)
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

    // Clear the buffer.
    display.clearDisplay();

    // Initialize pseudo-random number generator.
    randomSeed(analogRead(0));

    // Randomize the cells.
    int i;
    for (i=0; i<MX*MY/3; i++) {
        int x = random(MX);
        int y = random(MY);
        pm[x*MY + y] = 1;
        display.drawPixel(x, y, WHITE);
    }

    // Visualize the result.
    display.display();
    delay(100);
}

//
// Compute one cell.
// Return 1 when the cell is alive.
//
int compute_cell(int x, int y)
{
    int i = 0;
    int x0 = (x ? x : MX) - 1;
    int x1 = (x<MX-1 ? x+1 : 0);
    int y0 = (y ? y : MY) - 1;
    int y1 = (y<MY-1 ? y+1 : 0);

    if (pm[x0*MY + y0]) i++;
    if (pm[x0*MY + y1]) i++;
    if (pm[x0*MY + y])  i++;
    if (pm[x1*MY + y0]) i++;
    if (pm[x1*MY + y1]) i++;
    if (pm[x1*MY + y])  i++;
    if (pm[x*MY  + y0]) i++;
    if (pm[x*MY  + y1]) i++;

    if (i == 2)
        return pm[x*MY + y];
    if (i == 3)
        return 1;
    return 0;
}

void loop()
{
    int x, y;
    char *p;

    // Compute next generation.
    for (y=0; y<MY; y++) {
        for (x=0; x<MX; x++) {
            int c = compute_cell(x, y);
            pm2[x*MY+y] = c;
            if (c != pm[x*MY + y]) {
                display.drawPixel(x, y, c ? WHITE : BLACK);
            }
        }
    }
    p = pm;
    pm = pm2;
    pm2 = p;

    // Visualize the result.
    display.display();
    delay(100);
}
