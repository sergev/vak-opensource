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

#include <p32xxxx.h>
#include <plib.h>

#include "keyboard.h"

// Keyboard definitions.
#define P_PS2CLK            PORTDbits.RD6           // Pin 54 input - Keyboard clock
#define P_PS2CLK_PULLUP     CNPUEbits.CNPUE15
#define P_PS2CLK_INT        CNENbits.CNEN15

#define P_PS2DAT            PORTDbits.RD7           // Pin 55 input - Keyboard data
#define P_PS2DAT_PULLUP     CNPUEbits.CNPUE16

// Keyboard buffer.
#define INP_QUEUE_SIZE 256
volatile static unsigned char inp_queue[INP_QUEUE_SIZE];
volatile static int inp_queue_head, inp_queue_tail;

// Definitions related to setting the keyboard type.
#define CONFIG_US        0b111
#define CONFIG_FR        0b001
#define CONFIG_GR        0b010
#define CONFIG_IT        0b011
#define CONFIG_BE        0b100

// Used to store the keyboard layout output.
const unsigned int keyboard_option = CONFIG_US;

// Definitions related to setting the tab spacing
#define CONFIG_TAB2        0b111
#define CONFIG_TAB4        0b001
#define CONFIG_TAB8        0b010

// Definition of the keyboard PS/2 state machine.
#define PS2START    0
#define PS2BIT      1
#define PS2PARITY   2
#define PS2STOP     3

// PS2 KBD state machine and buffer.
static int ps2_state;
static int keyb_count, keyb_parity;

// Key codes that must be tracked for up/down state.
#define CTRL        0x14    // Left and right generate the same code.
#define L_SHFT      0x12
#define R_SHFT      0x59
#define CAPS        0x58

