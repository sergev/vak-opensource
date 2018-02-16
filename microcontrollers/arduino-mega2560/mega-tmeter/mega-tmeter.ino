/*
 * Transistor Meter.
 */
unsigned gate_output = 0;       // control voltage to the Gate pin of transistor
unsigned gate_input;            // measured voltage of the Gate pin
unsigned drain_input;           // measured current of the Drain pin

unsigned gate_mv;               // millivolts at the Gate pin
unsigned long drain_uamp;       // microamperes at the Drain pin

//
// Use Timer3, pins 2, 3 and 5 on Mega 2560 board.
//
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
 * Print volts.
 */
void print_mv(int mv)
{
    Serial.print(mv / 1000.0, 2);
    Serial.write("V");
}

/*
 * Print milliamperes.
 */
void print_uamp(int uamp)
{
    Serial.print(uamp / 1000.0, 2);
    Serial.write("mA");
}

/*
 * Measure analog voltage.
 * Take three samples and return a sum.
 */
unsigned measure(int pin)
{
    unsigned sum;

    delay(3);
    sum = analogRead(pin);
    delay(3);
    sum += analogRead(pin);
    delay(3);
    sum += analogRead(pin);
    return sum;
}

/*
 * Measure gate voltage.
 * Repeat until the value is stable for 3 attempts.
 */
void measure_gate()
{
    gate_input = measure(A0);
    gate_mv = gate_input * (5000.0 / 1024 / 3);
}

void measure_drain()
{
    drain_input = measure(A1);

    // ZXCT1009 current sense monitor is connected via
    // 51 Ohm resistor as a drain shunt, and 220 Ohm resistor as a load.
    drain_uamp = drain_input * ((5000.0 / 1024 / 3) * (100000.0 / 220 / 51));
}

/*
 * Print the current state of the device.
 */
void show_state()
{
    // Update input values.
    measure_gate();
    measure_drain();

    // Display all values.
    Serial.write("\r\n  Gate output: ");
    Serial.print(gate_output);

    Serial.write("\r\n   Gate input: ");
    print_mv(gate_mv);
    Serial.write(" (");
    Serial.print(gate_input);
    Serial.write(")");

    Serial.write("\r\nDrain current: ");
    print_uamp(drain_uamp);
    Serial.write(" (");
    Serial.print(drain_input);
    Serial.write(")");

    Serial.write("\r\n");
}

void set_gate(unsigned val)
{
    gate_output = val;
    analogWrite(gate_output_pin, gate_output);
    delay(10);
}

void measure_jfet()
{
    int i;
    //unsigned v1 = 0, v2 = 0;

    Serial.write("\r\nMeasuring N-JFET:\r\n\r\n");
    Serial.write(" Vg, V   Id, mA\r\n");
    Serial.write("---------------\r\n");
    digitalWrite(LED_BUILTIN, HIGH);
    for (i=255; i>=0; i--) {
        set_gate(i);
        measure_gate();
        measure_drain();

        // Print results.
        Serial.write(" ");
        Serial.print(((int)gate_mv - 5000) / 1000.0, 3);
        Serial.write("  ");
        Serial.print(drain_uamp / 1000.0, 3);
        Serial.write("\r\n");

        // Stop on zero.
        if (drain_input == 0)
            break;
    }
    set_gate(0);
    digitalWrite(LED_BUILTIN, LOW);
    Serial.write("Done\r\n");
}

/*
 * Top level menu.
 */
void menu_top()
{
again:
    show_state();

    Serial.write("\r\n  1. Set Gate = -5V");
    Serial.write("\r\n  2. Set Gate = -4V");
    Serial.write("\r\n  3. Set Gate = -3V");
    Serial.write("\r\n  4. Set Gate = -2V");
    Serial.write("\r\n  5. Set Gate = -1V");
    Serial.write("\r\n  6. Set Gate = 0V");
    Serial.write("\r\n  M. Measure JFET");
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
            set_gate(256 - 4.0 * 255/5);
            goto again;
        }
        if (cmd == '3') {
            set_gate(256 - 3.0 * 255/5);
            goto again;
        }
        if (cmd == '4') {
            set_gate(256 - 2.0 * 255/5);
            goto again;
        }
        if (cmd == '5') {
            set_gate(256 - 1.0 * 255/5);
            goto again;
        }
        if (cmd == '6') {
            set_gate(255);
            goto again;
        }
        if (cmd == 'm' || cmd == 'M') {
            measure_jfet();
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
    // A1 - drain current input via ZXCT1009 current sense monitor
    //
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(gate_output_pin, OUTPUT);
    pinMode(A0, INPUT);
    pinMode(A1, INPUT);

    // Configure timer 3 prescale factor as 1:1.
    // Set bits CS32,CS31,CS30 to 0b001.
    TCCR3B = (TCCR3B & ~7) | 1;

    delay(10);
    Serial.write("\r\n----------------\r\n");
    Serial.write("Transistor Meter\r\n");
}

void loop()
{
    menu_top();
}
