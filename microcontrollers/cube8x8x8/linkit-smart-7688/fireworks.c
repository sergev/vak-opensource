#include <stdlib.h>
#include "cube.h"

void effect_fireworks(int seconds, int delay)
{
#define NFIREWORKS 30
    int i, f, e;
    int origin_x = 3;
    int origin_y = 3;
    int origin_z = 3;
    int rand_y, rand_x, rand_z;
    int slowrate, gravity;
    int iterations = seconds * 1000 / delay / (36 + 25);

    /* Particles and their position, x,y,z and their movement, dx,dy,dz.
     * Scaled by 100 for integer arith. */
    int particles[NFIREWORKS][6];

    fill(0);
    for (i=0; i<iterations; i++) {

        origin_x = rand() % 4;
        origin_y = rand() % 4;
        origin_z = rand() % 2;
        origin_z += 5;
        origin_x += 2;
        origin_y += 2;

        /* shoot a particle up in the air */
        for (e=0; e<origin_z; e++) {
            setvoxel(origin_x, origin_y, e);
            delay_ms(delay * (2 + 2*e));
            fill(0);
        }

        /* Fill particle array */
        for (f=0; f<NFIREWORKS; f++) {
            /* Position */
            particles[f][0] = origin_x * 100;
            particles[f][1] = origin_y * 100;
            particles[f][2] = origin_z * 100;

            rand_x = rand() % 200;
            rand_y = rand() % 200;
            rand_z = rand() % 200;

            /* Movement */
            particles[f][3] = 100 - rand_x;     /* dx */
            particles[f][4] = 100 - rand_y;     /* dy */
            particles[f][5] = 100 - rand_z;     /* dz */
        }

        /* explode */
        for (e=0; e<25; e++) {
            /* Coefficient = tan((e+0.1)/20) * 1000. */
            static const int coeff[25] = {
                5, 55, 105, 156, 207, 260, 314, 370, 428, 489,
                552, 620, 691, 768, 850, 940, 1039, 1149, 1273, 1413,
                1574, 1763, 1989, 2264, 2610,
            };

            slowrate = 100 + coeff[e];
            gravity = coeff[e] / 20;

            for (f=0; f<NFIREWORKS; f++) {
                particles[f][0] += particles[f][3] * 100 / slowrate;
                particles[f][1] += particles[f][4] * 100 / slowrate;
                particles[f][2] += particles[f][5] * 100 / slowrate;
                particles[f][2] -= gravity;

                setvoxel(particles[f][0] / 100,
                         particles[f][1] / 100,
                         particles[f][2] / 100);
            }
            delay_ms(delay);
            fill(0);
        }
    }
}
