/*
 * Transistor Meter.
 */
unsigned gate_output = 0;       // control voltage to the Gate pin of transistor
unsigned gate_input;            // measured voltage of the Gate pin
unsigned drain_input;           // measured voltage of the Drain pin
unsigned volt5_input;           // measured voltage of +5V power source
unsigned volt15_input;          // measured voltage of +15V power source

// Use Timer3, pins 2, 3 and 5 on Mega 2560 board.
static const int gate_output_pin = 2;

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

#if 0
/*
 * Print the current state of the device.
 */
void print_binary(const char *str, int v)
{
    Serial.write(str);
    Serial.print((v >> 7) & 1);
    Serial.print((v >> 6) & 1);
    Serial.print((v >> 5) & 1);
    Serial.print((v >> 4) & 1);
    Serial.print((v >> 3) & 1);
    Serial.print((v >> 2) & 1);
    Serial.print((v >> 1) & 1);
    Serial.print(v & 1);
}
#endif

/*
 * Print the current state of the device.
 */
void show_state()
{
    // Update input values.
    gate_input = analogRead(A0);
    drain_input = analogRead(A1);
    volt5_input = analogRead(A2);
    volt15_input = analogRead(A3);

    // Display all values.
    Serial.write("\r\n Gate output: "); Serial.print(gate_output);
    Serial.write("\r\n  Gate input: "); Serial.print(gate_input);
    Serial.write("\r\n Drain input: "); Serial.print(drain_input);
    Serial.write("\r\n  Voltage +5: "); Serial.print(volt5_input);
    Serial.write("\r\n Voltage +15: "); Serial.print(volt15_input);
    //print_binary("\r\n TCCR3A = ", TCCR3A); print_binary(", TCCR3B = ", TCCR3B);; print_binary(", TCCR3C = ", TCCR3C);
    //print_binary("\r\n  OCR3A = ", OCR3A); print_binary(", OCR3B = ", OCR3B);
    //print_binary("\r\n   ICR3 = ", ICR3);
    Serial.write("\r\n");
}

void set_gate(unsigned val)
{
    gate_output = val;
    analogWrite(gate_output_pin, gate_output);
    delay(10);
}

/*
 * Top level menu.
 */
void menu_top()
{
again:
    show_state();

    Serial.write("\r\n  1. Set Gate = -5V");
    Serial.write("\r\n  2. Set Gate = -4.9V");
    Serial.write("\r\n  3. Set Gate = -4.8V");
    Serial.write("\r\n  4. Set Gate = -4.7V");
    Serial.write("\r\n  5. Set Gate = -4.5V");
    Serial.write("\r\n  6. Set Gate = -4V");
    Serial.write("\r\n  7. Set Gate = -3V");
    Serial.write("\r\n  8. Set Gate = -2V");
    Serial.write("\r\n  9. Set Gate = -1V");
    Serial.write("\r\n  0. Set Gate = 0V");
    Serial.write("\r\n\n");
    for (;;) {
        Serial.write("Command: ");
        int cmd = wait_input();
        Serial.write("\r\n");

        if (cmd == '\n' || cmd == '\r')
            break;

        if (cmd == '1') {
            set_gate(0);
            goto again;
        }
        if (cmd == '2') {
            set_gate(256 - 4.9 * 255/5);
            goto again;
        }
        if (cmd == '3') {
            set_gate(256 - 4.8 * 255/5);
            goto again;
        }
        if (cmd == '4') {
            set_gate(256 - 4.7 * 255/5);
            goto again;
        }
        if (cmd == '5') {
            set_gate(256 - 4.5 * 255/5);
            goto again;
        }
        if (cmd == '6') {
            set_gate(256 - 4.0 * 255/5);
            goto again;
        }
        if (cmd == '7') {
            set_gate(256 - 3.0 * 255/5);
            goto again;
        }
        if (cmd == '8') {
            set_gate(256 - 2.0 * 255/5);
            goto again;
        }
        if (cmd == '9') {
            set_gate(256 - 1.0 * 255/5);
            goto again;
        }
        if (cmd == '0') {
            set_gate(255);
            goto again;
        }
    }
}

void setup()
{
    Serial.begin(9600);

    // Configure pins.
    // D2 - gate output
    // A0 - gate input
    // A1 - drain input with divisor 1:3
    // A2 - +5V input with divisor 1:2
    // A3 - +15V input with divisor 1:4
    //
    pinMode(gate_output_pin, OUTPUT);

    // Clock select for Timer3: No prescaling.
    TCCR3B = (TCCR3B & ~7) | 1;
    delay(10);

    Serial.write("\r\n----------------\r\n");
    Serial.write("Transistor Meter\r\n");
}

void loop()
{
    menu_top();
}
