/*
 * Simulator for PIC17C4x processors.
 * No DC status bit.
 * No OV status bit.
 * No sleep and watchdog yet.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <termios.h>

#define TXTSIZE         8196    /* command segment size */
#define DATSIZE         256     /* data segment size */
#define STACKSIZE       16      /* call stack size */

/*
 * Processor configuration.
 */
#define CFGADDR         0xfe00

#define O_FOSC0         0x0001
#define O_FOSC1         0x0002
#define O_WDTPS0        0x0004
#define O_WDTPS1        0x0008
#define O_PM0           0x0010
#define O_PM1           0x0040
#define O_PM2           0x8000

int debug;
int option;
char *infile;
FILE *input;
unsigned long clock;
int trace_start, trace_end = -1;
struct termios stdtio;
int Wflag, Rflag;

unsigned char stimulus_a = 0xff;
unsigned char stimulus_b = 0xff;
unsigned char stimulus_c = 0xff;
unsigned char stimulus_d = 0xff;
unsigned char stimulus_e = 0x07;

int prescaler, plimit;
int txready;

unsigned short stack [STACKSIZE], *SP;
unsigned short PC, LATCH;

unsigned short text [TXTSIZE];
unsigned char tbusy [TXTSIZE];

unsigned char data [DATSIZE];
unsigned char data1 [DATSIZE-0x20];
unsigned char bank1 [8];
unsigned char bank2 [8];
unsigned char bank3 [8];

#define INDF0   data[0]
#define FSR0    data[1]
#define PCL     data[2]
#define PCLATH  data[3]
#define ALUSTA  data[4]
#define T0STA   data[5]
#define CPUSTA  data[6]
#define INTSTA  data[7]
#define INDF1   data[8]
#define FSR1    data[9]
#define WREG    data[0xa]
#define TMR0L   data[0xb]
#define TMR0H   data[0xc]
#define TBLPTRL data[0xd]
#define TBLPTRH data[0xe]
#define BSR     data[0xf]

#define PORTA   data[0x10]
#define DDRB    data[0x11]
#define PORTB   data[0x12]
#define RCSTA   data[0x13]
#define RCREG   data[0x14]
#define TXSTA   data[0x15]
#define TXREG   data[0x16]
#define SPBRG   data[0x17]
#define PRODL   data[0x18]
#define PRODH   data[0x19]

#define DDRC    bank1[0]
#define PORTC   bank1[1]
#define DDRD    bank1[2]
#define PORTD   bank1[3]
#define DDRE    bank1[4]
#define PORTE   bank1[5]
#define PIR     bank1[6]
#define PIE     bank1[7]

#define TMR1    bank2[0]
#define TMR2    bank2[1]
#define TMR3L   bank2[2]
#define TMR3H   bank2[3]
#define PR1     bank2[4]
#define PR2     bank2[5]
#define PR3L    bank2[6]
#define PR3H    bank2[7]

#define PW1DCL  bank3[0]
#define PW2DCL  bank3[1]
#define PW1DCH  bank3[2]
#define PW2DCH  bank3[3]
#define CA2L    bank3[4]
#define CA2H    bank3[5]
#define TCON1   bank3[6]
#define TCON2   bank3[7]

#define TMR0    (*(unsigned short*)&TMR0L)
#define TBLPTR  (*(unsigned short*)&TBLPTRL)
#define PROD    (*(unsigned short*)&PRODL)
#define TMR3    (*(unsigned short*)&TMR3L)
#define PR3     (*(unsigned short*)&PR3L)
#define CA2     (*(unsigned short*)&CA2L)

#define C       0x01    /* ALUSTA */
#define DC      0x02
#define Z       0x04
#define OV      0x08
#define FS0     0x10
#define FS1     0x20
#define FS2     0x40
#define FS3     0x80

#define STKAV   0x20    /* CPUSTA */
#define GLINTD  0x10
#define TO      0x08
#define PD      0x04

#define PS0     0x02    /* T0STA */
#define PS1     0x04
#define PS2     0x08
#define PS3     0x10
#define T0CS    0x20
#define T0SE    0x40
#define INTEDG  0x80

#define TX9D    0x01    /* TXSTA */
#define TRMT    0x02
#define SYNC    0x10
#define TXEN    0x20
#define TX9     0x40
#define CSRC    0x80

#define RX9D    0x01    /* RCSTA */
#define OERR    0x02
#define FERR    0x04
#define CREN    0x10
#define SREN    0x20
#define RX9     0x40
#define SPEN    0x80

#define INTE    0x01    /* INTSTA */
#define T0IE    0x02
#define T0CKIE  0x04
#define PEIE    0x08
#define INTF    0x10
#define T0IF    0x20
#define T0CKIF  0x40
#define PEIF    0x80

#define RCIE    0x01    /* PIE */
#define TXIE    0x02
#define CA1IE   0x04
#define CA2IE   0x08
#define TMR1IE  0x10
#define TMR2IE  0x20
#define TMR3IE  0x40
#define RBIE    0x80

#define RCIF    0x01    /* PIR */
#define TXIF    0x02
#define CA1IF   0x04
#define CA2IF   0x08
#define TMR1IF  0x10
#define TMR2IF  0x20
#define TMR3IF  0x40
#define RBIF    0x80

#define TMR1ON  0x01    /* TCON2 */
#define TMR2ON  0x02
#define TMR3ON  0x04
#define CA1     0x08
#define PWM1ON  0x10
#define PWM2ON  0x20
#define CA1OVF  0x40
#define CA2OVF  0x80

void printcmd (int cmd);

void quit ()
{
	fcntl (0, F_SETFL, 0);
	tcsetattr (0, TCSADRAIN, &stdtio);
	exit (-1);
}

void uerror (char *s, ...)
{
	va_list ap;

	va_start (ap, s);
	fprintf (stderr, "0x%x: ", PC);
	vfprintf (stderr, s, ap);
	va_end (ap);
	fprintf (stderr, "\r\n");
	quit ();
}

void set_trace (char *str)
{
	char *e;

	trace_start = strtol (str, &e, 0);
	if (! e || *e != ':')
		trace_end = trace_start;
	else
		trace_end = strtol (e+1, 0, 0);
}

