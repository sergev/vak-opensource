/*
 * Include processor definitions.
 */
#include "pic32mz.h"

#define MHZ     200             /* CPU clock. */

/*
 * Chip configuration.
 */
PIC32_DEVCFG (
    DEVCFG0_JTAG_DISABLE |      /* Disable JTAG port */
    DEVCFG0_TRC_DISABLE,        /* Disable trace port */

    /* Using primary oscillator with external crystal 24 MHz.
     * PLL multiplies it to 200 MHz. */
    DEVCFG1_FNOSC_SPLL |        /* System clock supplied by SPLL */
    DEVCFG1_POSCMOD_EXT |       /* External generator */
    DEVCFG1_FCKS_ENABLE |       /* Enable clock switching */
    DEVCFG1_FCKM_ENABLE |       /* Enable fail-safe clock monitoring */
    DEVCFG1_IESO |              /* Internal-external switch over enable */
    DEVCFG1_CLKO_DISABLE,       /* CLKO output disable */

    DEVCFG2_FPLLIDIV_3 |        /* PLL input divider = 3 */
    DEVCFG2_FPLLRNG_5_10 |      /* PLL input range is 5-10 MHz */
    DEVCFG2_FPLLMULT(50) |      /* PLL multiplier = 50x */
    DEVCFG2_FPLLODIV_2,         /* PLL postscaler = 1/2 */

    DEVCFG3_FETHIO |            /* Default Ethernet pins */
    DEVCFG3_USERID(0xffff));    /* User-defined ID */

/*
 * Boot code at bfc00000.
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
asm ("_estack   = _end + 0x1000");

volatile unsigned loop;

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
    /* Initialize coprocessor 0. */
    mtc0 (C0_COUNT, 0, 0);
    mtc0 (C0_COMPARE, 0, -1);
    mtc0 (C0_EBASE, 1, 0x9fc00000);     /* Vector base */
    mtc0 (C0_INTCTL, 1, 1 << 5);        /* Vector spacing 32 bytes */
    mtc0 (C0_CAUSE, 0, 1 << 23);        /* Set IV */
    mtc0 (C0_STATUS, 0, 0);             /* Clear BEV */

    /* Use pins RH0-RH2, RH6, RH11 as output: LED control. */
    LATHCLR = (1 << 0) |        /* RH0 - LED1 on SK (red), LED1 on MEB-II */
              (1 << 1) |        /* RH1 - LED2 on SK (yellow), LED3 on MEB-II */
              (1 << 2) |        /* RH2 - LED3 on SK (green), LED2 on MEB-II */
              (1 << 6) |        /* RH6 - LED4 on MEB-II */
              (1 << 11);        /* RH11 - LED5 on MEB-II */
    TRISHCLR = (1 << 0) |
               (1 << 1) |
               (1 << 2) |
               (1 << 6) |
               (1 << 11);

    while (1) {
        /* Invert pins RH0-RH2, RH6, RH11. */
        LATHINV = 1 << 0;  udelay (100000);
        LATHINV = 1 << 1;  udelay (100000);
        LATHINV = 1 << 2;  udelay (100000);
        LATHINV = 1 << 6;  udelay (100000);
        LATHINV = 1 << 11; udelay (100000);

        loop++;
    }
}
