/*
 * Include processor definitions.
 */
#include "pic32mz.h"

#define MHZ     200             /* CPU clock in MHz. */

/*
 * Main entry point at bd001000.
 * Setup stack pointer and $gp registers, and jump to main().
 */
asm ("          .section .startup");
asm ("          .globl _start");
asm ("          .type _start, function");
asm ("_start:   la      $sp, _estack");
asm ("          la      $ra, main");
asm ("          la      $gp, _gp");
asm ("          jr      $ra");
asm ("          .text");

/*
 * Secondary entry point at bd000000.
 */
asm ("          .section .exception");
asm ("          .org    0xf8");
asm ("_ebase:   .word   0x9d000000");           /* EBase value. */
asm ("_imgptr:  .word   -1");                   /* Image header pointer. */
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
    /* Use LED pins as output. */
    TRISGCLR = 1 << 6;      /* LED1: G6 */
    TRISDCLR = 1 << 4;      /* LED1: D4 */
    TRISBCLR = 1 << 11;     /* LED1: B11 */
    TRISGCLR = 1 << 15;     /* LED1: G15 */

    LATGCLR = 1 << 6;       /* Clear pin PG6. */
    LATDCLR = 1 << 4;       /* Clear pin PD4. */
    LATBCLR = 1 << 11;      /* Clear pin PB11. */
    LATGCLR = 1 << 15;      /* Clear pin PG15. */

    while (1) {
        LATGINV = 1 << 6;   /* Invert pin PG6. */
        udelay (200000);    /* Delay. */

        LATDINV = 1 << 4;   /* Invert pin PD4. */
        udelay (200000);    /* Delay. */

        LATBINV = 1 << 11;  /* Invert pin PB11. */
        udelay (200000);    /* Delay. */

        LATGINV = 1 << 15;  /* Invert pin PG15. */
        udelay (200000);    /* Delay. */
    }
}