void trace (char *str)
{
	if (! debug)
		return;
	printf ("(%ld) %04x: %s\r\n", clock, PC, str);
}

int hexdig (char *p)
{
	unsigned char val = 0;

	if      (*p >= '0' && *p <= '9') val = (*p - '0') << 4;
	else if (*p >= 'A' && *p <= 'F') val = (*p - 'A' + 10) << 4;
	else uerror ("bad hex digit");
	++p;
	if      (*p >= '0' && *p <= '9') val += *p - '0';
	else if (*p >= 'A' && *p <= 'F') val += *p - 'A' + 10;
	else uerror ("bad hex digit");
	return val;
}

int gethex (int *len, long *addr, unsigned char *line)
{
	char buf [80];
	unsigned char sum;
	int i, eof;
	static int high;
again:
	if (! fgets (buf, sizeof (buf), input))
		uerror ("unexpected EOF");
	if (buf[0] != ':')
		goto again;
	*len = hexdig (buf+1);
	if (strlen (buf) < *len * 2 + 11)
		uerror ("too short hex line");
	*addr = (long) high << 16 | hexdig (buf+3) << 8 | hexdig (buf+5);
	eof = hexdig (buf+7);

	sum = 0;
	for (i=0; i<*len; ++i) {
		line [i] = hexdig (buf+9 + i + i);
		sum += line [i];
	}

	sum += eof + *len + (*addr & 0xff) + (*addr >> 8 & 0xff);
	if (sum != (unsigned char) - hexdig (buf+9 + *len + *len))
		uerror ("bad hex checksum");

	if (eof == 4) {
		if (*len != 2)
			uerror ("invalid hex linear address record length");
		high = line[0] << 8 | line[1];
		goto again;
	}
	if (eof == 1)
		return 0;
	if (eof)
		uerror ("unknown hex record");
	return 1;
}

void readimage ()
{
	int len, i;
	long addr;
	unsigned char line [16];

	while (gethex (&len, &addr, line)) {
		if (addr == CFGADDR*2) {
			option = line [0] | line [1] << 8;
			continue;
		}
		if (addr & 1)
			uerror ("odd address");
		if (len & 1)
			uerror ("odd length");
		addr /= 2;
		if (addr > TXTSIZE)
			uerror ("invalid hex address");
		for (i=0; i<len; i+=2) {
			text [addr] = line [i] | line [i+1] << 8;
			tbusy [addr] = 1;
			++addr;
		}
	}
}

int pop ()
{
	--SP;
	if (SP < stack)
		SP = stack + STACKSIZE - 1;
	return *SP;
}

void push (int val)
{
	*SP++ = val;
	if (SP >= stack + STACKSIZE)
		SP = stack;
}

void status (int v, int flags)
{
	ALUSTA &= ~flags;
	if ((flags & C) && (v > 0xff))
		ALUSTA |= C;
	if ((flags & Z) && ! (v & 0xff))
		ALUSTA |= Z;
}

/*
 * Clock tick.
 * Process time-related events, set needed interrupt flags.
 */
void cycle ()
{
	static int read_delay;

	++clock;

	if (--read_delay <= 0) {
		read_delay = 1000 + rand() % 1000;
		if (read (0, &RCREG, 1) == 1) {
			if (RCREG == ('t' & 037)) {
				/* ^T - switch the debug mode */
				++debug;
				debug &= 3;
			} else {
				/* Receive interrupt */
				PIR |= RCIF;
			}
		}
	}
	if (txready && clock > txready) {
		/* Transmit buffer empty interrupt. */
		TXSTA |= TRMT;
		PIR |= TXIF;
		txready = 0;
	}
	if (T0STA & T0CS) {
		/* Advance timer 0. */
		++prescaler;
		if (prescaler >= plimit) {
			prescaler = 0;
			++TMR0;
			if (TMR0 == 0)
				INTSTA |= T0IF;
		}
	}
	if (TCON2 & TMR3ON) {
		/* Advance timer 3. */
		++TMR3;
		if (TMR3 == PR3) {
			TMR3 = 0;
			PIR |= TMR3IF;
		}
	}
}

unsigned char load0 (int addr)
{
	unsigned char val;

	addr = (unsigned char) addr;

	switch (addr) {
	case 0: addr = FSR0; break;
	case 8: addr = FSR1; break;
	}
	switch (addr) {
	default:
		switch (BSR >> 4) {
		case 0:
			val = data [addr];
			goto done;
		case 1:
			if (addr < 0x20)
				uerror ("invalid GPR bank 1 read");
			val = data1 [addr - 0x20];
			goto done;
		default:
			uerror ("invalid GPR bank %d read", BSR >> 4);
		}
	case 0x10: case 0x11: case 0x12: case 0x13:
	case 0x14: case 0x15: case 0x16: case 0x17:
		switch (BSR & 0xf) {
		case 0:
			if (addr == 0x14)               /* RCREG */
				PIR &= ~RCIF;
			val = data [addr];
			goto done;
		case 1:  val = bank1 [addr-0x10]; goto done;
		case 2:  val = bank2 [addr-0x10]; goto done;
		case 3:  val = bank3 [addr-0x10]; goto done;
		default: uerror ("invalid SFR bank %d read", BSR & 0xf);
		}
	case 0:    case 1:
	case 4:    case 5:    case 6:    case 7:
	case 8:    case 9:    case 0xa:  case 0xb:
	case 0xc:  case 0xd:  case 0xe:  case 0xf:
	case 0x18: case 0x19: case 0x1a: case 0x1b:
	case 0x1c: case 0x1d: case 0x1e: case 0x1f:
		val = data [addr]; goto done;
	case 2:
		val = PC; goto done;
	case 3:
		val = PC >> 8; goto done;
	}
done:
	if (Rflag && addr == Rflag)
		printf ("(%ld) %04x: read %02x/%d value %02x\r\n",
			clock, PC, addr, addr < 0x18 ? BSR & 0xf : BSR>>4, val);
	return val;
}

