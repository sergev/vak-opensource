// Binary Clock Shield for Arduino by Marcin Saj https://nixietester.com
// https://github.com/marcinsaj/Binary-Clock-Shield-for-Arduino
//
// Binary Clock RTC 24H with Interrupt, Alarm and Buttons Example
// This example demonstrates complete Binary Clock with Time and Alarm settings
//
// *It is recommended that the first start should be carried out with the serial terminal,
// for better knowing the setting options.
//
// The buttons allows you to set the time and alarm - exact hour, minute, second/alarm status.
// Alarm causes melody to play.
// How to use piezo with the tone() command to generate notes you can find here:
// http://www.arduino.cc/en/Tutorial/Tone
//
// A falling edge at the RTC INT/SQW output causes an interrupt,
// which is uses for regular - 1 per second - reading time from RTC and
// checking alarm status flag 'A2F'. Since we use RTC INT/SQW output for
// regular reading current time - square wave output SQW option,
// global interrupt flag INTCN is set to 0, this disables the interrupts from both RTC alarms.
// Referring to the documentation: when the INTCN is set to logic 0,
// the 'A2F' bit does not initiate an interrupt signal. By turning off the interrupts from the alarms,
// we can use the interrupt flag 'A2IE' as an info flag whether the alarm has been activated or not.
// Check RTC datasheet page 11-13 http://bit.ly/DS3231-RTC
//
// Hardware:
// Arduino Uno, Binary Clock Shield for Arduino
// Battery CR1216/CR1220
// INT/SQW   connected to Arduino pin 3  INT1
// PIEZO     connected to Arduino pin 11 PWM
// S3 button connected to Arduino pin A0
// S2 button connected to Arduino pin A1
// S1 button connected to Arduino pin A2
// LEDs      connected to Arduino pin A3
// RTC SDA   connected to Arduino pin A4
// RTC SCL   connected to Arduino pin A5
//
//                        +------+       +------+       +------+       +------+       +------+
//                        |LED 16|---<---|LED 15|---<---|LED 14|---<---|LED 13|---<---|LED 12|--<-+
//                        +------+       +------+       +------+       +------+       +------+    |
//                                                                                                |
//    +--------------->-------------->-------------->-------------->-------------->---------------+
//    |
//    |    +------+       +------+       +------+       +------+       +------+       +------+
//    +----|LED 11|---<---|LED 10|---<---|LED 09|---<---|LED 08|---<---|LED 07|---<---|LED 06|--<-+
//         +------+       +------+       +------+       +------+       +------+       +------+    |
//                                                                                                |
//    +--------------->-------------->-------------->-------------->-------------->---------------+
//    |
//    |    +------+       +------+       +------+       +------+       +------+       +------+
//    +----|LED 05|---<---|LED 04|---<---|LED 03|---<---|LED 02|---<---|LED 01|---<---|LED 0 |--<-- DATA_PIN
//         +------+       +------+       +------+       +------+       +------+       +------+

#include <FastLED.h>            // https://github.com/FastLED/FastLED
#include <DS3232RTC.h>          // https://github.com/JChristensen/DS3232RTC
#include <Streaming.h>          // http://arduiniana.org/libraries/streaming/
#include "pitches.h"            // Need to create the pitches.h library: https://arduino.cc/en/Tutorial/ToneMelody

#define DEBUG         1         // If 1 - serial debug ON, if 0 serial debug OFF
#define INT           3         // Interrupt. Arduino pin no.3 <-> Shield RTC INT/SQW pin
#define PIEZO         11        // The number of the Piezo pin
#define LED_PIN       A3        // Data pin that LEDs data will be written out over
#define NUM_LEDS      17        // All LEDs on shield
#define BRIGHTNESS    30        // The best tested LEDs brightness 20-60
#define LED_TYPE      WS2812B   // Datasheet: http://bit.ly/LED-WS2812B
#define COLOR_ORDER   GRB       // For color ordering use this sketch: http://bit.ly/RGBCalibrate

#define ALARM_REPEAT  3         // How many times play the melody alarm

