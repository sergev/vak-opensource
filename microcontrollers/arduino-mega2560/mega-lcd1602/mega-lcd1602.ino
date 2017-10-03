/*
 * This sketch prints "Hello World!" to the LCD and uses the
 * display() and noDisplay() functions to turn on and off
 * the display.
 *
 * This example code is in the public domain.
 *
 * http://www.arduino.cc/en/Tutorial/LiquidCrystalDisplay
 */
#include <LiquidCrystal.h>

// Initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup() {
    // Set up the LCD's number of columns and rows
    lcd.begin(16, 2);

    // Print a message to the LCD
    lcd.print("hello, world!");
}

void loop() {
    // Turn off the display
    lcd.noDisplay();
    delay(500);

    // Turn on the display
    lcd.display();
    delay(500);
}
