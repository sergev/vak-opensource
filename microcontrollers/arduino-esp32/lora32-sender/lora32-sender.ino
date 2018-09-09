#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeSans12pt7b.h>
#include "Adafruit_SSD1306.h"

#define SCK     5    // IO5  -- SCK
#define MISO    19   // IO19 -- MISO
#define MOSI    27   // IO27 -- MOSI
#define SS      18   // IO18 -- CS
#define RST     12   // IO14 -- RESET
#define DI0     26   // IO26 -- IRQ (Interrupt Request)

#define LED     23   // IO23 -- LED

//
// ETSI has defined 433 to 434 MHz frequency band for LoRa application.
// It uses 433.175 MHz, 433.375 MHz and 433.575 MHz frequency channels.
//
#define FREQ_HZ 433575000   // 433.575 MHz

unsigned int counter = 0;

//
// On Lora32 module, OLED reset signal is connected to gpio16.
//
Adafruit_SSD1306 display(16);

void setup()
{
    // Set LED pin.
    pinMode(LED, OUTPUT);

    // Initialize serial port.
    Serial.begin(115200);
    while (!Serial)
        ;
    Serial.println();
    Serial.println("LoRa Sender Test");

    // Initialize LoRa chip.
    SPI.begin(SCK, MISO, MOSI, SS);
    LoRa.setPins(SS, RST, DI0);
    if (!LoRa.begin(FREQ_HZ)) {
        Serial.println("Starting LoRa failed!");
        while (1)
            ;
    }
    //LoRa.onReceive(cbk);
    //LoRa.receive();

    Serial.println("init ok");

    // Initialize display with the I2C address 0x3C at pins io21 and io22.
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C, true, 21, 22);
    display.clearDisplay();

    // Set size of text.
    display.setFont(&FreeSans12pt7b);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 20);
    display.print("Init...");

    // Visualize the result.
    display.display();
    delay(1500);
}

void loop()
{
    counter++;
    display.clearDisplay();
    display.setCursor(0, 20);
    display.print("Send ");
    display.print(counter);
    display.display();

    Serial.println(String(counter));

    digitalWrite(LED, HIGH);    // turn the LED on (HIGH is the voltage level)

    // send packet
    LoRa.beginPacket();
    LoRa.print("hello ");
    LoRa.print(counter);
    LoRa.endPacket();

    digitalWrite(LED, LOW);     // turn the LED off by making the voltage LOW

    delay(1000);                // wait for a second
}
