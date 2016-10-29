/*
 * Copyright (C) 1999 Marek Michalkiewicz <marekm@linux.org.pl>
 * Copyright (C) 2000 Serge Vakulenko <vak@cronyx.ru>
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * without any conditions or restrictions.  This software is provided
 * "as is" without express or implied warranty.
 */
#ifndef _IO_H_
#define _IO_H_

/*
 * Global interrupt control.
 */
#define sei()  asm volatile ("sei" ::: "memory")
#define cli()  asm volatile ("cli" ::: "memory")

/*
 * Read a byte from the program space (flash).
 * Only low 64 kbytes are addressable.
 */
#define readb(addr) ({					\
	register unsigned char t;			\
	asm volatile (					\
		"lpm" "\n"				\
		"mov	%0,r0"				\
		: "=r" (t)				\
		: "z" ((int)addr));			\
	t; })

/*
 * Memory mapped I/O - use for non-constant I/O space addresses.
 */
#define __mmiob(port) (*((volatile unsigned char*)((char)(port) + 0x20)))

#define __mminw(port) (*((volatile unsigned short*)((char)(port) + 0x20)))

#define __mmoutw(val, port)				\
	asm volatile (					\
		"std Z+1, %B0" "\n" /* MSB first */	\
		"st Z, %A0"				\
		: /* no outputs */			\
		: "r" ((short)(val)),			\
		  "z" ((char)(port) + 0x20))

/*
 * Dumb versions of I/O macros.
 * The order of parameters is taken from Linux.
 */
#define inb_far(port)		__mmiob (port)
#define outb_far(val, port)	do { __mmiob (port) = (val); } while (0)
#define inw_far(port)		__mminw (port)
#define outw_far(val, port)	__mmoutw ((val), (port))
#define setb_far(bit, port)	do { __mmiob (port) |= 1 << (bit); } while (0)
#define clearb_far(bit, port)	do { __mmiob (port) &= ~(1 << (bit)); } while (0)
#define testb_far(bit, port)	(__mmiob (port) & (1 << (bit)))

/*
 * Macros for access to 8-bit I/O registers with constant address.
 * The order of parameters is as in Linux.
 */
#define outb_const(val, port)				\
	asm volatile (					\
		"out %1, %0"				\
		: /* no outputs */			\
		: "r" ((char)(val)),			\
		  "I" ((char)(port)))

#define outb_const_zero(port)				\
	asm volatile (					\
		"out %0, __zero_reg__"			\
		: /* no outputs */			\
		: "I" ((char)(port)))

#define inb_const(port) ({				\
	register unsigned char t;			\
	asm volatile (					\
		"in %0, %1"				\
		: "=r" (t)				\
		: "I" ((char)(port)));			\
	t; })

/*
 * Macros for access to 16-bit I/O registers (ADC, ICR1, OCR1, TCNT1) -
 * read low byte first, then high byte (from the TEMP register),
 * write high byte first (to the TEMP register), then low byte.
 */
#define outw_const(val, port)				\
	asm volatile (					\
		"out (%1)+1, %B0" "\n"			\
		"out %1, %A0"				\
		: /* no outputs */			\
		: "r" ((short)(val)),			\
		  "I" ((char)(port)))

#define inw_const(port) ({				\
	register unsigned short t;			\
	asm volatile (					\
		"in %A0, %1" "\n"			\
		"in %B0, (%1)+1"			\
		: "=r" (t)				\
		: "I" ((char)(port)));			\
	t; })

/*
 * Bit handling macros.
 */
#define setb_const(bit, port) 				\
	asm volatile (					\
		"sbi %0, %1"				\
		: /* no outputs */			\
		: "I" ((char)(port)),			\
		  "I" ((char)(bit)))

#define clearb_const(bit, port) 			\
	asm volatile (					\
		"cbi %0, %1"				\
		: /* no outputs */			\
		: "I" ((char)(port)),			\
		  "I" ((char)(bit)))