#define S1  A2                  // Push buttons connected to the A0, A1, A2 Arduino pins
#define S2  A1
#define S3  A0

// Array of LEDs
CRGB leds[NUM_LEDS];

// t variable for Arduino Time Library
time_t t;
tmElements_t tm;
// See the Arduino Time Library for details on the tmElements_t structure:
// http://playground.arduino.cc/Code/Time
// https://github.com/PaulStoffregen/Time

// Interrupt flag true/false
volatile bool RTCinterruptWasCalled = false;

// Bit array for storing binary format
bool binaryArray[NUM_LEDS];

// Notes in the melody:
const int melodyAlarm[] PROGMEM =
{
    NOTE_A4,  NOTE_A4,  NOTE_A4,  NOTE_F4,  NOTE_C5,  NOTE_A4,  NOTE_F4,  NOTE_C5,
    NOTE_A4,  NOTE_E5,  NOTE_E5,  NOTE_E5,  NOTE_F5,  NOTE_C5,  NOTE_GS4, NOTE_F4,
    NOTE_C5,  NOTE_A4,  NOTE_A5,  NOTE_A4,  NOTE_A4,  NOTE_A5,  NOTE_GS5, NOTE_G5,
    NOTE_FS5, NOTE_F5,  NOTE_FS5, 0,        NOTE_AS4, NOTE_DS5, NOTE_D5,  NOTE_CS5,
    NOTE_C5,  NOTE_B4,  NOTE_C5,  0,        NOTE_F4,  NOTE_GS4, NOTE_F4,  NOTE_A4,
    NOTE_C5,  NOTE_A4,  NOTE_C5,  NOTE_E5,  NOTE_A5,  NOTE_A4,  NOTE_A4,  NOTE_A5,
    NOTE_GS5, NOTE_G5,  NOTE_FS5, NOTE_F5,  NOTE_FS5, 0,        NOTE_AS4, NOTE_DS5,
    NOTE_D5,  NOTE_CS5, NOTE_C5,  NOTE_B4,  NOTE_C5,  0,        NOTE_F4,  NOTE_GS4,
    NOTE_F4,  NOTE_C5,  NOTE_A4,  NOTE_F4,  NOTE_C5,  NOTE_A4,
};

// Note durations: 4 = quarter note, 8 = eighth note, etc.:
// Some notes durations have been changed (1, 3, 6) to make them sound better
const byte noteDurations[] PROGMEM =
{
    2, 2, 2, 3, 6, 2, 3, 6,
    1, 2, 2, 2, 3, 6, 2, 3,
    6, 1, 2, 3, 6, 2, 4, 4,
    8, 8, 4, 3, 4, 2, 4, 4,
    8, 8, 4, 3, 6, 2, 3, 6,
    2, 3, 6, 1, 2, 3, 8, 2,
    4, 4, 8, 8, 4, 4, 4, 2,
    4, 4, 8, 8, 4, 4, 4, 2,
    3, 8, 2, 3, 8, 1,
};

// Counter of button presses
int countButtonPressed = 0;

// The current reading from the input pins
bool S1state = LOW;
bool S2state = LOW;
bool S3state = LOW;

// The previous reading from the input pins
bool lastreadS1 = LOW;
bool lastreadS2 = LOW;
bool lastreadS3 = LOW;

// The following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;   // The last time the input pin was toggled
unsigned long debounceDelay = 50;     // The debounce time. Increase if the output flickers

// Variables that store the current settings option
int settingsOption = 0;               // Time = 1, Alarm = 3
int settingsLevel = 0;                // Hours = 1, Minutes = 2, Seconds / On/Off Alarm = 3

// Variables that store the current alarm time and status
int hourAlarm;
int minuteAlarm;
int alarmStatus;

//################################################################################//
// CHECK BUTTONS
//################################################################################//

