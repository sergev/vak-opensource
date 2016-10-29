/*
 * Include processor definitions.
 */
#include "pic32mx.h"

#define MHZ     80              /* CPU clock is 80 MHz. */

/*
 * Main entry point at bd000000.
 * Setup stack pointer and $gp registers, and jump to main().
 */
asm ("          .section .exception");
asm ("          .globl _start");
asm ("          .type _start, function");
asm ("_start:   la      $sp, _estack");
asm ("          la      $ra, main");
asm ("          la      $gp, _gp");
asm ("          jr      $ra");
asm ("          .text");

/*
 * Delay for a given number of microseconds.
 * The processor has a 32-bit hardware Count register,
 * which increments at half CPU rate.
 * We use it to get a precise delay.
 */
void udelay (unsigned usec)
{
    unsigned now = mfc0 (C0_COUNT, 0);
    unsigned final = now + usec * MHZ / 2;

    for (;;) {
        now = mfc0 (C0_COUNT, 0);

        /* This comparison is valid only when using a signed type. */
        if ((int) (now - final) >= 0)
            break;
    }
}

int main()
{
    /* Set memory wait states, for speedup. */
    CHECON = 2;
    BMXCONCLR = 0x40;
    CHECONSET = 0x30;

    /* Enable cache for kseg0 segment. */
    int config = mfc0 (C0_CONFIG, 0);
    mtc0 (C0_CONFIG, 0, config | 3);

    /* Initialize coprocessor 0. */
    mtc0 (C0_COUNT, 0, 0);
    mtc0 (C0_COMPARE, 0, -1);
    mtc0 (C0_EBASE, 1, 0x9fc00000);     /* Vector base */
    mtc0 (C0_INTCTL, 1, 1 << 5);        /* Vector spacing 32 bytes */
    mtc0 (C0_CAUSE, 0, 1 << 23);        /* Set IV */
    mtc0 (C0_STATUS, 0, 0);             /* Clear BEV */

    /* Use all ports as digital. */
    //AD1PCFG = ~0;
    LATD = 0;

    /* Use pins D0, D1, D2 as output: LED control. */
    TRISDCLR = 1 << 0;
    TRISDCLR = 1 << 1;
    TRISDCLR = 1 << 2;

    for (;;) {
        /* Invert pin D0. */
        LATDINV = 1 << 0;
        udelay (100000);

        /* Invert pin D1. */
        LATDINV = 1 << 1;
        udelay (100000);

        /* Invert pin D2. */
        LATDINV = 1 << 2;
        udelay (100000);
    }
}
