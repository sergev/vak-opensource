#!/usr/bin/env python3
"""
Example from https://en.wikipedia.org/wiki/Borland_Graphics_Interface,
rewritten from C into Python.
"""
from sdl_bgi import *

initwindow(800, 800)
setbkcolor(BLACK)
cleardevice()
outtextxy(0, 0, "Drawing 1000 lines...")
for _ in range(1000):
    setcolor(COLOR(1 + random(255), 1 + random(255), 1 + random(255)))
    line(random(getmaxx()), random(getmaxy()), random(getmaxx()), random(getmaxy()))

getch()
closegraph()
