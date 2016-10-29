/*
  Blink
  Continuously turns on an LED for one second and then off for one second.
  This example is to be used with the chipKIT Uno32, Max32, Cerebot MX3ck, Cerebot MX4ck, and Cerebot MX7ck.
  This example code is in the public domain.
 */
#include <WProgram.h>

void setup()
{
    //Initialize the digital pin as an output
    //PIN_LED1 is type defined in the "Board_Defs.h" file for the particular board being used
    pinMode(PIN_LED1, OUTPUT);
}

void loop()
{
    digitalWrite(PIN_LED1, HIGH); // set the LED on
    delay(1000);                  // wait for a second
    digitalWrite(PIN_LED1, LOW);  // set the LED off
    delay(1000);                  // wait for a second
}