////////////////////////////////////////////////////////////////////////////////////
// Check if the S1 button has been pressed
////////////////////////////////////////////////////////////////////////////////////
int checkS1()
{
    // Read the state of the push button into a local variable:
    bool currentreadS1 = digitalRead(S1);

    // Check to see if you just pressed the button
    // (i.e. the input went from LOW to HIGH), and you've waited long enough
    // since the last press to ignore any noise:

    // Check if button changed, due to noise or pressing:
    if (currentreadS1 != lastreadS1)
    {
        // Reset the debouncing timer
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay)
    {
        // Whatever the reading is at, it's been there for longer than the debounce
        // delay, so take it as the actual current state:

        // If the button state has changed:
        if (currentreadS1 != S1state)
        {
            S1state = currentreadS1;

            // Return 1 only if the new button state is HIGH
            if (S1state == HIGH)
            {
                lastreadS1 = currentreadS1;
                return 1;
            }
        }
    }

    // Save S1 button state. Next time through the loop, it'll be the lastreadS2:
    lastreadS1 = currentreadS1;
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////
// Check if the S2 button has been pressed
////////////////////////////////////////////////////////////////////////////////////
int checkS2()
{
    // Read the state of the push button into a local variable:
    bool currentreadS2 = digitalRead(S2);

    // Check to see if you just pressed the button
    // (i.e. the input went from LOW to HIGH), and you've waited long enough
    // since the last press to ignore any noise:

    // Check if button changed, due to noise or pressing:
    if (currentreadS2 != lastreadS2)
    {
        // Reset the debouncing timer
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay)
    {
        // Whatever the reading is at, it's been there for longer than the debounce
        // delay, so take it as the actual current state:

        // If the button state has changed:
        if (currentreadS2 != S2state)
        {
            S2state = currentreadS2;

            // Return 1 only if the new button state is HIGH
            if (S2state == HIGH)
            {
                lastreadS2 = currentreadS2;
                return 1;
            }
        }
    }

    // Save S2 button state. Next time through the loop, it'll be the lastreadS2:
    lastreadS2 = currentreadS2;
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////
// Check if the S3 button has been pressed
////////////////////////////////////////////////////////////////////////////////////
int checkS3()
{
    // Read the state of the push button into a local variable:
    bool currentreadS3 = digitalRead(S3);

    // Check to see if you just pressed the button
    // (i.e. the input went from LOW to HIGH), and you've waited long enough
    // since the last press to ignore any noise:

    // Check if button changed, due to noise or pressing:
    if (currentreadS3 != lastreadS3)
    {
        // Reset the debouncing timer
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay)
    {
        // Whatever the reading is at, it's been there for longer than the debounce
        // delay, so take it as the actual current state:

        // If the button state has changed:
        if (currentreadS3 != S3state)
        {
            S3state = currentreadS3;

            // Return 1 only if the new button state is HIGH
            if (S3state == HIGH)
            {
                lastreadS3 = currentreadS3;
                return 1;
            }
        }
    }

    // Save S3 button state. Next time through the loop, it'll be the lastreadS3:
    lastreadS3 = currentreadS3;
    return 0;
}

//################################################################################//
// ALARM HANDLING
//################################################################################//

////////////////////////////////////////////////////////////////////////////////////
// Get alarm time and convert it from BCD to DEC format
// Get alarm status active/inactive
////////////////////////////////////////////////////////////////////////////////////
void getAlarmTimeAndStatus ()
{
    // Alarm Time HH:MM
    // 0x0B - alarm2 minute register address - check DS3231 datasheet
    // minutes using 7 bits of that register
    minuteAlarm = RTC.readRTC(0x0B);
    minuteAlarm = ((minuteAlarm & B01110000) >> 4)*10 + (minuteAlarm & B00001111);

    // 0x0C - alarm2 hour register address - check DS3231 datasheet
    // hours using 6 bits of that register
    hourAlarm = RTC.readRTC(0x0C);
    hourAlarm = ((hourAlarm & B00110000) >> 4)*10 + (hourAlarm & B00001111);

    // Alarm Status active/inactive
    // Check bit 2 of the control register - A2IE
    // if 1 - alarm active, if 0 - alarm inactive
    byte currentAlarmStatus = RTC.readRTC(0x0E) && B00000010;

    // alarmStatus: 1 - alarm inactive, 2 - alarm active
    // it is the simplest way to display alarm status on LEDs (bottom S-row)
    // because with 0 and 1 - for 0 no LEDs lit, and this could be misleading
    if (currentAlarmStatus == 0) alarmStatus = 1;
    else if (currentAlarmStatus == 1) alarmStatus = 2;
}
////////////////////////////////////////////////////////////////////////////////////
// Set alarm time and status
////////////////////////////////////////////////////////////////////////////////////
void setAlarmTimeAndStatus ()
{
    // Set alarm time
    RTC.setAlarm(ALM2_MATCH_HOURS, 0, minuteAlarm, hourAlarm, 0);

    // Read RTC control register
    byte controlRegister = RTC.readRTC(0x0E);

    // If the current alarm status is active, set bit 2 of the control register - A2IE
    // If inactive, clear bit 2 of the control register - A2IE
    if (alarmStatus == 2) controlRegister = controlRegister | B00000010;
    else controlRegister = controlRegister & B11111101;

    // Update the control register
    RTC.writeRTC(0x0E, controlRegister);
}

////////////////////////////////////////////////////////////////////////////////////
// Convert values from DEC to BIN format and display
////////////////////////////////////////////////////////////////////////////////////
void convertDecToBinaryAndDisplay(int bottomRow, int middleRow, int upperRow)
{
    bool oneBit;

    for(int i = 12; i < 17; i++)                        // H - upper row
    {
        oneBit = upperRow & B00000001;                  // Extraction of individual bits 0/1
        binaryArray[i] = oneBit;                        // Save bit in binary array
        upperRow = upperRow >> 1;                       // Bit shift

        if(binaryArray[i] == 1) leds[i] = CRGB::Blue;   // If 1 - turn on LED
        else leds[i] = CRGB::Black;                     // If 0 - turn off LED
    }

    for(int i = 6; i < 12; i++)                         // M - middle row
    {
        oneBit = middleRow & B00000001;                 // Extraction of individual bits 0/1
        binaryArray[i] = oneBit;                        // Save bit in binary array
        middleRow = middleRow >> 1;                     // Bit shift

        if(binaryArray[i] == 1) leds[i] = CRGB::Green;  // If 1 - turn on LED
        else leds[i] = CRGB::Black;                     // If 0 - turn off LED
    }

    for(int i = 0; i <6; i++)                           // S - bottom row
    {
        oneBit = bottomRow & B00000001;                 // Extraction of individual bits 0/1
        binaryArray[i] = oneBit;                        // Save bit in binary array
        bottomRow = bottomRow >> 1;                     // Bit shift

        if(binaryArray[i] == 1) leds[i] = CRGB::Red;    // If 1 - turn on LED
        else leds[i] = CRGB::Black;                     // If 0 - turn off LED
    }

    FastLED.show();
}

//################################################################################//
// RTC TIME & BINARY FORMAT
//################################################################################//

////////////////////////////////////////////////////////////////////////////////////
// Write time to RTC
////////////////////////////////////////////////////////////////////////////////////
void setNewTime ()
{
    // Save the time stored in tmElements_t tm structure
    RTC.write(tm);
}

////////////////////////////////////////////////////////////////////////////////////
// Get time from RTC and convert to BIN format
////////////////////////////////////////////////////////////////////////////////////
void getAndDisplayTime()
{
    // Read time from RTC
    t = RTC.get();
    // Convert time to binary format and display
    convertDecToBinaryAndDisplay(second(t), minute(t), hour(t));
}

//################################################################################//
// SERIAL DEBUG INFO
//################################################################################//

////////////////////////////////////////////////////////////////////////////////////
// Print Time in DEC & BIN format
////////////////////////////////////////////////////////////////////////////////////
void serialDebugTime()
{
#if DEBUG
    Serial << ("DEC:");
    Serial << ((hour(t)<10) ? "0" : "") << _DEC(hour(t)) << (":");
    Serial << ((minute(t)<10) ? "0" : "") << _DEC(minute(t)) << (":");
    Serial << ((second(t)<10) ? "0" : "") << _DEC(second(t)) << ("  ");

    Serial << ("BIN:");
    for(int i = 16; i >= 0; i--)
    {
        if(i == 11 || i == 5) Serial << (" ");
        Serial << binaryArray[i];
    }
    Serial << endl;
#endif
}

////////////////////////////////////////////////////////////////////////////////////
// Show the set alarm time and current alarm status
////////////////////////////////////////////////////////////////////////////////////
void serialDebugAlarmInfo ()
{
#if DEBUG
    Serial << endl << endl;
    Serial << F("-------------------------------------") << endl;
    Serial << F("---- Alarm Time: ");
    Serial << (hourAlarm < 10 ? "0" : "") << hourAlarm << ":";
    Serial << (minuteAlarm < 10 ? "0" : "") << minuteAlarm;
    Serial << endl;
    Serial << F("-------------------------------------") << endl;

    Serial << F("---- Alarm Status: ");
    Serial << (alarmStatus == 2 ? "ON" : "");
    Serial << (alarmStatus == 1 ? "OFF" : "");
    Serial << endl;
    Serial << F("-------------------------------------") << endl;
    Serial << endl;
#endif
}

////////////////////////////////////////////////////////////////////////////////////
// Show the Shield settings menu and alarm status
////////////////////////////////////////////////////////////////////////////////////
void serialDebugStartInfo ()
{
#if DEBUG
    Serial << F("-------------------------------------") << endl;
    Serial << F("------- BINARY CLOCK SHIELD ---------") << endl;
    Serial << F("----------- FOR ARDUINO -------------") << endl;
    Serial << F("-------------------------------------") << endl;
    Serial << F("------------- Options ---------------") << endl;
    Serial << F("S1 - Alarm Settings -----------------") << endl;
    Serial << F("S2 - Disable Alarm Melody -----------") << endl;
    Serial << F("S3 - Time Settings ------------------") << endl;
    Serial << F("-------------------------------------");

    serialDebugAlarmInfo();
#if 0
    Serial << F("#####################################") << endl;
    Serial << F("START WITHIN 10 SECONDS -------------") << endl;

    // Show progress bar: #37 * 270ms = ~10s delay
    for (int i = 0; i < 37; i++)
    {
        delay (270);
        Serial << ("#");
    }
#endif
    Serial << endl << endl;
#endif
}

////////////////////////////////////////////////////////////////////////////////////
// Show alarm settings
////////////////////////////////////////////////////////////////////////////////////
void serialDebugSettings ()
{
#if DEBUG
    if (settingsOption == 1)
    {
        Serial << endl << endl;
        Serial << F("-------------------------------------") << endl;
        Serial << F("---------- Time Settings ------------") << endl;
        Serial << F("-------------------------------------") << endl;
    }

    if (settingsOption == 3)
    {
        Serial << endl << endl;
        Serial << F("-------------------------------------") << endl;
        Serial << F("---------- Alarm Settings -----------") << endl;
        Serial << F("-------------------------------------") << endl;
    }

    if (settingsLevel == 1) Serial << F("--------------- Hour ----------------") << endl;
    if (settingsLevel == 2) Serial << F("-------------- Minute ---------------") << endl;
    if (settingsLevel == 3)
    {
        if(settingsOption == 1) Serial << F("-------------- Second ---------------") << endl;
        if(settingsOption == 3) Serial << F("-------------- ON/OFF ---------------") << endl;
    }

    Serial << F("S1 - Decrement ----------------------") << endl;
    Serial << F("S3 - Increment ----------------------") << endl;
    Serial << F("S2 - Save Current Settings Level ----") << endl;
    Serial << F("-------------------------------------") << endl;

    if (settingsLevel == 1) Serial << F("Current Hour: ") << countButtonPressed << (" ");
    if (settingsLevel == 2) Serial << F("Current Minute: ") << countButtonPressed << (" ");
    if (settingsLevel == 3)
    {
        if (settingsOption == 1) Serial << F("Current Second: ") << countButtonPressed << (" ");
        if (settingsOption == 3)
        {
            Serial << F("Alarm Status: ");
            Serial << (countButtonPressed == 2 ? "ON" : "");
            Serial << (countButtonPressed == 1 ? "OFF" : "");
            Serial << (" ");
        }
    }
#endif
}

////////////////////////////////////////////////////////////////////////////////////
// Show current alarm status during settings
////////////////////////////////////////////////////////////////////////////////////
void serialDebugCurrentModifiedValue ()
{
#if DEBUG
    if((settingsLevel == 3) & (settingsOption == 3))
    {
        Serial << (countButtonPressed == 2 ? "ON" : "");
        Serial << (countButtonPressed == 1 ? "OFF" : "");
    }
    else
    {
        Serial << countButtonPressed;
    }

    Serial << (" ");
#endif
}

//################################################################################//
// SETTINGS
//################################################################################//
////////////////////////////////////////////////////////////////////////////////////
// Depending on the options and settings level, assign to the
// countButtonPressed variable to be able modify the value
////////////////////////////////////////////////////////////////////////////////////
void setCurrentModifiedValue ()
{
    // Assign current time value stored in time_t t variable for modification
    if(settingsOption == 1)
    {
        if(settingsLevel == 1)  countButtonPressed = hour(t);
        if(settingsLevel == 2)  countButtonPressed = minute(t);
        if(settingsLevel == 3)  countButtonPressed = second(t);
    }

    // Alarm time and alarm status
    if(settingsOption == 3)
    {
        if(settingsLevel == 1)  countButtonPressed = hourAlarm;
        if(settingsLevel == 2)  countButtonPressed = minuteAlarm;
        if(settingsLevel == 3)  countButtonPressed = alarmStatus;
    }
}

////////////////////////////////////////////////////////////////////////////////////
// Check current modified value format of the countButtonPressed variable
////////////////////////////////////////////////////////////////////////////////////
void checkCurrentModifiedValueFormat()
{
    // Hours 0-23
    if (settingsLevel == 1)
    {
        if(countButtonPressed < 0) countButtonPressed = 23;
        if(countButtonPressed > 23) countButtonPressed = 0;
    }

    // Minutes 0-59
    if (settingsLevel == 2)
    {
        if(countButtonPressed < 0) countButtonPressed = 59;
        if(countButtonPressed > 59) countButtonPressed = 0;
    }

    // Seconds & Alarm status
    if (settingsLevel == 3)
    {
        // Seconds 0-59
        if (settingsOption == 1)
        {
            if(countButtonPressed < 0) countButtonPressed = 59;
            if(countButtonPressed > 59) countButtonPressed = 0;
        }

        // Alarm status 1/2
        if (settingsOption == 3)
        {
            if(countButtonPressed < 1) countButtonPressed = 2;
            if(countButtonPressed > 2) countButtonPressed = 1;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////
// Depending on the options and settings level, save the current modified value
////////////////////////////////////////////////////////////////////////////////////
void saveCurrentModifiedValue ()
{
    // Save current value in the tmElements_t tm structure
    if(settingsOption == 1)
    {
        if(settingsLevel == 1)  tm.Hour = countButtonPressed;
        if(settingsLevel == 2)  tm.Minute = countButtonPressed;
        if(settingsLevel == 3)  tm.Second = countButtonPressed;
    }

    // Alarm time and alarm status
    if(settingsOption == 3)
    {
        if(settingsLevel == 1)  hourAlarm = countButtonPressed;
        if(settingsLevel == 2)  minuteAlarm = countButtonPressed;
        if(settingsLevel == 3)  alarmStatus = countButtonPressed;
    }
}

////////////////////////////////////////////////////////////////////////////////////
// Display on LEDs only currently modified value
// convertDecToBinaryAndDisplay(bottom row, middle row, upper row)
////////////////////////////////////////////////////////////////////////////////////
void displayCurrentModifiedValue ()
{
    if (settingsLevel == 1) convertDecToBinaryAndDisplay(0, 0, countButtonPressed);
    if (settingsLevel == 2) convertDecToBinaryAndDisplay(0, countButtonPressed, 0);
    if (settingsLevel == 3) convertDecToBinaryAndDisplay(countButtonPressed, 0, 0);
}

//
//                       +-------------------------------+
//                       |           SETTINGS            |
//           +-----------+-------------------------------+
//           |  BUTTONS  |    S3   |     S2    |   S1    |
// +---------+-----------+---------+-----------+---------+
// |         |           |         |   ALARM   |         |
// |         |     0     |  ALARM  |   MELODY  |  TIME   |
// |   S     |           |         |   STOP    |         |
// +   E L   +-----------+---------+-----------+---------+
// |   T E   | ROW H = 1 |    +    |   SAVE    |    -    |
// |   T V   |           |         | LEVEL = 2 |         |
// +   I E   +-----------+---------+-----------+---------+
// |   N L   | ROW M = 2 |    +    |   SAVE    |    -    |
// |   G     |           |         | LEVEL = 3 |         |
// +   S     +-----------+---------+-----------+---------+
// |         | ROW S = 3 |    +    |   SAVE    |    -    |
// |         |           |         | LEVEL = 0 |         |
// +---------+-----------+---------+-----------+---------+
//
//                       +-------------------------------+
//                       |        SETTINGS OPTION        |
//                       +---------------+---------------+
//                       |   ALARM = 3   |   TIME = 1    |
// +---------------------+---------------+---------------+
// |   S     | ROW H = 1 |     3/1       |     1/1       |
// |   E L   |           |     HOUR      |     HOUR      |
// +   T E   +-----------+---------------+---------------+
// |   T V   | ROW M = 2 |     3/2       |     1/2       |
// |   I E   |           |    MINUTE     |    MINUTE     |
// +   N L   +-----------+---------------+---------------+
// |   G     | ROW S = 3 |     3/3       |     1/3       |
// |   S     |           | ALARM STATUS  |    SECOND     |
// +---------+-----------+---------------+---------------+
//
// The core of the settings menu
void settingsMenu ()
{
    // Main menu
    if ((settingsOption == 0) & (settingsLevel == 0))
    {
        // Time settings
        if (checkS1() == HIGH)
        {
            t = RTC.get();                          // Read time from RTC
            settingsOption = 1;                     // Set time option settings
            settingsLevel = 1;                      // Set hour level settings
            setCurrentModifiedValue();              // Assign hours for modify +/-
            serialDebugSettings();                  // Use serial monitor for showing settings
            displayCurrentModifiedValue();          // Display current hour on LEDs
        }

        // Alarm settings
        if (checkS3() == HIGH)
        {
            getAlarmTimeAndStatus();                // Read alarm time and status from RTC
            settingsOption = 3;                     // Set Alarm time option settings
            settingsLevel = 1;                      // Set hour level settings
            setCurrentModifiedValue();              // Assign hours for modify +/-
            serialDebugSettings();                  // Use serial monitor for showing settings
            displayCurrentModifiedValue();          // Display current alarm hour on LEDs
        }
    }

    // Any settings option level except main menu
    if (settingsLevel != 0)
    {
        // Decrement
        if (checkS1() == HIGH)
        {
            countButtonPressed--;                   // Decrement current value e.g. hour, minute, second, alarm status
            checkCurrentModifiedValueFormat();      // Check if the value has exceeded the range e.g minute = 60 and correct
            displayCurrentModifiedValue();          // Display current modified value on LEDs
            serialDebugCurrentModifiedValue();      // Use serial monitor for showing settings
        }

        // Increment
        if (checkS3() == HIGH)
        {
            countButtonPressed++;                   // Increment current value e.g. hour, minute, second, alarm status
            checkCurrentModifiedValueFormat();      // Check if the value has exceeded the range e.g minute = 60 and correct
            displayCurrentModifiedValue();          // Display current modified value on LEDs
            serialDebugCurrentModifiedValue();      // Use serial monitor for showing settings
        }

        // Save
        if (checkS2() == HIGH)
        {
            saveCurrentModifiedValue();             // Save current value e.g. hour, minute, second, alarm status
            settingsLevel++;                        // Go to next settings level - hour => minute => second / alarm status

            if (settingsLevel > 3)                  // If escape from settings then return to main menu
            {
                if (settingsOption == 1)            // If you were in the process of setting the time:
                {
                    setNewTime ();                  // Save time to the RTC
                }

                if (settingsOption == 3)            // If you were in the process of setting the alarm:
                {
                    setAlarmTimeAndStatus();        // Save time and alarm status to the RTC
                }

                serialDebugAlarmInfo();             // Show the time and alarm status info when you exit to the main menu
                settingsLevel = 0;                  // Escape to main menu
                settingsOption = 0;                 // Escape to main menu
            }
            else                                    // If you do not go to the main menu yet
            {
                checkCurrentModifiedValueFormat();  // Check if the value has exceeded the range e.g minute = 60 and correct
                setCurrentModifiedValue();          // Assign next variable for modify +/- hour => minute => second / alarm status
                displayCurrentModifiedValue();      // Display current modified value on LEDs
                serialDebugSettings();              // Use serial monitor for showing settings
            }
        }
    }
}

//################################################################################//
// MELODY ALARM
//################################################################################//

////////////////////////////////////////////////////////////////////////////////////
// During playing the alarm melody, time display function is disabled
////////////////////////////////////////////////////////////////////////////////////
void playAlarm ()
{
    bool stopAlarm = LOW;
    int howManyTimes = 0;
    unsigned long millis_time_now = 0;

    // Count how many notes are in the melody
    int allNotes = sizeof(noteDurations);

    while ((howManyTimes < ALARM_REPEAT) & (stopAlarm == LOW))
    {
        for (int thisNote = 0; thisNote < allNotes; thisNote++)
        {
            // To calculate the note duration, take one second divided by the note type.
            // e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
            int noteDuration = 1000 / pgm_read_byte(&noteDurations[thisNote]);
            tone(PIEZO, pgm_read_word(&melodyAlarm[thisNote]), noteDuration);

            // To distinguish the notes, set a minimum time between them.
            // The note's duration + 30% seems to work well:
            int pauseBetweenNotes = noteDuration * 1.30;

            // Millis time start
            millis_time_now = millis();

            // Pause between notes
            while(millis() < millis_time_now + pauseBetweenNotes)
            {
                // Stop alarm melody and go to main menu
                if (checkS2() == HIGH)
                {
                    // Prepare for escape to main menu
                    settingsLevel = 0;
                    settingsOption = 0;
                    stopAlarm = 1;

                    // Stop the tone playing
                    noTone(PIEZO);
                }
            }

            // Escape to main menu
            if (stopAlarm == 1) break;

            // Stop the tone playing
            noTone(PIEZO);
        }
        howManyTimes++;
    }
}

//################################################################################//
// SETUP
//################################################################################//
void RTCinterrupt()
{
    RTCinterruptWasCalled = true;
}

void setup()
{
#if DEBUG
    Serial.begin(115200);
#endif

    // Important power-up safety delay
    delay(100); // was 3000

    // Limit my draw to 450mA at 5V of power draw
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 450);

    // Initialize LEDs
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);

    // Initialize the buttons pins as an input
    pinMode(S1, INPUT);
    pinMode(S2, INPUT);
    pinMode(S3, INPUT);

    getAlarmTimeAndStatus();
    serialDebugStartInfo();

    // Configure an interrupt on the falling edge from the RTC INT/SQW output
    pinMode(INT, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INT), RTCinterrupt, FALLING);

    // Clear the alarm status flag 'A2F'
    RTC.alarm(ALARM_2);

    // Enable 1 Hz square wave RTC SQW output
    RTC.squareWave(SQWAVE_1_HZ);
}

//################################################################################//
// MAIN LOOP
//################################################################################//
void loop ()
{
    settingsMenu();

    if (RTCinterruptWasCalled & (settingsOption == 0))    // Display time but not during settings
    {
        RTCinterruptWasCalled = false;                    // Clear the interrupt flag
        getAndDisplayTime();                              // Get time from RTC, convert to binary format and display on LEDs
        serialDebugTime();                                // Use serial monitor for showing current time

        if ((RTC.alarm(ALARM_2)) & (alarmStatus == 2))
        {
#if DEBUG
            Serial << "   ALARM!\n";
#endif
            playAlarm();
        }
    }
}
