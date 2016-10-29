//
// Arduino Iambic Keyer
// October 23rd, 2009
//
// I present a basic iambic mode keyer that has adjustable speed
// and can be configured to operate in iambic mode a or b.
// At this point all it does is flash an LED in Morse, but it’s
// a good foundation to build upon.  There really isn’t anything novel
// about it, it’s the same basic algorithm used by many different keyers.
// Over the weeks I will add autospace, Ultimatic, and straight keying modes.
// I’ll also include the ability to swap paddles. But this will hold us
// for quite a while.  You can cut and paste this into an empty Arduino
// sketch, it’s completely self contained. It’ll run on any Arduino board,
// just wire up a paddle and it will flash the LED. Add a pushbutton switch
// and you can select between iambic mode a and b. It would be very simple
// to add a keying output in place of the LED drive, probably a good idea
// to include a 2N2222 or 2N7000 buffer stage.
//
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>

/////////////////////////////////////////////////////////////////////////////////////////
//
//  Iambic Morse Code Keyer Sketch
//  Copyright (c) 2009 Steven T. Elliott
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details:
//
//  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
//  Boston, MA  02111-1307  USA
//
/////////////////////////////////////////////////////////////////////////////////////////
//
//                         openQRP CPU Pin Definitions
//
/////////////////////////////////////////////////////////////////////////////////////////
//
//  Arduino  OQ PCB                                        OQ PCB    Arduino
//  -------  ------               ___________              ------    ----------
//           RESET-----------PC6 |1        28| PC5---------KEY_TX    Dig19 Ana5
//  Dig0     SER_REPLY-------PD0 |2  o   i 27| PC4---------LCD_E     Dig18 Ana4
//  Dig1     SER_CMD---------PD1 |3  o   i 26| PC3---------CMD_PB    Dig17 Ana3
//  Dig2     WIDE_SEL--------PD2 |4  o   i 25| PC2---------PB_NET    Dig16 Ana2
//  Dig3     SIDETONE--------PD3 |5  o   i 24| PC1---------ANT_XO    Dig15 Ana1
//  Dig4     LCD_D4----------PD4 |6  o   i 23| PC0---------MON_12V   Dig14 Ana0
//           VCC-------------VCC |7  o   o 22| GND---------AGND
//           GND-------------GND |8  o   o 21| AREF--------AREF
//           XTAL1-----------PB6 |9  i   o 20| AVCC--------AVCC
//           XTAL2-----------PB7 |10 i   o 19| PB5---------RIT_ON    Dig13
//  Dig5     FREQ_CNT--------PD5 |11 o   o 18| PB4---------NAR_SEL   Dig12
//  Dig6     LCD_D5----------PD6 |12 o   o 17| PB3---------LCD_RS    Dig11
//  Dig7     LCD_D6----------PD7 |13 o   o 16| PB2---------LPDL      Dig10
//  Dig8     LCD_D7----------PB0 |14 o   o 15| PB1---------RPDL      Dig9
//                                -----------
//                                 ATmega168

//
// Digital Pins
//
int         LPin     = 9;       // Left paddle input
int         RPin     = 10;      // Right paddle input
int         ledPin   = 13;      //
int         cmdPin   = 17;      // Command pushbutton input

/////////////////////////////////////////////////////////////////////////////////////////
//
//  keyerControl bit definitions
//
#define     DIT_L      0x01     // Dit latch
#define     DAH_L      0x02     // Dah latch
#define     DIT_PROC   0x04     // Dit is being processed
#define     IAMBICB    0x10     // 0 for Iambic A, 1 for Iambic B

/////////////////////////////////////////////////////////////////////////////////////////
//
//  Library Instantiations
//
/////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
//
//  Global Variables
//
ULONG       ditTime;                    // No. milliseconds per dit
UCHAR       keyerControl;
UCHAR       keyerState;

////////////////////////////////////////////////////////////////////////////////////////
//
//  State Machine Defines

enum KSTYPE {
    IDLE,
    CHK_DIT,
    CHK_DAH,
    KEYED_PREP,
    KEYED,
    INTER_ELEMENT,
};

///////////////////////////////////////////////////////////////////////////////
//
//  System Initialization
//
///////////////////////////////////////////////////////////////////////////////

