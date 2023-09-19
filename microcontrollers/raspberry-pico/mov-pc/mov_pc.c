//
// Figure out result of "mov rN, pc" instruction.
//
#include <stdio.h>
#include <pico/stdlib.h>

int main()
{
    unsigned count = 0;

    stdio_init_all();
    for (;;) {
        printf("------------ %u\n", ++count);

        unsigned pc, base;

        asm volatile(

            "      adr %1, base \n"
            "base: mov %0, pc \n"

            : "=r" (pc), "=r" (base)
        );

        printf("base = %08x\n", base);
        printf("pc   = %08x\n", pc);

        sleep_ms(1000);
    }
}
