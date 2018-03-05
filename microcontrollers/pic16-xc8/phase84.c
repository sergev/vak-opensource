#define _VERSION_ "V0.0.2"
/************************************************************************
*                            phase84.c
*
* Simple Phase control using  PIC16F84 - 100% C Program.
* Using even time slicing method for both Triac and IGBT Drive methods.
* Just change specific #define to choose which method is in use.
*
* Output pin for phase ctrl is RA0
* Zero Cross input is RB0
* Remainder of PORTC is used for level setting 0 to 255 in incs of 2
*
* Output is Active LOW - i.e. it is desigend to pull an LEDs Cathode in an
* opto-coupler low to turn the output ON.
*
* Output Driver Types:
* Either TRIAC or IGBT control can be used with this software by selecting
* the appropiate #define command.
*
* IGBT Driver:
* An IGBT driver turns the output ON at the zero cross point and turns
* the output OFF part way through the cycle - this has really low
* EMI properties and is one of the few methods allowed for phase control
* in European countrys (i.e complies with EN55015 EMI standard).
* Other countries are following suit for example New Zealand and Austrailia
* are almost there (See AS/NZS 4051 and CISPR15 EMI Standards )
* This method of dimming is audibly quiet, does not require extra filter
* componets (If designed properly).It also has a better lamp life and can
* control a wider range of lamps than the TRIAC method of dimming.
*
* TRIAC Driver:
* A TRIAC driver turns the output OFF at the zero-cross and turns the
* output ON part way through the cycle - this has high EMI output and needs
* filter inducors etc. This has been the most popular method of phase
* control since it was first demonstraited in London in 1962. To make
* the TRIAC dimmer complient with modern EMI standards it requires
* carefully designed filter circuits using large Toroid Inductors
* and mains voltage AC rated capacitors.
* This is still the simplest method to do, and quite cheap, but not
* recommended for comercial designs any more.
* It also has a tendancy to reduce Lamp life, and creates alot of audiable
* noise in both the inductors and lamps.
*
*
*
* Processor: PIC16F84 @ 10Mhz
*
* Author: Michael Pearce
*
* Copyright 2001 Michael Pearce
* All Rights Reserved
*
* Free to use for non-comercial, non-profit personal use only.
* For Commercial use please contact author (He doesnt Bite) at
*
* mimemike@paradise.net.nz  or   mike@lightneasy.com
*
* Started: 10 July 2001  (My 20 something Birthday)
*
*************************************************************************
*                          Version Information
*************************************************************************
* Version 0.0.1 - 10 July 2001
* Changed Port Pins to match the circuit diagram.
* Made Input type selectable by #define   (Switches or 8bit mode)
* UNTESTED!!!
* 98 Words ROM  (IGBT Output + Switch Input Version)
* 99 Words ROM  (IGBT Output + 8bit Input Version)
* 91 Words ROM  (TRIAC Output + Switch Input Version)
* 92 Words ROM  (TRIAC Ouptut + 8 bit Input Version)
*************************************************************************
* Version 0.0.1 - 10 July 2001
* Change User Input to an up and DOWN Key on 10ms Interrupt
* Test as Functional
* 98 Words ROM
*************************************************************************
* Version 0.0.0 - 10 July 2001
* First Draft of the Project
* Tested as functional
* 79 words of ROM
*************************************************************************/

#include <pic.h>

#define ON  0        /* To set Output to ON state  */
#define OFF 1        /* To Set Output to OFF state */

/*** Select output settings - Choose only one from each  ***/

/** Output Control Method **/
#define TRIAC    /* TRIAC Control Turns ON part way through cycle */
//#define IGBT       /* IGBT Control turnd OFF part way through cycle */


/** Mains Frequency **/
//#define HZ60     /* For 60Hz Mains system */
#define HZ50       /* For 50Hz Maind system */



/*** Select Input Mode - Switches or 8 BIT ***/
#define INPUT_SWITCHES   /* Use UP/DOWN Switches */
//#define INPUT_8BIT     /* Use 8 Bit I/O port */

/*********** Reload value for Phase Control ************************
* ((1/(MainsFreq*2)) / 256) / (1/(Crystal Freq/4)) = TimerCount
* For 50Hz Mains @ 10Mhz Clock         For 60Hz @ 10MHz Clock
* 1/(50Hz*2)  = 10ms                   1/(60Hz*2)  = 8.3ms
* 10ms/256    = 39us                   8.3ms/256   = 32us
* 1/(10MHz/4) = 0.4us                  1/(10MHz/4) = 0.4us
* 39us/0.4us  = 97                     32us/0.4us  = 80
*
* Subtract Interrupt time and other delays (Guessing at around 5)
* For 50Hz: 97 - 5 = 93                For 60Hz: 80 - 5 = 75
*
* Reload @ 50Hz  = -93 or 163          Reload @ 60Hz = -75 or 181
* (Counter counts up and resets at Zero)
* NOTE: These allow a MAXIMUM num of intstruction cycles between
* interrupts of 93 for 50Hz System, or 75 for 60Hz System.
*******************************************************************/

#ifdef  HZ50
 #define TIMER_RELOAD 163
#endif
#ifdef  HZ60
 #define TIMER_RELOAD 181
#endif


/**** I/O Ports ****/
#ifndef BITNUM
 #define BITNUM(adr, bit) ((unsigned)(&adr)*8+(bit))  /* used for port defs*/
#endif

