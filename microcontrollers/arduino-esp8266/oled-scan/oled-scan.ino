/*
 * Show the Wi-Fi band activity.
 *
 * Copyright (C) 2017 Serge Vakulenko, <vak@besm6.org>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
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

#define NB      (11+4)  /* 11 channels plus 2+2 side bands */

static int band[NB];    /* signal levels for bands */

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
// Reset bands data.
//
static void reset_bands()
{
    int i;
    for (i=0; i<NB; i++) {
        band[i] = -99;
    }
}

//
// Draw the band level.
//
static void draw_band(int index)
{
    int x = index * 8;
    int y;
    int level = band[index] + 95;

    for (y = MY-1; y >= 0; y-=2) {
        if (level < 0)
            break;
        display.drawFastHLine(x, y, 7, WHITE);
        level -= 4;
    }
}

//
// Draw all bands
//
void draw_bands()
{
    int i;
    for (i=0; i<NB; i++) {
          draw_band(i);
    }
}

//
// Add a signal level to the band.
//
void add_band(int index, int q)
{
    int p = band[index];

    if (p < q) {
        int t = p;
        p = q;
        q = t;
    }
    // P and Q are signal levels in dB, P >= Q.
    // Compute the dB value of the sum of two power levels.
    if (p == q) {
        // When P and Q are equal, result is P+3.01 dB
        p += 3;
    }
    else if (p-1 == q) {
        // When P and Q differ by 1dB, result is P+1.76 dB
        p += 2;
    }
    else if (p-2 == q) {
        // When P and Q differ by 2dB, result is P+0.969 dB
        p += 2;
    }
    else if (p-3 == q) {
        // When P and Q differ by 3dB, result is P+0.511 dB
        p += 1;
    }

    band[index] = p;
}

//
// Add the signal level at given channel
// Channel is in range 1...11
// RSSI is in range -94...-35 dB
//
static void add_channel(int chan, int rssi)
{
    // Ignore incorrect data.
    if (chan < 1 || chan > 11)
        return;

    // Assume the channel emits the given rssi level at his central band (chan+1),
    // half this power at neighbour bands (chan, chan+2) - which is rssi-3 dB,
    // and 1/8 of this power at more distant bands (chan-1, chan+3) - which is rssi-9 dB,
    add_band(chan-1, rssi-9);
    add_band(chan,   rssi-3);
    add_band(chan+1, rssi);
    add_band(chan+2, rssi-3);
    add_band(chan+3, rssi-9);
}

void loop()
{
    Serial.println("scan start");

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    display.clearDisplay();
    display.setCursor(0, 0);
    reset_bands();
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
            add_channel(chan, rssi);
        }
    }
    Serial.println("");

    // Visualize the result.
    draw_bands();
    display.display();

    // Wait a bit before scanning again
    delay(1000);
}
