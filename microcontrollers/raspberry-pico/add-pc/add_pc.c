//
// Figure out result of "add pc, rN" instruction.
//
#include <stdio.h>
#include <pico/stdlib.h>

void add_pc(int n)
{
    unsigned a, b, c, d;

    asm volatile(
        "movs %0, #0 \n"
        "movs %1, #0 \n"
        "movs %2, #0 \n"
        "movs %3, #0 \n"
        "add pc, %4 \n"
        "movs %0, #1 \n"
        "movs %1, #1 \n"
        "movs %2, #1 \n"
        "movs %3, #1 \n"
        : "=r" (a), "=r" (b), "=r" (c), "=r" (d) : "r" (n)
    );
    printf("n = %u, abcd = %u %u %u %u\n", n, a, b, c, d);
}
