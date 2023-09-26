//
// Figure out result of "add pc, rN" instruction.
//
#include <stdio.h>
#include <pico/stdlib.h>

void add_pc(int n);

int main()
{
    unsigned count = 0;

    stdio_init_all();
    for (;;) {
        printf("------------ %u\n", ++count);

        add_pc(0);
        add_pc(2);
        add_pc(4);
        add_pc(6);

        sleep_ms(1000);
    }
}
