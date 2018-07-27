/*
 * Morse CW beacon for LoRa ESP32 board with OLED display.
 *
 * Copyright (c) 2018 Serge Vakulenko
 *
 * Permission to use, copy, modify, and/or distribute this
 * software for any purpose with or without fee is hereby granted,
 * provided that the above copyright notice and this permission
 * notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Fixed8x16.h"
#include "Adafruit_SSD1306.h"

//
// Board pins.
//
#if 0
#define LED         23  // IO23 -- LED for TTGO Lora32 board
#else
#define LED         25  // IO25 -- LED for Heltec Lora32 board
#endif

//
// LoRa pins.
//
#define LORA_SCK    5   // IO5  -- SCK
#define LORA_MISO   19  // IO19 -- MISO
#define LORA_MOSI   27  // IO27 -- MOSI
#define LORA_SS     18  // IO18 -- CS
#define LORA_RST    12  // IO12 -- RESET for V2.1 (use IO14 for V2.0)
#define LORA_DI0    26  // IO26 -- Interrupt Request

//
// OLED pins.
//
#define OLED_RST    16  // IO16 -- RESET
#if 0
#define OLED_SDA    21  // IO21 -- SDA for TTGO Lora32 board
#define OLED_SCL    22  // IO22 -- SCL
#else
#define OLED_SDA    4   // IO4  -- SDA for Heltec Lora32 board
#define OLED_SCL    15  // IO15 -- SCL
#endif

//
// ETSI has defined 433 to 434 MHz frequency band for LoRa application.
// It uses 433.175 MHz, 433.375 MHz and 433.575 MHz frequency channels.
//
#define FREQ_HZ     433575000   // 433.575 MHz

//
// Set output power to 17 dBm = 50 mW.
//
#define TX_DBM      17

//
// Morse speed, words per minute.
//
#define WPM         12

Adafruit_SSD1306 display(OLED_RST);

int dot_msec   = 1200 / WPM;
int dash_msec  = 3600 / WPM;
int pause_msec = 3600 / WPM;

int space_seen;

void setup()
{
    // Set LED pin.
    pinMode(LED, OUTPUT);

    // Initialize serial port.
    Serial.begin(115200);
    while (!Serial)
        ;
    Serial.println();
    delay(1000);
    Serial.println("LoRa Morse Beacon v1.0, by Serge Vakulenko KK6ABQ");

    // Initialize LoRa chip.
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DI0);
    if (!LoRa.begin(FREQ_HZ)) {
        Serial.println("Starting LoRa failed!");
        while (1)
            ;
    }
    LoRa.setSignalBandwidth(41700);
    LoRa.setCodingRate4(5);
    LoRa.setSpreadingFactor(6);
    LoRa.disableCrc();
    LoRa.setTxPower(TX_DBM);
    Serial.println("init ok");

    // Initialize OLED display with the I2C address 0x3C.
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C, true, OLED_SDA, OLED_SCL);
    display.clearDisplay();

    // Set font.
    display.setFont(&Fixed8x16);
    display.setTextColor(WHITE);
    display.setCursor(0, Fixed8x16.yAdvance);

    // Visualize the result.
    display.display();
}

void beep(int msec)
{
    // Compute preamble length of required duration.
    // To measure the timing, use plen = msec.
    int plen = (1000L*msec - 19089) / 1536;

    digitalWrite(LED, HIGH);        // LED on

    LoRa.setPreambleLength(plen);   // preamble length
    LoRa.beginPacket(true);         // no header
    LoRa.write(0);                  // data 1 byte
    LoRa.endPacket();

    digitalWrite(LED, LOW);         // LED off
}

void send_morse(char *sym, char *str)
{
    display.print(sym);
    display.display();
    Serial.print(sym);

    if (*str++ == '.')
        beep(dot_msec);
    else
        beep(dash_msec);

    while (*str) {
        delay(dot_msec);

        if (*str++ == '.')
            beep(dot_msec);
        else
            beep(dash_msec);
    }
}

void send_char(int c)
{
    if (isspace(c) || ! isprint(c))
        c = ' ';
    else if (isalpha(c) && islower(c))
        c = toupper(c);

    switch (c) {
    default:
        return;
    case ' ' :
        if (! space_seen) {
            display.write(' ');
            display.display();
            Serial.write(c);
            delay(dot_msec + pause_msec);
        }
        space_seen = 1;
        return;

    case 'A': send_morse("A", ".-");     break;
    case 'B': send_morse("B", "-...");   break;
    case 'C': send_morse("C", "-.-.");   break;
    case 'D': send_morse("D", "-..");    break;
    case 'E': send_morse("E", ".");      break;
    case 'F': send_morse("F", "..-.");   break;
    case 'G': send_morse("G", "--.");    break;
    case 'H': send_morse("H", "....");   break;
    case 'I': send_morse("I", "..");     break;
    case 'J': send_morse("J", ".---");   break;
    case 'K': send_morse("K", "-.-");    break;
    case 'L': send_morse("L", ".-..");   break;
    case 'M': send_morse("M", "--");     break;
    case 'N': send_morse("N", "-.");     break;
    case 'O': send_morse("O", "---");    break;
    case 'P': send_morse("P", ".--.");   break;
    case 'Q': send_morse("Q", "--.-");   break;
    case 'R': send_morse("R", ".-.");    break;
    case 'S': send_morse("S", "...");    break;
    case 'T': send_morse("T", "-");      break;
    case 'U': send_morse("U", "..-");    break;
    case 'V': send_morse("V", "...-");   break;
    case 'W': send_morse("W", ".--");    break;
    case 'X': send_morse("X", "-..-");   break;
    case 'Y': send_morse("Y", "-.--");   break;
    case 'Z': send_morse("Z", "--..");   break;
    case '0': send_morse("0", "-----");  break;
    case '1': send_morse("1", ".----");  break;
    case '2': send_morse("2", "..---");  break;
    case '3': send_morse("3", "...--");  break;
    case '4': send_morse("4", "....-");  break;
    case '5': send_morse("5", ".....");  break;
    case '6': send_morse("6", "-....");  break;
    case '7': send_morse("7", "--...");  break;
    case '8': send_morse("8", "---..");  break;
    case '9': send_morse("9", "----.");  break;
    case '.': send_morse(".", ".-.-.-"); break;
    case ',': send_morse(",", "--..--"); break;
    case '?': send_morse("?", "..--.."); break;
    case '/': send_morse("/", "-..-.");  break;
    case '=': send_morse("=", "-...-");  break;
    }
    space_seen = 0;
}

void send_message(const char *str)
{
    while (*str) {
        send_char(*str++);
        delay(pause_msec);
    }
    display.print(" =\n");
    display.display();
    Serial.println(" =");
    delay(dot_msec + pause_msec);
    send_morse("", "-...-");
    delay(dot_msec + pause_msec);

    display.clearDisplay();
    display.setCursor(0, Fixed8x16.yAdvance);
}

void loop()
{
    send_message("KK6ABQ/B QRPP 50 mW FOX HUNT");
}
