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
# Find minimal distance between patches of given color.
#
def find_min_distance(type):
    # Find coordinates of all points of given type.
    points = [(i % NUM_X, i // NUM_X) for i, val in enumerate(layout) if val == type]
    #print(f"points {type}: {points}")

    # Compute min distance.
    min_distance = float('inf')
    for i in range(len(points)):
        for j in range(i + 1, len(points)):
            ax, ay = points[i]
            bx, by = points[j]
            distance = math.sqrt(((ax - bx)**2) + ((ay - by)**2))
            if distance < min_distance:
                min_distance = distance
    return min_distance

def find_mean_distance(type):
    # Find coordinates of all points of given type.
    points = [(i % NUM_X, i // NUM_X) for i, val in enumerate(layout) if val == type]
    #print(f"points {type}: {points}")

    # Compute distance.
    distance = 0.0
    for i in range(len(points)):
        for j in range(i + 1, len(points)):
            ax, ay = points[i]
            bx, by = points[j]
            distance += math.sqrt(((ax - bx)**2) + ((ay - by)**2))
    return distance

#
# Select random patch.
#
def random_index():
    return random.randint(0, NUM_X * NUM_Y - 1)

#
# Exchange two patches with given indices.
#
def swap_patches(a, b):
    t = layout[a]
    layout[a] = layout[b]
    layout[b] = t

#
# Exchange two arbitrary patches.
# The first patch should have the given color.
# The second patch must have different color.
# Return indices of these patches.
#
def swap_two_patches(type):
    a = random_index()
    while layout[a] != type:
        a = random_index()
    while True:
        b = random_index()
        if layout[b] != layout[a]:
            swap_patches(a, b)
            return (a, b)

#
# Try to swap two arbitrary patches.
# Return true when it helps.
#
last_cost = 0
def find_better_layout(type, object_func):
    global last_cost
    (a, b) = swap_two_patches(type)

    # Find minimal distance between patches of minority colors.
    dist_white     = object_func('w')
    dist_violet    = object_func('v')
    dist_lilac     = object_func('l')
    dist_turquoise = object_func('q')
    dist_teal      = object_func('t')

    # Use a product of minimal distances.
    cost = dist_white * dist_violet * dist_lilac * dist_turquoise * dist_teal

    if cost <= last_cost:
        # Restore layout
        swap_patches(a, b)
        return False

    print(a, b, f"-- w{dist_white} * v{dist_violet} * l{dist_lilac} * q{dist_turquoise} * q{dist_teal} -> {cost}")
    last_cost = cost
    return True

#
# Optimize the layout for patches of given color..
# Return false when done.
#
def optimize(type, object_func):
    # Make thousand attempts.
    for _ in range(10000):
        if find_better_layout(type, object_func):
            return True
    return False

random.shuffle(layout)

while optimize('w', find_mean_distance):
    pass
while optimize('v', find_mean_distance):
    pass
while optimize('l', find_mean_distance):
    pass
while optimize('q', find_mean_distance):
    pass
while optimize('t', find_mean_distance):
    pass

while optimize('w', find_min_distance):
    pass
while optimize('v', find_min_distance):
    pass
while optimize('l', find_min_distance):
    pass
while optimize('q', find_min_distance):
    pass
while optimize('t', find_min_distance):
    pass

print("Done.")
draw()
getch()

closegraph()
