//
// Test ADD/SUB instructions and flags.
//
#include <stdio.h>
#include <pico/stdlib.h>

void add(int a, int b);
void sub(int a, int b);

int main()
{
    unsigned count = 0;

    stdio_init_all();
    for (;;) {
        printf("------------ %u\n", ++count);

        add(0x7502fe1f, 0xc015a134);
        add(0x8c832ed2, 0xd306fde3);
        add(0x9b74a3db, 0x776dd37e);
        add(0xce671cdd, 0x549c13ba);

        sub(0x7502fe1f, 0xc015a134);
        sub(0x8c832ed2, 0xd306fde3);
        sub(0x9b74a3db, 0x776dd37e);
        sub(0xce671cdd, 0x549c13ba);

        sleep_ms(1000);
    }
}
