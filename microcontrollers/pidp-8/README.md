# USB HID interface to the PiDP-8 panel

Here you can find a firmware for the Numato PIC32 MX development board,
to use it as a USB interface for the PiDP-8 front panel.

Links:
    [Numato PIC32 MX development board](http://numato.com/pic32-mx-development-board-pic32mx795f512h/)
    [PiDP-8 replica project](http://obsolescence.wix.com/obsolescence#!pidp-8/cbie)

![PiDP-8](https://static.wixstatic.com/media/ce503a_f5c17f45dbaa4583b962da77780c8ae3.jpg/v1/fill/w_897,h_631,al_c,q_90,usm_0.66_1.00_0.01/ce503a_f5c17f45dbaa4583b962da77780c8ae3.jpg)

![PIC32](http://cdn3.bigcommerce.com/s-hziwvf/products/99/images/532/pic32mxdevboard1__31450.1437709090.500.750.jpg?c=2)

# PiDP Connector
 Signal  |PIC32| Pin | Pin |PIC32| Signal
---------|-----|-----|-----|-----|---------
 +5V     |     |  2  |  1  |     | (+3.3V - not used)
 +5V     |     |  4  |  3  | RD0 | col1
 GND     |     |  6  |  5  | RD1 | col2
         |     |  8  |  7  | RD2 | col3
         |     | 10  |  9  |     | GND
 row3    | RG8 | 12  | 11  | RG7 | row2
         |     | 14  | 13  | RE7 | xled8
 xled4   | RE3 | 16  | 15  | RE2 | xled3
 xled5   | RE4 | 18  | 17  |     |
 GND     |     | 20  | 19  | RD8 | col9
 xled6   | RE5 | 22  | 21  | RD7 | col8
 col7    | RD6 | 24  | 23  | RD9 | col10
 col6    | RD5 | 26  | 25  |     |
         |     | 28  | 27  |     |
 GND     |     | 30  | 29  | RD3 | col4
 col11   | RD10| 32  | 31  | RD4 | col5
         |     | 34  | 33  | RD11| col12
 row1    | RG6 | 36  | 35  |     |
 xled1   | RE0 | 38  | 37  | RE6 | xled7
 xled2   | RE1 | 40  | 39  |     | GND

# Numato headers

Header P6 (near RESET button), top view:
 PIC32  | Signal | Pin
--------|--------|----
 RD8    | col9   | 19
 RD9    | col10  | 23
 RD10   | col11  | 32
 RD11   | col12  | 33
 RD0    | col1   | 3
 RC13   |        |
 RC14   |        |
 RD5    | col6   | 26
 RD4    | col5   | 31
 RD3    | col4   | 29
 RD1    | col2   | 5
 RD2    | col3   | 7
 RD6    | col7   | 24

Header P5 (near FWUP button), top view:
 PIC32  | Signal | Pin
--------|--------|----
 RF0    |        |
 RF1    |        |
 RE0    | xled1  | 38
 RE1    | xled2  | 40
 RE2    | xled3  | 15
 RE3    | xled4  | 16
 RE4    | xled5  | 18
 RE5    | xled6  | 22
 RE6    | xled7  | 37
 RE7    | xled8  | 13
 RG6    | row1   | 36
 RG7    | row2   | 11
 RG8    | row3   | 12

RD7 (col8, pin 21) is located at header P2.

# Control algorithm
(1) Configure xledK pins as inputs.
(2) Configure rowJ pins as inputs.
(3) Configure colN pins as inputs.
(4) Enable pull-up resistors on colN pins.
(5) Switch colN pins to output.
(6) For every xledK pin:
    (*) For every colN pin:
        Set colN pin LOW when the corresponding LED[K,N] should be ON,
        otherwise set it HIGH.
    (*) Set xledK pin HIGH.
    (*) Switch xledK pin to output.
    (*) Wait 300usec.
    (*) Set xledK pin LOW.
    (*) Switch xledK pin to input.
(7) Switch colN pins to input.
(8) For every rowJ pin:
    (*) Switch rowJ pin to output.
    (*) Set rowJ pin LOW.
    (*) Wait 3usec.
    (*) For every colN pin:
        Read colN pin - store it as a state of sw[J,N] switch.
    (*) Switch rowJ pin to input.
(9) Go to (5).

Alternative algorithm:

(1) Configure xledK pins as outputs.
(2) Configure rowJ pins as inputs.
(3) Configure colN pins as inputs.
(4) Enable pull-up resistors on colN pins.
(5) For every colN pin:
    (*) For every xledK pin:
        Set xledK pin HIGH when the corresponding LED[K,N] should be ON,
        otherwise set it LOW.
    (*) Set colN pin LOW.
    (*) Switch colN pin to output.
    (*) Wait 300usec.
    (*) Set colN pin HIGH.
    (*) Switch colN pin to input.
(6) For every xledK pin:
    Set xledK pin LOW.
(7) For every rowJ pin:
    (*) Switch rowJ pin to output.
    (*) Set rowJ pin LOW.
    (*) Wait 3usec.
    (*) For every colN pin:
        Read colN pin - store it as a state of sw[J,N] switch.
    (*) Switch rowJ pin to input.
(8) Go to (5).
