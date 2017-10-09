/*
 * Convey's Life on 128x32 display.
 */
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ESP8266WiFi.h"

/*
 * On W8266 module, OLED reset signal is connected to pin D0.
 */
Adafruit_SSD1306 display(D0);

#define MX      128     /* x-side length */
#define MY      32      /* y-side length */

static int bar[MX];

void setup()
{
    // Initialize with the I2C addr 0x3C (for the 128x32)
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

    // Clear the buffer.
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.print("Scan...");

    // Set WiFi to station mode
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    //WiFi.disconnect();
    Serial.println("Setup done");

    // Visualize the result.
    display.display();
    delay(100);
}

//
// Reset the bar.
//
static void reset_bar()
{
    int x;
    for (x=0; x<MX; x++) {
        bar[x] = 0;
    }
}

//
// Increase the bar.
//
static void raise_bar(int x0, int dy)
{
    int x, y;
    for (x = x0; x < x0 + 7; x++) {
        bar[x] += dy;
        y = MY - bar[x];
        if (y >= 0 && y < MY)
            display.drawPixel(x, y, WHITE);
    }
}

//
// Draw the signal level at given channel
// Channel is in range 1...11
// RSSI is in range -94...-35 dB
//
static void draw_net(int chan, int rssi)
{
    // Ignore incorrect data.
    if (chan < 1 || chan > 11)
        return;

    int level;
    if      (rssi <= -90) level = 0;
    else if (rssi <= -80) level = 1;
    else if (rssi <= -70) level = 2;
    else if (rssi <= -60) level = 3;
    else if (rssi <= -50) level = 4;
    else if (rssi <= -40) level = 5;
    else                  level = 6;

    // Draw scale: 8 pixels per 5 MHz
    //TODO: level
    int x0 = chan*8 + 8;
    raise_bar(x0 - 16, 2);
    raise_bar(x0 - 8,  2);
    raise_bar(x0,      2);
    raise_bar(x0 + 8,  2);
    raise_bar(x0 + 16, 2);
}

void loop()
{
    Serial.println("scan start");

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    display.clearDisplay();
    display.setCursor(0, 0);
    reset_bar();
    if (n == 0) {
        display.print("no nets");
        Serial.println("no networks found");
    } else {
        display.print(n);
        display.print(" nets");
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            int chan = WiFi.channel(i);
            int rssi = WiFi.RSSI(i);
            Serial.print(i + 1);
            Serial.print(": channel ");
            Serial.print(chan);
            Serial.print(", ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(rssi);
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
            draw_net(chan, rssi);
        }
    }
    Serial.println("");

    // Visualize the result.
    display.display();

    // Wait a bit before scanning again
    delay(1000);
}
