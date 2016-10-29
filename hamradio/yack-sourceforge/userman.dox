/*!

@mainpage YACK - Yet Another CW Keyer - User Manual


@section license License

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
    

@section intro Introduction

YACK is a universal CW keyer library and application for the AVR architecture that is designed to be
reusable. It consists of a set of functions to read, play, record and decode CW input from a paddle
keyer (single or double lever). The library can be used to easily create keyers, CW decoders and trainers, beacons,
door openers, alarm clocks and many more applications at very moderate cost. It can also be mixed with other
applications in the same chip.

Components of the application can be left out or included on as needed, reducing the memory footprint. The library 
can be found in the file yack.c 

To demonstrate the library, a standard keyer application has been written which can be found in main.c

This document describes the operation of the keyer from a user perspective

Version: 0.7

@section hw Hardware

As is, the library is configured to run on a stock ATTINY45 cpu with its internal oscillator at 8MHz, prescaled to 1MHz.

The ATTINY45 comes with 4KB of Flash memory and 256 Byte each for RAM and EEPROM. It can be ported to other AVR chips
if these have sufficient features to support the intended application. As the library was written in C, processors without
internal stack will not work with this application unless significantly rewritten.

Hardware connections are defined in yack.h 

- Pin 1 : PB5 - RESET (Can be used for additional button)
- Pin 2 : PB3 - DIT (towards GND, buffer with 10nF cap)
- Pin 3 : PB4 - DAH (towards GND, buffer with 10nF cap)
- Pin 4 : GND
- Pin 5 : PB0 - TX key line (polarity configurable)
- Pin 6 : PB1 - Sidetone (Connect a piezo disc)
- Pin 7 : PB2 - Command button (towards GND)
- Pin 8 : VCC (5V)

@section usage Usage

After reset in default mode, the keyer operates as regular IAMBIC keyer in IAMBIC B at 12 WPM
(words per minute = 60 CPM). The sidetone generator operates at 800 Hz.

@subsection speed Speed Change

Speed can be changed py pressing and holding the command key while operating the DIT and DAH paddles.
DIT reduces speed while DAH increases speed. The keyer plays an alternating sequence of dit and dah while
changing speed without keying the transmitter.

@subsection cmode Command mode

Pressing the command button without changing speed will switch the keyer into command mode. This will be 
confirmed with the '?' character. Another press of the same button takes the keyer back into regular
keyer mode and will be confirmed by a single dot.

During command mode the transceiver is never keyed and sidetone is always activated. Further
functions can be accessed by keying one-letter commands as listed below.

@subsubsection Version V - Version

The keyer responds with the current keyer software version number

@subsubsection pitch P - Pitch

Allows modifying the sidetone pitch to a higher or lower level. A sequence of dits will be played
and the pitch can be modified with the dit and dah paddles. If no paddle is touched for 5 seconds, 
OK is sounded and the mode terminates, leaving the user in command mode.

@subsubsection reset R - Reset

All settings are returned to their default values except for the stored messages in the message buffers. 
Restored settings include speed and pitch, Paddle Swap, TX level inversion, sidetone and TX keyer settings.

@subsubsection tune U - Tune mode

The transceiver is keyed for a duration of 20 seconds for tuning purposes. Tuning mode is aborted once either
DIT or DAH paddles are touched or the control key is pressed.

@subsubsection ia A - IAMBIC A

Keyer sets IAMBIC A as permanent keying mode. Request is answered with "A OK".

@subsubsection ib B - IAMBIC B

Keyer sets IAMBIC B as permanent keying mode. Request is answered with "B OK".

@subsubsection ultimatic L - Ultimatic

Sets the keyer into ULTIMATIC mode. In Ultimatic mode always tha last paddle to be touched is repeated indefinitely
when paddles are squeezed

@subsubsection dahprio D - DAH priority mode. In squeezed state a sequence of DAHs is sent.

Some of the first generation keyers exhibited this behaviour so the chip can simulate that

@subsubsection swap X - Paddle swapping

DIT and DAH paddles are swapped. Request is answered with "OK".

@subsubsection side S - Sidetone toggle

The builtin sidetone generator setting is toggled (ON -> OFF or OFF -> ON). NOTE: This setting is only of relevance 
for regular keying mode. Sidetone is always on in command mode. Request is answered with "OK".

@subsubsection txtog K - TX Keying toggle

Toggles the setting of the TX keyer output. In default state the keyer switches the output line when it  is in keyer mode. 
Toggling this setting enables or disables that function. NOTE: Keying is always off in Command mode. Request is answered 
with "OK".

@subsubsection farnsworth Z - Set Farnsworth pause

Allows setting of an extended inter-character Pause in all sending modes which makes fast keying easier to understand. 
Note that this of course only influences RECEPTION, not TRANSMISSION. If you desire farnsworth mode in transmission, please 
manually pause during characters.
 
@subsubsection lvtog I - TX level inverter toggle

This function toggles wether the "active" level on the keyer output is VCC or GND. This setting is dependent on the 
attached keying circuit. Request is answered with "OK".

@subsubsection query W - Query current WPM speed

Keyer responds with current keying speed in WPM.

@subsubsection msgrec 1 and 2 - Record internal messages 1 or 2

The keyer immediately responds with "1" or "2" after which a message up to 100 characters can be keyed at current WPM speed.
After 5 seconds of inactivity the message is played back once and then stored in EEPROM. Choosing "1" or "2" but not keying 
a new message deletes the chosen message buffer content.

@subsubsection msgplay E and T - Play back internal messages 1 or 2 

The stored messages 1 or 2 are played back with keying enabled (if configured). A press of the command key immediately
returns the keyer to keyer mode so a QSO can be started.

@subsubsection beacon N - Automatic Beacon

The keyer responds with "N" after which a number between 0 and 9999 can be keyed. After a 5 second timeout the keyer
responds by repeating the number and "OK". Once the keyer returns to keyer mode, the content of message buffer 2 is
repeated in intervals of n seconds. The setting is preserved in EEPROM so the chip can be used as a fox hunt keyer.

Returning to command mode and entering an interval of 0 (or none at all) stops beacon mode.

@subsubsection lock 0 - Lock configuration

The 0 command locks or unlocks the main configuration items but not speed, pitch and playback functions.

@subsubsection trainer C - Callsign trainer

The keyer plays a generated callsign (sidetone only) and the user must repeat it. If it was repeated correctly, "R" is 
played and the next callsign is given. If a mistake was sensed, the error prosign (8 dots) is sounded and
the current callsign is repeated again for the user to try once more. If nothing is keyed for 10 seconds, the keyer returns
to command mode.


*/