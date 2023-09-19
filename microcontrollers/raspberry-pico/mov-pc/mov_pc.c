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

        unsigned base, pc1, pc2, pc3, pc4;

        asm volatile(

            "adr %0, base \n"
        "base: \n"
            "mov %1, pc \n"
            "mov %2, pc \n"
            "mov %3, pc \n"
            "mov %4, pc \n"

            : "=r" (base), "=r" (pc1), "=r" (pc2), "=r" (pc3), "=r" (pc4)
        );

        printf("base = %08x\n", base);
        printf("pc1  = %08x\n", pc1);
        printf("pc2  = %08x\n", pc2);
        printf("pc3  = %08x\n", pc3);
        printf("pc4  = %08x\n", pc4);

        sleep_ms(1000);
    }
}
