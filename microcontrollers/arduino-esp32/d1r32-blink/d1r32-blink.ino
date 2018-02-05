//
// Pins for D1 R32 board.
//
static const int PIN_A0 = 2;    // LED
static const int PIN_A1 = 4;
static const int PIN_A2 = 36;
static const int PIN_A3 = 34;
static const int PIN_A4 = 36;
static const int PIN_A5 = 39;
static const int PIN_D0 = 3;    // RXD
static const int PIN_D1 = 1;    // TXD
static const int PIN_D2 = 26;
static const int PIN_D3 = 25;
static const int PIN_D4 = 17;
static const int PIN_D5 = 16;
static const int PIN_D6 = 27;
static const int PIN_D7 = 14;
static const int PIN_D8 = 12;
static const int PIN_D9 = 13;
static const int PIN_D10 = 5;   // SS
static const int PIN_D11 = 23;  // MOSI
static const int PIN_D12 = 19;  // MISO
static const int PIN_D13 = 18;  // SCK
static const int PIN_SDA = 28;
static const int PIN_SCL = 29;
static const int PIN_PROG = 0;

static const int PIN_LED = 2;

void setup()
{
    pinMode(PIN_LED, OUTPUT);
}

void loop()
{
    // Blink twice.
    digitalWrite(PIN_LED, HIGH); delay(100);
    digitalWrite(PIN_LED, LOW);  delay(100);
    digitalWrite(PIN_LED, HIGH); delay(100);
    digitalWrite(PIN_LED, LOW);  delay(100);
    delay(500);
}
