/*
 * UART demo for pic16f1705 microcontroller.
 *
 * Copyright (C) 2018 Serge Vakulenko, <serge@besm6.org>
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is hereby
 * granted, provided that the above copyright notice appear in all
 * copies and that both that the copyright notice and this
 * permission notice and warranty disclaimer appear in supporting
 * documentation, and that the name of the author not be used in
 * advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * The author disclaim all warranties with regard to this
 * software, including all implied warranties of merchantability
 * and fitness.  In no event shall the author be liable for any
 * special, indirect or consequential damages or any damages
 * whatsoever resulting from loss of use, data or profits, whether
 * in an action of contract, negligence or other tortious action,
 * arising out of or in connection with the use or performance of
 * this software.
 */
#include <xc.h>

//
// Register: CONFIG1 @ 0x8007
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
// (from microchip/xc8/v1.45/docs/chips/16f1705.html)
//
// FOSC = Oscillator Selection Bits
// #pragma config FOSC = ECH       // External Clock, High Power Mode (4-20 MHz): device clock supplied to CLKIN pins
// #pragma config FOSC = ECM       // External Clock, Medium Power Mode (0.5-4 MHz): device clock supplied to CLKIN pins
// #pragma config FOSC = ECL       // External Clock, Low Power Mode (0-0.5 MHz): device clock supplied to CLKIN pins
// #pragma config FOSC = INTOSC    // INTOSC oscillator: I/O function on CLKIN pin
// #pragma config FOSC = EXTRC     // EXTRC oscillator: External RC circuit connected to CLKIN pin
// #pragma config FOSC = HS        // HS Oscillator, High-speed crystal/resonator connected between OSC1 and OSC2 pins
// #pragma config FOSC = XT        // XT Oscillator, Crystal/resonator connected between OSC1 and OSC2 pins
// #pragma config FOSC = LP        // LP Oscillator, Low-power crystal connected between OSC1 and OSC2 pins
//
// WDTE = Watchdog Timer Enable
// #pragma config WDTE = ON         // Watchdog enabled
// #pragma config WDTE = NSLEEP     // Watchdog enabled while running and disabled in Sleep
// #pragma config WDTE = SWDTEN     // Watchdog controlled by the SWDTEN bit in the WDTCON register
// #pragma config WDTE = OFF        // Watchdog disabled
//
// PWRTE = Power-up Timer Enable
// #pragma config PWRTE = OFF       // Power-up Timer disabled
// #pragma config PWRTE = ON        // Power-up Timer enabled
//
// MCLRE = MCLR Pin Function Select
// #pragma config MCLRE = ON        // MCLR/VPP pin function is MCLR
// #pragma config MCLRE = OFF       // MCLR/VPP pin function is digital input
//
// CP = Flash Program Memory Code Protection
// #pragma config CP = OFF          // Program memory code protection is disabled
// #pragma config CP = ON           // Program memory code protection is enabled
//
// BOREN = Brown-out Reset Enable
// #pragma config BOREN = ON        // Brown-out Reset enabled
// #pragma config BOREN = NSLEEP    // Brown-out Reset enabled while running and disabled in Sleep
// #pragma config BOREN = SBODEN    // Brown-out Reset controlled by the SBOREN bit in the BORCON register
// #pragma config BOREN = OFF       // Brown-out Reset disabled
//
// CLKOUTEN = Clock Out Enable
// #pragma config CLKOUTEN = OFF    // CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin
// #pragma config CLKOUTEN = ON     // CLKOUT function is enabled on the CLKOUT pin
//
// IESO = Internal/External Switchover Mode
// #pragma config IESO = ON         // Internal/External Switchover Mode is enabled
// #pragma config IESO = OFF        // Internal/External Switchover Mode is disabled
//
// FCMEN = Fail-Safe Clock Monitor Enable
// #pragma config FCMEN = ON        // Fail-Safe Clock Monitor is enabled
// #pragma config FCMEN = OFF       // Fail-Safe Clock Monitor is disabled
//
#pragma config FOSC     = INTOSC    // INTOSC oscillator: I/O function on CLKIN pin
#pragma config WDTE     = OFF       // Watchdog disabled
#pragma config PWRTE    = OFF       // Power-up Timer disabled
#pragma config MCLRE    = ON        // MCLR/VPP pin function is MCLR
#pragma config CP       = ON        // Program memory code protection is enabled
#pragma config BOREN    = ON        // Brown-out Reset enabled
#pragma config CLKOUTEN = OFF       // CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin
#pragma config IESO     = ON        // Internal/External Switchover Mode is enabled
#pragma config FCMEN    = ON        // Fail-Safe Clock Monitor is enabled

