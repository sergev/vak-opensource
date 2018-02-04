/*
 * Testing LoL shield.
 *
 * Writen for the LoL Shield, designed by Jimmie Rodgers:
 * http://jimmieprodgers.com/kits/lolshield/
 *
 * Copyright (C) 2013 Serge Vakulenko, <serge@vak.ru>
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
#include <runtime/lib.h>
#include "devcfg.h"

/* Number of control pins for LoL Shield. */
#define LOL_NPINS   12

/* Number of rows on LoL Shield. */
#define LOL_NROW    9

/* Number of columns on LoL Shield. */
#define LOL_NCOL    14

/* Sequence of pins to set high during refresh cycle. */
static const unsigned pin_mask [LOL_NPINS] = {
    1 << 14,        /* RB14 - labeled D13 on board */
    1 << 13,        /* RB13 - D12 */
    1 << 12,        /* RB12 - D11 */
    1 << 11,        /* RB11 - D10 */
    1 << 10,        /* RB10 - D9 */
    0x10000 << 2,   /* RA2  - D8 */
    0x10000 << 3,   /* RA3  - D7 */
    1 << 9,         /* RB9  - D6 */
    1 << 8,         /* RB8  - D5 */
    0x10000 << 4,   /* RA4  - D4 */
    1 << 7,         /* RB7  - D3 */
    1 << 6,         /* RB6  - D2 */
};

/* Remap pixels to pin indexes. */
static const unsigned char charlieplexing [LOL_NROW*LOL_NCOL*2] =
{
    0,8,0,7,0,6,0,5,0,4,0,3,0,2,0,1,0,9,9,0,0,10,10,0,0,11,11,0,
    1,8,1,7,1,6,1,5,1,4,1,3,1,2,1,0,1,9,9,1,1,10,10,1,1,11,11,1,
    2,8,2,7,2,6,2,5,2,4,2,3,2,1,2,0,2,9,9,2,2,10,10,2,2,11,11,2,
    3,8,3,7,3,6,3,5,3,4,3,2,3,1,3,0,3,9,9,3,3,10,10,3,3,11,11,3,
    4,8,4,7,4,6,4,5,4,3,4,2,4,1,4,0,4,9,9,4,4,10,10,4,4,11,11,4,
    5,8,5,7,5,6,5,4,5,3,5,2,5,1,5,0,5,9,9,5,5,10,10,5,5,11,11,5,
    6,8,6,7,6,5,6,4,6,3,6,2,6,1,6,0,6,9,9,6,6,10,10,6,6,11,11,6,
    7,8,7,6,7,5,7,4,7,3,7,2,7,1,7,0,7,9,9,7,7,10,10,7,7,11,11,7,
    8,7,8,6,8,5,8,4,8,3,8,2,8,1,8,0,8,9,9,8,8,10,10,8,8,11,11,8,
};

/*
 * Display a picture on LoL shield.
 * Duration in milliseconds is specified.
 */
void lol(msec, data)
    unsigned msec;
    const short *data;
{
    unsigned row, mask, amask, bmask;
    const unsigned char *map;
    unsigned low [LOL_NPINS];

    /* Clear pin masks. */
    mask = 0;
    for (row = 0; row < LOL_NPINS; row++) {
    low [row] = 0;
    mask |= pin_mask [row];
    }
    amask = mask >> 16;
    bmask = mask & 0xffff;

    /* Convert image to array of pin masks. */
    for (row = 0; row < LOL_NROW; row++) {
    mask = *data++ & ((1 << LOL_NCOL) - 1);
    map = &charlieplexing [row * LOL_NCOL * 2];
    while (mask != 0) {
        if (mask & 1) {
        low [map[0]] |= pin_mask [map[1]];
        }
        map += 2;
        mask >>= 1;
    }
    }

    /* Display the image. */
    if (msec < 1)
    msec = 20;
    while (msec-- > 0) {
    for (row = 0; row < LOL_NPINS; row++) {
        /* Set all pins to tristate. */
        TRISASET = amask;
        TRISBSET = bmask;

        /* Set one pin to high. */
        mask = pin_mask [row];
        if (mask & 0xffff) {
        TRISBCLR = mask;
        LATBSET = mask;
        } else {
        mask >>= 16;
        TRISACLR = mask;
        LATASET = mask;
        }

        /* Set other pins to low. */
        mask = low [row];
        TRISBCLR = mask & 0xffff;
        LATBCLR = mask & 0xffff;
        mask >>= 16;
        TRISACLR = mask;
        LATACLR = mask;

        /* Pause to make it visible. */
        udelay(1000 / LOL_NPINS);

        /* Set all low. */
        LATACLR = amask;
        LATBCLR = bmask;
    }
    }

    /* Turn display off. */
    TRISASET = amask;
    TRISBSET = bmask;
}