void setup() {

    // Setup outputs
    pinMode(ledPin, OUTPUT);      // sets the digital pin as output

    // Setup control input pins
    pinMode(LPin, INPUT);        // sets Left Paddle digital pin as input
    pinMode(RPin, INPUT);        // sets Right Paddle digital pin as input
    pinMode(cmdPin, INPUT);      // sets PB analog pin 3 as input
    digitalWrite(ledPin, LOW);   // turn the LED off

    keyerState = IDLE;
    keyerControl = 0;
    loadWPM(15);                 // Fix speed at 15 WPM
}

///////////////////////////////////////////////////////////////////////////////
//
//  Main Work Loop
//
///////////////////////////////////////////////////////////////////////////////

void loop()
{
    static long ktimer;
    int debounce;

    // Basic Iambic Keyer
    // keyerControl contains processing flags and keyer mode bits
    // Supports Iambic A and B
    // State machine based, uses calls to millis() for timing.

    switch (keyerState) {
    case IDLE:
        // Wait for direct or latched paddle press
        if ((digitalRead(LPin) == LOW) ||
            (digitalRead(RPin) == LOW) ||
            (keyerControl & 0x03))
        {
            update_PaddleLatch();
            keyerState = CHK_DIT;
        }
        break;

    case CHK_DIT:
        // See if the dit paddle was pressed
        if (keyerControl & DIT_L) {
            keyerControl |= DIT_PROC;
            ktimer = ditTime;
            keyerState = KEYED_PREP;
        } else {
            keyerState = CHK_DAH;
        }
        break;

    case CHK_DAH:
        // See if dah paddle was pressed
        if (keyerControl & DAH_L) {
            ktimer = ditTime*3;
            keyerState = KEYED_PREP;
        } else {
            keyerState = IDLE;
        }
        break;

    case KEYED_PREP:
        // Assert key down, start timing, state shared for dit or dah
        digitalWrite(ledPin, HIGH);         // turn the LED on
        ktimer += millis();                 // set ktimer to interval end time
        keyerControl &= ~(DIT_L + DAH_L);   // clear both paddle latch bits
        keyerState = KEYED;                 // next state
        break;

    case KEYED:
        // Wait for timer to expire
        if (millis() > ktimer) {            // are we at end of key down ?
            digitalWrite(ledPin, LOW);      // turn the LED off
            ktimer = millis() + ditTime;    // inter-element time
            keyerState = INTER_ELEMENT;     // next state

        } else if (keyerControl & IAMBICB) {
            update_PaddleLatch();           // early paddle latch in Iambic B mode
        }
        break;

    case INTER_ELEMENT:
        // Insert time between dits/dahs
        update_PaddleLatch();               // latch paddle state
        if (millis() > ktimer) {            // are we at end of inter-space ?
            if (keyerControl & DIT_PROC) {             // was it a dit or dah ?
                keyerControl &= ~(DIT_L + DIT_PROC);   // clear two bits
                keyerState = CHK_DAH;                  // dit done, check for dah
            } else {
                keyerControl &= ~(DAH_L);              // clear dah latch
                keyerState = IDLE;                     // go idle
            }
        }
        break;
    }

    // Simple Iambic mode select
    // The mode is toggled between A & B every time switch is pressed
    // Flash LED to indicate new mode.
    if (digitalRead(cmdPin) == LOW) {
        // Give switch time to settle
        debounce = 100;
        do {
            // wait here until switch is released, we debounce to be sure
            if (digitalRead(cmdPin) == LOW) {
                debounce = 100;
            }
            delay(2);
        } while (debounce--);

        keyerControl ^= IAMBICB;        // Toggle Iambic B bit
        if (keyerControl & IAMBICB) {   // Flash once for A, twice for B
            flashLED(2);
        } else {
            flashLED(1);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//    Flash LED as a signal
//
//    count specifies the number of flashes
//
///////////////////////////////////////////////////////////////////////////////

void flashLED (int count)
{
    int i;
    for (i=0; i<count; i++) {
        digitalWrite(ledPin, HIGH);        // turn the LED on
        delay (250);
        digitalWrite(ledPin, LOW);         // turn the LED off
        delay (250);
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//    Latch dit and/or dah press
//
//    Called by keyer routine
//
///////////////////////////////////////////////////////////////////////////////

void update_PaddleLatch()
{
    if (digitalRead(RPin) == LOW) {
        keyerControl |= DIT_L;
    }
    if (digitalRead(LPin) == LOW) {
        keyerControl |= DAH_L;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//    Calculate new time constants based on wpm value
//
///////////////////////////////////////////////////////////////////////////////

void loadWPM (int wpm)
{
    ditTime = 1200/wpm;
}
