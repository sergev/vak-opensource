/*
 * USB interface to the PiDP-8 front panel.
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

/*
 * USB packet size.
 */
#define	PACKET_SIZE	    64

/*
 * Initialize the PiDP-8 panel interface.
 */
void pidp_init(void);

/*
 * Run one cycle of the PiDP-8 panel control interface.
 */
void pidp_step(void);

/*
 * Process the packet, received via USB.
 * Prepare an output packet to send.
 */
void pidp_request(const unsigned char *receive, unsigned char *send);

/*
 * Delay for a given number of microseconds.
 * The processor has a 32-bit hardware Count register,
 * which increments at half CPU rate.
 * We use it to get a precise delay.
 */
void udelay (unsigned usec);
