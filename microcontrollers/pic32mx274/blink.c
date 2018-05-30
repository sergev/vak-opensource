/*
 * Include processor definitions.
 */
#include "pic32mx.h"

#define MHZ     40              /* CPU clock is 40 MHz. */

/*
 * Chip configuration.
 */
PIC32_DEVCFG (
    DEVCFG0_JTAGDIS |           /* Disable JTAG port */
    DEVCFG0_DEBUG_DISABLED,     /* ICE debugger disabled */
#if 0
    // Internal RC oscillator
    DEVCFG1_FNOSC_FRCPLL |      /* Fast RC oscillator with PLL */
    DEVCFG1_POSCMOD_DISABLE |   /* Primary oscillator disabled */
    DEVCFG1_FPBDIV_2 |          /* Peripheral bus clock = SYSCLK/2 */
    DEVCFG1_OSCIOFNC_OFF |      /* CLKO output disable */
    DEVCFG1_FCKM_DISABLE,       /* Fail-safe clock monitor disable */
#else
    // External crystal 8 MHz
    DEVCFG1_FNOSC_PRIPLL |      /* Primary oscillator with PLL... */
    DEVCFG1_POSCMOD_HS |        /* ...in high speed mode */
    DEVCFG1_FPBDIV_2 |          /* Peripheral bus clock = SYSCLK/2 */
    DEVCFG1_OSCIOFNC_OFF |      /* CLKO output disable */
    DEVCFG1_FCKM_DISABLE,       /* Fail-safe clock monitor disable */
#endif
    DEVCFG2_FPLLIDIV_2 |        /* PLL divider = 1/2 */
    DEVCFG2_FPLLMUL_20 |        /* PLL multiplier = 20x */
    DEVCFG2_UPLLIDIV_2 |        /* USB PLL divider = 1/2 */
    DEVCFG2_UPLLDIS |           /* Disable USB PLL */
    DEVCFG2_FPLLODIV_2,         /* PLL postscaler = 1/2 */

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
#if 0
    /* Use all ports as digital. */
    ANSELA = 0;
    ANSELB = 0;
    LATA = 0;
    LATB = 0;

    /* Use pin RA10 as output: LED1 control. */
    TRISACLR = 1 << 10;

    /* Use pin RB15 as output: LED2 control. */
    TRISBCLR = 1 << 15;
    LATBSET = 1 << 15;

    for (;;) {
        /* Invert pin RA10. */
        LATAINV = 1 << 10;

        /* Invert pin RB15. */
        LATBINV = 1 << 15;

        /* Delay. */
        udelay (500000);
    }
#else
    /* Disable JTAG and Trace ports, to make more pins available. */
    DDPCONCLR = 3 << 2;

    /* Use all ports as digital. */
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;
    LATA = 0;
    LATB = 0;
    LATC = 0;

    /* Use pins RA10, RA7 and RA8 as output: LED control. */
    TRISACLR = 1 << 10;
    TRISACLR = 1 << 7;
    TRISACLR = 1 << 8;

    for (;;) {
        /* Invert pin RA10. */
        LATAINV = 1 << 10;
        udelay(100000);
        LATAINV = 1 << 10;

        /* Invert pin RA7. */
        LATAINV = 1 << 7;
        udelay(100000);
        LATAINV = 1 << 7;

        /* Invert pin RA8. */
        LATAINV = 1 << 8;
        udelay(100000);
        LATAINV = 1 << 8;

        /* Invert pin RA7. */
        LATAINV = 1 << 7;
        udelay(100000);
        LATAINV = 1 << 7;
    }
#endif
}
