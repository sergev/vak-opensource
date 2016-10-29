#include "uart.h"

/*
 * UART registers
 */
#define _REG8(a)	*(volatile unsigned char*)(0x14000000 + (a))

#define UART_RBR	_REG8 (0x3f8)	/* receive buffer */
#define UART_THR	_REG8 (0x3f8)	/* transmit buffer */
#define UART_IER	_REG8 (0x3f8+1)	/* interrupt enable */
#define UART_IIR	_REG8 (0x3f8+2)	/* interrupt identification */
#define UART_FCR	_REG8 (0x3f8+2)	/* FIFO control */
#define UART_LCR	_REG8 (0x3f8+3)	/* line control */
#define UART_MCR	_REG8 (0x3f8+4)	/* modem control */
#define UART_LSR	_REG8 (0x3f8+5)	/* line status */
#define UART_MSR	_REG8 (0x3f8+6)	/* modem status */
#define UART_SPR	_REG8 (0x3f8+7)	/* scratchpad */
#define UART_DLL	_REG8 (0x3f8)	/* divisor lsb */
#define UART_DLM	_REG8 (0x3f8+1)	/* divisor msb */

/*
 * Line control register
 */
#define LCR_5BITS		0x00    /* character length: 5 bits */
#define LCR_6BITS		0x01    /* character length: 6 bits */
#define LCR_7BITS		0x02    /* character length: 7 bits */
#define LCR_8BITS		0x03    /* character length: 8 bits */

#define LCR_STOPB		0x04    /* use 2 stop bits */
#define LCR_PENAB		0x08    /* parity enable */
#define LCR_PEVEN		0x10    /* even parity */
#define LCR_PFORCE		0x20    /* force parity */
#define LCR_SBREAK		0x40    /* break control */
#define LCR_DLAB		0x80    /* divisor latch access bit */

/*
 * FIFO control register
 */
#define FCR_ENABLE		0x01	/* enable FIFO */
#define FCR_RCV_RST		0x02	/* clear receive FIFO */
#define FCR_XMT_RST		0x04	/* clear transmit FIFO */

#define FCR_TRIGGER_1		0x00	/* receive FIFO level: 1/4 byte */
#define FCR_TRIGGER_4		0x40	/* receive FIFO level: 4/16 bytes */
#define FCR_TRIGGER_8		0x80	/* receive FIFO level: 8/56 bytes */
#define FCR_TRIGGER_14		0xc0	/* receive FIFO level: 14/60 bytes */

/*
 * Line status register
 */
#define LSR_RXRDY		0x01	/* receiver ready */
#define LSR_OE			0x02	/* overrun error */
#define LSR_PE			0x04	/* parity error */
#define LSR_FE			0x08	/* framing error */
#define LSR_BI			0x10	/* break interrupt */
#define LSR_TXRDY		0x20	/* transmitter holding register empty */
#define LSR_TEMT		0x40	/* transmitter empty */
#define LSR_FIFOE		0x80	/* error in receive FIFO */

/*
 * Interrupt enable register
 */
#define IER_ERXRDY		0x01	/* enable receive data/timeout intr */
#define IER_ETXRDY		0x02	/* enable transmitter interrupts */
#define IER_ERLS		0x04	/* enable receive line status intr */
#define IER_EMSC		0x08	/* enable modem status interrupts */

/*
 * Interrupt identification register
 */
#define IIR_NOPEND		0x01	/* no interrupt pending */
#define IIR_IMASK		0x0e	/* interrupt type mask */
#define IIR_FENAB		0xc0	/* set if FIFOs are enabled */

#define IIR_RLS			0x06	/* receiver line status */
#define IIR_RXRDY		0x04	/* receiver data available */
#define IIR_RXTOUT		0x0c	/* character timeout indication */
#define IIR_TXRDY		0x02	/* transmitter holding reg empty */
#define IIR_MLSC		0x00	/* modem status */

/*
 * Modem control register
 */
#define MCR_DTR			0x01	/* control DTR output */
#define MCR_RTS			0x02	/* control RTS output */
#define MCR_OUT1		0x04	/* control OUT1 output */
#define MCR_OUT2		0x08	/* control OUT2 output, used as
					 * global interrupt enable in PCs */
#define MCR_LOOPBACK		0x10	/* set local loopback mode */

/*
 * Modem status register
 */
#define MSR_DCTS		0x01	/* CTS changed */
#define MSR_DDSR		0x02	/* DSR changed */
#define MSR_TERI		0x04	/* RI changed from 0 to 1 */
#define MSR_DDCD		0x08	/* DCD changed */
#define MSR_CTS			0x10	/* CTS input */
#define MSR_DSR			0x20	/* DSR input */
#define MSR_RI			0x40	/* RI input */
#define MSR_DCD			0x80	/* DCD input */

/*
 * Compute the 16-bit baud rate divisor, given
 * the oscillator frequency and baud rate.
 * Round to the nearest integer.
 */
#define DL_BAUD(fr,bd)	(((fr)/8 + (bd)) / (bd) / 2)

/*
 * Initialize UART registers.
 */
void uart_init ()
{
	/* Setup UART. */
	unsigned divisor = DL_BAUD (50000000, 115200);
	UART_LCR = LCR_8BITS | LCR_DLAB;
	UART_DLM = divisor >> 8;
	UART_DLL = divisor;
	UART_LCR = LCR_8BITS;
	UART_SPR = 0;
	UART_IER = 0;
	UART_MSR = 0;
	UART_MCR = MCR_DTR | MCR_RTS | MCR_OUT2;
	UART_FCR = FCR_RCV_RST | FCR_XMT_RST | FCR_ENABLE;

	/* Clear pending status, data and irq. */
	(void) UART_LSR;
	(void) UART_MSR;
	(void) UART_RBR;
	(void) UART_IIR;
}

/*
 * Send a byte to UART.
 */
void uart_putchar (int c)
{
	/* Wait for transmitter holding register empty. */
	while (! (UART_LSR & LSR_TXRDY))
		continue;
again:
	/* Send byte. */
	UART_THR = c;

	/* Wait for transmitter holding register empty. */
	while (! (UART_LSR & LSR_TXRDY))
		continue;

	if (c == '\n') {
		/* Add CR after NL. */
		c = '\r';
		goto again;
	}
}

/*
 * Send a string to UART.
 */
void uart_puts (const char *p)
{
	for (; *p; ++p)
		uart_putchar (*p);
}
