/*
 * Demo for LED cube 8x8x8.
 * Test backlight LEDs.
 */
#include <stdio.h>
#include <unistd.h>
#include "cube.h"

int main()
{
    gpio_init();
    for (;;) {
        gpio_backlight_upper(1);
        usleep(500000);
        gpio_backlight_upper(0);
        gpio_backlight_lower(1);
        usleep(500000);
        gpio_backlight_lower(0);
        printf(".");
        fflush(stdout);
    }
    return 0;
}
