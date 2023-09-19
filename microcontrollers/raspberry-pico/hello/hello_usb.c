#include <stdio.h>
#include <pico/stdlib.h>

int main()
{
    unsigned count = 0;

    stdio_init_all();
    for (;;) {
        printf("Hello, world! %u\n", ++count);
        sleep_ms(1000);
    }
}
