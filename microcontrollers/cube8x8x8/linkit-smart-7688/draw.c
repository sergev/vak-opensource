/*
 *   Draw functions
 */
#include <stdio.h>
#include <sys/time.h>
#include "cube.h"

unsigned char cube[8][8];

int sensor_active;

static int sensor_state = -1;
static int sensor_timestamp;

/*
 * Delay loop in milliseconds.
 */
void delay_ms(unsigned duration)
{
    struct timeval t0, now;
    int z, msec, sensor;

    if (sensor_active)
        return;

    gettimeofday(&t0, 0);
    z = 0;
    for (;;) {
        /* Send layer data. Latch is active,
         * so previous layer is still displayed. */
        gpio_plane(cube[z]);

        /* Disable output, activate latch,
         * switch to next layer. */
        gpio_oe(0);
        gpio_le(0);
        gpio_le(1);
        gpio_layer(z);
        gpio_oe(1);

        /* Next layer. */
        z++;
        if (z >= CUBE_SIZE) {
            z = 0;

            /* Check time. */
            gettimeofday(&now, 0);
            msec = (now.tv_sec - t0.tv_sec) * 1000;
            msec += (now.tv_usec - t0.tv_usec) / 1000;
            if (msec >= duration)
                break;

            /* Poll sensor. */
            int timestamp = now.tv_sec*1000 + now.tv_usec/1000;
            sensor = gpio_sensor();
            if (sensor_state < 0) {
                /* Initial state. */
                sensor_state = sensor;
                sensor_timestamp = timestamp;
                continue;
            }
            /* Bounce suppression: 500 msec. */
            if (sensor != sensor_state && (timestamp - sensor_timestamp) > 500) {
                if (sensor_state >= 0) {
                    /* Sensor changed. */
                    if (sensor == 0) {
                        /* Active low.
                         * Remember the timestamp for bounce suppression. */
                        sensor_active = 1;
                        sensor_timestamp = timestamp;
                        break;
                    }
                }
                sensor_state = sensor;
            }
        }
    }
}

/*
 * This function validates that we are drawing inside the cube.
 */
static inline int inrange(int x, int y, int z)
{
    return x >= 0 && x < 8 && y >= 0 && y < 8 && z >= 0 && z < 8;
}

/*
 * Set a single voxel to ON
 */
void setvoxel(int x, int y, int z)
{
    if (inrange(x, y, z))
        cube[z][y] |= (1 << x);
}

/*
 * Set a single voxel to ON
 */
void clrvoxel(int x, int y, int z)
{
    if (inrange(x, y, z))
        cube[z][y] &= ~(1 << x);
}

/*
 * Get the current status of a voxel
 */
int getvoxel(int x, int y, int z)
{
    if (! inrange(x, y, z))
        return 0;

    return (cube[z][y] >> x) & 1;
}

/*
 * In some effect we want to just take bool and write it to a voxel
 * this function calls the apropriate voxel manipulation function.
 */
void altervoxel(int x, int y, int z, int state)
{
    if (state == 1) {
        setvoxel(x, y, z);
    } else {
        clrvoxel(x, y, z);
    }
}

/*
 * Makes sure x1 is alwas smaller than x2
 * This is usefull for functions that uses for loops,
 * to avoid infinite loops
 */
static void argorder(int ix1, int ix2, int *ox1, int *ox2)
{
    if (ix1 > ix2) {
        int tmp;
        tmp = ix1;
        ix1= ix2;
        ix2 = tmp;
    }
    *ox1 = ix1;
    *ox2 = ix2;
}

/*
 * Sets all voxels along a X/Y plane at a given point
 * on axis Z
 */
static void setplane_z(int z)
{
    int i;

    if (z>=0 && z<8) {
        for (i=0; i<8; i++)
            cube[z][i] = 0xff;
    }
}

/*
 * Clears voxels in the same manner as above
 */
static void clrplane_z(int z)
{
    int i;

    if (z>=0 && z<8) {
        for (i=0; i<8; i++)
            cube[z][i] = 0x00;
    }
}

static void setplane_x(int x)
{
    int z, y;

    if (x>=0 && x<8) {
        for (z=0; z<8; z++) {
            for (y=0; y<8; y++) {
                cube[z][y] |= (1 << x);
            }
        }
    }
}

static void clrplane_x(int x)
{
    int z;
    int y;

    if (x>=0 && x<8) {
        for (z=0; z<8; z++) {
            for (y=0; y<8; y++) {
                cube[z][y] &= ~(1 << x);
            }
        }
    }
}

static void setplane_y(int y)
{
    int z;

    if (y>=0 && y<8) {
        for (z=0; z<8; z++)
            cube[z][y] = 0xff;
    }
}

static void clrplane_y(int y)
{
    int z;

    if (y>=0 && y<8) {
        for (z=0; z<8; z++)
            cube[z][y] = 0x00;
    }
}

void setplane(char axis, int i)
{
    switch (axis)
    {
        case AXIS_X:
            setplane_x(i);
            break;

       case AXIS_Y:
            setplane_y(i);
            break;

       case AXIS_Z:
            setplane_z(i);
            break;
    }
}

void clrplane(char axis, int i)
{
    switch (axis)
    {
        case AXIS_X:
            clrplane_x(i);
            break;

       case AXIS_Y:
            clrplane_y(i);
            break;

       case AXIS_Z:
            clrplane_z(i);
            break;
    }
}

/*
 * Fill a value into all 64 byts of the cube buffer
 * Mostly used for clearing: fill(0x00)
 * or setting all on: fill(0xff)
 */
