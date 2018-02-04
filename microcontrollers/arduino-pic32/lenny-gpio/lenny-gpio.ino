/*
 * Test GPIO pins using a text menu at console port.
 */
static int is_output_pin[NUM_DIGITAL_PINS];

static int pin_value[NUM_DIGITAL_PINS];

static const char *pin_name[NUM_DIGITAL_PINS] = {
    "D0",   "D1",   "D2",   "D3",   "D4",   "D5",   "D6",   "D7",
    "D8",   "D9",   "D10",  "D11",  "D12",  "D13",  "A0",   "A1",
    "A2",   "A3",   "A4",   "A5",   "AREF", "PROG", "L",    "TX",
    "RX",   "SDI2", "SCK2", "SDO2", "SDA",  "SCL",
};

int wait_input()
{
    for (;;) {
        int c = Serial.read();
        if (c >= 0)
            return c;
    }
}

void show_pins()
{
    int i;

    // Update input values.
    for (i=0; i<NUM_DIGITAL_PINS; i++)
        if (! is_output_pin[0])
            pin_value[i] = digitalRead(i);

    //
    // Digital pins.
    //
    Serial.write('\n');
    Serial.write("     Pins: D0 1 2 3 4 5 6 7 8 9 10 11 12 13\r\n");
    Serial.write("Direction:  ");
    Serial.write(is_output_pin[0]  ? 'o' : 'i'); Serial.write(" ");
    Serial.write(is_output_pin[1]  ? 'o' : 'i'); Serial.write(" ");
    Serial.write(is_output_pin[2]  ? 'o' : 'i'); Serial.write(" ");
    Serial.write(is_output_pin[3]  ? 'o' : 'i'); Serial.write(" ");
    Serial.write(is_output_pin[4]  ? 'o' : 'i'); Serial.write(" ");
    Serial.write(is_output_pin[5]  ? 'o' : 'i'); Serial.write(" ");
    Serial.write(is_output_pin[6]  ? 'o' : 'i'); Serial.write(" ");
    Serial.write(is_output_pin[7]  ? 'o' : 'i'); Serial.write(" ");
    Serial.write(is_output_pin[8]  ? 'o' : 'i'); Serial.write(" ");
    Serial.write(is_output_pin[9]  ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[10] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[11] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[12] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[13] ? 'o' : 'i'); Serial.write("\r\n");

    Serial.write("    Value:  ");
    Serial.print(pin_value[0]);  Serial.write(" ");
    Serial.print(pin_value[1]);  Serial.write(" ");
    Serial.print(pin_value[2]);  Serial.write(" ");
    Serial.print(pin_value[3]);  Serial.write(" ");
    Serial.print(pin_value[4]);  Serial.write(" ");
    Serial.print(pin_value[5]);  Serial.write(" ");
    Serial.print(pin_value[6]);  Serial.write(" ");
    Serial.print(pin_value[7]);  Serial.write(" ");
    Serial.print(pin_value[8]);  Serial.write(" ");
    Serial.print(pin_value[9]);  Serial.write("  ");
    Serial.print(pin_value[10]); Serial.write("  ");
    Serial.print(pin_value[11]); Serial.write("  ");
    Serial.print(pin_value[12]); Serial.write("  ");
    Serial.print(pin_value[13]); Serial.write("\r\n");

    //
    // Analog pins.
    //
    Serial.write("     Pins: A0 1 2 3 4 5\r\n");
    Serial.write("Direction:  ");
    Serial.write(is_output_pin[14] ? 'o' : 'i'); Serial.write(" ");
    Serial.write(is_output_pin[15] ? 'o' : 'i'); Serial.write(" ");
    Serial.write(is_output_pin[16] ? 'o' : 'i'); Serial.write(" ");
    Serial.write(is_output_pin[17] ? 'o' : 'i'); Serial.write(" ");
    Serial.write(is_output_pin[18] ? 'o' : 'i'); Serial.write(" ");
    Serial.write(is_output_pin[19] ? 'o' : 'i'); Serial.write("\r\n");

    Serial.write("    Value:  ");
    Serial.print(pin_value[14]); Serial.write(" ");
    Serial.print(pin_value[15]); Serial.write(" ");
    Serial.print(pin_value[16]); Serial.write(" ");
    Serial.print(pin_value[17]); Serial.write(" ");
    Serial.print(pin_value[18]); Serial.write(" ");
    Serial.print(pin_value[19]); Serial.write("\r\n");

    //
    // Other pins.
    //
    Serial.write("     Pins: AREF PROG L TX RX SDI2 SCK2 SDO2 SDA SCL\r\n");
    Serial.write("Direction:   ");
    Serial.write(is_output_pin[20] ? 'o' : 'i'); Serial.write("    ");
    Serial.write(is_output_pin[21] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[22] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[23] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[24] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[25] ? 'o' : 'i'); Serial.write("    ");
    Serial.write(is_output_pin[26] ? 'o' : 'i'); Serial.write("    ");
    Serial.write(is_output_pin[27] ? 'o' : 'i'); Serial.write("    ");
    Serial.write(is_output_pin[28] ? 'o' : 'i'); Serial.write("   ");
    Serial.write(is_output_pin[29] ? 'o' : 'i'); Serial.write("\r\n");

    Serial.write("    Value:   ");
    Serial.print(pin_value[20]); Serial.write("    ");
    Serial.print(pin_value[21]); Serial.write("  ");
    Serial.print(pin_value[22]); Serial.write("  ");
    Serial.print(pin_value[23]); Serial.write("  ");
    Serial.print(pin_value[24]); Serial.write("  ");
    Serial.print(pin_value[25]); Serial.write("    ");
    Serial.print(pin_value[26]); Serial.write("    ");
    Serial.print(pin_value[27]); Serial.write("    ");
    Serial.print(pin_value[28]); Serial.write("   ");
    Serial.print(pin_value[29]); Serial.write("\r\n");
}

