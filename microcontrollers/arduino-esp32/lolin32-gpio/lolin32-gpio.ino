/*
 * Test GPIO pins using a text menu at console port.
 */
static const int index_to_pin[] = {
    0,  2,  4,  5,  12, 13, 14, 15, 16, 17,
    18, 19, 22, 23, 25, 26, 27, 32, 33, 34,
    35,
};

static int is_output_pin[21];

static int pin_value[21];

int wait_input()
{
    for (;;) {
        int c = Serial.read();
        if (c >= 0)
            return c;
        //ESP.wdtFeed();
    }
}

void show_pins()
{
    // The chip ID is essentially its MAC address(length: 6 bytes).
    uint64_t chipid = ESP.getEfuseMac();
    int i;

    // Update input values.
    for (i=0; i<21; i++)
        if (! is_output_pin[0])
            pin_value[i] = digitalRead(index_to_pin[i]);

    Serial.write('\n');
    Serial.printf("  Chip id: %04X%08X\r\n", (uint16_t)(chipid >> 32), (uint32_t)chipid);
    Serial.write("     Pins:  0  2  4  5 12 13 14 15 16 17\r\n");

    Serial.write("Direction:  ");
    Serial.write(is_output_pin[0] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[1] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[2] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[3] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[4] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[5] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[6] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[7] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[8] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[9] ? 'o' : 'i'); Serial.write("\r\n");

    Serial.write("    Value:  ");
    Serial.print(pin_value[0]); Serial.write("  ");
    Serial.print(pin_value[1]); Serial.write("  ");
    Serial.print(pin_value[2]); Serial.write("  ");
    Serial.print(pin_value[3]); Serial.write("  ");
    Serial.print(pin_value[4]); Serial.write("  ");
    Serial.print(pin_value[5]); Serial.write("  ");
    Serial.print(pin_value[6]); Serial.write("  ");
    Serial.print(pin_value[7]); Serial.write("  ");
    Serial.print(pin_value[8]); Serial.write("  ");
    Serial.print(pin_value[9]); Serial.write("\r\n");

    Serial.write("     Pins: 18 19 22 23 25 26 27 32 33 34 35\r\n");

    Serial.write("Direction:  ");
    Serial.write(is_output_pin[10] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[11] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[12] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[13] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[14] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[15] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[16] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[17] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[18] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[19] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[20] ? 'o' : 'i'); Serial.write("\r\n");

    Serial.write("    Value:  ");
    Serial.print(pin_value[10]); Serial.write("  ");
    Serial.print(pin_value[11]); Serial.write("  ");
    Serial.print(pin_value[12]); Serial.write("  ");
    Serial.print(pin_value[13]); Serial.write("  ");
    Serial.print(pin_value[14]); Serial.write("  ");
    Serial.print(pin_value[15]); Serial.write("  ");
    Serial.print(pin_value[16]); Serial.write("  ");
    Serial.print(pin_value[17]); Serial.write("  ");
    Serial.print(pin_value[18]); Serial.write("  ");
    Serial.print(pin_value[19]); Serial.write("  ");
    Serial.print(pin_value[20]); Serial.write("\r\n");
}

void menu_pin(int index)
{
    int pin = index_to_pin[index];

again:
    show_pins();

    Serial.write("\r\n  1. Pin ");
    Serial.print(index);
    Serial.write(" direction: ");
    Serial.write(is_output_pin[index] ? "Output" : "Input");

    if (is_output_pin[index]) {
        Serial.write("\r\n  2. Pin ");
        Serial.print(index);
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
    for (i=0; i<21; i++)
        pinMode(index_to_pin[i], INPUT);
    delay(10);
    Serial.write("\r\n-----------------\r\n");
    Serial.write("Testing GPIO pins\r\n");
}

void loop()
{
    show_pins();

    Serial.write("\r\n  0. Select pin "); Serial.print(index_to_pin[0]);
    Serial.write("\r\n  1. Select pin "); Serial.print(index_to_pin[1]);
    Serial.write("\r\n  2. Select pin "); Serial.print(index_to_pin[2]);
    Serial.write("\r\n  3. Select pin "); Serial.print(index_to_pin[3]);
    Serial.write("\r\n  4. Select pin "); Serial.print(index_to_pin[4]);
    Serial.write("\r\n  5. Select pin "); Serial.print(index_to_pin[5]);
    Serial.write("\r\n  6. Select pin "); Serial.print(index_to_pin[6]);
    Serial.write("\r\n  7. Select pin "); Serial.print(index_to_pin[7]);
    Serial.write("\r\n  8. Select pin "); Serial.print(index_to_pin[8]);
    Serial.write("\r\n  9. Select pin "); Serial.print(index_to_pin[9]);
    Serial.write("\r\n  a. Select pin "); Serial.print(index_to_pin[10]);
    Serial.write("\r\n  b. Select pin "); Serial.print(index_to_pin[11]);
    Serial.write("\r\n  c. Select pin "); Serial.print(index_to_pin[12]);
    Serial.write("\r\n  d. Select pin "); Serial.print(index_to_pin[13]);
    Serial.write("\r\n  e. Select pin "); Serial.print(index_to_pin[14]);
    Serial.write("\r\n  f. Select pin "); Serial.print(index_to_pin[15]);
    Serial.write("\r\n  g. Select pin "); Serial.print(index_to_pin[16]);
    Serial.write("\r\n  h. Select pin "); Serial.print(index_to_pin[17]);
    Serial.write("\r\n  i. Select pin "); Serial.print(index_to_pin[18]);
    Serial.write("\r\n  j. Select pin "); Serial.print(index_to_pin[19]);
    Serial.write("\r\n  k. Select pin "); Serial.print(index_to_pin[20]);
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
    }
}