/*
 * Demo 1:
 * 1) vertical lines;
 * 2) horizontal lines;
 * 3) all LEDs on.
 */
void demo1()
{
    static unsigned short picture[9];
    int y, frame;

    for (;;) {
    for (frame = 0; frame<14; frame++) {
        memset(picture, 0, sizeof(picture));

        for (y=0; y<9; y++)
        picture[y] |= 1 << frame;

        /* Display a frame. */
        lol(100, picture);
    }
    for (frame = 0; frame<9; frame++) {
        memset(picture, 0, sizeof(picture));

        picture[frame] = (1 << 14) - 1;

        /* Display a frame. */
        lol(100, picture);
    }
    memset(picture, 0xFF, sizeof(picture));

    /* Display a frame. */
    lol(250, picture);
    lol(250, picture);
    }
}

/*
 * Demo 2: bouncing ball.
 */
void demo2()
{
    static unsigned short picture[9];
    int x, y, dx, dy;

    memset(picture, 0, sizeof(picture));
    x = 0;
    y = 0;
    dx = 1;
    dy = 1;
    for (;;) {
    /* Draw ball. */
    picture[y] |= 1 << x;
    lol(50, picture);
    picture[y] &= ~(1 << x);

    /* Move the ball. */
    x += dx;
    if (x < 0 || x >= 14) {
        dx = -dx;
        x += dx + dx;
    }
    y += dy;
    if (y < 0 || y >= 9) {
        dy = -dy;
        y += dy + dy;
    }
    }
}

#define ROW(a,b,c,d,e,f,g) (a | b<<1 | c<<2 | d<<3 | e<<4 | f<<5 | g<<6)
#define _ 0
#define O 1

#define __  ROW(_,_,_,_,_,_,_)
#define _space __,__,__
#define _comma \
            ROW(O,O,_,_,_,_,_), \
            ROW(_,O,O,_,_,_,_), \
            __,__
#define _exclam __,\
            ROW(O,_,O,O,O,O,O), \
            __,__
#define _dot \
            ROW(O,O,_,_,_,_,_), \
            ROW(O,O,_,_,_,_,_), \
            __,__,__
#define _A  ROW(O,O,O,O,O,_,_), \
            ROW(_,_,O,_,_,O,_), \
            ROW(_,_,O,_,_,_,O), \
            ROW(_,_,O,_,_,O,_), \
            ROW(O,O,O,O,O,_,_), \
            __
#define _B  ROW(O,O,O,O,O,O,O), \
            ROW(O,_,_,O,_,_,O), \
            ROW(O,_,_,O,_,_,O), \
            ROW(O,_,_,O,_,_,O), \
            ROW(_,O,O,_,O,O,_), \
            __
#define _C  ROW(_,O,O,O,O,O,_), \
            ROW(O,_,_,_,_,_,O), \
            ROW(O,_,_,_,_,_,O), \
            ROW(O,_,_,_,_,_,O), \
            ROW(_,O,_,_,_,O,_), \
            __
