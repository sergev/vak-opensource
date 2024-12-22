#include <stdio.h>

#define SET_TPIDR(value) \
  ({ __asm __volatile ("msr tpidr_el0, %0" : : "r" (value)); NULL; })

#define GET_TPIDR() \
  ({ size_t value; __asm __volatile ("mrs %0, tpidr_el0" : "=r" (value)); value; })

int main()
{
    printf("Start.\n");

    size_t old_tpidr = GET_TPIDR();
    printf("old_tpidr = 0x%lx\n", old_tpidr);

    size_t tpidr = 0;
    SET_TPIDR(0x1122334455);
    tpidr = GET_TPIDR();
    SET_TPIDR(old_tpidr);
    printf("tpidr = 0x%lx\n", tpidr);

    printf("Done.\n");
}
