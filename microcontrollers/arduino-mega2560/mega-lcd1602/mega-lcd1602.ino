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

//
// Initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to.
//
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

//
// Constants for input buttons.
//
enum {
    BUTTON_NONE,
    BUTTON_RIGHT,
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_SELECT,
};

//
// Read the buttons.
//
int read_buttons()
{
    // Expected input values are:
    // 0 - right button pressed
    // 131 - up
    // 307 - down
    // 480 - left
    // 721 - select button
    // 1023 - none
    int input = analogRead(0);

    if (input > 872) return BUTTON_NONE;
    if (input > 600) return BUTTON_SELECT;
    if (input > 393) return BUTTON_LEFT;
    if (input > 219) return BUTTON_DOWN;
    if (input > 65)  return BUTTON_UP;

    return BUTTON_RIGHT;
}

//
// Initialize the application.
//
void setup()
{
    // Set up the screen width and height.
    lcd.begin(16, 2);

    // Print a message.
    lcd.print("hello, world!");
}

//
// Run the application.
//
void loop()
{
    delay(10);

    // Display seconds elapsed since power-up.
    // Use right side of the second line.
    lcd.setCursor(9, 1);
    lcd.print(millis() / 1000);

    //
    // Read the buttons.
    // Repeat twice to stabilize.
    //
    static int last_key = BUTTON_NONE;
    int key = read_buttons();
    if (key == last_key) {
        // Move to the begining of the second line.
        lcd.setCursor(0, 1);

        // Perform an action.
        switch (key) {
        case BUTTON_RIGHT:  lcd.print("RIGHT "); break;
        case BUTTON_LEFT:   lcd.print("LEFT  "); break;
        case BUTTON_UP:     lcd.print("UP    "); break;
        case BUTTON_DOWN:   lcd.print("DOWN  "); break;
        case BUTTON_SELECT: lcd.print("SELECT"); break;
        case BUTTON_NONE:   lcd.print("NONE  "); break;
        }
    }
    last_key = key;
}
