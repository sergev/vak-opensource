void setup()
{
    // Set pins LED1-3 as output.
    pinMode(PIN_LED1, OUTPUT);
    pinMode(PIN_LED2, OUTPUT);
    pinMode(PIN_LED3, OUTPUT);
}

void loop()
{
    // Blink LED1.
    digitalWrite(PIN_LED1, HIGH);
    delay(100);
    digitalWrite(PIN_LED1, LOW);

    // Blink LED2.
    digitalWrite(PIN_LED2, HIGH);
    delay(100);
    digitalWrite(PIN_LED2, LOW);

    // Blink LED3.
    digitalWrite(PIN_LED3, HIGH);
    delay(100);
    digitalWrite(PIN_LED3, LOW);

    // Blink LED2.
    digitalWrite(PIN_LED2, HIGH);
    delay(100);
    digitalWrite(PIN_LED2, LOW);
}
