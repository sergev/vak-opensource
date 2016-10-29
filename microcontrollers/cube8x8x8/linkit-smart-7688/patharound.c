#include <stdlib.h>
#include "cube.h"

/*
 * circle, len 16, offset 28
 */
static void getpath(unsigned char path, unsigned char *destination, int length)
{
    static const unsigned char paths[44] = {
        0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00,
        0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x71,
        0x72,0x73,0x74,0x75,0x76,0x77,0x67,0x57,
        0x47,0x37,0x27,0x17,0x04,0x03,0x12,0x21,
        0x30,0x40,0x51,0x62,0x73,0x74,0x65,0x56,
        0x47,0x37,0x26,0x15
    };
    int offset = path ? 28 : 0;
    int i;

    for (i = 0; i < length; i++)
        destination[i] = paths[i+offset];
}

static void pathmove(unsigned char *path, int length)
{
    int i, z;
    unsigned char state;

    for (i=(length-1); i>=1; i--) {
        for (z=0; z<8; z++) {
            state = getvoxel(((path[(i-1)]>>4) & 0x0f), (path[(i-1)] & 0x0f), z);
            altervoxel(((path[i]>>4) & 0x0f), (path[i] & 0x0f), z, state);
        }
    }
    for (i=0; i<8; i++)
        clrvoxel(((path[0]>>4) & 0x0f), (path[0] & 0x0f), i);
}

void effect_rand_patharound(int seconds, int delay)
{
    int z, dz, i;
    unsigned char path[28];
    int iterations = seconds * 1000 / delay;

    getpath(0, path, 28);
    fill(0);

    z = 4;
    for (i = 0; i < iterations; i++) {
        dz = rand()%3 - 1;
        z += dz;

        if (z>7)
            z = 7;

        if (z<0)
            z = 0;

        pathmove(path, 28);
        setvoxel(0, 7, z);
        delay_ms(delay);
    }
}

void effect_arrow_patharound(int seconds, int delay)
{
    static unsigned char arrow[10] = {
        0x18, 0x3c, 0x7e, 0xff, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c,
    };
    int i, z, r;
    unsigned char path[28];
    int iterations = seconds * 1000 / delay;

    getpath(0, path, 28);
    fill(0);

    for (i = 0; i < iterations; i++) {
        pathmove(path, 28);

        r = i % 28;
        if (r < sizeof(arrow)) {
            r = arrow[r];
            for (z=0; z<8; z++) {
                if ((r >> z) & 1) {
                    setvoxel(0, 7, z);
                }
            }
        }
        delay_ms(delay);
    }
}
