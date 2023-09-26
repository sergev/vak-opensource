//
// Test ADD/SUB instructions and flags.
//
#include <stdio.h>
#include <pico/stdlib.h>

static inline __attribute__((always_inline))
int get_apsr(int c)
{
    int flags;

    asm volatile("mrs %0, apsr" : "=r" (flags) : "r" (c));
    return flags;
}

void add(int a, int b)
{
    int c = a + b;
    int flags = get_apsr(c);

    printf("%08x + %08x -> %08x flags %08x\n", a, b, c, flags);
}

void sub(int a, int b)
{
    int c = a - b;
    int flags = get_apsr(c);

    printf("%08x - %08x -> %08x flags %08x\n", a, b, c, flags);
}
