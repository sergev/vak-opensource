/*!
 
 @file      main.c
 @brief     CW Keyer application
 @author    Jan Lategahn DK3LJ jan@lategahn.com (C) 2010

 This file implements a sample CW keyer application by using the yack.c
 library. It is targeted at the ATTINY45 microcontroller but can be used
 for other ATMEL controllers in the same way. Note the enclosed documentation
 for further defails.
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 @version   0.7
 
 @date      15.10.2010  - Created
 @date      16.12.2010  - Submitted to SVN
 @date      16.11.2011  - Last change
 
 */ 


#ifndef F_CPU
#error F_CPU undefined!! Please define in Makefile
#endif

#include <avr/io.h> 
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "yack.h"

// Time after which callsign training is assumed complete
#define		TRAINTIMEOUT	10      // 10 Seconds
#define		PITCHREPEAT		10		// 10 e's will be played for pitch adjust
#define     FARNSREPEAT     10      // 10 a's will be played for Farnsworth

// Some texts in Flash used by the application
const char  txok[] PROGMEM 		= "OK";
const char  vers[] PROGMEM      = "V0.7";

void pitch(void)
/*! 
 @brief     Pitch change mode
 
 This function implements pitch change mode. A series of dots is played and pitch can 
 be adjusted using the paddle levers.
 
 Once 10 dots have been played at the same pitch, the mode terminates
 */
{
	word	timer=PITCHREPEAT;
	
	while (timer) 			// while not yet timed out
	{
		timer--;
		yackchar('E');		// play an 'e'
		
		if(!(KEYINP & (1<<DITPIN))) // if DIT was keyed
	  	{
	  		yackpitch(DOWN);		// increase the pitch
	  		timer=PITCHREPEAT;
	  	}
		
		if(!(KEYINP & (1<<DAHPIN))) // if DAH was keyed
	  	{
	  		yackpitch(UP);	// lower the pitch
	  		timer=PITCHREPEAT;
	  	}
	  	
	}
	
}



void setfarns(void)
/*! 
 @brief     Farnsworth change mode
 
 This function implements farnsworth pause change mode. Farnsworth pauses can be added or removed
 with the paddle keys.
 
 */
{
	byte	timer=0;
	
	while (timer++ != FARNSREPEAT) 			// while not yet timed out
	{
        
		if(!(KEYINP & (1<<DITPIN))) // if DIT was keyed
	  	{
	  		yackspeed(DOWN,FARNSWORTH);		// increase the pitch
	  		timer=0;
	  	}
		
		else if(!(KEYINP & (1<<DAHPIN))) // if DAH was keyed
	  	{
	  		yackspeed(UP,FARNSWORTH);	// lower the pitch
	  		timer=0;
	  	}
	  
        else 
            yackdelay(IWGLEN);
        
	}
	
}




word lfsr(byte n)
/*! 
 @brief     Simple random number generator
 
 The library function random number generator works fine but is 
 a nightmare in size as it utilizes various multiplications and
 divisions (+600 Byte). This routine implements a LFSR (linear
 feedback shift register) in the Galois method which is good enough 
 for this specific application.
 
 @param n    a number between 2 and 255
 @return     a random number between 0 and n-1
 */
{
	
	static word 	lfsr = 0xACE1;
	byte			random;
	
  	lfsr = (lfsr >> 1) ^ (-(lfsr & 1u) & 0xB400u);    
	
	random = lfsr >> 8; // Byte = upper byte of word 
	
	while (random >= n) random -= n; // Cheap modulo :-)
	
	return random;
	
}





void rndcall(char* call)
/*! 
 @brief     Randomize a callsign
 
 This creates a random callsign with 2 letters + 1 digit + 2 letters
 
 @param call a pointer to a buffer of sufficient size to store the callsign
 */
{
	byte	i;
	
	// Generate a random callsign 2 Char, 1 Digit, 2 Char
	
	for (i=0;i<5;i++)
	{
		if (i == 2)
			call[i]=lfsr(10) + '0';
		else
			call[i]=lfsr(26) + 'A';
		
	}
}




