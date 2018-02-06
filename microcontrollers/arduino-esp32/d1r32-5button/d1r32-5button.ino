/*
 * Test joystick buttons on LCD4884 shield.
 */
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

//
// SainSmart LCD4884 Arduino shield.
//
Adafruit_PCD8544 display = Adafruit_PCD8544(
    26,     // D2 - SCLK
    25,     // D3 - DIN
    17,     // D4 - DC
    16,     // D5 - CS
    27);    // D6 - RST

void display_button(const char *name, int value)
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Press button");

    display.setCursor(0, 15);
    display.setTextSize(1);
    display.println(name);

    display.setCursor(0, 30);
    display.setTextSize(2);
    display.println(value);
}

void setup()
{
    display.begin();
    display.setContrast(50);
    display.setTextColor(BLACK);

    display_button("NONE", 1023);
}

//
// Get joystick input.
// Return ADC value.
// Decode the button into name pointer.
//
int get_joystick(const char **namep)
{
    static const char *prev;

    int input = analogRead(2); // Pin A0 on D1 R32 board.

    //
    // Observed values:
    // None   1178 - 1259
    // Up     1022 - 1109
    // Right   837 - 909
    // Down    624 - 716
    // Select  292 - 384
    // Left      0 - 0
    //
    if      (input > 1144) *namep = "NONE";
    else if (input > 966)  *namep = "UP";
    else if (input > 777)  *namep = "RIGHT";
    else if (input > 504)  *namep = "DOWN";
    else if (input > 146)  *namep = "SELECT";
    else                   *namep = "LEFT";

    return input;
}

void loop()
{
    const char *name;
    static const char *prev;
    int value;

    display.display();
    value = get_joystick(&name);

    if (name == prev) {
        // Handle the new joystick state.
        display_button(name, value);
    } else {
        // Wait until joystick is stable.
        prev = name;
    }
}