unsigned char load (int addr)
{
	unsigned char val = load0 (addr);

	switch ((unsigned char) addr) {
	case 0:
		if (! (ALUSTA & FS1)) {
			if (ALUSTA & FS0) ++FSR0;
			else              --FSR0;
		}
		return val;
	case 8:
		if (! (ALUSTA & FS3)) {
			if (ALUSTA & FS2) ++FSR1;
			else              --FSR1;
		}
		break;
	}
	return val;
}

void store (int addr, int val)
{
	addr = (unsigned char) addr;
	val = (unsigned char) val;

	switch (addr) {
	case 0: addr = FSR0; break;
	case 8: addr = FSR1; break;
	}
	if (Wflag && addr == Wflag)
		printf ("(%ld) %04x: write %02x/%d value %02x\r\n",
			clock, PC, addr, addr < 0x18 ? BSR & 0xf : BSR>>4, val);

	switch (addr) {
	case 2:                         /* PCL */
		PC = PCLATH << 8 | val;
		return;
	case 5:                         /* T0STA */
		if (val & PS3)
			plimit = 256;
		else
			switch (val & (PS2 | PS1 | PS0)) {
			case 0:           plimit = 1; break;
			case PS0:         plimit = 2; break;
			case PS1:         plimit = 4; break;
			case PS1|PS0:     plimit = 8; break;
			case PS2:         plimit = 16; break;
			case PS2|PS0:     plimit = 32; break;
			case PS2|PS1:     plimit = 64; break;
			case PS2|PS1|PS0: plimit = 128; break;
			}
		data [addr] = val;
		return;
	case 0xb: case 0xc:             /* TMR0 */
		prescaler = 0;
		data [addr] = val;
		return;
	case 0:    case 1:    case 3:
	case 4:    case 6:    case 7:
	case 8:    case 9:    case 0xa:
	case 0xd:  case 0xe:  case 0xf:
	case 0x18: case 0x19: case 0x1a: case 0x1b:
	case 0x1c: case 0x1d: case 0x1e: case 0x1f:
		data [addr] = val;
		return;
	default:
		switch (BSR >> 4) {
		case 0:
			data [addr] = val;
			return;
		case 1:
			if (addr < 0x20)
				uerror ("invalid GPR bank 1 write");
			data1 [addr-0x20] = val;
			return;
		default:
			uerror ("invalid GPR bank %d write", BSR >> 4);
		}
	case 0x10: case 0x11: case 0x12: case 0x13:
	case 0x14: case 0x15: case 0x16: case 0x17:
		break;
	}

	switch (BSR & 0xf) {
	default:
		uerror ("invalid SFR bank %d write", BSR & 0xf);
		break;

	case 0:
		switch (addr) {
		case 0x10:              /* PORTA */
			data [addr] = (val & 0x80) | (stimulus_a & 0x3f);
			break;
		case 0x12:              /* PORTB */
			data [addr] = (val & ~DDRB) | (stimulus_b & DDRB);
			break;
		case 0x15:              /* TXSTA */
			data [addr] &= TRMT;
			data [addr] |= val & ~TRMT;
			if (TXSTA & TRMT)
				/* Transmit buffer empty interrupt. */
				PIR |= TXIF;
			break;
		case 0x16:              /* TXREG */
			if ((RCSTA & SPEN) && (TXSTA & TXEN) &&
			    (TXSTA & TRMT)) {
				int bits = 8;

				putchar (val);
				fflush (stdout);
				TXSTA &= ~TRMT;
				PIR &= ~TXIF;
				if (TXSTA & TX9)
					++bits;
				txready = clock;
				if (TXSTA & SYNC)
					txready += bits * (SPBRG+1);
				else
					txready += (bits+2) * 16 * (SPBRG+1);

			}
			data [addr] = val;
			break;
		default:
			data [addr] = val;
			break;
		}
		break;

	case 1:
		switch (addr) {
		case 0x11:              /* PORTC */
			bank1 [addr-0x10] = (val & ~DDRC) | (stimulus_c & DDRC);
			break;
		case 0x12:              /* PORTD */
			bank1 [addr-0x10] = (val & ~DDRD) | (stimulus_d & DDRD);
			break;
		case 0x15:              /* PORTE */
			bank1 [addr-0x10] = (val & ~DDRE) | (stimulus_e & DDRE);
			break;
		case 0x16:              /* PIR */
			PIR &= TXIF | RCIF;
			PIR |= val & ~(TXIF | RCIF);
			break;
		default:
			bank1 [addr-0x10] = val;
			break;
		}
		break;

	case 2:
		bank2 [addr-0x10] = val;
		break;

	case 3:
		bank3 [addr-0x10] = val;
		break;
	}
}

void dump ()
{
	printf ("A0=%02x A1=%02x A2=%02x A3=%02x A4=%02x\r\n",
		WREG, FSR1, TBLPTRL, TBLPTRH, data[0x20]);
	printf ("A5=%02x A6=%02x A7=%02x A8=%02x A9=%02x\r\n",
		data[0x21], data[0x22], data[0x23], data[0x24], data[0x25]);

}

