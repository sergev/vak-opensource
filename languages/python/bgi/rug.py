#!/usr/bin/env python3
from sdl_bgi import *
import random

num_white     = 13
num_violet    = 13
num_lilac     = 7
num_turquoise = 7
num_teal      = 40

layout = ['a'] * num_white + \
         ['v'] * num_violet + \
         ['l'] * num_lilac + \
         ['q'] * num_turquoise + \
         ['t'] * num_teal

initwindow(800, 1000)
setbkcolor(BLACK)
cleardevice()
#outtextxy(0, 0, "Drawing a rug...")

#color_white     = COLOR(230, 230, 230)
#color_violet    = COLOR(75, 72, 97)
#color_lilac     = COLOR(170, 148, 174)
#color_turquoise = COLOR(132, 178, 198)
#color_teal      = COLOR(63, 99, 111)

#print("white:", color_white)
#print("violet:", color_violet)
#print("lilac:", color_lilac)
#print("turquoise:", color_turquoise)
#print("teal:", color_teal)

def set_color(ch):
    if ch == 'a':
        print('white')
        setcolor(COLOR(230, 230, 230))
        setfillstyle(1, COLOR(230, 230, 230))
        return
    if ch == 'v':
        print('violet')
        setcolor(COLOR(75, 72, 97))
        setfillstyle(1, COLOR(75, 72, 97))
        return
    if ch == 'l':
        print('lilac')
        setcolor(COLOR(170, 148, 174))
        setfillstyle(1, COLOR(170, 148, 174))
        return
    if ch == 'q':
        print('turquoise')
        setcolor(COLOR(132, 178, 198))
        setfillstyle(1, COLOR(132, 178, 198))
        return
    if ch == 't':
        print('teal')
        setcolor(COLOR(63, 99, 111))
        setfillstyle(1, COLOR(63, 99, 111))
        return

def draw():
    index = 0
    for y in range(0, 1000, 100):
        for x in range(0, 800, 100):
            set_color(layout[index])
            #circle(x + 50, y + 50, 49)
            fillellipse(x + 50, y + 50, 49, 49)
            index += 1

while True:
    random.shuffle(layout)
    draw()
    getch()

closegraph()
