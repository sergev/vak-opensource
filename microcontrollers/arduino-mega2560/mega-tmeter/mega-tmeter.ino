/*
 * Transistor Meter.
 */
unsigned gate_output = 0;       // control voltage to the Gate pin of transistor
unsigned gate_input;            // measured voltage of the Gate pin
unsigned drain_input;           // measured voltage of the Drain pin
unsigned drain_current_input;   // measured current of the Drain pin
unsigned power5_input;          // measured voltage of +5V power source
unsigned power15_input;         // measured voltage of +15V power source

unsigned gate_mv;               // millivolts at the Gate pin
unsigned drain_mv;              // millivolts at the Drain pin
unsigned long drain_uamp;       // microamperes at the Drain pin
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
#define DR_CURRENT_MULT (100000.0 / 220 / 51)                       // resistors 51 and 220

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

/*
 * Measure drain voltage.
 * Repeat until the value is stable for 3 attempts.
 */
void measure_drain_voltage()
{
    drain_input = measure(A1);
    drain_mv = drain_input * ((5000.0 / 1024 / 3) * DRAIN_MULT);
}

void measure_drain_current()
{
    drain_current_input = measure(A4);
    drain_uamp = drain_current_input * ((5000.0 / 1024 / 3) * DR_CURRENT_MULT);
}

/*
 * Measure +5V voltage.
 * Repeat until the value is stable for 3 attempts.
 */
void measure_power5()
{
    power5_input = measure(A2);
    power5_mv = power5_input * ((5000.0 / 1024 / 3) * POWER5_MULT);
}

/*
 * Measure +15V voltage.
 * Repeat until the value is stable for 3 attempts.
 */
void measure_power15()
{
    power15_input = measure(A3);
    power15_mv = power15_input * ((5000.0 / 1024 / 3) * POWER15_MULT);
}

/*
 * Print the current state of the device.
 */
void show_state()
{
    // Update input values.
    measure_gate();
    measure_drain_voltage();
    measure_drain_current();
    measure_power5();
    measure_power15();

    // Display all values.
    Serial.write("\r\n  Gate output: ");
    Serial.print(gate_output);

    Serial.write("\r\n   Gate input: ");
    print_mv(gate_mv);
    Serial.write(" (");
    Serial.print(gate_input);
    Serial.write(")");

    Serial.write("\r\nDrain voltage: ");
    if (power15_mv > 5000) {
        print_mv(drain_mv);
        Serial.write(" (");
        Serial.print(drain_input);
        Serial.write(")");
    } else {
        Serial.write("No power!");
    }

    Serial.write("\r\nDrain current: ");
    print_uamp(drain_uamp);
    Serial.write(" (");
    Serial.print(drain_current_input);
    Serial.write(")");

    Serial.write("\r\n    Power +5V: ");
    print_mv(power5_mv);
    Serial.write(" (");
    Serial.print(power5_input);
    Serial.write(")");

    Serial.write("\r\n   Power +15V: ");
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

void measure_jfet()
{
    int i;
    //unsigned v1 = 0, v2 = 0;

    Serial.write("\r\nMeasuring N-JFET:\r\n\r\n");
    Serial.write(" Vg, V   Id, mA\r\n");
    Serial.write("---------------\r\n");
    //for (i=255; i>=0; i--) {
    for (i=0; i<256; i++) {
        set_gate(i);
        measure_gate();
        measure_drain_current();

        // Print results.
        Serial.write(" ");
        Serial.print(((int)gate_mv - 5000) / 1000.0, 3);
        Serial.write("  ");
        Serial.print(drain_uamp / 1000.0, 3);
        Serial.write("\r\n");
#if 0
        // Stop when last three values are same.
        if (v1 == v2 && v2 == drain_current_input)
            break;
        v1 = v2;
        v2 = drain_current_input;
#endif
    }
    set_gate(0);
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
    // A1 - drain voltage input with divisor 1:4.3
    // A2 - +5V input with divisor 1:2
    // A3 - +15V input with divisor 1:4.3
    // A4 - drain current input via ZXCT1009 current sense monitor
    //
    pinMode(gate_output_pin, OUTPUT);
    pinMode(A0, INPUT);
    pinMode(A1, INPUT);
    pinMode(A2, INPUT);
    pinMode(A3, INPUT);
    pinMode(A4, INPUT);

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