void fill(int pattern)
{
    int z, y;

    for (z=0; z<8; z++) {
        for (y=0; y<8; y++) {
            cube[z][y] = pattern;
        }
    }
}

/*
 * Returns a byte with a row of 1's drawn in it.
 * byteline(2,5) gives 0b00111100
 */
static char byteline(int start, int end)
{
    return (0xff << start) & ~(0xff << (end+1));
}

/*
 * Draw a box with all walls drawn and all voxels inside set
 */
void box_filled(int x1, int y1, int z1, int x2, int y2, int z2)
{
    int iy;
    int iz;

    argorder(x1, x2, &x1, &x2);
    argorder(y1, y2, &y1, &y2);
    argorder(z1, z2, &z1, &z2);

    for (iz=z1; iz<=z2; iz++) {
        for (iy=y1; iy<=y2; iy++) {
            cube[iz][iy] |= byteline(x1, x2);
        }
    }
}

/*
 * Darw a hollow box with side walls.
 */
void box_walls(int x1, int y1, int z1, int x2, int y2, int z2)
{
    int iy;
    int iz;

    argorder(x1, x2, &x1, &x2);
    argorder(y1, y2, &y1, &y2);
    argorder(z1, z2, &z1, &z2);

    for (iz=z1; iz<=z2; iz++) {
        for (iy=y1; iy<=y2; iy++) {
            if (iy == y1 || iy == y2 || iz == z1 || iz == z2) {
                cube[iz][iy] = byteline(x1, x2);
            } else {
                cube[iz][iy] |= ((0x01 << x1) | (0x01 << x2));
            }
        }
    }
}

/*
 * Draw a wireframe box. This only draws the corners and edges,
 * no walls.
 */
void box_wireframe(int x1, int y1, int z1, int x2, int y2, int z2)
{
    int iy;
    int iz;

    argorder(x1, x2, &x1, &x2);
    argorder(y1, y2, &y1, &y2);
    argorder(z1, z2, &z1, &z2);

    /* Lines along X axis */
    cube[z1][y1] = byteline(x1, x2);
    cube[z1][y2] = byteline(x1, x2);
    cube[z2][y1] = byteline(x1, x2);
    cube[z2][y2] = byteline(x1, x2);

    /* Lines along Y axis */
    for (iy=y1; iy<=y2; iy++) {
        setvoxel(x1, iy, z1);
        setvoxel(x1, iy, z2);
        setvoxel(x2, iy, z1);
        setvoxel(x2, iy, z2);
    }

    /* Lines along Z axis */
    for (iz=z1; iz<=z2; iz++) {
        setvoxel(x1, y1, iz);
        setvoxel(x1, y2, iz);
        setvoxel(x2, y1, iz);
        setvoxel(x2, y2, iz);
    }
}

/*
 * Divide by 10 and round.
 */
static int div10(int a)
{
    int b;

    b = a/10;
    a -= b*10;
    if (a >= 5)
        b++;
    return b;
}

/*
 * Get absolute value.
 */
static int iabs(int a)
{
    if (a < 0)
        a = -a;
    return a;
}

/*
 * Get a maximum of three integers.
 */
static int max3(int a, int b, int c)
{
    if (a < b)
        a = b;
    if (a < c)
        a = c;
    return a;
}

/*
 * Draw a line between any coordinates in 3d space.
 * Uses integer values for input, so dont expect smooth animations.
 */
void line(int x1, int y1, int z1, int x2, int y2, int z2, int le)
{
    int t, a, b, c, a1, b1, c1, i;

    a1 = x2-x1;
    b1 = y2-y1;
    c1 = z2-z1;
    t = max3(iabs(a1), iabs(b1), iabs(c1));
    if (t == 0)
        t = 1;
    a = x1*10;
    b = y1*10;
    c = z1*10;
    a1 = a1*10/t;
    b1 = b1*10/t;
    c1 = c1*10/t;
    for (i=0; i<t; i++) {
        altervoxel(div10(a), div10(b), div10(c), le);
        a += a1;
        b += b1;
        c += c1;
    }
    altervoxel(x2, y2, z2, le);
}

/*
 * Shift the entire contents of the cube along an axis
 * This is great for effects where you want to draw something
 * on one side of the cube and have it flow towards the other
 * side. Like rain flowing down the Z axiz.
 */
void shift(char axis, int direction)
{
    int i, x, y;
    int ii, iii;
    int state;

    for (i = 0; i < 8; i++) {
        if (direction == -1) {
            ii = i;
        } else {
            ii = (7-i);
        }

        for (x = 0; x < 8; x++) {
            for (y = 0; y < 8; y++) {
                if (direction == -1) {
                    iii = ii+1;
                } else {
                    iii = ii-1;
                }

                if (axis == AXIS_Z) {
                    state = getvoxel(x, y, iii);
                    altervoxel(x, y, ii, state);
                }

                if (axis == AXIS_Y) {
                    state = getvoxel(x, iii, y);
                    altervoxel(x, ii, y, state);
                }

                if (axis == AXIS_X) {
                    state = getvoxel(iii, y, x);
                    altervoxel(ii, y, x, state);
                }
            }
        }
    }

    if (direction == -1) {
        i = 7;
    } else {
        i = 0;
    }

    for (x = 0; x < 8; x++) {
        for (y = 0; y < 8; y++) {
            if (axis == AXIS_Z)
                clrvoxel(x, y, i);

            if (axis == AXIS_Y)
                clrvoxel(x, i, y);

            if (axis == AXIS_X)
                clrvoxel(i, y, x);
        }
    }
}