// this is a map of the keycode characters and the character to be returned for the keycode
const char key_codes[5][128] = {
    // US Layout
    {                                                                           // Base 10
        0,      F9,       0,      F5,      F3,      F1,      F2,     F12,       // 00-07
        0,     F10,      F8,      F6,      F4,     TAB,     '`',       0,       // 08-15
        0,     ALT,  L_SHFT,      0,     CTRL,     'q',     '1',       0,       // 16-23
        0,       0,     'z',     's',     'a',     'w',     '2',       0,       // 24-31
        0,     'c',     'x',     'd',     'e',     '4',     '3',       0,       // 32-39
        0,     ' ',     'v',     'f',     't',     'r',     '5',       0,       // 40-48
        0,     'n',     'b',     'h',     'g',     'y',     '6',       0,       // 48-56
        0,       0,     'm',     'j',     'u',     '7',     '8',       0,       // 56-63
        0,     ',',     'k',     'i',     'o',     '0',     '9',       0,       // 64-71
        0,     '.',     '/',     'l',     ';',     'p',     '-',       0,       // 72-79
        0,       0,    '\'',       0,     '[',     '=',       0,       0,       // 80-87
     CAPS,  R_SHFT,   ENTER,     ']',      0,     0x5c,       0,       0,       // 88-95
        0,       0,       0,       0,       0,       0,    BKSP,       0,       // 96-103
        0,     '1',       0,     '4',     '7',       0,       0,       0,       // 104-111
      '0',     '.',     '2',     '5',     '6',     '8',     ESC,       0,       // 112-119
      F11,     '+',     '3',     '-',     '*',     '9',       0,       0        // 120-127
    },
    // FR Layout
    {                                                                           // Base 10
        0,      F9,       0,      F5,      F3,      F1,      F2,     F12,       // 00-07
        0,     F10,      F8,      F6,      F4,     TAB,       0,       0,       // 08-15
        0,     ALT,  L_SHFT,      0,     CTRL,     'a',     '&',       0,       // 16-23
        0,       0,     'w',     's',     'q',     'z',       0,       0,       // 24-31
        0,     'c',     'x',     'd',     'e',     '\'',    '"',       0,       // 32-39
        0,     ' ',     'v',     'f',     't',     'r',     '(',       0,       // 40-48
        0,     'n',     'b',     'h',     'g',     'y',     '-',       0,       // 48-56
        0,       0,     ',',     'j',     'u',       0,     '_',       0,       // 56-63
        0,     ';',     'k',     'i',     'o',       0,       0,       0,       // 64-71
        0,     ':',     '!',     'l',     'm',     'p',     ')',       0,       // 72-79
        0,       0,       0,       0,     '^',     '=',       0,       0,       // 80-87
     CAPS,  R_SHFT,   ENTER,     '$',      0,      '*',       0,       0,       // 88-95
        0,     '<',       0,       0,       0,       0,    BKSP,       0,       // 96-103
        0,     '1',       0,     '4',     '7',       0,       0,       0,       // 104-111
      '0',     '.',     '2',     '5',     '6',     '8',     ESC,       0,       // 112-119
      F11,     '+',     '3',     '-',     '*',     '9',       0,       0        // 120-127
    },
    // GR Layout
    {                                                                           // Base 10
        0,      F9,       0,      F5,      F3,      F1,      F2,     F12,       // 00-07
        0,     F10,      F8,      F6,      F4,     TAB,     '^',       0,       // 08-15
        0,     ALT,  L_SHFT,      0,     CTRL,     'q',     '1',       0,       // 16-23
        0,       0,     'y',     's',     'a',     'w',     '2',       0,       // 24-31
        0,     'c',     'x',     'd',     'e',     '4',     '3',       0,       // 32-39
        0,     ' ',     'v',     'f',     't',     'r',     '5',       0,       // 40-47
        0,     'n',     'b',     'h',     'g',     'z',     '6',       0,       // 48-55
        0,       0,     'm',     'j',     'u',     '7',     '8',       0,       // 56-63
        0,     ',',     'k',     'i',     'o',     '0',     '9',       0,       // 64-71
        0,     '.',     '-',     'l',       0,     'p',       0,       0,       // 72-79
        0,       0,       0,       0,       0,     '\'',      0,       0,       // 80-87
     CAPS,  R_SHFT,   ENTER,     '+',      0,      '#',       0,       0,       // 88-95
        0,     '<',       0,       0,       0,       0,    BKSP,       0,       // 96-103
        0,     '1',       0,     '4',     '7',       0,       0,       0,       // 104-111
      '0',     '.',     '2',     '5',     '6',     '8',     ESC,       0,       // 112-119
      F11,     '+',     '3',     '-',     '*',     '9',       0,       0        // 120-127
    },
    // IT Layout
    {                                                                           // Base 10
        0,      F9,       0,      F5,      F3,      F1,      F2,     F12,       // 00-07
        0,     F10,      F8,      F6,      F4,     TAB,    0x5c,       0,       // 08-15
        0,     ALT,  L_SHFT,      0,     CTRL,     'q',     '1',       0,       // 16-23
        0,       0,     'z',     's',     'a',     'w',     '2',       0,       // 24-31
        0,     'c',     'x',     'd',     'e',     '4',     '3',       0,       // 32-39
        0,     ' ',     'v',     'f',     't',     'r',     '5',       0,       // 40-48
        0,     'n',     'b',     'h',     'g',     'y',     '6',       0,       // 48-56
        0,       0,     'm',     'j',     'u',     '7',     '8',       0,       // 56-63
        0,     ',',     'k',     'i',     'o',     '0',     '9',       0,       // 64-71
        0,     '.',     '-',     'l',       0,     'p',    '\'',       0,       // 72-79
        0,       0,       0,       0,       0,       0,       0,       0,       // 80-87
     CAPS,  R_SHFT,   ENTER,     '+',       0,       0,       0,       0,       // 88-95
        0,     '<',       0,       0,       0,       0,    BKSP,       0,       // 96-103
        0,     '1',       0,     '4',     '7',       0,       0,       0,       // 104-111
      '0',     '.',     '2',     '5',     '6',     '8',     ESC,       0,       // 112-119
      F11,     '+',     '3',     '-',     '*',     '9',       0,       0        // 120-127
    },
    // BE Layout
    {                                                                           // Base 10
        0,      F9,       0,      F5,      F3,      F1,      F2,     F12,       // 00-07
        0,     F10,      F8,      F6,      F4,     TAB,       0,       0,       // 08-15
        0,     ALT,  L_SHFT,      0,     CTRL,     'a',     '&',       0,       // 16-23
        0,       0,     'w',     's',     'q',     'z',       0,       0,       // 24-31
        0,     'c',     'x',     'd',     'e',     '\'',    '"',       0,       // 32-39
        0,     ' ',     'v',     'f',     't',     'r',     '(',       0,       // 40-48
        0,     'n',     'b',     'h',     'g',     'y',       0,       0,       // 48-56
        0,       0,     ',',     'j',     'u',       0,     '!',       0,       // 56-63
        0,     ';',     'k',     'i',     'o',       0,       0,       0,       // 64-71
        0,     ':',     '=',     'l',     'm',     'p',     ')',       0,       // 72-79
        0,       0,       0,       0,     '^',     '-',       0,       0,       // 80-87
     CAPS,  R_SHFT,   ENTER,     '$',       0,       0,       0,       0,       // 88-95
        0,     '<',       0,       0,       0,       0,    BKSP,       0,       // 96-103
        0,     '1',       0,     '4',     '7',       0,       0,       0,       // 104-111
      '0',     '.',     '2',     '5',     '6',     '8',     ESC,       0,       // 112-119
      F11,     '+',     '3',     '-',     '*',     '9',       0,       0        // 120-127
    }
};

