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

int main()
{
    int gd = DETECT, gm;
    int sx, sy, ex, ey, p = 0, page = 0;
    initgraph(&gd, &gm, "");
    setwritemode(XOR_PUT);
    while (!xkbhit()) {
        if (ismouseclick(WM_LBUTTONDOWN)) {
            if (p == 0) {
                p = 1;
                setcolor((rand() % MAXCOLORS) + 1);
                setlinestyle(rand() % USERBIT_LINE, 0, rand() % 4);
                ex = sx = mousex();
                ey = sy = mousey();
            } else if (ex != mousex() || ey != mousey()) {
                line(sx, sy, ex, ey);
                ex = mousex();
                ey = mousey();
                line(sx, sy, ex, ey);
                delay(10);
            }
        } else {
            if (p) {
                setwritemode(COPY_PUT);
                line(sx, sy, ex, ey);
                setwritemode(XOR_PUT);
                p = 0;
            }
        }
    }
    closegraph();
}