#define _D  ROW(O,O,O,O,O,O,O), \
            ROW(O,_,_,_,_,_,O), \
            ROW(O,_,_,_,_,_,O), \
            ROW(_,O,_,_,_,O,_), \
            ROW(_,_,O,O,O,_,_), \
            __
#define _E  ROW(O,O,O,O,O,O,O), \
            ROW(O,_,_,O,_,_,O), \
            ROW(O,_,_,O,_,_,O), \
            ROW(O,_,_,O,_,_,O), \
            ROW(O,_,_,_,_,_,O), \
            __
#define _F  ROW(O,O,O,O,O,O,O), \
            ROW(_,_,_,O,_,_,O), \
            ROW(_,_,_,O,_,_,O), \
            ROW(_,_,_,O,_,_,O), \
            ROW(_,_,_,_,_,_,O), \
            __
#define _G  ROW(_,O,O,O,O,O,_), \
            ROW(O,_,_,_,_,_,O), \
            ROW(O,_,_,_,_,_,O), \
            ROW(O,_,_,O,_,_,O), \
            ROW(_,O,O,O,_,O,_), \
            __
#define _H  ROW(O,O,O,O,O,O,O), \
            ROW(_,_,_,O,_,_,_), \
            ROW(_,_,_,O,_,_,_), \
            ROW(_,_,_,O,_,_,_), \
            ROW(O,O,O,O,O,O,O), \
            __
#define _I  ROW(O,_,_,_,_,_,O), \
            ROW(O,O,O,O,O,O,O), \
            ROW(O,_,_,_,_,_,O), \
            __
#define _K  ROW(O,O,O,O,O,O,O), \
            ROW(_,_,_,O,_,_,_), \
            ROW(_,_,_,O,O,_,_), \
            ROW(_,_,O,_,_,O,_), \
            ROW(O,O,_,_,_,_,O), \
            __
#define _L  ROW(O,O,O,O,O,O,O), \
            ROW(O,_,_,_,_,_,_), \
            ROW(O,_,_,_,_,_,_), \
            ROW(O,_,_,_,_,_,_), \
            ROW(O,_,_,_,_,_,_), \
            __
#define _M  ROW(O,O,O,O,O,O,O), \
            ROW(_,_,_,_,_,O,_), \
            ROW(_,_,_,_,O,_,_), \
            ROW(_,_,_,_,_,O,_), \
            ROW(O,O,O,O,O,O,O), \
            __
#define _N  ROW(O,O,O,O,O,O,O), \
            ROW(_,_,_,_,O,_,_), \
            ROW(_,_,_,O,_,_,_), \
            ROW(_,_,O,_,_,_,_), \
            ROW(O,O,O,O,O,O,O), \
            __
#define _O  ROW(_,O,O,O,O,O,_), \
            ROW(O,_,_,_,_,_,O), \
            ROW(O,_,_,_,_,_,O), \
            ROW(O,_,_,_,_,_,O), \
            ROW(_,O,O,O,O,O,_), \
            __
#define _P  ROW(O,O,O,O,O,O,O), \
            ROW(_,_,_,O,_,_,O), \
            ROW(_,_,_,O,_,_,O), \
            ROW(_,_,_,O,_,_,O), \
            ROW(_,_,_,_,O,O,_), \
            __
#define _R  ROW(O,O,O,O,O,O,O), \
            ROW(_,_,_,O,_,_,O), \
            ROW(_,_,O,O,_,_,O), \
            ROW(_,O,_,O,_,_,O), \
            ROW(O,_,_,_,O,O,_), \
            __
#define _S  ROW(_,_,_,_,O,O,_), \
            ROW(O,_,_,O,_,_,O), \
            ROW(O,_,_,O,_,_,O), \
            ROW(O,_,_,O,_,_,O), \
            ROW(_,O,O,_,_,_,_), \
            __
#define _T  ROW(_,_,_,_,_,_,O), \
            ROW(_,_,_,_,_,_,O), \
            ROW(O,O,O,O,O,O,O), \
            ROW(_,_,_,_,_,_,O), \
            ROW(_,_,_,_,_,_,O), \
            __
