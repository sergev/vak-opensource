/*
 * PIC17C4x registers.
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

/*
 * Unbanked registers.
 */
char  INDF0   @ 0;
char  FSR0    @ 1;
char  PCL     @ 2;
char  PCLATH  @ 3;
char  ALUSTA  @ 4;
char  T0STA   @ 5;
char  CPUSTA  @ 6;
char  INTSTA  @ 7;
char  INDF1   @ 8;
char  FSR1    @ 9;
char  WREG    @ 0x0a;
char  TMR0L   @ 0x0b;
char  TMR0H   @ 0x0c;
short TMR0    @ 0x0b;
char  TBLPTRL @ 0x0d;
char  TBLPTRH @ 0x0e;
short TBLPTR  @ 0x0d;
char  BSR     @ 0x0f;
char  PRODL   @ 0x18;
char  PRODH   @ 0x19;
short PROD    @ 0x18;

/*
 * Bank 0.
 */
char  PORTA   @ 0x10;
char  DDRB    @ 0x11;
char  PORTB   @ 0x12;
char  RCSTA   @ 0x13;
char  RCREG   @ 0x14;
char  TXSTA   @ 0x15;
char  TXREG   @ 0x16;
char  SPBRG   @ 0x17;

/*
 * Bank 1.
 */
char  DDRC    @ 0x10;
char  PORTC   @ 0x11;
char  DDRD    @ 0x12;
char  PORTD   @ 0x13;
char  DDRE    @ 0x14;
char  PORTE   @ 0x15;
char  PIR     @ 0x16;
char  PIE     @ 0x17;

/*
 * Bank 2.
 */
char  TMR1    @ 0x10;
char  TMR2    @ 0x11;
char  TMR3L   @ 0x12;
char  TMR3H   @ 0x13;
short TMR3    @ 0x12;
char  PR1     @ 0x14;
char  PR2     @ 0x15;
char  PR3L    @ 0x16;
char  PR3H    @ 0x17;
short PR3     @ 0x16;
char  CA1L    @ 0x16;
char  CA1H    @ 0x17;
short CA1     @ 0x16;

/*
 * Bank 3.
 */
char  PW1DCL  @ 0x10;
char  PW2DCL  @ 0x11;
char  PW1DCH  @ 0x12;
char  PW2DCH  @ 0x13;
char  CA2L    @ 0x14;
char  CA2H    @ 0x15;
short CA2     @ 0x14;
char  TCON1   @ 0x16;
char  TCON2   @ 0x17;

/*
 * C compiler working registers.
 */
char  A0      @ 0x0a;		/* WREG */
char  A1      @ 0x09;		/* FSR1 */
char  A2      @ 0x0d;		/* TBLPTRL */
char  A3      @ 0x0e;		/* TBLPTRH */

/*
 * Interrupt bits.
 */
#define PEIF    INTSTA.7        /* peripheral interrupt flag */
#define T0CKIF  INTSTA.6        /* external interrupt on T0CKI pin */
#define T0IF    INTSTA.5        /* timer 0 interrupt flag */
#define INTF    INTSTA.4        /* external interrupt on INT pin */
#define PEIE    INTSTA.3        /* peripheral interrupt enable */
#define T0CKIE  INTSTA.2        /* external T0CKI interrupt enable */
#define T0IE    INTSTA.1        /* timer 0 interrupt enable */
#define INTE    INTSTA.0        /* external INT interrupt enable */

#define RBIE    PIE.7           /* PORTB interrupt on change enable */
#define TMR3IE  PIE.6           /* timer 3 interrupt enable */
#define TMR2IE  PIE.5           /* timer 2 interrupt enable */
#define TMR1IE  PIE.4           /* timer 1 interrupt enable */
#define CA2IE   PIE.3           /* capture 2 interrupt enable */
#define CA1IE   PIE.2           /* capture 1 interrupt enable */
#define TXIE    PIE.1           /* USART transmit interrupt enable */
#define RCIE    PIE.0           /* USART receive interrupt enable */

#define RBIF    PIR.7           /* PORTB interrupt on change */
#define TMR3IF  PIR.6           /* timer 3 interrupt */
#define TMR2IF  PIR.5           /* timer 2 interrupt */
#define TMR1IF  PIR.4           /* timer 1 interrupt */
#define CA2IF   PIR.3           /* capture 2 interrupt */
#define CA1IF   PIR.2           /* capture 1 interrupt */
#define TXIF    PIR.1           /* USART transmit interrupt */
#define RCIF    PIR.0           /* USART receive interrupt */

/*
 * ALUSTA bits.
 */
#define FS3     ALUSTA.7        /* FSR1 do not change */
#define FS2     ALUSTA.6        /* FSR1 post auto increment */
#define FS1     ALUSTA.5        /* FSR0 do not change */
#define FS0     ALUSTA.4        /* FSR1 post auto increment */
#define OV      ALUSTA.3        /* arithmetic overflow flag */
#define Z       ALUSTA.2        /* zero result flag */
#define DC      ALUSTA.1        /* digit carry/not borrow flag */
#define C       ALUSTA.0        /* carry/not borrow flag */

/*
 * CPUSTA bits.
 */
#define STKAV   CPUSTA.5        /* stack is available */
#define GLINTD  CPUSTA.4        /* global interrupt disable */
#define TO      CPUSTA.3        /* no watchdog timeout */
#define PD      CPUSTA.2        /* no power down */

/*
 * T0STA bits.
 */
#define INTEDG  T0STA.7         /* interrupt on rising edge of RA0 */
#define T0SE    T0STA.6         /* increment TMR0 on rising edge of RA1 */
#define T0CS    T0STA.5         /* internal timer 0 clock source */
#define PS3     T0STA.4         /* 1:256 timer 0 prescale, PS2-PS0 don't care */
#define PS2     T0STA.3         /* add 1:16 timer 0 prescale */
#define PS1     T0STA.2         /* add 1:4 timer 0 prescale */
#define PS0     T0STA.1         /* add 1:2 timer 0 prescale */

/*
 * TXSTA bits.
 */
#define CSRC    TXSTA.7         /* internal sync clock source */
#define TX9     TXSTA.6         /* 9-bit transmission enable */
#define TXEN    TXSTA.5         /* transmit enable */
#define SYNC    TXSTA.4         /* sync mode select */
#define TRMT    TXSTA.1         /* transmit shift register empty, readonly */
#define TX9D    TXSTA.0         /* 9-th bit of transmit data */

/*
 * RCSTA bits.
 */
#define SPEN    RCSTA.7         /* configure RA5/RA4 as serial port */
#define RX9     RCSTA.6         /* 9-bit reception enable */
#define SREN    RCSTA.5         /* sync single receive enable */
#define CREN    RCSTA.4         /* continuous receive enable */
#define FERR    RCSTA.2         /* framing error, readonly */
#define OERR    RCSTA.1         /* overrun error, readonly */
#define RX9D    RCSTA.0         /* 9-th bit of receive data, readonly */

#define reg0()   asm (" reg 0")
#define reg1()   asm (" reg 1")
#define reg2()   asm (" reg 2")
#define reg3()   asm (" reg 3")
#define dat0()   asm (" dat 0")
#define dat1()   asm (" dat 1")
#define awake()  asm (" awake")
#define intr_disable() { GLINTD = 1; }
#define intr_enable()  { GLINTD = 0; }

/*
 * Functions for reading/writing data from the page 1 by pointer.
 * Usage:
 * 	val = peek (ptr)
 *	poke (ptr, val)
 */
char peek ();
void poke ();
