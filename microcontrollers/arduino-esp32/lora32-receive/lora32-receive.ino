#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeSans12pt7b.h>
#include "Adafruit_SSD1306.h"

#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     12   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define BAND    433E6

String rssi = "RSSI --";
String packSize = "--";
String packet;

//
// On Lora32 module, OLED reset signal is connected to gpio16.
//
Adafruit_SSD1306 display(16);

void loraData()
{
    display.clearDisplay();
    display.setCursor(0, 20);
    display.print("Received " +  packSize + " bytes");
    //display.drawStringMaxWidth(0, 26, 128, packet);
    display.setCursor(0, 420);
    display.print(rssi);
    display.display();

    Serial.println(rssi);
}

void cbk(int packetSize)
{
    packet = "";
    packSize = String(packetSize,DEC);
    for (int i = 0; i < packetSize; i++) {
        packet += (char) LoRa.read();
    }
    rssi = "RSSI " + String(LoRa.packetRssi(), DEC);
    loraData();
}

void setup()
{
    // Initialize serial port.
    Serial.begin(115200);
    while (!Serial)
        ;
    Serial.println();
    Serial.println("LoRa Receiver Callback");

    // Initialize LoRa chip.
    SPI.begin(SCK, MISO, MOSI, SS);
    LoRa.setPins(SS, RST, DI0);
    if (!LoRa.begin(BAND)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }
    //LoRa.onReceive(cbk);
    LoRa.receive();

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
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        cbk(packetSize);
    }
    delay(10);
}
