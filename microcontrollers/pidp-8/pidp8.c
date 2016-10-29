/*
 * Interface to the PiDP-8 front panel.
 *
 * Copyright (C) 2016 Serge Vakulenko, <serge@vak.ru>
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
#include "pidp8.h"
#include "pic32mx.h"
#include <stdio.h>

/*
 * col pins: port D
 */
#define COL1        (1 << 0)
#define COL2        (1 << 1)
#define COL3        (1 << 2)
#define COL4        (1 << 3)
#define COL5        (1 << 4)
#define COL6        (1 << 5)
#define COL7        (1 << 6)
#define COL8        (1 << 7)
#define COL9        (1 << 8)
#define COL10       (1 << 9)
#define COL11       (1 << 10)
#define COL12       (1 << 11)
#define COL_INPUT   TRISDSET
#define COL_OUTPUT  TRISDCLR
#define COL_SET     LATDSET
#define COL_CLR     LATDCLR

/*
 * xled pins: port E
 */
#define XLED1       (1 << 0)
#define XLED2       (1 << 1)
#define XLED3       (1 << 2)
#define XLED4       (1 << 3)
#define XLED5       (1 << 4)
#define XLED6       (1 << 5)
#define XLED7       (1 << 6)
#define XLED8       (1 << 7)
#define XLED_OUTPUT TRISECLR
#define XLED_SET    LATESET
#define XLED_CLR    LATECLR

/*
 * row pins: port G
 */
#define ROW1        (1 << 6)
#define ROW2        (1 << 7)
#define ROW3        (1 << 8)
#define ROW_INPUT   TRISGSET

/*
 * Initialize the interface.
 */
void pidp_init()
{
    /* Configure xledK pins as outputs. */
    XLED_OUTPUT = XLED1 | XLED2 | XLED3 | XLED4 |
                  XLED5 | XLED6 | XLED7 | XLED8;

    /* Configure rowJ pins as inputs. */
    ROW_INPUT = ROW1 | ROW2 | ROW3;

    /* Configure colN pins as inputs. */
    COL_INPUT = COL1 | COL2 | COL3 | COL4  | COL5  | COL6 |
                COL7 | COL8 | COL9 | COL10 | COL11 | COL12;

    /* Enable pull-up resistors on colN pins. */
    //TODO
}

/*
 * Simulate one cycle of the interface.
 */
void pidp_step()
{
    int n, k;

    /* For every colN pin: */
    for (n=0; n<12; n++) {
        /* For every xledK pin:
         * Set xledK pin HIGH when the corresponding LED[K,N] should be ON.
         * Otherwise set it LOW. */
        for (k=0; k<12; k++) {
            if (/*led[n][k]*/ 1)
                XLED_SET = 1 << k;
            else
                XLED_CLR = 1 << k;
        }

        /* Set colN pin LOW. */
        COL_CLR = 1 << n;

        /* Switch colN pin to output. */
        COL_OUTPUT = 1 << n;

        /* Wait 300usec. */
        udelay(300);

        /* Set colN pin HIGH. */
        COL_SET = 1 << n;

        /* Switch colN pin to input. */
        COL_INPUT = 1 << n;
    }

    //TODO
#if 0
(6) For every xledK pin:
    Set xledK pin LOW.
(7) For every rowJ pin:
    (*) Switch rowJ pin to output.
    (*) Set rowJ pin LOW.
    (*) Wait 3usec.
    (*) For every colN pin:
        Read colN pin - store it as a state of sw[J,N] switch.
    (*) Switch rowJ pin to input.
#endif
}

/*
 * Clear an array.
 */
static void memzero (void *data, unsigned nbytes)
{
    unsigned char *charp = (unsigned char*) data;

    while (nbytes-- > 0)
        *charp++ = 0;
}


/*
 * Process the packet, received via USB.
 * Prepare an output packet to send.
 */
void pidp_request(const unsigned char *receive, unsigned char *send)
{
    //TODO
    memzero (&send, PACKET_SIZE);
}
