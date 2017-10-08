// Example sketch for testing OLED display

// We need to include Wire.h for I2C communication
#include <Wire.h>
#include "OLED.h"

// Declare OLED display
OLED display(SDA, SCL);
// SDA and SCL are the GPIO pins of ESP8266 that are connected to respective pins of display.

//static const int OLED_RESET = 4;

void setup() {
    pinMode(D2, OUTPUT);
    digitalWrite(D2, HIGH);   // turn D2 low to reset OLED
    delay(50);
    digitalWrite(D2, LOW);    // while OLED is running, must set D2 in high
    delay(50);
      
    Serial.begin(9600);
    Serial.println("OLED test!");
#if 0
    // Reset OLED
    pinMode(OLED_RESET, OUTPUT);
    digitalWrite(OLED_RESET, LOW);
    delay(1);
    digitalWrite(OLED_RESET, HIGH);
    delay(10);
    digitalWrite(OLED_RESET, LOW);
#endif
    // Initialize display
    display.begin();

    // Test message
    display.print("Hello World");
    delay(1000);

    // Test long message
    display.print("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.");
    delay(3*1000);

    // Test display clear
    display.clear();
    delay(1000);

    // Test message postioning
    display.print("TOP-LEFT");
    display.print("2nd row", 1);
    display.print("RIGHT-BOTTOM", 3, 4);
    delay(1000);

    // Test display OFF
    display.off();
    display.print("3rd row", 2, 8);
    delay(1000);

    // Test display ON
    display.on();
    delay(1000);
}

int r = 0, c = 0;

void loop() {
    if (r == 0) 
        display.clear();
    
    c = micros() % 6;
    display.print("Hello World", r, c);
    delay(500);

    r++;
    if (r > 3)
        r = 0;
}