// This map is with the shift key pressed.
const char key_shift_codes[5][128] = {
    // US Layout
    {                                                                           // Base 10
        0,      F9,       0,      F5,      F3,      F1,      F2,     F12,       // 00-07
        0,     F10,      F8,      F6,      F4,     TAB,     '~',       0,       // 08-15
        0,     ALT,  L_SHFT,      0,     CTRL,     'Q',     '!',       0,       // 16-23
        0,       0,     'Z',     'S',     'A',     'W',     '@',       0,       // 24-31
        0,     'C',     'X',     'D',     'E',     '$',     '#',       0,       // 32-39
        0,     ' ',     'V',     'F',     'T',     'R',     '%',       0,       // 40-47
        0,     'N',     'B',     'H',     'G',     'Y',     '^',       0,       // 48-55
        0,       0,     'M',     'J',     'U',     '&',     '*',       0,       // 56-63
        0,     '<',     'K',     'I',     'O',     ')',     '(',       0,       // 64-71
        0,     '>',     '?',     'L',     ':',     'P',     '_',       0,       // 72-79
        0,       0,    '\"',       0,     '{',     '+',       0,       0,       // 80-87
     CAPS,  R_SHFT,   ENTER,     '}',       0,     '|',       0,       0,       // 88-95
        0,       0,       0,       0,       0,       0,    BKSP,       0,       // 96-103
        0,     '1',       0,     '4',     '7',       0,       0,       0,       // 104-111
       '0',    '.',     '2',     '5',     '6',     '8',     ESC,       0,       // 112-119
      F11,     '+',     '3',     '-',     '*',     '9',       0,       0        // 120-127
    },
    // FR Layout
    {                                                                           // Base 10
        0,      F9,       0,      F5,      F3,      F1,      F2,     F12,       // 00-07
        0,     F10,      F8,      F6,      F4,     TAB,       0,       0,       // 08-15
        0,     ALT,  L_SHFT,      0,     CTRL,     'A',     '1',       0,       // 16-23
        0,       0,     'W',     'S',     'Q',     'Z',     '2',       0,       // 24-31
        0,     'C',     'X',     'D',     'E',     '4',     '3',       0,       // 32-39
        0,     ' ',     'V',     'F',     'T',     'R',     '5',       0,       // 40-47
        0,     'N',     'B',     'H',     'G',     'Y',     '6',       0,       // 48-55
        0,       0,     '?',     'J',     'U',     '7',     '8',       0,       // 56-63
        0,     '.',     'K',     'I',     'O',     '0',     '9',       0,       // 64-71
        0,     '/',       0,     'L',     'M',     'P',       0,       0,       // 72-79
        0,       0,     '%',       0,       0,     '+',       0,       0,       // 80-87
     CAPS,  R_SHFT,   ENTER,       0,       0,       0,       0,       0,       // 88-95
        0,     '>',       0,       0,       0,       0,    BKSP,       0,       // 96-103
        0,     '1',       0,     '4',     '7',       0,       0,       0,       // 104-111
       '0',    '.',     '2',     '5',     '6',     '8',     ESC,       0,       // 112-119
      F11,     '+',     '3',     '-',     '*',     '9',       0,       0        // 120-127
    },
    // GR Layout
    {                                                                           // Base 10
        0,      F9,       0,      F5,      F3,      F1,      F2,     F12,       // 00-07
        0,     F10,      F8,      F6,      F4,     TAB,       0,       0,       // 08-15
        0,     ALT,  L_SHFT,      0,     CTRL,     'Q',     '!',       0,       // 16-23
        0,       0,     'Y',     'S',     'A',     'W',     '\"',      0,       // 24-31
        0,     'C',     'X',     'D',     'E',     '$',       0,       0,       // 32-39
        0,     ' ',     'V',     'F',     'T',     'R',     '%',       0,       // 40-47
        0,     'N',     'B',     'H',     'G',     'Z',     '&',       0,       // 48-55
        0,       0,     'M',     'J',     'U',     '/',     '(',       0,       // 56-63
        0,     ';',     'K',     'I',     'O',     '=',     ')',       0,       // 64-71
        0,     ':',     '_',     'L',       0,     'P',     '?',       0,       // 72-79
        0,       0,       0,       0,       0,       0,       0,       0,       // 80-87
     CAPS,  R_SHFT,   ENTER,     '*',       0,       0,       0,       0,       // 88-95
        0,     '>',       0,       0,       0,       0,    BKSP,       0,       // 96-103
        0,     '1',       0,     '4',     '7',       0,       0,       0,       // 104-111
       '0',    '.',     '2',     '5',     '6',     '8',     ESC,       0,       // 112-119
      F11,     '+',     '3',     '-',     '*',     '9',       0,       0        // 120-127
    },
    // IT Layout
    {                                                                           // Base 10
        0,      F9,       0,      F5,      F3,      F1,      F2,     F12,       // 00-07
        0,     F10,      F8,      F6,      F4,     TAB,     '|',       0,       // 08-15
        0,     ALT,  L_SHFT,      0,     CTRL,     'Q',     '!',       0,       // 16-23
        0,       0,     'Z',     'S',     'A',     'W',    '\"',       0,       // 24-31
        0,     'C',     'X',     'D',     'E',     '$',       0,       0,       // 32-39
        0,     ' ',     'V',     'F',     'T',     'R',     '%',       0,       // 40-47
        0,     'N',     'B',     'H',     'G',     'Y',     '&',       0,       // 48-55
        0,       0,     'M',     'J',     'U',     '/',     '(',       0,       // 56-63
        0,     ';',     'K',     'I',     'O',     '=',     ')',       0,       // 64-71
        0,     ':',     '_',     'L',       0,     'P',     '?',       0,       // 72-79
        0,       0,       0,       0,       0,     '^',       0,       0,       // 80-87
     CAPS,  R_SHFT,   ENTER,     '*',       0,       0,       0,       0,       // 88-95
        0,     '>',       0,       0,       0,       0,    BKSP,       0,       // 96-103
        0,     '1',       0,     '4',     '7',       0,       0,       0,       // 104-111
       '0',    '.',     '2',     '5',     '6',     '8',     ESC,       0,       // 112-119
      F11,     '+',     '3',     '-',     '*',     '9',       0,       0        // 120-127
    },
    // BE Layout
    {                                                                           // Base 10
        0,      F9,       0,      F5,      F3,      F1,      F2,     F12,       // 00-07
        0,     F10,      F8,      F6,      F4,     TAB,       0,       0,       // 08-15
        0,     ALT,  L_SHFT,      0,     CTRL,     'A',     '1',       0,       // 16-23
        0,       0,     'W',     'S',     'Q',     'Z',     '2',       0,       // 24-31
        0,     'C',     'X',     'D',     'E',     '4',     '3',       0,       // 32-39
        0,     ' ',     'V',     'F',     'T',     'R',     '5',       0,       // 40-47
        0,     'N',     'B',     'H',     'G',     'Y',     '6',       0,       // 48-55
        0,       0,     '?',     'J',     'U',     '7',     '8',       0,       // 56-63
        0,     '.',     'K',     'I',     'O',     '0',     '9',       0,       // 64-71
        0,     '/',     '+',     'L',     'M',     'P',       0,       0,       // 72-79
        0,       0,     '%',       0,       0,     '_',       0,       0,       // 80-87
     CAPS,  R_SHFT,   ENTER,     '*',       0,       0,       0,       0,       // 88-95
        0,     '>',       0,       0,       0,       0,    BKSP,       0,       // 96-103
        0,     '1',       0,     '4',     '7',       0,       0,       0,       // 104-111
       '0',    '.',     '2',     '5',     '6',     '8',     ESC,       0,       // 112-119
      F11,     '+',     '3',     '-',     '*',     '9',       0,       0        // 120-127
    }
};

