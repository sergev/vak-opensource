//
// Include processor definitions.
//
#include "pic32mm.h"

#define MHZ     25              // CPU clock is 25 MHz

//
// Chip configuration.
//
PIC32_DEVCFG(
    DEVCFG0_DEBUG_DISABLED |    /* ICE debugger disabled */
    DEVCFG0_JTAGDIS,            /* Disable JTAG port */

    DEVCFG1_FNOSC_FRCPLL |      /* Fast RC oscillator with PLL */
    DEVCFG1_POSCMOD_DISABLE |   /* Primary oscillator disabled */
    DEVCFG1_FPBDIV_2 |          /* Peripheral bus clock = SYSCLK/2 */
    DEVCFG1_OSCIOFNC_OFF |      /* CLKO output disable */
    DEVCFG1_FCKM_DISABLE,       /* Fail-safe clock monitor disable */

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
asm("          .section .exception");
asm("          .globl _start");
asm("          .type _start, function");
asm("_start:   la      $sp, _estack");
asm("          la      $ra, main");
asm("          la      $gp, _gp");
asm("          jr      $ra");
asm("          .text");

/*
 * Delay for a given number of microseconds.
 * The processor has a 32-bit hardware Count register,
 * which increments at half CPU rate.
 * We use it to get a precise delay.
 */
void udelay(unsigned usec)
{
    unsigned now = mfc0(C0_Count, 0);
    unsigned final = now + usec * MHZ / 2;

    for (;;) {
        now = mfc0(C0_Count, 0);

        /* This comparison is valid only when using a signed type. */
        if ((int) (now - final) >= 0)
            break;
    }
}

int main()
{
    /* Initialize coprocessor 0. */
    mtc0(C0_Count, 0, 0);
    mtc0(C0_Compare, 0, -1);
    mtc0(C0_EBase, 1, 0x9fc00000);      /* Vector base */
    mtc0(C0_IntCtl, 1, 1 << 5);         /* Vector spacing 32 bytes */
    mtc0(C0_Cause, 0, 1 << 23);         /* Set IV */
    mtc0(C0_Status, 0, 0);              /* Clear BEV */

    /* Use all ports as digital. */
    ANSELA = 0;
    ANSELB = 0;
    LATA = 0;
    LATB = 0;

    /* Use pin RA0 as output: LED control. */
    TRISACLR = 1 << 0;

    for (;;) {                          // This loop determines the initial time between blinks.
        int j = 200000;

        while (j > 10000) {             // This loop controlls how quickly blinking speeds up.
            LATAINV = 1 << 0;           // Toggle light status. (Can be viewed in LATA SFR)

            udelay(j);                  // Kill time.

            j *= 15;                    // Increase constant to increas blinking speed faster.
            j >>= 4;
        }
    }
}
