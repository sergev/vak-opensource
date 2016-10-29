/*
 * LED blinking example for mikroE MINI-32 board.
 *
 * Copyright (C) 2012 Serge Vakulenko, <serge@vak.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 *
 * As a special exception to the GPL, permission is granted for additional
 * uses of the text contained in this file.  See the accompanying file
 * "COPY-UOS.txt" for details.
 */
#include "pic32mx.h"

#define MHZ     80              /* CPU clock is 80 MHz. */

/*
 * Entry point at 9d00bff0.
 */
asm ("          .section .startup,\"ax\",@progbits");
asm ("          .globl _init");
asm ("          .type _init, function");
asm ("_start:   la      $ra, _init");
asm ("          jr      $ra");
asm ("          .text");

/*
 * Exception section at 9d000000.
 * Setup stack pointer and $gp registers, and jump to main().
 */
asm ("          .section .exception,\"ax\",@progbits");
asm ("          .globl _start");
asm ("          .type _start, function");
asm ("_init:    la      $sp, _estack");
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
    mtc0 (C0_EBASE, 1, 0x9d000000);     /* Vector base */
    mtc0 (C0_INTCTL, 1, 1 << 5);        /* Vector spacing 32 bytes */
    mtc0 (C0_CAUSE, 0, 1 << 23);        /* Set IV */
    mtc0 (C0_STATUS, 0, 0);             /* Clear BEV */

    /* Use all ports as digital. */
    AD1PCFG = ~0;

    /* Use pin RG6 as output: LED2 control. */
    TRISGCLR = 1 << 6;
    LATG = 0;

    /* Use pin RD6 as output: LED3 control. */
    TRISDCLR = 1 << 6;
    LATD = 0;

#define TIMO 125000             /* Delay 1/8 of second */
    for (;;) {
        /* Stop simulation. */
        asm volatile ("sltiu $zero, $zero, 0xABC2");

        LATGSET = 1 << 6;       /* Blink LED2: pin RG6. */
        udelay (TIMO);
        LATGCLR = 1 << 6;
        udelay (TIMO);
        LATGSET = 1 << 6;
        udelay (TIMO);
        LATGCLR = 1 << 6;
        udelay (TIMO);

        LATDSET = 1 << 6;       /* Blink LED3: pin RD6. */
        udelay (TIMO);
        LATDCLR = 1 << 6;
        udelay (TIMO);
        LATDSET = 1 << 6;
        udelay (TIMO);
        LATDCLR = 1 << 6;
        udelay (TIMO);
    }
}
