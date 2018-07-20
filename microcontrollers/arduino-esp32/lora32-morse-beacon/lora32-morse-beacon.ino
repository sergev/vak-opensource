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
#define RST     12   // IO12 -- RESET (If Lora does not work, replace it with IO14)
#define DI0     26   // IO26 -- IRQ(Interrupt Request)

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
    //LoRa.setSignalBandwidth(7800);
    //LoRa.setSignalBandwidth(10400);
    //LoRa.setSignalBandwidth(15600);
    //LoRa.setSignalBandwidth(20800);
    //LoRa.setSignalBandwidth(31250);
    LoRa.setSignalBandwidth(41700);
    //LoRa.setSignalBandwidth(62500);
    LoRa.setCodingRate4(5);
    LoRa.setSpreadingFactor(6);

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

void send(int plen)
{
    // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED, HIGH);

    LoRa.setPreambleLength(plen);
    LoRa.beginPacket();
    LoRa.write(' ');
    LoRa.endPacket();

    // turn the LED off by making the voltage LOW
    digitalWrite(LED, LOW);
}

void loop()
{
    counter++;
    display.clearDisplay();
    display.setCursor(0, 20);
    display.print("Send ");
    display.print(counter);

    display.setCursor(0, 42);
    display.print(LoRa.getSignalBandwidth());
    display.display();

    Serial.println(String(counter));

    send(300); delay(300);
    send(100); delay(300);
    send(100); delay(300);
    send(300); delay(900);
}
