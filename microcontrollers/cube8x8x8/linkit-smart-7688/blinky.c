#include "cube.h"

void effect_blinky(int repeat, int mindelay, int maxdelay)
{
    int i, r;

    fill(0);

    for (r=0; r<repeat; r++) {
        i = maxdelay;
        while (i >= mindelay) {
            fill(0xff);
            delay_ms(mindelay);

            fill(0);
            delay_ms(i);

            i = i * 8/10;
        }

        i = mindelay;
        while (i <= maxdelay) {
            fill(0xff);
            delay_ms(mindelay);

            fill(0);
            delay_ms(i);

            i = i * 12/10;
        }
    }
}
