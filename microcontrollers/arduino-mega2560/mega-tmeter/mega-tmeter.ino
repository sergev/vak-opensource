/*
 * Transistor Meter.
 */
unsigned gate_output = 0;       // control voltage to the Gate pin of transistor
unsigned gate_input;            // measured voltage of the Gate pin
unsigned drain_input;           // measured voltage of the Drain pin
unsigned power5_input;          // measured voltage of +5V power source
unsigned power15_input;         // measured voltage of +15V power source

unsigned gate_mv;               // millivolts at the Gate pin
unsigned drain_mv;              // millivolts at the Drain pin
unsigned power5_mv;             // +5V power source in millivolts
unsigned power15_mv;            // +15V power source in millivolts

//
// Use Timer3, pins 2, 3 and 5 on Mega 2560 board.
//
static const int gate_output_pin = 2;

//
// Calibration coefficients for resistor dividers.
// Typical resistors are about 2% imprecise.
// It makes sense to use an external voltmeter to figure out
// the calibration coefficients for your case.
//
#define CALIBRATE_DRAIN     (11.33 / 11.21)     // drain divider
#define CALIBRATE_POWER5    (5.00  / 5.08)      // +5V divider
#define CALIBRATE_POWER15   (11.39 / 11.06)     // +15V divider

//
// Coefficients of resistor dividers.
//
#define DRAIN_MULT      (((33 + 10) / 10.0) * CALIBRATE_DRAIN)      // resistors 33k and 10k
#define POWER5_MULT     (((10 + 10) / 10.0) * CALIBRATE_POWER5)     // resistors 10k and 10k
#define POWER15_MULT    (((33 + 10) / 10.0) * CALIBRATE_POWER15)    // resistors 33k and 10k

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
    Serial.write("mV");
}

void measure_voltages()
{
    // Update input values.
    gate_input = analogRead(A0);
    drain_input = analogRead(A1);
    power5_input = analogRead(A2);
    power15_input = analogRead(A3);

    // Compute voltages.
    gate_mv = gate_input * (5000.0 / 1024);
    drain_mv = drain_input * ((5000.0 / 1024) * DRAIN_MULT);
    power5_mv = power5_input * ((5000.0 / 1024) * POWER5_MULT);
    power15_mv = power15_input * ((5000.0 / 1024) * POWER15_MULT);
}

/*
 * Print the current state of the device.
 */
void show_state()
{
    // Update input values.
    measure_voltages();

    // Display all values.
    Serial.write("\r\nGate output: ");
    Serial.print(gate_output);

    Serial.write("\r\n Gate input: ");
    print_mv(gate_mv);
    Serial.write(" (");
    Serial.print(gate_input);
    Serial.write(")");

    Serial.write("\r\nDrain input: ");
    if (power15_mv > 5000) {
        print_mv(drain_mv);
        Serial.write(" (");
        Serial.print(drain_input);
        Serial.write(")");
    } else {
        Serial.write("No power!");
    }

    Serial.write("\r\n  Power +5V: ");
    print_mv(power5_mv);
    Serial.write(" (");
    Serial.print(power5_input);
    Serial.write(")");

    Serial.write("\r\n Power +15V: ");
    if (power15_mv > 5000) {
        print_mv(power15_mv);
        Serial.write(" (");
        Serial.print(power15_input);
        Serial.write(")");
    } else {
        Serial.write("Not connected!");
    }

    Serial.write("\r\n");
}

void set_gate(unsigned val)
{
    gate_output = val;
    analogWrite(gate_output_pin, gate_output);
    delay(10);
}

void print_json(unsigned a, unsigned b)
{
    if (i > 0)
        Serial.write(",");
    Serial.write("{");
    Serial.print(a);
    Serial.write(",");
    Serial.print(b);
    Serial.write("}");
}

void measure_up()
{
    int i;

    Serial.write("Measuring from -5V to 0V\r\n");
    for (i=0; i<256; i++) {
        set_gate(i);
        measure_voltages();
        if (i > 0)
            Serial.write(",");
        print_json(i, gate_mv, drain_mv);
    }
    Serial.write("\r\nDone\r\n");
}

void measure_down()
{
    int i;

    Serial.write("Measuring from 0V to -5V\r\n");
    for (i=255; i>=0; i--) {
        set_gate(i);
        measure_voltages();
        if (i < 255)
            Serial.write(",");
        print_json(i, gate_mv, drain_mv);
    }
    Serial.write("\r\nDone\r\n");
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
    Serial.write("\r\n  U. Measure JFET up");
    Serial.write("\r\n  D. Measure JFET down");
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
        if (cmd == 'u' || cmd == 'U') {
            measure_up();
            goto again;
        }
        if (cmd == 'd' || cmd == 'D') {
            measure_down();
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
    // A1 - drain input with divisor 1:4.3
    // A2 - +5V input with divisor 1:2
    // A3 - +15V input with divisor 1:4.3
    //
    pinMode(gate_output_pin, OUTPUT);
    pinMode(A0, INPUT);
    pinMode(A1, INPUT);
    pinMode(A2, INPUT);
    pinMode(A3, INPUT);

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