void menu_pin(int index)
{
    int pin = index;

again:
    show_pins();

    Serial.write("\r\n  1. Pin ");
    Serial.print(pin_name[index]);
    Serial.write(" direction: ");
    Serial.write(is_output_pin[index] ? "Output" : "Input");

    if (is_output_pin[index]) {
        Serial.write("\r\n  2. Pin ");
        Serial.print(pin_name[index]);
        Serial.write(" output: ");
        Serial.print(pin_value[index]);
    }
    Serial.write("\r\n\n");
    for (;;) {
        Serial.write("Command: ");
        int cmd = wait_input();
        Serial.write("\r\n");

        if (cmd == '\n' || cmd == '\r')
            break;

        if (cmd == '1') {
            if (is_output_pin[index]) {
                pinMode(pin, INPUT);
                is_output_pin[index] = false;
            } else {
                pinMode(pin, OUTPUT);
                is_output_pin[index] = true;
            }
            goto again;
        }
        if (cmd == '2') {
            if (is_output_pin[index]) {
                if (pin_value[index] == LOW) {
                    digitalWrite(pin, HIGH);
                    pin_value[index] = HIGH;
                } else {
                    digitalWrite(pin, LOW);
                    pin_value[index] = LOW;
                }
            }
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
    pinMode(22, OUTPUT); digitalWrite(22, LOW); is_output_pin[22] = 1;
    pinMode(23, OUTPUT); digitalWrite(23, LOW); is_output_pin[23] = 1;
    pinMode(24, OUTPUT); digitalWrite(24, LOW); is_output_pin[24] = 1;
    pinMode(26, OUTPUT); digitalWrite(26, LOW); is_output_pin[26] = 1;
    delay(10);

    Serial.write("\r\n-----------------\r\n");
    Serial.write("Testing GPIO pins\r\n");
}

void loop()
{
    show_pins();

    Serial.write("\r\n  0. Select pin 0    k. Select pin AREF");
    Serial.write("\r\n  1. Select pin 1    l. Select pin PROG");
    Serial.write("\r\n  2. Select pin 2    m. Select pin L LED");
    Serial.write("\r\n  3. Select pin 3    n. Select pin TX LED");
    Serial.write("\r\n  4. Select pin 4    o. Select pin RX LED");
    Serial.write("\r\n  5. Select pin 5    p. Select pin SDI2");
    Serial.write("\r\n  6. Select pin 6    q. Select pin SCK2");
    Serial.write("\r\n  7. Select pin 7    r. Select pin SDO2");
    Serial.write("\r\n  8. Select pin 8    s. Select pin SDA");
    Serial.write("\r\n  9. Select pin 9    t. Select pin SCL");
    Serial.write("\r\n  a. Select pin 10");
    Serial.write("\r\n  b. Select pin 11");
    Serial.write("\r\n  c. Select pin 12");
    Serial.write("\r\n  d. Select pin 13");
    Serial.write("\r\n  e. Select pin A0");
    Serial.write("\r\n  f. Select pin A1");
    Serial.write("\r\n  g. Select pin A2");
    Serial.write("\r\n  h. Select pin A3");
    Serial.write("\r\n  i. Select pin A4");
    Serial.write("\r\n  j. Select pin A5");
    Serial.write("\r\n\n");
    for (;;) {
        Serial.write("Command: ");
        int cmd = wait_input();
        Serial.write("\r\n");

        if (cmd == '\n' || cmd == '\r')
            break;

        if (cmd == '0') { menu_pin(0); break; }
        if (cmd == '1') { menu_pin(1); break; }
        if (cmd == '2') { menu_pin(2); break; }
        if (cmd == '3') { menu_pin(3); break; }
        if (cmd == '4') { menu_pin(4); break; }
        if (cmd == '5') { menu_pin(5); break; }
        if (cmd == '6') { menu_pin(6); break; }
        if (cmd == '7') { menu_pin(7); break; }
        if (cmd == '8') { menu_pin(8); break; }
        if (cmd == '9') { menu_pin(9); break; }
        if (cmd == 'a') { menu_pin(10); break; }
        if (cmd == 'b') { menu_pin(11); break; }
        if (cmd == 'c') { menu_pin(12); break; }
        if (cmd == 'd') { menu_pin(13); break; }
        if (cmd == 'e') { menu_pin(14); break; }
        if (cmd == 'f') { menu_pin(15); break; }
        if (cmd == 'g') { menu_pin(16); break; }
        if (cmd == 'h') { menu_pin(17); break; }
        if (cmd == 'i') { menu_pin(18); break; }
        if (cmd == 'j') { menu_pin(19); break; }
        if (cmd == 'k') { menu_pin(20); break; }
        if (cmd == 'l') { menu_pin(21); break; }
        if (cmd == 'm') { menu_pin(22); break; }
        if (cmd == 'n') { menu_pin(23); break; }
        if (cmd == 'o') { menu_pin(24); break; }
        if (cmd == 'p') { menu_pin(25); break; }
        if (cmd == 'q') { menu_pin(26); break; }
        if (cmd == 'r') { menu_pin(27); break; }
        if (cmd == 's') { menu_pin(28); break; }
        if (cmd == 't') { menu_pin(29); break; }
    }
}
