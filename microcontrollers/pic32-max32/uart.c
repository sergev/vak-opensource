/*
 * Include processor definitions.
 */
#include "pic32mx.h"

#define MHZ     80              /* CPU clock in MHz. */

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
 * Image header pointer.
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
void udelay(unsigned usec)
{
    unsigned now = mfc0(C0_COUNT, 0);
    unsigned final = now + usec * MHZ / 2;

    for (;;) {
        now = mfc0(C0_COUNT, 0);

        /* This comparison is valid only when using a signed type. */
        if ((int) (now - final) >= 0)
            break;
    }
}

/*
 * Send a byte to the UART transmitter, with interrupts disabled.
 */
void putch(unsigned char c)
{
    /* Wait for transmitter shift register empty. */
    while (! (U1STA & PIC32_USTA_TRMT))
        continue;

again:
    /* Send byte. */
    U1TXREG = c;

    /* Wait for transmitter shift register empty. */
    while (! (U1STA & PIC32_USTA_TRMT))
        continue;

    if (c == '\n') {
        c = '\r';
        goto again;
    }
}

/*
 * Send a string to UART.
 */
void putstr(char *str)
{
    while (*str)
        putch(*str++);
}

int main()
{
    /* Initialize UART. */
    U1BRG = PIC32_BRG_BAUD(MHZ * 500000, 115200);
    U1STA = 0;
    U1MODE = PIC32_UMODE_PDSEL_8NPAR |      /* 8-bit data, no parity */
             PIC32_UMODE_ON;                /* UART Enable */
    U1STASET = PIC32_USTA_URXEN |           /* Receiver Enable */
               PIC32_USTA_UTXEN;            /* Transmit Enable */

    unsigned int count = 0;
    while (count < 2) {
        putstr("\r\nHello, World!\r\n");

        putch('.'); udelay(1000000);
        putch('.'); udelay(1000000);
        putch('.'); udelay(1000000);
        
        count++;
    }
    
    /* Stop simulation. */
    asm volatile ("di; wait; ei");

    while (1) {};  // Loop forever. Bare metal code must not return from main().

    return 0;
}