#define testb_const(bit, port) ({			\
	register unsigned char t;			\
	asm volatile (					\
		"clr %0" "\n"				\
		"sbic %1, %2" "\n"			\
		"inc %0"				\
		: "=r" (t)				\
		: "I" ((char)(port)),			\
		  "I" ((char)(bit)));			\
	t; })

/*
 * Optimized versions of I/O macros.
 */
#ifdef __OPTIMIZE__

#define inb(port) (! __builtin_constant_p (port) || (port)>=0x40 ? \
	inb_far (port) : inb_const (port))

#define outb(val, port) do {				\
	if (! __builtin_constant_p (port) || (port)>=0x40) \
		outb_far (val, port);			\
	else if (__builtin_constant_p (val) && (val) == 0) \
		outb_const_zero (port);			\
	else						\
		outb_const ((val), (port));		\
	} while (0)

#define inw(port) (! __builtin_constant_p (port) || (port)>=0x40 ? \
	inw_far (port) : inw_const (port))

#define outw(val, port) do {				\
	if (! __builtin_constant_p (port) || (port)>=0x40) \
		outw_far (val, port);			\
	else						\
		outw_const ((val), (port));		\
	} while (0)

#define setb(bit, port) do {				\
	if (! __builtin_constant_p (port) || (port)>=0x40) \
		setb_far (bit, port);			\
	else if (! __builtin_constant_p (bit))		\
		asm volatile (				\
		"in __tmp_reg__, %0" "\n"		\
		"or __tmp_reg__, %1" "\n"		\
		"out %0, __tmp_reg__"			\
		: /* no outputs */			\
		: "I" ((char)(port)),			\
		  "r" ((char)1 << (bit)));		\
	else if ((port) >= 0x20) {			\
		register char dummy;			\
		asm volatile (				\
		"in %0, %1" "\n"			\
		"ori %0, %2" "\n"			\
		"out %1, %0"				\
		: "=r" (dummy)				\
		: "I" ((char)(port)),			\
		  "M" ((unsigned char)1 << (bit)));	\
	} else						\
		setb_const (bit, port);			\
	} while (0)

#define clearb(bit, port) do {				\
	if (! __builtin_constant_p(port) || (port)>=0x40) \
		clearb_far (bit, port);			\
	else if (! __builtin_constant_p(bit))		\
		asm volatile (				\
		"in __tmp_reg__, %0" "\n"		\
		"and __tmp_reg__, %1" "\n"		\
		"out %0, __tmp_reg__"			\
		: /* no outputs */			\
		: "I" ((char)(port)),			\
		  "r" ((char)~((char)1 << (bit))));	\
	else if ((port) >= 0x20) {			\
		register char dummy;			\
		asm volatile (				\
		"in %0, %1" "\n"			\
		"andi %0, %2" "\n"			\
		"out %1, %0"				\
		: "=r" (dummy)				\
		: "I" ((char)(port)),			\
		  "M" (0xff & ~(1 << (bit))));		\
	} else						\
		clearb_const (bit, port);		\
	} while (0)

#define testb(bit, port) ({				\
	unsigned char t;				\
	if (! __builtin_constant_p (port) || (port)>=0x40) \
		t = testb_far (bit, port);		\
	else if (! __builtin_constant_p (bit))		\
		t = (inb_const (port) & (1 << (bit))) != 0; \
	else if ((port) >= 0x20)			\
		asm volatile (				\
		"clr %0" "\n"				\
		"in __tmp_reg__, %1" "\n"		\
		"sbrc __tmp_reg__, %2" "\n"		\
		"inc %0"				\
		: "=r" (t)				\
		: "I" ((char)(port)),			\
		  "I" ((char)(bit)));			\
	else						\
		t = testb_const (bit, port);		\
	t; })

#else /*__OPTIMIZE__*/

#define inb	inb_far
#define outb	outb_far
#define inw	inw_far
#define outw	outw_far
#define setb	setb_far
#define clearb	clearb_far
#define testb	testb_far

#endif /*__OPTIMIZE__*/
#endif /* _IO_H_ */
