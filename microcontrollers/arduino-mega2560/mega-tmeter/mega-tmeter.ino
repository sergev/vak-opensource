/*
 * Transistor Meter.
 */

/*
 * Read a character from the serial port.
 */
int wait_input()
{
    for (;;) {
        int c = Serial.read();
        if (c >= 0)
            return c;
    }
}

/*
 * Print the current state of the device.
 */
void show_state()
{
#if 0
    // Update input values.
    int i;
    for (i=0; i<NUM_DIGITAL_PINS; i++)
        pin_value[i] = digitalRead(i);

    //
    // Digital pins.
    //
    Serial.write('\n');

    Serial.write("    Value:  ");
    Serial.print(pin_value[13]); Serial.write("\r\n");
#endif
}

/*
 * Top level menu.
 */
void menu_top()
{
again:
    show_state();

    //TODO:
    Serial.write("\r\n  1. Command 1");
    Serial.write("\r\n  2. Command 2");

    Serial.write("\r\n\n");
    for (;;) {
        Serial.write("Command: ");
        int cmd = wait_input();
        Serial.write("\r\n");

        if (cmd == '\n' || cmd == '\r')
            break;

        if (cmd == '1') {
            //TODO
            //pinMode(pin, INPUT);
            //pinMode(pin, OUTPUT);
            goto again;
        }
        if (cmd == '2') {
            //TODO
            //digitalWrite(pin, HIGH);
            //digitalWrite(pin, LOW);
            goto again;
        }
    }
}

void setup()
{
    int i;

    Serial.begin(9600);

    for (i=0; i<NUM_DIGITAL_PINS; i++)
        pinMode(i, INPUT);
    delay(10);

    Serial.write("\r\n-----------------\r\n");
    Serial.write("Transistor Meter\r\n");
}

void loop()
{
    menu_top();
}
