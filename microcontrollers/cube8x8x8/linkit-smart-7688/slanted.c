#include "cube.h"

void effect_slanted_plane(int delay)
{
    int i, j, an[8];

    for (j=7; j<15; j++)
        an[j-7] = j;
    for (i=0; i<=16; i++) {
        for (j=0; j<8; j++) {
            if (an[j] < 8 && an[j] >= 0)
                line(0, an[j], j, 7, an[j], j, 1);
        }
        for (j=0; j<8; j++) {
            if ((an[j]+1) < 8 && an[j] >= 0)
                line(0, an[j]+1, j, 7, an[j]+1, j, 0);
        }
        for (j=0; j<8; j++) {
            if (an[j] > 0)
                an[j]--;
        }
        delay_ms(delay);
    }
    for (j=0; j<8; j++)
        an[j] = 1-j;
    for (i=0; i<=16; i++) {
        for (j=0; j<8; j++) {
            if (an[j] < 8 && an[j] >= 0)
                line(0, an[j], j, 7, an[j], j, 1);
        }
        for (j=0; j<8; j++) {
            if ((an[j]-1) < 7 && an[j] > 0)
                line(0, an[j]-1, j, 7, an[j]-1, j, 0);
        }
        for (j=0; j<8; j++) {
            if (an[j] < 7)
                an[j]++;
        }
        delay_ms(delay);
    }
}