//
// Register: CONFIG2 @ 0x8008
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// WRT = Flash Memory Self-Write Protection
// #pragma config WRT = OFF         // Write protection off
// #pragma config WRT = BOOT        // 000h to 1FFh write protected, 200h to 1FFFh may be modified by EECON control
// #pragma config WRT = HALF        // 000h to FFFh write protected, 1000h to 1FFFh may be modified by EECON control
// #pragma config WRT = ALL         // 000h to 1FFFh write protected, no addresses may be modified by EECON control
//
// PPS1WAY = Peripheral Pin Select one-way control
// #pragma config PPS1WAY = ON      // The PPSLOCK bit cannot be cleared once it is set by software
// #pragma config PPS1WAY = OFF     // The PPSLOCK bit can be set and cleared repeatedly by software
//
// ZCDDIS = Zero-cross detect disable
// #pragma config ZCDDIS = ON       // Zero-cross detect circuit is disabled at POR
// #pragma config ZCDDIS = OFF      // Zero-cross detect circuit is enabled at POR
//
// PLLEN = Phase Lock Loop enable
// #pragma config PLLEN = ON        // 4x PLL is always enabled
// #pragma config PLLEN = OFF       // 4x PLL is enabled when software sets the SPLLEN bit
//
// STVREN = Stack Overflow/Underflow Reset Enable
// #pragma config STVREN = ON       // Stack Overflow or Underflow will cause a Reset
// #pragma config STVREN = OFF      // Stack Overflow or Underflow will not cause a Reset
//
// BORV = Brown-out Reset Voltage Selection
// #pragma config BORV = LO         // Brown-out Reset Voltage (Vbor), low trip point selected.
// #pragma config BORV = HI         // Brown-out Reset Voltage (Vbor), high trip point selected.
//
// LPBOR = Low-Power Brown Out Reset
// #pragma config LPBOR = OFF       // Low-Power BOR is disabled
// #pragma config LPBOR = ON        // Low-Power BOR is enabled
//
// LVP = Low-Voltage Programming Enable
// #pragma config LVP = ON          // Low-voltage programming enabled
// #pragma config LVP = OFF         // High-voltage on MCLR/VPP must be used for programming
//
#pragma config WRT      = OFF       // Write protection off
#pragma config PPS1WAY  = ON        // The PPSLOCK bit cannot be cleared once it is set by software
#pragma config ZCDDIS   = ON        // Zero-cross detect circuit is disabled at POR
#pragma config PLLEN    = ON        // 4x PLL is always enabled
#pragma config STVREN   = ON        // Stack Overflow or Underflow will cause a Reset
#pragma config BORV     = LO        // Brown-out Reset Voltage (Vbor), low trip point selected.
#pragma config LPBOR    = OFF       // Low-Power BOR is disabled
#pragma config LVP      = ON        // Low-voltage programming enabled

//
// Compute UART divisor for async mode, BRG16=0, BRGH=0.
//
#define UART_DIVISOR(hz, baud)      (((hz / baud + 32) / 64) - 1)

//
// Receive a character from UART.
//
char getch()
{
    char c;

    for (;;) {
        // Wait till the data is received.
        if (! RCIF)
            continue;

        // Get received data.
        c = RCREG;

        // Ignore in case of frame error.
        if (FERR)
            continue;

        // Clear overflow flag.
        OERR = 0;
        return c;
    }
}

//
// Send a character to UART.
//
void putch(char c)
{
    // Wait till the transmitter register becomes empty.
    while (! TXIF)
        continue;

    // Transmit the data.
    TXREG = c;
}

//
// Send a string to UART.
//
void putstr(const char *str)
{
    while (*str)
        putch(*str++);
}

void main()
{
    // Configure pins.
    // RA0: ICSPDAT
    ANSA0  = 0;         // digital
    TRISA0 = 1;         // input

    // RA1: ICSPCLK
    ANSA1  = 0;         // digital
    TRISA1 = 1;         // input

    // RA2: DAC1OUT2
    ANSA2  = 0;         // digital
    TRISA2 = 0;         // output

    // RA3: /MCLR, always digital input

    // RA4: unused
    ANSA4  = 0;         // digital
    TRISA4 = 0;         // output
    RA4PPS = 0b10100;   // RA4 = UART TX output

    // RA5: unused
    TRISA5 = 1;         // input, always digital
    RXPPS  = 0b00101;   // UART RX input = RA5

    // RC0
    ANSC0  = 0;         // digital
    TRISC0 = 0;         // output

    // RC1
    ANSC1  = 0;         // digital
    TRISC1 = 0;         // output

    // RC2
    ANSC2  = 0;         // digital
    TRISC2 = 0;         // output

    // RC3
    ANSC3  = 0;         // digital
    TRISC3 = 0;         // output

    // RC4
    ANSC4  = 0;         // digital
    TRISC4 = 0;         // output

    // RC5
    ANSC5  = 0;         // digital
    TRISC5 = 0;         // output

    // Internal oscillator frequency select.
    // Switch from default 4*500kHz to 32 MHz
    // (when SPLLEN bit is set).
    IRCF3 = 1;
    IRCF2 = 1;
    IRCF1 = 1;
    IRCF0 = 0;

    // Initialize uart port.
    SPBRG = UART_DIVISOR(CPU_HZ, UART_BAUD);
    TXSTA = 0x20;      // TXEN
    RCSTA = 0x90;      // SPEN, SREN
    _delay(CPU_HZ/2);
    putstr("\r\n\r\nTest started.\r\n");

    for (;;) {
        // Receive a byte from uart.
        char cmd = getch();

        putstr("Got: '");
        putch(cmd);
        putstr("'\r\n");
    }
}