#define _U  ROW(_,O,O,O,O,O,O), \
            ROW(O,_,_,_,_,_,_), \
            ROW(O,_,_,_,_,_,_), \
            ROW(O,_,_,_,_,_,_), \
            ROW(_,O,O,O,O,O,O), \
            __
#define _V  ROW(_,_,O,O,O,O,O), \
            ROW(_,O,_,_,_,_,_), \
            ROW(O,_,_,_,_,_,_), \
            ROW(_,O,_,_,_,_,_), \
            ROW(_,_,O,O,O,O,O), \
            __
#define _W  ROW(_,O,O,O,O,O,O), \
            ROW(O,_,_,_,_,_,_), \
            ROW(_,O,O,O,O,O,_), \
            ROW(O,_,_,_,_,_,_), \
            ROW(_,O,O,O,O,O,O), \
            __
#define _X  ROW(O,O,_,_,_,O,O), \
            ROW(_,_,O,_,O,_,_), \
            ROW(_,_,_,O,_,_,_), \
            ROW(_,_,O,_,O,_,_), \
            ROW(O,O,_,_,_,O,O), \
            __
#define _Y  ROW(_,_,_,_,_,O,O), \
            ROW(_,_,_,_,O,_,_), \
            ROW(O,O,O,O,_,_,_), \
            ROW(_,_,_,_,O,_,_), \
            ROW(_,_,_,_,_,O,O), \
            __
//
// One, two, buckle my shoe.
// Three, four, knock on the door.
// Five, six, pick up sticks.
// Seven, eight, lay them straight.
// Nine, ten, do it again!
//
static const unsigned char phrase [] = {
    __,_O,_N,_E,_comma,_T,_W,_O,_comma,_B,_U,_C,_K,_L,_E,_space,_M,_Y,_space,_S,_H,_O,_E,_dot,
    _T,_H,_R,_E,_E,_comma,_F,_O,_U,_R,_comma,_K,_N,_O,_C,_K,_space,_O,_N,_space,_T,_H,_E,_space,_D,_O,_O,_R,_dot,
    _F,_I,_V,_E,_comma,_S,_I,_X,_comma,_P,_I,_C,_K,_space,_U,_P,_space,_S,_T,_I,_C,_K,_S,_dot,
    _S,_E,_V,_E,_N,_comma,_E,_I,_G,_H,_T,_comma,_L,_A,_Y,_space,_T,_H,_E,_M,_space,_S,_T,_R,_A,_I,_G,_H,_T,_dot,
    _N,_I,_N,_E,_comma,_T,_E,_N,_comma,_D,_O,_space,_I,_T,_space,_A,_G,_A,_I,_N,_exclam,
    __, __, __, __, __, __, __, __, __, __, __, __, __, __,
    0xff
};

void shift_picture(unsigned short *picture, int next)
{
    int y;

    for (y=0; y<9; y++) {
    picture[y] >>= 1;
    if (y > 0 && ((next << y) & 0x80))
        picture[y] |= 1 << 13;
    }
}

/*
 * Demo 3: moving text.
 */
void demo3()
{
    static unsigned short picture[9];
    const unsigned char *next;

    for (;;) {
    memset(picture, 0, sizeof(picture));
    for (next=phrase; *next!=0xff; next++) {
        shift_picture(picture, *next);
        lol(70, picture);
    }
    }
}

int main()
{
    /* Unlock CFGCON register. */
    SYSKEY = 0;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    CFGCON &= (1 << 13);            // clear IOLOCK

    /* Disable JTAG ports, to make more pins available. */
    CFGCON &= (1 << 3);             // clear JTAGEN

    /* Use all ports as digital. */
    ANSELA = 0;
    ANSELB = 0;

    for (;;) {
    //demo1();
    //demo2();
    demo3();
    }
}
