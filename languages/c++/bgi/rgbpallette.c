/*
  BGI library implementation for Microsoft(R) Windows(TM)
  Copyright (C) 2006  Daniil Guitelson

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <graphics.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int gd = DETECT, gm;
    g_palettetype pal;
    int i, ht, y, xmax;

    initgraph(&gd, &gm, "");

    getpalette(&pal);

    for (i = 0; i < pal.size; i++)
        setrgbpalette(pal.colors[i], i * 16, i * 16, i * 16);

    ht = (getmaxy() + 1) / 16;
    xmax = getmaxx();
    y = 0;
    for (i = 0; i < pal.size; i++) {
        setcolor(i);
        setfillstyle(SOLID_FILL, i);
        bar(0, y, xmax, y + ht);
        y += ht;
    }
    line(0, 0, getmaxx(), getmaxy());
    readkey();
    closegraph();
    return 0;
}