// This map is for when the keycode preceeded by 0xe0.
const char key_E0_codes[56] =
    // General Layout on all Keyboard for the Keypad.
    {                                                                           // Base 10
          0,       0,     '/',       0,       0,       0,       0,       0,     // 72-79
          0,       0,       0,       0,       0,       0,       0,       0,     // 80-87
          0,       0, NUM_ENT,       0,       0,       0,       0,       0,     // 88-95
          0,       0,       0,       0,       0,       0,       0,       0,     // 96-103
          0,     END,       0,    LEFT,    HOME,       0,       0,       0,     // 104-111
         INSERT, DEL,    DOWN,       0,   RIGHT,      UP,       0,    NUML,     // 112-119
          0,       0,   PDOWN,       0,       0,     PUP,   SLOCK,       0,     // 120-127
    };


// Initialise the keyboard and sound routines.
void keyboard_init(void) {
    // Enable pullups on the clock and data lines.
    // This stops them from floating and generating random characters
    // when no keyboard is attached.
    CNCONbits.ON = 1;                          // Turn on Change Notification module.
    P_PS2CLK_PULLUP = 1;                       // Turn on the pullup for pin D6 also called CN15.
    P_PS2DAT_PULLUP = 1;                       // Turn on the pullup for pin D7 also called CN16.

    // Setup Change Notification interrupt.
    P_PS2CLK_INT = 1;                          // Enable PS2CLK (CN15) as a change interrupt
    mCNSetIntPriority(3);                      // Set interrupt priority to 3
    mCNClearIntFlag();                         // Clear the interrupt flag
    mCNIntEnable(1);                           // Enable interrupt

    ps2_state = PS2START;

    // Init global variables.
    inp_queue_head = inp_queue_tail = 0;
}

