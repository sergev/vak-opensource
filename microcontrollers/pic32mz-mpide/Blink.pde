/*
 * Blink
 * Turns on LED1-LED4 sequentially and repeatedly.
 *
 * This example code is in the public domain.
 */

void setup() {
    // Initialize digital pins as an output.
    // Pins PIN_LED1-PIN_LED4 have LEDs connected on chipKIT WiFire board:
    pinMode(PIN_LED1, OUTPUT);
    pinMode(PIN_LED2, OUTPUT);
    pinMode(PIN_LED3, OUTPUT);
    pinMode(PIN_LED4, OUTPUT);
    digitalWrite(PIN_LED1, LOW);    // set the LED1 off
    digitalWrite(PIN_LED2, LOW);    // set the LED2 off
    digitalWrite(PIN_LED3, LOW);    // set the LED3 off
    digitalWrite(PIN_LED4, LOW);    // set the LED4 off
}

void loop() {
    digitalWrite(PIN_LED1, HIGH);   // set the LED1 on
    delay(200);                     // wait
    digitalWrite(PIN_LED1, LOW);    // set the LED1 off
    digitalWrite(PIN_LED2, HIGH);   // set the LED2 on
    delay(200);                     // wait
    digitalWrite(PIN_LED2, LOW);    // set the LED2 off
    digitalWrite(PIN_LED3, HIGH);   // set the LED3 on
    delay(200);                     // wait
    digitalWrite(PIN_LED3, LOW);    // set the LED3 off
    digitalWrite(PIN_LED4, HIGH);   // set the LED4 on
    delay(200);                     // wait
    digitalWrite(PIN_LED4, LOW);    // set the LED4 off
    digitalWrite(PIN_LED3, HIGH);   // set the LED3 on
    delay(200);                     // wait
    digitalWrite(PIN_LED3, LOW);    // set the LED3 off
    digitalWrite(PIN_LED2, HIGH);   // set the LED2 on
    delay(200);                     // wait
    digitalWrite(PIN_LED2, LOW);    // set the LED2 off
}