void run ()
{
	unsigned short cmd;
	int v;

	memset (data, 0xff, sizeof (data));
	memset (data1, 0xff, sizeof (data1));
	SP     = stack;
	PC     = 0;
	INDF0  = 0;
	ALUSTA = 0xff;
	T0STA  = 0;
	CPUSTA = 0x3c;
	INTSTA = 0;
	INDF1  = 0;
	TBLPTR = 0;
	BSR    = 0;
	DDRB   = 0xff;
	TXSTA  = 0x02;
	DDRC   = 0xff;
	DDRD   = 0xff;
	DDRE   = 0x07;
	PIR    = 0x02;
	PIE    = 0;
	TCON1  = 0;
	TCON2  = 0;
	plimit = 1;
	for (;;) {
		/* Set peripheral interrupt bit. */
		if (PIR & PIE) INTSTA |= PEIF;
		else           INTSTA &= ~PEIF;

		/* Process interrupts. */
		if (! (CPUSTA & GLINTD)) {
			if ((INTSTA & INTF) && (INTSTA & INTE)) {
				/* External interrupt on INT pin. */
				trace ("external INT interrupt");
				CPUSTA |= GLINTD;
				INTSTA &= ~INTF;
				cycle ();
				push (PC);
				PC = 0x08;
			} else if ((INTSTA & T0IF) && (INTSTA & T0IE)) {
				/* Timer 0 interrupt. */
				trace ("timer 0 interrupt");
				CPUSTA |= GLINTD;
				INTSTA &= ~T0IF;
				cycle ();
				push (PC);
				PC = 0x10;
			} else if ((INTSTA & T0CKIF) && (INTSTA & T0CKIE)) {
				/* External interrupt on T0CKI pin. */
				trace ("external T0CKI interrupt");
				CPUSTA |= GLINTD;
				INTSTA &= ~T0CKIF;
				cycle ();
				push (PC);
				PC = 0x18;
			} else if ((INTSTA & PEIF) && (INTSTA & PEIE)) {
				/* Peripheral interrupt. */
				trace ("peripheral interrupt");
				CPUSTA |= GLINTD;
				cycle ();
				push (PC);
				PC = 0x20;
			}
		}
		if (PC > TXTSIZE)
			uerror ("ran out of program memory");
		if (debug > 1 ||
		    (PC >= trace_start && PC <= trace_end)) {
			printf ("(%ld) %04x: %04x <", clock, PC, text [PC]);
			printcmd (text [PC]);
			printf (">\t= %02x -- %02x <", load0 (text [PC]), WREG);
			if (ALUSTA & Z) printf ("Z,");
			if (ALUSTA & C) printf ("C,");
			if (ALUSTA & OV) printf ("OV,");
			if (ALUSTA & DC) printf ("DC,");
			if (ALUSTA & (FS0|FS1|FS2|FS3)) printf ("FS%d", ALUSTA>>4);
			if (! ALUSTA) printf ("0");
			printf ("> %02x=", INTSTA);
			if (INTSTA & INTF) printf ((INTSTA & INTE) ? "INT," : "int,");
			if (INTSTA & T0IF) printf ((INTSTA & T0IE) ? "T0I," : "t0i,");
			if (INTSTA & T0CKIF) printf ((INTSTA & T0CKIE) ? "T0CKI," : "t0cki,");
			if (INTSTA & PEIF) printf ((INTSTA & PEIE) ? "PEI" : "pei");
			printf (" %02x/%02x=", PIR, PIE);
			if (PIR & RCIF) printf ((PIE & RCIE) ? "RCI," : "rci,");
			if (PIR & TXIF) printf ((PIE & TXIE) ? "TXI," : "txi,");
			if (PIR & CA1IF) printf ((PIE & CA1IE) ? "CA1I," : "ca1i,");
			if (PIR & CA2IF) printf ((PIE & CA2IE) ? "CA2I" : "ca2i");
			if (PIR & TMR1IF) printf ((PIE & TMR1IE) ? "TMR1I," : "tmr1i,");
			if (PIR & TMR2IF) printf ((PIE & TMR2IE) ? "TMR2I," : "tmr2i,");
			if (PIR & TMR3IF) printf ((PIE & TMR3IE) ? "TMR3I," : "tmr3i,");
			if (PIR & RBIF) printf ((PIE & RBIE) ? "RBI" : "rbi");
			printf ("\r\n");
		}
		cmd = text [PC++];
		cycle ();
		switch (cmd >> 8) {
		default:
badcmd:                 uerror ("invalid command: 0x%x", cmd);
			continue;
		case 0x00:
			switch (cmd & 0xff) {
			default:
				goto badcmd;
			case 0x00:              /* nop */
				continue;
			case 0x02:              /* ret */
				PC = pop ();
				cycle ();
				continue;
			case 0x05:              /* reti */
				--SP;
				if (SP < stack)
					SP = stack + STACKSIZE - 1;
				PC = *SP;
				CPUSTA &= ~GLINTD;
				cycle ();
				continue;
			case 0x06:              /* dump */
				dump ();
				continue;
			case 0x07:              /* halt */
				return;
			case 0x03:              /* sleep */
				/* Not imlemented yet. */
				continue;
			case 0x04:              /* awake */
				/* Not imlemented yet. */
/* putchar ('#'); fflush (stdout); */
				continue;
			}
		case 0x01:                      /* atx */
			store (cmd, WREG);
			continue;
		case 0x02:                      /* a-bx */
			v = load (cmd) - WREG;
			if (! (ALUSTA & C))
				--v;
			WREG = v;
			status (v, OV | Z | DC);
			if (v >= 0) ALUSTA |= C;
			else        ALUSTA &= ~C;
			continue;
		case 0x03:                      /* x-ba */
			v = load0 (cmd) - WREG;
			if (! (ALUSTA & C))
				--v;
			store (cmd, v);
			status (v, OV | Z | DC);
			if (v >= 0) ALUSTA |= C;
			else        ALUSTA &= ~C;
			continue;
		case 0x04:                      /* a-x */
			v = load (cmd) - WREG;
			WREG = v;
			status (v, OV | Z | DC);
			if (v >= 0) ALUSTA |= C;
			else        ALUSTA &= ~C;
			continue;
		case 0x05:                      /* x-a */
			v = load0 (cmd) - WREG;
			store (cmd, v);
			status (v, OV | Z | DC);
			if (v >= 0) ALUSTA |= C;
			else        ALUSTA &= ~C;
			continue;
		case 0x06:                      /* x--a */
			v = load (cmd) - 1;
			WREG = v;
			status (v, OV | Z | DC);
			if (v >= 0) ALUSTA |= C;
			else        ALUSTA &= ~C;
			continue;
		case 0x07:                      /* x-- */
			v = load0 (cmd) - 1;
			store (cmd, v);
			status (v, OV | Z | DC);
			if (v >= 0) ALUSTA |= C;
			else        ALUSTA &= ~C;
			continue;
		case 0x08:                      /* a|x */
			WREG |= load (cmd);
			status (WREG, Z);
			continue;
		case 0x09:                      /* x|a */
			v = load0 (cmd) | WREG;
			store (cmd, v);
			status (v, Z);
			continue;
		case 0x0a:                      /* a&x */
			WREG &= load (cmd);
			status (WREG, Z);
			continue;
		case 0x0b:                      /* x&a */
			v = load0 (cmd) & WREG;
			store (cmd, v);
			status (v, Z);
			continue;
		case 0x0c:                      /* a^x */
			WREG ^= load (cmd);
			status (WREG, Z);
			continue;
		case 0x0d:                      /* x^a */
			v = load0 (cmd) ^ WREG;
			store (cmd, v);
			status (v, Z);
			continue;
		case 0x0e:                      /* a+x */
			v = load (cmd) + WREG;
			WREG = v;
			status (v, OV | Z | DC | C);
			continue;
		case 0x0f:                      /* x+a */
			v = load0 (cmd) + WREG;
			store (cmd, v);
			status (v, OV | Z | DC | C);
			continue;
		case 0x10:                      /* a+cx */
			v = load (cmd) + WREG;
			if (ALUSTA & C)
				++v;
			WREG = v;
			status (v, OV | Z | DC | C);
			continue;
		case 0x11:                      /* x+ca */
			v = load0 (cmd) + WREG;
			if (ALUSTA & C)
				++v;
			store (cmd, v);
			status (v, OV | Z | DC | C);
			continue;
		case 0x12:                      /* xca */
			WREG = load (cmd) ^ 0xff;
			status (WREG, Z);
			continue;
		case 0x13:                      /* xc */
			v = load0 (cmd) ^ 0xff;
			store (cmd, v);
			status (v, Z);
			continue;
		case 0x14:                      /* x++a */
			v = load (cmd) + 1;
			WREG = v;
			status (v, OV | Z | DC | C);
			continue;
		case 0x15:                      /* x++ */
			v = load0 (cmd) + 1;
			store (cmd, v);
			status (v, OV | Z | DC | C);
			continue;
		case 0x16:                      /* x--a? */
			v = (load (cmd) - 1) & 0xff;
			WREG = v;
			if (! v) ++PC, cycle ();
			continue;
		case 0x17:                      /* x--? */
			v = (load0 (cmd) - 1) & 0xff;
			store (cmd, v);
			if (! v) ++PC, cycle ();
			continue;
		case 0x18:                      /* xc>>a */
			v = load (cmd);
			if (ALUSTA & C)
				v |= 0x100;
			if (v & 1) ALUSTA |= C;
			else       ALUSTA &= ~C;
			WREG = v >> 1;
			continue;
		case 0x19:                      /* xc>>x */
			v = load0 (cmd);
			if (ALUSTA & C)
				v |= 0x100;
			if (v & 1) ALUSTA |= C;
			else       ALUSTA &= ~C;
			store (cmd, v >> 1);
			continue;
		case 0x1a:                      /* xc<<a */
			v = load (cmd) << 1;
			if (ALUSTA & C)
				v |= 1;
			if (v & 0x100) ALUSTA |= C;
			else           ALUSTA &= ~C;
			WREG = v;
			continue;
		case 0x1b:                      /* xc<<x */
			v = load0 (cmd) << 1;
			if (ALUSTA & C)
				v |= 1;
			if (v & 0x100) ALUSTA |= C;
			else           ALUSTA &= ~C;
			store (cmd, v);
			continue;
		case 0x1c:                      /* xwa */
			v = load (cmd);
			WREG = v << 4 | v >> 4;
			continue;
		case 0x1d:                      /* xw */
			v = load0 (cmd);
			store (cmd, v << 4 | v >> 4);
			continue;
		case 0x1e:                      /* x++a? */
			v = (load (cmd) + 1) & 0xff;
			WREG = v;
			if (! v) ++PC, cycle ();
			continue;
		case 0x1f:                      /* x++? */
			v = (load0 (cmd) + 1) & 0xff;
			store (cmd, v);
			if (! v) ++PC, cycle ();
			continue;
		case 0x20:                      /* x>>a */
			v = load (cmd) >> 1;
			WREG = v;
			continue;
		case 0x21:                      /* x>>x */
			v = load0 (cmd) >> 1;
			store (cmd, v);
			continue;
		case 0x22:                      /* x<<a */
			v = load (cmd) << 1;
			WREG = v;
			continue;
		case 0x23:                      /* x<<x */
			v = load0 (cmd) << 1;
			store (cmd, v);
			continue;
		case 0x24:                      /* x++az? */
			v = (load (cmd) + 1) & 0xff;
			WREG = v;
			if (v) ++PC, cycle ();
			continue;
		case 0x25:                      /* x++z? */
			v = (load0 (cmd) + 1) & 0xff;
			store (cmd, v);
			if (v) ++PC, cycle ();
			continue;
		case 0x26:                      /* x--az? */
			v = (load (cmd) - 1) & 0xff;
			WREG = v;
			if (v) ++PC, cycle ();
			continue;
		case 0x27:                      /* x--z? */
			v = (load0 (cmd) - 1) & 0xff;
			store (cmd, v);
			if (v) ++PC, cycle ();
			continue;
		case 0x28:                      /* xza */
			WREG = 0;
			store (cmd, 0);
			continue;
		case 0x29:                      /* xz */
			store (cmd, 0);
			continue;
		case 0x2a:                      /* xsa */
			WREG = 0xff;
			store (cmd, 0xff);
			continue;
		case 0x2b:                      /* xs */
			store (cmd, 0xff);
			continue;
		case 0x2c:                      /* anax */
			v = (WREG ^ 0xff) + 1;
			WREG = v;
			store (cmd, v);
			status (v, OV | Z | DC | C);
			continue;
		case 0x2d:                      /* anx */
			v = (WREG ^ 0xff) + 1;
			store (cmd, v);
			status (v, OV | Z | DC | C);
			continue;
		case 0x2e:                      /* adax */
			v = WREG;
			if ((WREG & 0xf) > 9 || (ALUSTA & DC))
				v += 6;
			if ((WREG & 0xf0) > 0x90 || (ALUSTA & C))
				v += 0x60;
			WREG = v;
			store (cmd, v);
			status (v, C);
			continue;
		case 0x2f:                      /* adx */
			v = WREG;
			if ((WREG & 0xf) > 9 || (ALUSTA & DC))
				v += 6;
			if ((WREG & 0xf0) > 0x90 || (ALUSTA & C))
				v += 0x60;
			store (cmd, v);
			status (v, C);
			continue;
		case 0x30:                      /* x<=a? */
			if (WREG > load (cmd))
				++PC, cycle ();
			continue;
		case 0x31:                      /* x!=a? */
			if (WREG == load (cmd))
				++PC, cycle ();
			continue;
		case 0x32:                      /* x>=a? */
			if (WREG < load (cmd))
				++PC, cycle ();
			continue;
		case 0x33:                      /* x? */
			if (! load (cmd))
				++PC, cycle ();
			continue;
		case 0x34:                      /* a*x */
			v = load (cmd);
			PROD = WREG * v;
/* printf ("%04x: %d * %d => %d\r\n", PC, WREG, v, PROD); */
			continue;
		case 0xa0: case 0xa1:           /* llx */
			store (cmd, LATCH);
			continue;
		case 0xa2: case 0xa3:           /* lhx */
			store (cmd, LATCH >> 8);
			continue;
		case 0xa4: case 0xa5:           /* xll */
			LATCH = (LATCH & 0xff00) | load (cmd);
			continue;
		case 0xa6: case 0xa7:           /* xhl */
			LATCH = (LATCH & 0xff) | load (cmd) << 8;
			continue;
		case 0xa8:                      /* plx */
			store (cmd, LATCH);
/* printf ("(%ld) %04x: plx %04x => %04x\r\n", clock, PC, TBLPTR, text [TBLPTR]); */
			if (TBLPTR < TXTSIZE) LATCH = text [TBLPTR];
			else                  LATCH = ~0;
			continue;
		case 0xa9:                      /* pl++x */
			store (cmd, LATCH);
			if (TBLPTR < TXTSIZE) LATCH = text [TBLPTR];
			else                  LATCH = ~0;
			++TBLPTR;
			continue;
		case 0xaa:                      /* phx */
			store (cmd, LATCH >> 8);
			if (TBLPTR < TXTSIZE) LATCH = text [TBLPTR];
			else                  LATCH = ~0;
			continue;
		case 0xab:                      /* ph++x */
			store (cmd, LATCH >> 8);
			if (TBLPTR < TXTSIZE) LATCH = text [TBLPTR];
			else                  LATCH = ~0;
			++TBLPTR;
			continue;
		case 0xae:                      /* xhp */
			LATCH = (LATCH & 0xff) | load (cmd) << 8;
			if (TBLPTR < TXTSIZE) text [TBLPTR] = LATCH;
			continue;
		case 0xaf:                      /* xhp++ */
			LATCH = (LATCH & 0xff) | load (cmd) << 8;
			if (TBLPTR < TXTSIZE) text [TBLPTR] = LATCH;
			++TBLPTR;
			continue;
		case 0xb0:                      /* cta */
			WREG = cmd & 0xff;
			continue;
		case 0xb1:                      /* a+c */
			v = (cmd & 0xff) + WREG;
			WREG = v;
			status (v, OV | Z | DC | C);
			continue;
		case 0xb2:                      /* c-a */
			v = (cmd & 0xff) - WREG;
			WREG = v;
			status (v, OV | Z | DC);
			if (v >= 0) ALUSTA |= C;
			else        ALUSTA &= ~C;
			continue;
		case 0xb3:                      /* a|c */
			WREG = (cmd & 0xff) | WREG;
			status (WREG, Z);
			continue;
		case 0xb4:                      /* a^c */
			WREG = (cmd & 0xff) ^ WREG;
			status (WREG, Z);
			continue;
		case 0xb5:                      /* a&c */
			WREG = (cmd & 0xff) & WREG;
			status (WREG, Z);
			continue;
		case 0xb6:                      /* retc */
			WREG = (cmd & 0xff);
			PC = pop ();
			cycle ();
			continue;
		case 0xb7:                      /* lcall */
			push (PC);
			PC = PCLATH << 8 | (cmd & 0xff);
			cycle ();
			continue;
		case 0xb8:                      /* reg */
			BSR = (BSR & 0xf0) | (cmd & 0x0f);
			continue;
		case 0xba: case 0xbb:           /* dat */
			BSR = (BSR & 0x0f) | (cmd & 0xf0);
			continue;
		case 0xbc:                      /* a*c */
			PROD = WREG * (cmd & 0xff);
			continue;
		case 0xc0: case 0xc1: case 0xc2: case 0xc3:
		case 0xc4: case 0xc5: case 0xc6: case 0xc7:
		case 0xc8: case 0xc9: case 0xca: case 0xcb:
		case 0xcc: case 0xcd: case 0xce: case 0xcf:
		case 0xd0: case 0xd1: case 0xd2: case 0xd3:
		case 0xd4: case 0xd5: case 0xd6: case 0xd7:
		case 0xd8: case 0xd9: case 0xda: case 0xdb:
		case 0xdc: case 0xdd: case 0xde: case 0xdf:     /* goto */
			PC = cmd & 0x1fff;
			cycle ();
			continue;
		case 0xe0: case 0xe1: case 0xe2: case 0xe3:
		case 0xe4: case 0xe5: case 0xe6: case 0xe7:
		case 0xe8: case 0xe9: case 0xea: case 0xeb:
		case 0xec: case 0xed: case 0xee: case 0xef:
		case 0xf0: case 0xf1: case 0xf2: case 0xf3:
		case 0xf4: case 0xf5: case 0xf6: case 0xf7:
		case 0xf8: case 0xf9: case 0xfa: case 0xfb:
		case 0xfc: case 0xfd: case 0xfe: case 0xff:     /* call */
			push (PC);
			PC = cmd & 0x1fff;
			cycle ();
			continue;
		case 0x60: case 0x61: case 0x62: case 0x63:
		case 0x64: case 0x65: case 0x66: case 0x67:
		case 0x68: case 0x69: case 0x6a: case 0x6b:
		case 0x6c: case 0x6d: case 0x6e: case 0x6f:
		case 0x70: case 0x71: case 0x72: case 0x73:
		case 0x74: case 0x75: case 0x76: case 0x77:
		case 0x78: case 0x79: case 0x7a: case 0x7b:
		case 0x7c: case 0x7d: case 0x7e: case 0x7f:     /* xtr */
			store (cmd >> 8 & 0x1f, load (cmd));
			continue;
		case 0x40: case 0x41: case 0x42: case 0x43:
		case 0x44: case 0x45: case 0x46: case 0x47:
		case 0x48: case 0x49: case 0x4a: case 0x4b:
		case 0x4c: case 0x4d: case 0x4e: case 0x4f:
		case 0x50: case 0x51: case 0x52: case 0x53:
		case 0x54: case 0x55: case 0x56: case 0x57:
		case 0x58: case 0x59: case 0x5a: case 0x5b:
		case 0x5c: case 0x5d: case 0x5e: case 0x5f:     /* rtx */
			v = load (cmd >> 8 & 0x1f);
			store (cmd, v);
			status (v, Z);
			continue;
		case 0x38: case 0x39: case 0x3a: case 0x3b:
		case 0x3c: case 0x3d: case 0x3e: case 0x3f:     /* bt */
			v = load0 (cmd);
			v ^= 1 << (cmd >> 8 & 7);
			store (cmd, v);
			continue;
		case 0x80: case 0x81: case 0x82: case 0x83:
		case 0x84: case 0x85: case 0x86: case 0x87:     /* bs */
			v = load0 (cmd);
			v |= 1 << (cmd >> 8 & 7);
			store (cmd, v);
			continue;
		case 0x88: case 0x89: case 0x8a: case 0x8b:
		case 0x8c: case 0x8d: case 0x8e: case 0x8f:     /* bz */
			v = load0 (cmd);
			v &= ~(1 << (cmd >> 8 & 7));
			store (cmd, v);
			continue;
		case 0x90: case 0x91: case 0x92: case 0x93:
		case 0x94: case 0x95: case 0x96: case 0x97:     /* bz? */
			v = 1 << (cmd >> 8 & 7);
			if (load (cmd) & v)
				++PC, cycle ();
			continue;
		case 0x98: case 0x99: case 0x9a: case 0x9b:
		case 0x9c: case 0x9d: case 0x9e: case 0x9f:     /* bs? */
			v = 1 << (cmd >> 8 & 7);
			if (! (load (cmd) & v))
				++PC, cycle ();
			continue;
		}
	}
}

