// Based on Maximite code.
//
// These files are distributed on condition that the following copyright
// notice and credits are retained in any subsequent versions:
//
// Copyright 2011, 2012 Geoff Graham.
//
// Based on code by Lucio Di Jasio in his book "Programming 32-bit
// Microcontrollers in C - Exploring the PIC32".
//
// Non US keyboard support by Fabrice Muller.

/*
----------------------- PIC32 HARDWARE --------------------------
Interrupts.......................................................
PRIORITY     DESCRIPTION                 MAX SPEED
   7         Video horizontal sync       every 66uS
   6         Serial interface            every 17uS when open
   5         I2C interface               ?
   4         USB polling                 every 100uS to 1mS
   3         PS2 keyboard clock          every 50 uS when sending
   2         Counting pins (11 to 14)    up to every 5uS
   1         MMBasic clocks and timers   every 1 mS

If you modify these priorities then you must also adjust the code of
lower priority interrupts to take into account that some thing like
LATEbits.LATE4 = 1 is not an atomic operation and a higher interrupt
might change PORTE while the macro us being executed.

Timers...........................................................
TIMER NBR    DESCRIPTION                 INTERRUPT
  core       Not Used                       no
   1         USB polling                    yes
   2         Sound                          no
   3         Video horizontal sync          yes
   4         MMBasic clocks and timers      yes
   5         Serial interface               yes
*/

#if !defined(MAXIMITE) && !defined(UBW32) && !defined(DUINOMITE)
#error Must define the target hardware in the project file
#endif

#include <p32xxxx.h>                    // Device specific defines.
#include <plib.h>                       // Peripheral libraries.
#include <stdlib.h>                     // Standard library functions.
#include <string.h>                     // String functions.

#include "maximite.h"                   // Helpful defines.
#if defined(MAXIMITE) || \
    (defined(UBW32) && defined(__DEBUG)) || \
    defined(DUINOMITE)
#include "configuration.h"              // Config pragmas.
#endif

#include "video.h"
#include "keyboard.h"

int input_key(void) {
    int c;

    do {
        video_show_cursor(1);
        c = keyboard_inkey();
        if (c == '\r') c = '\n';
    } while (c == -1);
    video_show_cursor(0);
    return c;
}

int main(void) {
    // Initial setup of the I/O ports.
    AD1PCFG = 0xFFFF;               // Default all pins to digital.
    mJTAGPortEnable(0);             // Turn off JTAG.

    // Setup the CPU.
    // System config performance.
    SYSTEMConfigPerformance(CLOCKFREQ);
#if defined(MAXIMITE) || (defined(UBW32) && defined(__DEBUG)) || \
    defined(DUINOMITE)
    // Fix the peripheral bus to the main clock speed.
    mOSCSetPBDIV(OSC_PB_DIV_1);
#endif

    INTEnableSystemMultiVectoredInt();  // Allow vectored interrupts.

    keyboard_init();       // Initialise and startup the keyboard routines.
    video_init();          // Start the video state machine.
    video_clear_screen();  // Clear the video buffer.

    DelayUs(1*1000*1000);
    while (keyboard_inkey() != -1);

    video_display_string("MAXIMITE console\r\n");

    while (1) {
      int ch = input_key();
      video_put_char(ch);
      {
          static char buf[10];
          sprintf(buf, "<%02X>", ch);
          video_display_string(buf);
      }
      if (ch == '\n') video_put_char('\r');
    }
}
