#include <graphics.h>
#include <stdio.h>

int main()
{
    int gd = DETECT, gm = 0;
    int c;
    initgraph(&gd, &gm, "DISABLE_DEBUG");
    do {
        c = readkey();
        if (c == 0) {
            c = readkey();
            printf("Ext key: %i\n", c);
        } else {
            printf("Std key: %i\n", c);
        }
    } while (c != KEY_ESCAPE);
    closegraph();
}