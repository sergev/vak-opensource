#include "lib.h"

void say(const char *message)
{
    asm (
        "mov     x1, %0 \n" // message
        "mov     x0, #1 \n" // stdout
        "mov     x2, 4 \n"  // length
        "mov     x16, #4\n" // SYS_write
        "svc     0x80"
    : : "r"(message));
}