void printcmd (int cmd)
{
	char *m;
	int arg;

	switch (cmd) {
	case 0x0000: printf ("nop");   return;
	case 0x0002: printf ("ret");   return;
	case 0x0003: printf ("sleep"); return;
	case 0x0004: printf ("awake"); return;
	case 0x0005: printf ("reti");  return;
	case 0x170a: printf ("a--?");  return;
	case 0x1d0a: printf ("aw");    return;
	case 0x1f0a: printf ("a++?");  return;
	case 0x250a: printf ("a++z?"); return;
	case 0x270a: printf ("a--z?"); return;
	case 0x290a: printf ("az");    return;
	case 0x2b0a: printf ("as");    return;
	case 0x330a: printf ("a?");    return;
	case 0x9a04: printf ("z?");    return;
	case 0x9204: printf ("nz?");   return;
	case 0x9804: printf ("c?");    return;
	case 0x9004: printf ("b?");    return;
	}

	switch (cmd >> 8) {
	case 0x01: m = "atx";    goto shortop;
	case 0x02: m = "a-bx";   goto shortop;
	case 0x03: m = "x-ba";   goto shortop;
	case 0x04: m = "a-x";    goto shortop;
	case 0x05: m = "x-a";    goto shortop;
	case 0x06: m = "x--a";   goto shortop;
	case 0x07: m = "x--";    goto shortop;
	case 0x08: m = "a|x";    goto shortop;
	case 0x09: m = "x|a";    goto shortop;
	case 0x0a: m = "a&x";    goto shortop;
	case 0x0b: m = "x&a";    goto shortop;
	case 0x0c: m = "a^x";    goto shortop;
	case 0x0d: m = "x^a";    goto shortop;
	case 0x0e: m = "a+x";    goto shortop;
	case 0x0f: m = "x+a";    goto shortop;
	case 0x10: m = "a+cx";   goto shortop;
	case 0x11: m = "x+ca";   goto shortop;
	case 0x12: m = "xca";    goto shortop;
	case 0x13: m = "xc";     goto shortop;
	case 0x14: m = "x++a";   goto shortop;
	case 0x15: m = "x++";    goto shortop;
	case 0x16: m = "x--a?";  goto shortop;
	case 0x17: m = "x--?";   goto shortop;
	case 0x18: m = "xc>>a";  goto shortop;
	case 0x19: m = "xc>>x";  goto shortop;
	case 0x1a: m = "xc<<a";  goto shortop;
	case 0x1b: m = "xc<<x";  goto shortop;
	case 0x1c: m = "xwa";    goto shortop;
	case 0x1d: m = "xw";     goto shortop;
	case 0x1e: m = "x++a?";  goto shortop;
	case 0x1f: m = "x++?";   goto shortop;
	case 0x20: m = "x>>a";   goto shortop;
	case 0x21: m = "x>>x";   goto shortop;
	case 0x22: m = "x<<a";   goto shortop;
	case 0x23: m = "x<<x";   goto shortop;
	case 0x24: m = "x++az?"; goto shortop;
	case 0x25: m = "x++z?";  goto shortop;
	case 0x26: m = "x--az?"; goto shortop;
	case 0x27: m = "x--z?";  goto shortop;
	case 0x28: m = "xza";    goto shortop;
	case 0x29: m = "xz";     goto shortop;
	case 0x2a: m = "xsa";    goto shortop;
	case 0x2b: m = "xs";     goto shortop;
	case 0x2c: m = "anax";   goto shortop;
	case 0x2d: m = "anx";    goto shortop;
	case 0x2e: m = "adax";   goto shortop;
	case 0x2f: m = "adx";    goto shortop;
	case 0x30: m = "x>=a?";  goto shortop;
	case 0x31: m = "x!=a?";  goto shortop;
	case 0x32: m = "x<=a?";  goto shortop;
	case 0x33: m = "x?";     goto shortop;
	case 0x34: m = "a*x";    goto shortop;
	case 0x6a: m = "xta";    goto shortop;
	case 0xa0: m = "llx";    goto shortop;
	case 0xa2: m = "lhx";    goto shortop;
	case 0xa4: m = "xll";    goto shortop;
	case 0xa6: m = "xhl";    goto shortop;
	case 0xa8: m = "plx";    goto shortop;
	case 0xa9: m = "pl++x";  goto shortop;
	case 0xae: m = "xhp";    goto shortop;
	case 0xaf: m = "xhp++";  goto shortop;
	case 0xb0: m = "cta";    goto shortop;
	case 0xb1: m = "a+c";    goto shortop;
	case 0xb2: m = "c-a";    goto shortop;
	case 0xb3: m = "a|c";    goto shortop;
	case 0xb4: m = "a^c";    goto shortop;
	case 0xb5: m = "a&c";    goto shortop;
	case 0xb6: m = "retc";   goto shortop;
	case 0xb7: m = "lcall";  goto shortop;
	case 0xb8: m = "reg";    goto shortop;
	case 0xba: m = "dat";    goto shortop;
	case 0xbc: m = "a*c";
shortop:        printf ("%s", m);
		arg = cmd & 0xff;
		if (arg)
			printf (" 0x%x", arg);
		return;
	}

	switch (cmd >> 8 & 0xf8) {
	case 0x38: m = "bt";  goto bitop;
	case 0x80: m = "bs";  goto bitop;
	case 0x88: m = "bz";  goto bitop;
	case 0x90: m = "bz?"; goto bitop;
	case 0x98: m = "bs?";
bitop:          printf ("%s 0x%x, %d", m, cmd & 0xff, cmd >> 8 & 7);
		break;

	case 0xc0: case 0xc8: case 0xd0: case 0xd8: m = "goto"; goto longop;
	case 0xe0: case 0xe8: case 0xf0: case 0xf8: m = "call";
longop:         printf ("%s", m);
		arg = cmd & 0x1fff;
		if (arg)
			printf (" 0x%x", arg);
		break;

	case 0x60: case 0x68: case 0x70: case 0x78:
		printf ("xtr 0x%x, 0x%x", cmd & 0xff, cmd >> 8 & 0x1f);
		break;

	case 0x40: case 0x48: case 0x50: case 0x58:
		printf ("rtx 0x%x, 0x%x", cmd >> 8 & 0x1f, cmd & 0xff);
		break;

	default:
		printf ("?");
		break;
	}
}

