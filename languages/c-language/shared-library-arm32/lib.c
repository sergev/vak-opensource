#include "lib.h"

void say(const char *message)
{
    asm (
        "mov     r1, %0 \n" // message
        "mov     r0, #1 \n" // stdout
        "mov     r2, 4 \n"  // length
        "mov     r7, #4\n"  // SYS_write
        "svc     #0"
    : : "r"(message));
}
