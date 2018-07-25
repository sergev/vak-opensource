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

//
// On Lora32 module, OLED reset signal is connected to gpio16.
//
Adafruit_SSD1306 display(16);

//
// Timing for 10 words per minute.
//
int dot_msec   = 120;
int dash_msec  = 360;
int pause_msec = 360;
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
    SPI.begin(SCK, MISO, MOSI, SS);
    LoRa.setPins(SS, RST, DI0);
    if (!LoRa.begin(FREQ_HZ)) {
        Serial.println("Starting LoRa failed!");
        while (1)
            ;
    }
    LoRa.setSignalBandwidth(41700);
    LoRa.setCodingRate4(5);
    LoRa.setSpreadingFactor(6);
    LoRa.disableCrc();

    Serial.println("init ok");

    // Initialize display with the I2C address 0x3C at pins io21 and io22.
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C, true, 21, 22);
    display.clearDisplay();

    // Set size of text.
    display.setFont(&FreeSans12pt7b);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 20);

    // Visualize the result.
    display.display();
}

void beep(int msec)
{
    // Compute preamble length of required duration.
    // To measure the timing, use plen = msec.
    int plen = (1000L*msec - 19089) / 1536;

    // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED, HIGH);

    LoRa.setPreambleLength(plen);   // preamble length
    LoRa.beginPacket(true);         // no header
    LoRa.write(0);                  // data 1 byte
    LoRa.endPacket();

    // turn the LED off by making the voltage LOW
    digitalWrite(LED, LOW);
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
            display.print(" ");
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
    // TODO: fix scrooling
    display.print("=\n");
    display.display();
    Serial.println(" =");
    delay(dot_msec + pause_msec);
    send_morse("", "-...-");
    delay(dot_msec + pause_msec);
}

void loop()
{
    send_message("KK6ABQ/B");
#if 0
    // Measure timing.
    unsigned long t0, t1, t2, t3, t4, t5, t6, t7;
    t0 = micros(); send(300); t1 = micros(); delay(100);
    t2 = micros(); send(100); t3 = micros(); delay(100);
    t4 = micros(); send(100); t5 = micros(); delay(100);
    t6 = micros(); send(300); t7 = micros(); delay(900);
    Serial.printf("%u %u %u %u %u %u %u %u\r\n", t0, t1, t2, t3, t4, t5, t6, t7);
#endif
}