void sigint ()
{
	printf ("\r\nInterrupted\r\n");
	quit ();
}

void set_stimulus (unsigned char *dst, char *p)
{
	for (; *p; ++p)
		if (*p>='0' && *p<='7')
			*dst &= ~(1 << (*p-'0'));
}

int main (int argc, char **argv)
{
	int i;
	char *cp;
	struct termios tio;

	for (i=1; i<argc; i++)
		switch (argv[i][0]) {
		case '-':
			for (cp=argv[i]; *cp; cp++) switch (*cp) {
			case 't':
				debug++;
				break;
			case 'T':
				if (cp [1]) {
					/* -Targ */
					set_trace (cp + 1);
					while (*++cp);
					--cp;
				} else if (i+1 < argc)
					/* -T arg */
					set_trace (argv[++i]);
				break;
			case 'W':
				if (cp [1]) {
					/* -Warg */
					Wflag = strtol (cp+1, 0, 0);
					while (*++cp);
					--cp;
				} else if (i+1 < argc)
					/* -W arg */
					Wflag = strtol (argv[++i], 0, 0);
				break;
			case 'R':
				if (cp [1]) {
					/* -Rarg */
					Rflag = strtol (cp+1, 0, 0);
					while (*++cp);
					--cp;
				} else if (i+1 < argc)
					/* -R arg */
					Rflag = strtol (argv[++i], 0, 0);
				break;
			case 'a':
				set_stimulus (&stimulus_a, cp+1);
				while (*++cp);
				--cp;
				break;
			case 'b':
				set_stimulus (&stimulus_b, cp+1);
				while (*++cp);
				--cp;
				break;
			case 'c':
				set_stimulus (&stimulus_c, cp+1);
				while (*++cp);
				--cp;
				break;
			case 'd':
				set_stimulus (&stimulus_d, cp+1);
				while (*++cp);
				--cp;
				break;
			case 'e':
				set_stimulus (&stimulus_e, cp+1);
				while (*++cp);
				--cp;
				break;
			}
			break;
		default:
			if (infile)
				goto usage;
			infile = argv[i];
			input = fopen (infile, "r");
			if (! input)
				uerror ("cannot open");
			break;
		}

	if (! infile) {
usage:          printf ("PIC 17c4x Simulator, by Serge V.Vakulenko\n");
		printf ("Copyright (C) 1997 Cronyx Engineering Ltd.\n\n");
		printf ("Usage:\n\tas17 [-o outfile.hex] infile.s ...\n\n");
		return -1;
	}

	readimage ();
	if (debug)
		printf ("Got image %s\n", infile);

	signal (SIGINT, sigint);
	signal (SIGTERM, sigint);
	signal (SIGQUIT, sigint);

	tcgetattr (0, &tio);
	stdtio = tio;

	tio.c_iflag &= ~(INLCR | ICRNL | IGNCR | ISTRIP);
	tio.c_oflag &= ~(ONLCR);
	tio.c_lflag &= ~(ECHO | ICANON);
	tcsetattr (0, TCSADRAIN, &tio);

	fcntl (0, F_SETFL, O_NONBLOCK);

	if (debug)
		printf ("Running...\r\n");
	run ();

	fcntl (0, F_SETFL, 0);
	tcsetattr (0, TCSADRAIN, &stdtio);
	return 0;
}
