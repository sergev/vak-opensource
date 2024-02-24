#!/usr/bin/env python3
from sdl_bgi import *
import random, time, math

# Number of patches
NUM_X = 8
NUM_Y = 10

SIZE_X = 50
SIZE_Y = 50

num_white     = 13
num_violet    = 13
num_lilac     = 7
num_turquoise = 7
num_teal      = 40

layout = ['w'] * num_white + \
         ['v'] * num_violet + \
         ['l'] * num_lilac + \
         ['q'] * num_turquoise + \
         ['t'] * num_teal

initwindow(NUM_X * SIZE_X, NUM_Y * SIZE_Y)
setbkcolor(BLACK)
cleardevice()
#outtextxy(0, 0, "Drawing a rug...")

#
# Draw one patch at given coordinates.
#
def draw_patch(x, y, type):

    # Set color of this patch.
    color = COLOR(0, 0, 0)
    setcolor(color)
    if type == 'w':
        #print('white')
        color = COLOR(230, 230, 230)
    elif type == 'v':
        #print('violet')
        color = COLOR(75, 72, 97)
    elif type == 'l':
        #print('lilac')
        color = COLOR(170, 148, 174)
    elif type == 'q':
        #print('turquoise')
        color = COLOR(132, 178, 198)
    elif type == 't':
        #print('teal')
        color = COLOR(63, 99, 111)
    setfillstyle(1, color)

    RX = SIZE_X // 2
    RY = SIZE_Y // 2
    fillellipse(x + RX, y + RY, RX, RY)

#
# Draw the whole rug.
#
def draw():
    index = 0
    for y in range(0, NUM_Y * SIZE_Y, SIZE_Y):
        for x in range(0, NUM_X * SIZE_X, SIZE_X):
            draw_patch(x, y, layout[index])
            index += 1

#
# Find minimal distance between given patch and others of the same color.
#
def find_min_distance(index, type):
    min_distance = float('inf')
    ax = index % NUM_X
    ay = index // NUM_X
    for dest_index, dest_type in enumerate(layout):
        if dest_type == type and dest_index != index:
            bx = dest_index % NUM_X
            by = dest_index // NUM_X
            distance = math.sqrt(((ax - bx)**2) + ((ay - by)**2))
            if distance < min_distance:
                min_distance = distance
    return min_distance

#
# Exchange two patches with given indices.
#
def swap_patches(a, b):
    t = layout[a]
    layout[a] = layout[b]
    layout[b] = t

#
# Try to move a patch with given index.
# Return true when it helps.
#
def move_patch(src_index, src_type):

    src_cost = find_min_distance(src_index, src_type)
    best_cost = 0
    best_dest = 0

    for dest_index, dest_type in enumerate(layout):
        if dest_type == src_type:
            continue

        if dest_type != 't':
            # No need to estimate teal color
            dest_cost = find_min_distance(dest_index, dest_type)

        swap_patches(src_index, dest_index)
        if dest_type != 't':
            new_cost = find_min_distance(src_index, dest_type)
            if new_cost < dest_cost:
                # It's worse for destination patch
                swap_patches(src_index, dest_index)
                #print(f"{src_index} {dest_index} -- worse for destination")
                continue

        new_cost = find_min_distance(dest_index, src_type)
        swap_patches(src_index, dest_index)
        if new_cost <= src_cost:
            # No enhancement for source patch
            #print(f"{src_index} {dest_index} -- no enhancement {src_cost}")
            continue

        if new_cost > best_cost:
            # Remember this variant
            best_cost = new_cost
            best_dest = dest_index

    if best_cost > 0:
        swap_patches(src_index, best_dest)
        print(f"{src_type} {src_index} {best_dest} -> {best_cost}")
        return True

    return False

#
# Optimize the layout.
# Return false when no enhancement anymore.
#
def optimize():
    count = 0
    for index, type in enumerate(layout):
        if type != 't':
            # Skip teal color
            if move_patch(index, type):
                count += 1
    return count > 0

random.shuffle(layout)
while optimize():
    pass
print("Done.")
draw()
getch()

closegraph()