static bit CH_OUT     @ BITNUM(PORTA,0);        /* Phase Ctrl Output PIN */
static bit T_CH_OUT   @ BITNUM(TRISA,0);        /* Tris for above */

static bit ZERO_INT   @ BITNUM(PORTC,0);        /* Zero Cross Interrupt PIN*/
static bit T_ZERO_INT @ BITNUM(TRISC,0);        /* Tris for above */

static bit SW_UP      @ BITNUM(PORTA,1);        /* Increase Power Switch */
static bit T_SW_UP    @ BITNUM(TRISA,1);        /* Tris for above */

static bit SW_DWN     @ BITNUM(PORTA,2);        /* Decrease Power Switch */
static bit T_SW_DWN   @ BITNUM(TRISA,2);        /* Tris for above */

#define    D_IN         PORTC                   /* Data Input port */
static bit D_IN_LSB   @ BITNUM(PORTA,3);        /* LSB For data input PORT */
static bit T_DIN_LSB  @ BITNUM(TRISA,3);        /* TRIS for above */



/**** Global Variables ****/
unsigned char CH_COUNT;        /* Interrupt Counter */
unsigned char CH_LEVEL;        /* Target Level      */

bit INT_10MS;                  /* 10ms interrupt flag */


/*********************************************************************
*                          main
*********************************************************************/
void main(void)
{
 /***** Set Up OPTION ******
 * RB Pullup = Enabled = 0
 * IntEdge   = Rise    = 1
 * T0 Src    = Internal= 0
 * T0 Edge   = L to H  = 0
 * Prescale  = WDT     = 1
 * Prescale  = 1:1     = 000
 ***************************/
 //OPTION=0b01001000;


 /**** Set up Ports ****/
 TRISA=0xFF;      /* Port A as Inputs */
 TRISC=0xFF;      /* Port B as Inputs */


 T_CH_OUT=1;      /* Output Pin to High Impedance State */
 CH_OUT=0;        /* When Tris set to Output, pull LOW  */

 T_ZERO_INT=1;    /* Zero Cross Interrupt Pin as INPUT  */
 ZERO_INT=1;

 SW_UP=1;         /* Switch as INPUT */
 T_SW_UP=1;

 SW_DWN=1;        /* Switch as INPUT */
 T_SW_DWN=1;



 /**** Initialise Variables ****/
 CH_COUNT=0;      /* Set Inital output to 0%            */
 CH_LEVEL=0;
 INT_10MS=0;      /* Dont check keys till interrupt */

 /**** Set Up Interrupts ****/
 INTCON=0x00;     /* Disable All Interrupts and clear flags */

 INTE=1;          /* Enable ZeroCross Interrupt */
                  /* Note: T0IE enable from Zero Cross Routine */
 GIE=1;           /* Enable Global interrupts   */


 /**** Main Program Loop ****/
 /* Loop FOREVER */
 while(1)
 {
  /* Check for 10ms interrupt */
  if(INT_10MS)
  {
   INT_10MS=0;                  /* Clear Flag */

   /* If using UP/Down Switches */
   #ifdef INPUT_SWITCHES
    if(SW_UP==0)CH_LEVEL++;      /* Check for INCREASE Switch */
    if(SW_DWN==0)CH_LEVEL--;     /* Check for DECREASE Switch */
   #endif

   /* If using 8-bit port */
   #ifdef INPUT_8BIT
    CH_LEVEL=D_IN &0xFE;         /* Read in all except LSB */
    if(D_IN_LSB)CH_LEVEL |=0x01; /* Read in LSB */
   #endif

  }
 }
}
/************** END OF main *******************/


/***********************************************************************
*                        PhaseControl
*
* This is the Interrupt routine.
* On Zero Cross it resets the output and reloads the count
* On Timer it counts down till time to toggle output.
***********************************************************************/
void interrupt PhaseControl(void)
{
 /**** Timer OverFlow Interrupt ****/
 if(T0IF)
 {
  TMR0=TIMER_RELOAD;    /* Reload the Timer ASAP */
  T0IF=0;               /* Clear the Timer Flag  */

  if(CH_COUNT==0)
  {
   #ifdef TRIAC         /* TRIAC MODE      */
    CH_OUT=ON;          /* Turn Output ON  */
    T_CH_OUT=ON;
   #endif

   #ifdef IGBT          /* IGBT MODE       */
    T_CH_OUT=OFF;       /* Turn Output OFF */
   #endif
  }
  else
  {
   CH_COUNT--;          /* Keep Decreasing Till Zero */
  }
 }

 /**** Zero Cross Interrupt ****/
 if(INTF)
 {
  INTF=0;               /* Clear Interrupt Flag */
  CH_COUNT=CH_LEVEL;    /* Load Level Setting   */

  #ifdef TRIAC          /* TRIAC MODE */
   T_CH_OUT=OFF;        /* Turn Output Off */
  #endif

  #ifdef IGBT           /* IGBT MODE */
   if(CH_COUNT !=0)
   {
    CH_OUT=ON;          /* Turn Output ON if > 0% setting */
    T_CH_OUT=ON;
   }
   else
   {
    T_CH_OUT=OFF;       /* else Ensure it is OFF */
   }
  #endif
  T0IF=1;               /* Force Instant entry to Timer 0 Interrupt */
  T0IE=1;               /* Ensure that TMR0 Interrupt is enabled    */
  INT_10MS=1;           /* Indicate 10ms Interrupt Occured */
 }

}
/*************** END OF PhaseControl ****************************/
