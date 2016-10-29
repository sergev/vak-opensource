/*
 * Sample program for demonstrating the usage of interrupts
 * and serial port i/o. Try "sim17 hello.hex".
 *
 * Copyright (C) 1997-2002 Serge Vakulenko <vak@cronyx.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 */
#include "pic17c43.h"

#define HZ      10      /* Timer ticks per second. */
#define BAUD    9600    /* Baud rate of the serial port. */
#define MESSAGE "Hello from Cronyx!"

#define OUTBUFSZ       40
#define INBUFSZ        20

/* Transmit ring buffer. */
char out_buf [OUTBUFSZ];
char *out_first;
char *out_last;

/* Receive ring buffer. */
char in_buf [INBUFSZ];
char *in_first;
char *in_last;

/* Timer ticks counter. */
short clock;

void main ();
void handle_timer ();
void handle_txd ();
void handle_rxd ();
void putchar ();
void print ();
void prints ();
void idle ();

char save_wreg @ 0x1a;
char save_status @ 0x1b;
char save_bsr @ 0x1c;
char save_pclath;
short save_tblptr;
char save_fsr0;
long save_a4567;

asm ("_a4567 .equ A4");
long a4567;

#define ISAVE \
	save_wreg = WREG; \
	save_status = ALUSTA; \
	save_bsr = BSR

#define IRESTORE \
	WREG = save_wreg; \
	ALUSTA = save_status; \
	BSR = save_bsr

#define ISAVE2 \
	reg0 (); \
	save_pclath = PCLATH; \
	save_tblptr = TBLPTR; \
	save_a4567 = a4567; \
	save_fsr0 = FSR0

#define IRESTORE2 \
	FSR0 = save_fsr0; \
	a4567 = save_a4567; \
	TBLPTR = save_tblptr; \
	PCLATH = save_pclath

void _startup () @ 0
{
	main ();
}

interrupt void _intr_timer0 () @ 0x10
{
	ISAVE;
	handle_timer ();
	IRESTORE;
}

interrupt void _intr_periph () @ 0x20
{
	ISAVE;
	ISAVE2;
	reg1 ();
	if (RCIF) {
		reg0 ();
		handle_rxd ();
		reg1 ();
	}
	if (TXIE && TXIF) {
		reg0 ();
		handle_txd ();
		reg1 ();
	}
	PIR = 0;
	reg0 ();
	IRESTORE2;
	IRESTORE;
}

void init ()
{
	BSR = 0;

	/* Timer initialization.
	 * Make HZ interrupts per second.
	 * Use 1:128 prescaler. */
	T0CS = 1;
	PS3 = 0;
	PS2 = 1;
	PS1 = 1;
	PS0 = 1;
	TMR0 = - (MHZ / 128 / HZ + 2) / 4;

	/* Serial port initialization. */
	RCSTA = 0;
	TXSTA = 0;
	SPBRG = ((MHZ / BAUD) + 32) / 64 - 1;
	SPEN = 1;
	CREN = 1;
	TXEN = 1;
	in_first = in_buf;
	in_last = in_buf;
	out_first = out_buf;
	out_last = out_buf;

	/* Initialize the interrupt registers.
	 * Enable interrupts from timer, serial transmitter and receiver. */
	reg1 ();
	PIR = 0;
	PIE = 0;
	TXIE = 1;
	RCIE = 1;
	reg0 ();

	INTSTA = 0;
	PEIE = 1;
	T0IE = 1;
}

void print (char far *s)
{
	char c;

	for (;;) {
		c = *s++;
		if (! c)
			return;
		putchar (c);
	}
}

void puts (char *s)
{
	char c;

	for (;;) {
		c = *s++;
		if (! c)
			return;
		putchar (c);
	}
}

void prints (short v)
{
	if (v > 9999) putchar ((char) (v / 10000) + '0');
	if (v > 999)  putchar ((char) ((v / 1000) % 10) + '0');
	if (v > 99)   putchar ((char) ((v / 100) % 10) + '0');
	if (v > 9)    putchar ((char) ((v / 10) % 10) + '0');
	putchar ((char) (v % 10) + '0');
}

void idle ()
{
	awake ();
}

/*
 * Timer interrupt handler.
 */
void handle_timer ()
{
	ISAVE2;
	++clock;
	TMR0 = - (MHZ / 128 / HZ + 2) / 4;
	IRESTORE2;
}

/*
 * Start the transmittion of the next byte from the queue.
 * Disabling RTS stops the transmittion.
 */
void out_start ()
{
	if (out_first == out_last)
		return;
	TXREG = *out_first++;
	if (out_first >= out_buf + OUTBUFSZ)
		out_first = out_buf;
	reg1 ();
	TXIE = 1;
	reg0 ();
}

/*
 * Put the byte to the transmit queue.
 * Block the process if the queue is full.
 */
void putchar (char c)
{
	int newlast;

	/* Wait for free place in the transmit queue. */
	newlast = out_last + 1;
	if (newlast >= out_buf + OUTBUFSZ)
		newlast = out_buf;
	while (out_first == newlast)
		idle ();
	*out_last = c;
	out_last = newlast;
	reg1 ();
	if (! TXIE) {
		reg0 ();
		out_start ();
	}
	reg0 ();
}

/*
 * Transmit interrupt handler.
 */
void handle_txd ()
{
	reg1 ();
	TXIE = 0;
	reg0 ();
	out_start ();
}

void handle_rxd ()
{
	int newlast;

	if (FERR) {
		/* Framing error, ignore the data. */
		WREG = RCREG;
	} else {
		newlast = in_last + 1;
		if (newlast >= in_buf + INBUFSZ)
			newlast = in_buf;
		if (in_first == newlast) {
			/* Input queue overflow, ignore the data. */
			WREG = RCREG;
		} else {
			*in_last = RCREG;
			in_last = newlast;
		}
	}
	/* Clear the overflow flag. */
	OERR = 0;
}

/*
 * Get the byte from the receive queue.
 * Block the process if the queue is empty.
 */
char getchar ()
{
	char in;

	/* Wait for data in the receive queue. */
	while (in_first == in_last)
		idle ();
	in = *in_first++;
	if (in_first >= in_buf + INBUFSZ)
		in_first = in_buf;
	return in;
}

void gets (char *p)
{
	char c;

	for (;;) {
		c = getchar ();
		if (c == '\r')
			c = '\n';
		putchar (c);
		if (c == '\n') {
			*p = 0;
			return;
		}
		*p++ = c;
	}
}

void main ()
{
	char buf [80];

	awake ();
	init ();
	intr_enable ();

	for (;;) {
		prints (clock);
		print (": ");
		print (MESSAGE);
		print ("\r\nEnter any string: ");
		gets (buf);
		print ("\r\nYour string: ");
		puts (buf);
		print ("\r\n\r\n");
	}
}
