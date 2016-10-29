#include <stdlib.h>
#include "cube.h"

static void draw_positions_axis(char axis, unsigned char positions[64], int invert)
{
    int x, y, p;

    fill(0);

    for (x=0; x<8; x++) {
        for (y=0; y<8; y++) {
            if (invert) {
                p = (7-positions[(x*8)+y]);
            } else {
                p = positions[(x*8)+y];
            }

            if (axis == AXIS_Z)
                setvoxel(x, y, p);

            if (axis == AXIS_Y)
                setvoxel(x, p, y);

            if (axis == AXIS_X)
                setvoxel(p, y, x);
        }
    }
}

void effect_boxside_randsend_parallel(char axis, int origin, int delay,
    int mode)
{
    int i;
    int done;
    unsigned char cubepos[64];
    unsigned char pos[64];
    int notdone = 1;
    int notdone2 = 1;
    int sent = 0;

    for (i=0; i<64; i++) {
        pos[i] = 0;
    }

    while (notdone) {
        if (mode == 1) {
            notdone2 = 1;
            while (notdone2 && sent<64) {
                i = rand()%64;
                if (pos[i] == 0) {
                    sent++;
                    pos[i] += 1;
                    notdone2 = 0;
                }
            }
        } else if (mode == 2) {
            if (sent<64) {
                pos[sent] += 1;
                sent++;
            }
        }

        done = 0;
        for (i=0; i<64; i++) {
            if (pos[i] > 0 && pos[i] <7) {
                pos[i] += 1;
            }

            if (pos[i] == 7)
                done++;
        }

        if (done == 64)
            notdone = 0;

        for (i=0; i<64; i++) {
            if (origin == 0) {
                cubepos[i] = pos[i];
            } else {
                cubepos[i] = (7-pos[i]);
            }
        }

        delay_ms(delay);
        draw_positions_axis(axis, cubepos, 0);
    }
}

void effect_axis_updown_randsuspend(char axis, int delay, int sleep, int invert)
{
    unsigned char positions[64];
    unsigned char destinations[64];

    int i, px;

    /* Set 64 random positions */
    for (i=0; i<64; i++) {
        positions[i] = 0;       /* Set all starting positions to 0 */
        destinations[i] = rand()%8;
    }

    /* Loop 8 times to allow destination 7 to reach all the way */
    for (i=0; i<8; i++) {
        /* For every iteration, move all position
         * one step closer to their destination */
        for (px=0; px<64; px++) {
            if (positions[px] < destinations[px]) {
                positions[px]++;
            }
        }

        /* Draw the positions and take a nap */
        draw_positions_axis(axis, positions, invert);
        delay_ms(delay);
    }

    /* Set all destinations to 7 (opposite from the side they started out) */
    for (i=0; i<64; i++) {
        destinations[i] = 7;
    }

    /* Suspend the positions in mid-air for a while */
    delay_ms(sleep);

    /* Then do the same thing one more time */
    for (i=0; i<8; i++) {
        for (px=0; px<64; px++) {
            if (positions[px] < destinations[px]) {
                positions[px]++;
            }
            if (positions[px] > destinations[px]) {
                positions[px]--;
            }
        }
        draw_positions_axis(axis, positions, invert);
        delay_ms(delay);
    }
}