void cstrain(void)
/*! 
 @brief     Callsign trainer mode
 
 This implements callsign training. The keyer plays a random callsign and the 
 user repeats it on the paddle. If a mistake happens, the error prosign is
 sounded, the callsign sent again and the user attempts one more time.
 */
{
	char	call[5]; 	// A buffer to store the callsign
	char	c;			// The character returned by IAMBIC keyer
	byte	i;			// Counter
	byte	n;			// Playback counter
	word	timer;		// Timeout timer
	
	while(1)	// Endless loop will exit throught RETURN statement only
		
	{
		rndcall(call); // Make up a callsign
		
		i=0; // i counts the number of chracters correctly guessed
		
		while(i<5)
		{
            if (!i) // If nothing guessed yet, play the callsign
			{
				yackdelay(2 * IWGLEN); // Give him some time to breathe b4 next callsign
				for (n=0;n<5;n++)
                {    
					yackchar(call[n]);
                    yackfarns(); // Add potential farnsworth delays
                    if(yackctrlkey(TRUE)) 
                        return; // Abort if requested..
                }
			}
			
			timer = YACKSECS(TRAINTIMEOUT);
			
			do 
			{ 
				
				c=yackiambic(OFF); 	// Wait for a character 
				yackbeat();			// FSM heartbeat
				timer--;			// Countdown
                
			} while ((!c) && timer && !(yackctrlkey(FALSE))); // Stop when character or timeout
			
			if (timer == 0 || yackctrlkey(TRUE))	// If termination because of timeout 
				return;				// then return
			
			if (call[i] == c)		// Was it the right character?
				i++;				// then increment counter
			else
			{
		   		yackerror();		// Send an error prosign
				i=0;				// And reset the counter
			}
			
		}
		
		yackchar ('R');
		
	}
}



void beacon(byte mode)
/*! 
 @brief     Beacon mode
 
 This routine can read a beacon transmission interval up to 
 9999 seconds and store it in EEPROM (RECORD mode)
 In PLAY mode, when called in the YACKBEAT loop, it plays back
 message 2 in the programmed interval
 
 @param mode RECORD (read and store the beacon interval) or PLAY (beacon)

 @see main
 
*/
{

	static word interval = 65000; // A dummy value that can not be reached
	static word timer;
	char c;

	
	if (interval == 65000) // 
		interval = yackuser(READ, 1, 0);	
	
	if (mode == RECORD)
	{
		interval = 0; // Reset previous settings
		timer = YACKSECS(DEFTIMEOUT);
		
		yackchar('N');
		
		while(--timer)
		{	
			c=yackiambic(FALSE);
			yackbeat();
			
			if (c>='0' && c<='9')
			{
				interval *= 10;
				interval += c - '0';
				timer = YACKSECS(DEFTIMEOUT);
			}
		}
		
		if (interval >= 0 && interval <= 9999)
		{
			yackuser(WRITE, 1, interval); // Record interval
			yacknumber(interval); // Playback number
		}
		else 
		{
			yackerror();
		}
		
	}

	
	if ((mode == PLAY) && interval)
	{

#ifdef POWERSAVE

        // If we execute this, the interval counter is positive which means we are waiting
        // for a message playback. In this case we must not allow the CPU to enter sleep mode.
        
        yackpower(FALSE); // Inhibit sleep mode
        
#endif
        
		if (timer) timer--; // Countdown until a second has expired
		else 
		{
			timer = YACKSECS(1); // Reset timer
			
			if ((--interval)==0) // Interval was > 0. Did decrement bring it to 0?
			{

				interval = yackuser(READ, 1, 0); // Reset the interval timer
				yackmessage(PLAY,2); // And play message 2

				
			} 
			
		}
				
	}
	
}




