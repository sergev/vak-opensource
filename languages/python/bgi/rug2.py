#!/usr/bin/env python3
from sdl_bgi import *
import random, time, math

# Number of patches
NUM_X = 8
NUM_Y = 10

SIZE_X = 50
SIZE_Y = 50

UPPER_LEFT = 0
UPPER_RIGHT = NUM_X - 1
LOWER_LEFT = NUM_X * (NUM_Y - 1)
LOWER_RIGHT = (NUM_X * NUM_Y) - 1

layout = ['w'] * 13 + \
         ['v'] * 13 + \
         ['l'] * 7 + \
         ['q'] * 7 + \
         ['t'] * 40

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
# Exchange two patches with given indices.
#
def swap_patches(a, b):
    t = layout[a]
    layout[a] = layout[b]
    layout[b] = t

#
# Select random patch.
#
def random_index():
    return random.randint(0, NUM_X * NUM_Y - 1)

#
# Check for corner.
#
def is_corner(index):
    return (index == UPPER_LEFT) or (index == UPPER_RIGHT) or (index == LOWER_LEFT) or (index == LOWER_RIGHT)

#
# Set corner to teal color.
#
def fix_corner(index):
    if layout[index] != 't':
        while True:
            dest_index = random_index()
            if layout[dest_index] == 't' and not is_corner(dest_index):
                swap_patches(index, dest_index)
                return

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
# Try to move a patch with given index.
# Return true when it helps.
#
def move_patch(src_index, src_type):

    src_cost = find_min_distance(src_index, src_type)
    best_cost = 0
    best_dest = 0

    for dest_index, dest_type in enumerate(layout):
        if dest_type == src_type or is_corner(dest_index):
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
        print(f"{src_type} {src_index} {best_dest}: {src_cost:.3} -> {best_cost:.3}")
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
fix_corner(UPPER_LEFT)
fix_corner(UPPER_RIGHT)
fix_corner(LOWER_LEFT)
fix_corner(LOWER_RIGHT)
while optimize():
    print("---")
print("Done.")
draw()
getch()

closegraph()
