
// Pins for NodeMCU board.
// For details, see:
// https://github.com/esp8266/Arduino/blob/2.3.0/variants/nodemcu/pins_arduino.h
//
// Pin D0 or LED_BUILTIN - LED on NodeMCU board
// Pin D1 or SCL
// Pin D2 or SDA
// Pin D3
// Pin D4   
// Pin D5 or SCK
// Pin D6 or MISO
// Pin D7 or MOSI
// Pin D8 or SS  
// Pin D9  - serial receive
// Pin D10 - serial transmit
// Pin A0 

static const uint8_t SD2 = 9;  // SD2 pin
static const uint8_t SD3 = 10; // SD3 pin

static const int T = 100;

void setup() 
{
//  Serial.begin(9600);
    pinMode(D0, OUTPUT);  digitalWrite(D0, HIGH);
    pinMode(D1, OUTPUT);  digitalWrite(D1, HIGH);
    pinMode(D2, OUTPUT);  digitalWrite(D2, HIGH);
    pinMode(D3, OUTPUT);  digitalWrite(D3, HIGH);
    pinMode(D4, OUTPUT);  digitalWrite(D4, HIGH);
    pinMode(D5, OUTPUT);  digitalWrite(D5, HIGH);
    pinMode(D6, OUTPUT);  digitalWrite(D6, LOW);
    pinMode(D7, OUTPUT);  digitalWrite(D7, LOW);
    pinMode(D8, OUTPUT);  digitalWrite(D8, LOW);
//  pinMode(SD2, OUTPUT); digitalWrite(SD2, HIGH); // Causes a hangup
    pinMode(SD3, OUTPUT); digitalWrite(SD3, HIGH);
}

void loop() 
{
    digitalWrite(D0, LOW);  delay(T); digitalWrite(D0, HIGH); 
    digitalWrite(D1, LOW);  delay(T); digitalWrite(D1, HIGH); 
    digitalWrite(D2, LOW);  delay(T); digitalWrite(D2, HIGH); 
    digitalWrite(D3, LOW);  delay(T); digitalWrite(D3, HIGH); 
    digitalWrite(D4, LOW);  delay(T); digitalWrite(D4, HIGH); 
    digitalWrite(D5, LOW);  delay(T); digitalWrite(D5, HIGH); 
    digitalWrite(D6, HIGH); delay(T); digitalWrite(D6, LOW); // No effect
    digitalWrite(D7, HIGH); delay(T); digitalWrite(D7, LOW); // No effect
    digitalWrite(D8, HIGH); delay(T); digitalWrite(D8, LOW); // Need to re-solder LED to GND
//  digitalWrite(SD2, LOW); delay(T); digitalWrite(SD2, HIGH); // Causes a hangup
    digitalWrite(SD3, LOW); delay(T); digitalWrite(SD3, HIGH); 
//  Serial.println("(*)");
}
