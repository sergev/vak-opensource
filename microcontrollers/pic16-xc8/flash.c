/*
 * Include standard PIC header file.
 */
#include <xc.h>

/*
 * Setup chip configuration.
 */

//
// Register: CONFIG1 @ 0x8007
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
// (from microchip/xc8/v1.45/docs/chips/16f1705.html)
//
// FOSC = Oscillator Selection Bits
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// #pragma config WDTE = ON         // Watchdog enabled
// #pragma config WDTE = NSLEEP     // Watchdog enabled while running and disabled in Sleep
// #pragma config WDTE = SWDTEN     // Watchdog controlled by the SWDTEN bit in the WDTCON register
// #pragma config WDTE = OFF        // Watchdog disabled
//
// PWRTE = Power-up Timer Enable
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// #pragma config PWRTE = OFF       // Power-up Timer disabled
// #pragma config PWRTE = ON        // Power-up Timer enabled
//
// MCLRE = MCLR Pin Function Select
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// #pragma config MCLRE = ON        // MCLR/VPP pin function is MCLR
// #pragma config MCLRE = OFF       // MCLR/VPP pin function is digital input
//
// CP = Flash Program Memory Code Protection
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// #pragma config CP = OFF          // Program memory code protection is disabled
// #pragma config CP = ON           // Program memory code protection is enabled
//
// BOREN = Brown-out Reset Enable
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// #pragma config BOREN = ON        // Brown-out Reset enabled
// #pragma config BOREN = NSLEEP    // Brown-out Reset enabled while running and disabled in Sleep
// #pragma config BOREN = SBODEN    // Brown-out Reset controlled by the SBOREN bit in the BORCON register
// #pragma config BOREN = OFF       // Brown-out Reset disabled
//
// CLKOUTEN = Clock Out Enable
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// #pragma config CLKOUTEN = OFF    // CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin
// #pragma config CLKOUTEN = ON     // CLKOUT function is enabled on the CLKOUT pin
//
// IESO = Internal/External Switchover Mode
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// #pragma config IESO = ON         // Internal/External Switchover Mode is enabled
// #pragma config IESO = OFF        // Internal/External Switchover Mode is disabled
//
// FCMEN = Fail-Safe Clock Monitor Enable
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// #pragma config FCMEN = ON        // Fail-Safe Clock Monitor is enabled
// #pragma config FCMEN = OFF       // Fail-Safe Clock Monitor is disabled
//
#pragma config FOSC     = INTOSC    // INTOSC oscillator: I/O function on CLKIN pin
#pragma config WDTE     = OFF       // Watchdog disabled
#pragma config PWRTE    = ON        // Power-up Timer enabled
#pragma config MCLRE    = ON        // MCLR/VPP pin function is MCLR
#pragma config CP       = OFF       // Program memory code protection is disabled
#pragma config BOREN    = OFF       // Brown-out Reset disabled
#pragma config CLKOUTEN = OFF       // CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin
#pragma config IESO     = OFF       // Internal/External Switchover Mode is disabled
#pragma config FCMEN    = OFF       // Fail-Safe Clock Monitor is disabled

//
// Register: CONFIG2 @ 0x8008
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// WRT = Flash Memory Self-Write Protection
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// #pragma config WRT = OFF         // Write protection off
// #pragma config WRT = BOOT        // 000h to 1FFh write protected, 200h to 1FFFh may be modified by EECON control
// #pragma config WRT = HALF        // 000h to FFFh write protected, 1000h to 1FFFh may be modified by EECON control
// #pragma config WRT = ALL         // 000h to 1FFFh write protected, no addresses may be modified by EECON control
//
// PPS1WAY = Peripheral Pin Select one-way control
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// #pragma config PPS1WAY = ON      // The PPSLOCK bit cannot be cleared once it is set by software
// #pragma config PPS1WAY = OFF     // The PPSLOCK bit can be set and cleared repeatedly by software
//
// ZCDDIS = Zero-cross detect disable
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// #pragma config ZCDDIS = ON       // Zero-cross detect circuit is disabled at POR
// #pragma config ZCDDIS = OFF      // Zero-cross detect circuit is enabled at POR
//
// PLLEN = Phase Lock Loop enable
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// #pragma config PLLEN = ON        // 4x PLL is always enabled
// #pragma config PLLEN = OFF       // 4x PLL is enabled when software sets the SPLLEN bit
//
// STVREN = Stack Overflow/Underflow Reset Enable
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// #pragma config STVREN = ON       // Stack Overflow or Underflow will cause a Reset
// #pragma config STVREN = OFF      // Stack Overflow or Underflow will not cause a Reset
//
// BORV = Brown-out Reset Voltage Selection
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// #pragma config BORV = LO         // Brown-out Reset Voltage (Vbor), low trip point selected.
// #pragma config BORV = HI         // Brown-out Reset Voltage (Vbor), high trip point selected.
//
// LPBOR = Low-Power Brown Out Reset
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// #pragma config LPBOR = OFF       // Low-Power BOR is disabled
// #pragma config LPBOR = ON        // Low-Power BOR is enabled
//
// LVP = Low-Voltage Programming Enable
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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

void mdelay(unsigned msec)
{
	unsigned char i;

	while (msec-- > 0) {
		for (i=0; i<200; i++)
			;
	}
}

void main()
{
	unsigned char count = 0;

	PORTC = count;
	TRISC = 0;
	while (1) {
		mdelay(250);
                PORTC = count++;
	}
}
