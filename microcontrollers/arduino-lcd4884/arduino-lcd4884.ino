/*
 * Example for LCD4884 shield with 'Joystick' button.
 * NOTE: Start Serial Monitor to see switch voltage values.
 * V1.00 02/08/2016
 * Questions: terry@yourduino.com
 */
#include <font_6x8.h>
#include <font_big.h>
#include <LCD4884.h>

#define LCD_BACKLIGHT_PIN   7

void setup()
{
    Serial.begin(115200);
    pinMode(LCD_BACKLIGHT_PIN, OUTPUT);
    lcd.LCD_init();         // create instance of LCD
    lcd.LCD_clear();        // blank the display

    for (int a = 0; a < 5; a++) {
        digitalWrite(LCD_BACKLIGHT_PIN, LOW);
        delay(300);
        digitalWrite(LCD_BACKLIGHT_PIN, HIGH);
        delay(300);
    }
    for (int a = 0; a < 6; a++) {
        lcd.LCD_write_string(0, a, "01234567980123", MENU_NORMAL);
        delay(1000);
    }
    delay(1000);

    // Show the BIG characters (0..9, + - only)
    lcd.LCD_clear();
    lcd.LCD_write_string_big(0, 0, "012345", MENU_NORMAL);
    lcd.LCD_write_string_big(0, 3, "-+-+-+", MENU_NORMAL);
    delay(1000);

    // Now read the joystick
    lcd.LCD_clear();
}

void loop()
{
    lcd.LCD_write_string(1, 1, "PUSH A BUTTON", MENU_NORMAL);
    delay(100);

    int switchVoltage = analogRead(0);
    Serial.print("Switch analog value = ");
    Serial.println(switchVoltage);

    if (switchVoltage == 0) {
        lcd.LCD_write_string(2, 2, "LEFT ", MENU_NORMAL);

    } else if (switchVoltage > 0 && switchVoltage < 180) {
        lcd.LCD_write_string(2, 2, "PUSH IN", MENU_NORMAL);

    } else if (switchVoltage > 180 && switchVoltage < 400) {
        lcd.LCD_write_string(2, 2, "DOWN ", MENU_NORMAL);

    } else if (switchVoltage > 400 && switchVoltage < 600) {
        lcd.LCD_write_string(2, 2, "RIGHT", MENU_NORMAL);

    } else if (switchVoltage > 600 && switchVoltage < 800) {
        lcd.LCD_write_string(2, 2, "UP   ", MENU_NORMAL);

    } else if (switchVoltage > 800) {
        lcd.LCD_write_string(2, 2, "NONE    ", MENU_NORMAL);
    }
}