// Change notification interrupt service routine.
void __ISR( _CHANGE_NOTICE_VECTOR, ipl3) CNInterrupt(void) {
    unsigned char c = 0;

    static char left_shift = 0;
    static char right_shift = 0;
    static char ctrl = 0;
    static char caps_lock = 0;
    static char alt_gr_down = 0;
    static char key_up_code = 0;
    static char key_E0 = 0;
    static unsigned char code = 0;

    // Make sure it was a falling edge.
    if(P_PS2CLK == 0) {
        // Sample the data.
        const int d = P_PS2DAT;

        switch (ps2_state) {
            default:
            case PS2START:
                if (!d) {                     // PS2DAT == 0
                    keyb_count = 8;           // Init bit counter
                    keyb_parity = 0;          // Init parity check
                    code = 0;
                    ps2_state = PS2BIT;
                }
                break;

            case PS2BIT:
                code >>= 1;                    // Shift in data bit
                if (d) code |= 0x80;           // PS2DAT == 1
                keyb_parity ^= code;           // Calculate parity
                if (--keyb_count == 0)         // All bit read
                    ps2_state = PS2PARITY;            
                break;

            case PS2PARITY:
                if (d) keyb_parity ^= 0x80;    // PS2DAT == 1
                if (keyb_parity & 0x80)        // Parity odd, continue
                    ps2_state = PS2STOP;
                else
                    ps2_state = PS2START;
                break;

            case PS2STOP:
                if (d) {                       // PS2DAT == 1
                    if (code == 0xf0)
                        key_up_code = 1;
                    else if(code == 0xe0)
                        key_E0 = 1;
                    else {
                        // Process a scan code from the keyboard into an ASCII character.
                        // It then inserts it into the keyboard queue.
                        if (key_up_code) {
                            if (code == L_SHFT) left_shift = 0;      // Left shift button is released.
                            if (code == R_SHFT) right_shift = 0;     // Right shift button is released.
                            if (code == ctrl) ctrl = 0;              // Left control button is released.
                            goto skip_out;
                        }

                        // We are only here if the key has been pressed (NOT released).

                        if (code == L_SHFT) {                        // Left shift button is pressed.
                            left_shift = 1; 
                            goto skip_out;
                        }
                        
                        if (code == R_SHFT) {                        // Right shift button is pressed.
                            right_shift = 1;
                            goto skip_out;
                        }
                        
                        if (code == ctrl) {                          // Left control button is pressed. 
                            ctrl = 1; 
                            goto skip_out; 
                        }
                        
                        if (code == CAPS) {                          // Caps lock pressed.
                            caps_lock = !caps_lock; 
                            goto skip_out; 
                        }

                        // For the US keyboard and the right Alt key 
                        // we need to make it the same as the left Alt key.
                        if (keyboard_option == CONFIG_US && key_E0 && code == 0x11)
                            key_E0 = 0;

                        // Now get the character into c. Why, oh why, are scan codes so random?
                        if (key_E0 && code == 0x11) {                  // AltGr key pressed on non US Keyboard
                            alt_gr_down = 1;
                            goto skip_out;
                        } else if (key_E0 && code == 0xf0) {           // AltGr key released on non US Keyboard
                            alt_gr_down = 0;
                            goto skip_out;
                        } else if (!key_E0 && code == 0x83) {          // A special case
                            c = F7;                                       
                        } else if (key_E0) {
                            // A keycode preceeded by 0xe0.
                            if ((code >= 0x48 && code < 0x80) && alt_gr_down != 1)
                                c = key_E0_codes[code - 0x48];
                            else
                                c = 0;                                 // Invalid char
                        } else if ((code >= 0x15 && code < 0x62) && alt_gr_down != 0) {  // A keycode preceeded by Alt-Gr
                            switch (keyboard_option) {                                   // An international keycode pressed with
                                case CONFIG_US:                                          // The AltGr key
                                    break;                                               // No code for US keyboard
                                // French Keyboard
                                case CONFIG_FR:                                       
                                    switch (code) {
                                        case 0x45:
                                            c = 0x40;       // @
                                            alt_gr_down = 0;
                                            break;
                                        case 0x25:
                                            c = 0x7b;       // {
                                            alt_gr_down = 0;
                                            break;
                                        case 0x2e:
                                            c = 0x5b;       // [
                                            alt_gr_down = 0;
                                            break;
                                        case 0x55:
                                            c = 0x7d;       // }
                                            alt_gr_down = 0;
                                            break;
                                        case 0x4e:
                                            c = 0x5d;       // ]
                                            alt_gr_down = 0;
                                            break;
                                        case 0x3e:
                                            c = 0x5c;       // '\'
                                            alt_gr_down = 0;
                                            break;
                                        case 0x1e:
                                            c = 0x7e;       // ~
                                            alt_gr_down = 0;
                                            break;
                                        case 0x36:
                                            c = 0x7c;       // |
                                            alt_gr_down = 0;
                                            break;
                                        case 0x26:
                                            c = 0x23;       // #
                                            alt_gr_down = 0;
                                            break;
                                        default:
                                            c = 0;          // Invalid code
                                            alt_gr_down = 0;
                                            break;
                                    }
                                    break;
                                // German Keyboard
                                case CONFIG_GR:                                       
                                    switch (code) {
                                        case 0x15:
                                            c = 0x40;       // @
                                            alt_gr_down = 0;
                                            break;
                                        case 0x3d:
                                            c = 0x7b;       // {
                                            alt_gr_down = 0;
                                            break;
                                        case 0x3e:
                                            c = 0x5b;       // [
                                            alt_gr_down = 0;
                                            break;
                                        case 0x45:
                                            c = 0x7d;       // }
                                            alt_gr_down = 0;
                                            break;
                                        case 0x46:
                                            c = 0x5d;       // ]
                                            alt_gr_down = 0;
                                            break;
                                        case 0x4e:
                                            c = 0x5c;       // '\'
                                            alt_gr_down = 0;
                                            break;
                                        case 0x5b:
                                            c = 0x7e;       // ~
                                            alt_gr_down = 0;
                                            break;
                                        case 0x61:
                                            c = 0x7c;       // |
                                            alt_gr_down = 0;
                                            break;
                                        default:
                                            c = 0;          // Invalid code
                                            alt_gr_down = 0;
                                            break;
                                    }
                                    break;
                                // Italian Keyboard
                                case CONFIG_IT:                          
                                    switch (code) {
                                        case 0x4C:
                                            c = 0x40;       // @
                                            alt_gr_down = 0;
                                            break;
                                        case 0x54:
                                            c = 0x5b;       // [
                                            alt_gr_down = 0;
                                            break;
                                        case 0x5b:
                                            c = 0x5d;       // ]
                                            alt_gr_down = 0;
                                            break;
                                        case 0x52:
                                            c = 0x23;       // #
                                            alt_gr_down = 0;
                                            break;
                                        default:
                                            c = 0;          // invalid code
                                            alt_gr_down = 0;
                                            break;
                                    }
                                    break;
                                // Belgian Keyboard
                                case CONFIG_BE:                          
                                    switch (code) {
                                        case 0x1e:
                                            c = 0x40;       // @
                                            alt_gr_down = 0;
                                            break;
                                        case 0x46:
                                            c = 0x7b;       // {
                                            alt_gr_down = 0;
                                            break;
                                        case 0x54:
                                            c = 0x5b;       // [
                                            alt_gr_down = 0;
                                            break;
                                        case 0x45:
                                            c = 0x7d;       // }
                                            alt_gr_down = 0;
                                            break;
                                        case 0x5b:
                                            c = 0x5d;       // ]
                                            alt_gr_down = 0;
                                            break;
                                        case 0x1a:
                                            c = 0x5c;       // '\'
                                            alt_gr_down = 0;
                                            break;
                                        case 0x4a:
                                            c = 0x7e;       // ~
                                            alt_gr_down = 0;
                                            break;
                                        case 0x16:
                                            c = 0x7c;       // |
                                            alt_gr_down = 0;
                                            break;
                                        case 0x26:
                                            c = 0x23;       // #
                                            alt_gr_down = 0;
                                            break;
                                        default:
                                            c = 0;          // Invalid code
                                            alt_gr_down = 0;
                                            break;
                                    }
                                    break;
                            }
                        } else {
                            switch (keyboard_option) {
                                case CONFIG_US:
                                    if (left_shift || right_shift)
                                        c = key_shift_codes[0][code % 128];  // A keycode preceeded by a shift
                                    else
                                        c = key_codes[0][code % 128];        // Just a keycode
                                    break;
                                case CONFIG_FR:
                                    if (left_shift || right_shift)
                                        c = key_shift_codes[1][code % 128];  // A keycode preceeded by a shift
                                    else
                                        c = key_codes[1][code % 128];        // Just a keycode
                                    break;
                                case CONFIG_GR:
                                    if (left_shift || right_shift)
                                        c = key_shift_codes[2][code % 128];  // A keycode preceeded by a shift
                                    else
                                        c = key_codes[2][code % 128];        // Just a keycode
                                    break;
                                case CONFIG_IT:
                                    if (left_shift || right_shift)
                                        c = key_shift_codes[3][code % 128];  // A keycode preceeded by a shift
                                    else
                                        c = key_codes[3][code % 128];        // Just a keycode
                                    break;
                                case CONFIG_BE:
                                    if (left_shift || right_shift)
                                        c = key_shift_codes[4][code % 128];  // A keycode preceeded by a shift
                                    else
                                        c = key_codes[4][code % 128];        // Just a keycode
                                    break;
                                }
                        }

                        if (!c) goto skip_out;

                        if (c <= 0x7f) {                                // A normal character
                            if (caps_lock && c >= 'a' && c <= 'z')      // Adjust for caps lock
                                c -= 32;
                            if (ctrl) c &= 0x1F;                        // Adjust for control
                        } else {                                        // Must be a function key or similar
                            if (left_shift || right_shift) c |= 0b00100000;
                            if (ctrl) c |= 0b01000000;
                        }

                        // Place into the queue.
                        inp_queue[inp_queue_head] = c;                  
                        // Increment the head index.
                        inp_queue_head = (inp_queue_head + 1) % INP_QUEUE_SIZE;           

                        skip_out:
                        // End lump of self contained code.
                        key_up_code = 0;
                        key_E0 = 0;
                    }
                    code = 0;
                }
                ps2_state = PS2START;
                break;
        }
    }
    // Clear interrupt flag.
    mCNClearIntFlag();
}

int keyboard_inkey(void) {
    int c = -1;
    if (inp_queue_head != inp_queue_tail) {
        c = inp_queue[inp_queue_tail];
        inp_queue_tail = (inp_queue_tail + 1) % INP_QUEUE_SIZE;
    }
   return c;
}
