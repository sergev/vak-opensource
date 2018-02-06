/*
 * Test 5-button joystick on LCD4884 shield.
 */
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

//
// SainSmart LCD4884 Arduino shield.
//
Adafruit_PCD8544 display = Adafruit_PCD8544(
    2,      // D2 - SCLK
    3,      // D3 - DIN
    4,      // D4 - DC
    5,      // D5 - CS
    6);     // D6 - RST

void display_button(const char *name, int minv, int maxv)
{
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Press button");

    display.setCursor(0, 15);
    display.println(name);

    display.setCursor(0, 30);
    display.print(minv);
    display.print(" - ");
    display.println(maxv);
}

//
// Get joystick input.
// Return ADC value.
// Decode the button into name pointer.
//
int get_joystick(const char **namep)
{
    int input = analogRead(0); // Read pin A0

    //
    // Observed values:
    // None   1022 - 1023
    // Up      741 - 742
    // Right   506 - 506
    // Down    329 - 330
    // Select  144 - 145
    // Left      0 - 0
    //
    if      (input > 882)  *namep = "NONE";
    else if (input > 624)  *namep = "UP";
    else if (input > 424)  *namep = "RIGHT";
    else if (input > 242)  *namep = "DOWN";
    else if (input > 73)   *namep = "SELECT";
    else                   *namep = "LEFT";

    return input;
}

void setup()
{
    display.begin();
    display.setContrast(50);
    display.setTextColor(BLACK);
    display.setTextSize(1);
}

void loop()
{
    const char *name;
    static const char *prev;
    int value;
    static int minv, maxv;

    display.display();
    value = get_joystick(&name);

    if (name == prev) {
        // While joystick is stable, collect min and max values.
        if (value < minv)
            minv = value;
        if (value > maxv)
            maxv = value;
        display_button(name, minv, maxv);
    } else {
        // Wait until joystick is stable.
        prev = name;
        minv = value;
        maxv = value;
    }
}
