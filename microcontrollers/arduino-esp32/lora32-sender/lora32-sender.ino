#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeSans12pt7b.h>
#include "Adafruit_SSD1306.h"

#define SCK     5    // GPIO5  -- SCK
#define MISO    19   // GPIO19 -- MISO
#define MOSI    27   // GPIO27 -- MOSI
#define SS      18   // GPIO18 -- CS
#define RST     12   // GPIO14 -- RESET (If Lora does not work, replace it with GPIO14)
#define DI0     26   // GPIO26 -- IRQ(Interrupt Request)
#define BAND    433E6

unsigned int counter = 0;

//
// On Lora32 module, OLED reset signal is connected to gpio16.
//
Adafruit_SSD1306 display(16);

void setup()
{
    // Set LED pin.
    pinMode(2, OUTPUT);

    // Initialize serial port.
    Serial.begin(115200);
    while (!Serial)
        ;
    Serial.println();
    Serial.println("LoRa Sender Test");

    // Initialize LoRa chip.
    SPI.begin(SCK, MISO, MOSI, SS);
    LoRa.setPins(SS, RST, DI0);
    if (!LoRa.begin(BAND)) {
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
    display.clearDisplay();
    display.setCursor(0, 20);
    display.print("Send ");
    display.print(counter);
    display.display();

    Serial.println(String(counter));

    // send packet
    LoRa.beginPacket();
    LoRa.print("hello ");
    LoRa.print(counter);
    LoRa.endPacket();

    counter++;
    digitalWrite(2, HIGH);      // turn the LED on (HIGH is the voltage level)
    delay(1000);                // wait for a second
    digitalWrite(2, LOW);       // turn the LED off by making the voltage LOW
    delay(1000);                // wait for a second
}
