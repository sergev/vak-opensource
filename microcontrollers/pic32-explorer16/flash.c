/*
 * Include processor definitions.
 */
#include "pic32mx.h"

#define MHZ     80              /* CPU clock is 80 MHz. */

/*
 * Chip configuration.
 */
PIC32_DEVCFG (
    DEVCFG0_DEBUG_ENABLED,      /* ICE debugger enabled */

    DEVCFG1_FNOSC_PRIPLL |      /* Primary oscillator with PLL */
    DEVCFG1_POSCMOD_HS |        /* HS oscillator */
    DEVCFG1_OSCIOFNC_OFF |      /* CLKO output disable */
    DEVCFG1_FPBDIV_8 |          /* Peripheral bus clock = SYSCLK/8 */
    DEVCFG1_FCKM_DISABLE |      /* Fail-safe clock monitor disable */
    DEVCFG1_FCKS_DISABLE,       /* Clock switching disable */

    DEVCFG2_FPLLIDIV_2 |        /* PLL divider = 1/2 */
    DEVCFG2_FPLLMUL_20 |        /* PLL multiplier = 20x */
    DEVCFG2_UPLLIDIV_2 |        /* USB PLL divider = 1/2 */
    DEVCFG2_UPLLDIS |           /* Disable USB PLL */
    DEVCFG2_FPLLODIV_1,         /* PLL postscaler = 1/1 */

    DEVCFG3_USERID(0xffff) |    /* User-defined ID */
    DEVCFG3_FSRSSEL_7);         /* Assign irq priority 7 to shadow set */

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

    /* Disable JTAG port, to make all LEDs available. */
    //DDPCON = 0;

    /* Use pins PA0-PA7 as output: LED control. */
    LATACLR = 0xFF;
    TRISACLR = 0xFF;

    while (1) {
        /* Invert pins PA7-PA0. */
        LATAINV = 1 << 0; udelay (100000);
        LATAINV = 1 << 1; udelay (100000);
        LATAINV = 1 << 2; udelay (100000);
        LATAINV = 1 << 3; udelay (100000);
        LATAINV = 1 << 4; udelay (100000);
        LATAINV = 1 << 5; udelay (100000);
        LATAINV = 1 << 6; udelay (100000);
        LATAINV = 1 << 7; udelay (100000);
        loop++;
    }
}