void commandmode(void)
/*! 
 @brief     Command mode
 
 This routine implements command mode. Entries are read from the paddle
 and interpreted as commands.
 
*/
{
	
	char 	c;				// Character from Morse key
    word    timer;          // Exit timer
	
	yackinhibit(ON); 		// Sidetone = on, Keyer = off
	
	yackchar('?'); 			// Play Greeting
	
    timer = YACKSECS(DEFTIMEOUT); // Time out after 10 seconds
    
    while ((yackctrlkey(TRUE)==0) && (timer-- > 0))
	{
		
		c=yackiambic(OFF);
        if (c) timer = YACKSECS(DEFTIMEOUT); // Reset timeout if character read
        
		yackbeat();
        
        lfsr(255);          // Keep seeding the LFSR so we get different callsigns
		
        if (!yackflag(CONFLOCK)) // No Configuration lock?
        {    
            switch (c) // These are the lockable configuration commands
            {
                    
                case	'R': // Reset
                    yackreset();
                    c = TRUE;
                    break;
                    
                case	'A': // IAMBIC A
                    yackmode(IAMBICA);
                    c = TRUE;
                    break;
                    
                case	'B': // IAMBIC B
                    yackmode(IAMBICB);
                    c = TRUE;
                    break;

                case	'L': // ULTIMATIC
                    yackmode(ULTIMATIC);
                    c = TRUE;
                    break;
                    
                case    'D': // DAHPRIO
                    yackmode(DAHPRIO);
                    c = TRUE;
                    break;
                    
                case	'X': // Paddle swapping
                    yacktoggle(PDLSWAP);
                    c = TRUE;
                    break;
                    
                case    'S': // Sidetone toggle
                    yacktoggle(SIDETONE);
                    c = TRUE;
                    break;
                    
                case    'K': // TX keying toggle
                    yacktoggle(TXKEY);
                    c = TRUE;
                    break;
                    
                case    'Z': // Farnsworth pause
                    setfarns();
                    c = TRUE;
                    break;
                        
                case    'I': // TX level inverter toggle
                    yacktoggle(TXINV);
                    c = TRUE;
                    break;
                    
                case	'1': // Record Macro 1
                    yackchar('1');
                    yackmessage(RECORD,1); 
                    c = TRUE;
                    break;
                    
                case	'2': // Record Macro 2
                    yackchar('2');
                    yackmessage(RECORD,2); 
                    c = TRUE;
                    break;
                    
                case	'N': // Automatic Beacon
                    beacon(RECORD);
                    c = TRUE;
                    break;
                    
            }
            
        }
        
        switch (c) // Commands that can be used anytime
        {
                
            case    'V': // Version
                yackstring(vers);
                c = TRUE;
                break;
    
                
            case	'P': // Pitch
                pitch();
                c = TRUE;
                break;
                
            case	'U': // Tune
                yackinhibit(OFF);
                yacktune();
                yackinhibit(ON);
                c = TRUE;
                break;
                
            case	'C': // Callsign training
                cstrain();
                c = TRUE;
                break;
                
            case    '0': // Lock changes
                yacktoggle(CONFLOCK);
                c = TRUE;
                break;
                
            case	'E': // Playback Macro 1
                yackinhibit(OFF);
                yackmessage(PLAY,1);
                yackinhibit(ON);
                timer = YACKSECS(MACTIMEOUT);
                c = FALSE;
                break;
                
            case	'T': // Playback Macro 2
                yackinhibit(OFF);
                yackmessage(PLAY,2);
                yackinhibit(ON);
                timer = YACKSECS(MACTIMEOUT);
                c = FALSE;
                break;
                
            case	'N': // Automatic Beacon
                beacon(RECORD);
                c = TRUE;
                break;
                
            case    'W': // Query WPM
                yacknumber(yackwpm());
                c = TRUE;
                break;
                
                
        }
        
        if (c == TRUE) // If c still contains a string, the command was not handled properly
            yackstring(txok);
        else if (c)
            yackerror();
        
            
	}
        
        
	yackchar('E'); 			// Sign off

	yackinhibit(OFF);		// Back to normal mode
	
}



int main(void) 
/*! 
 @brief     Trivial main routine
 
 Yack library is initialized, command mode is entered on request and both 
 beacon and keyer routines are called in 10 ms intervals.
 
 @return Not relevant
*/
{
	
	yackinit(); 					// Initialize YACK hardware
	
	while(1) // Endless core loop of the keyer app
	{
		
		if (yackctrlkey(TRUE)) // If command key pressed, go to command mode
			commandmode();
		
       	yackbeat();
		beacon(PLAY); // Play beacon if requested
       	yackiambic(OFF);
        
	}
	
	return 0;
}


