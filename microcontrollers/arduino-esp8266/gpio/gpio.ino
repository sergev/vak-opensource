/*
 * Test GPIO pins using a text menu at console port.
 *
 * NodeMCU board has following GPIO pins:
 *      Pin D0 or LED_BUILTIN
 *      Pin D1 or SCL
 *      Pin D2 or SDA
 *      Pin D3
 *      Pin D4
 *      Pin D5 or SCK
 *      Pin D6 or MISO
 *      Pin D7 or MOSI
 *      Pin D8 or SS
 *      Pin D9  - serial receive
 *      Pin D10 - serial transmit
 *
 * Analog input pin:
 *      Pin A0
 *
 * For details, see:
 * https://github.com/esp8266/Arduino/blob/2.3.0/variants/nodemcu/pins_arduino.h
 */
static const int index_to_pin[] = { D0, D1, D2, D3, D4, D5, D6, D7, D8 };

static int is_output_pin[9];

static int output_value[9];

int wait_input()
{
    for (;;) {
        int c = Serial.read();
        if (c >= 0)
            return c;
        ESP.wdtFeed();
    }
}

void show_pins()
{
    Serial.write('\n');
    Serial.write("Pins:     D0 D1 D2 D3 D4 D5 D6 D7 D8\r\n");

    Serial.write("Direction: ");
    Serial.write(is_output_pin[0] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[1] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[2] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[3] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[4] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[5] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[6] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[7] ? 'o' : 'i'); Serial.write("  ");
    Serial.write(is_output_pin[8] ? 'o' : 'i'); Serial.write("\r\n");

    Serial.write("Value:     ");
    Serial.print(output_value[0]); Serial.write("  ");
    Serial.print(output_value[1]); Serial.write("  ");
    Serial.print(output_value[2]); Serial.write("  ");
    Serial.print(output_value[3]); Serial.write("  ");
    Serial.print(output_value[4]); Serial.write("  ");
    Serial.print(output_value[5]); Serial.write("  ");
    Serial.print(output_value[6]); Serial.write("  ");
    Serial.print(output_value[7]); Serial.write("  ");
    Serial.print(output_value[8]); Serial.write("\r\n");
}

void menu_pin(int index)
{
    int pin = index_to_pin[index];

again:
    show_pins();

    Serial.write("\r\n  1. Pin D");
    Serial.print(index);
    Serial.write(" direction: ");
    Serial.write(is_output_pin[index] ? "Output" : "Input");

    if (is_output_pin[index]) {
        Serial.write("\r\n  2. Pin D");
        Serial.print(index);
        Serial.write(" output: ");
        Serial.print(output_value[index]);
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
                if (output_value[index] == LOW) {
                    digitalWrite(pin, HIGH);
                    output_value[index] = HIGH;
                } else {
                    digitalWrite(pin, LOW);
                    output_value[index] = LOW;
                }
            }
            goto again;
        }
    }
}

void setup()
{
    Serial.begin(9600);
    pinMode(D0, INPUT);
    pinMode(D1, INPUT);
    pinMode(D2, INPUT);
    pinMode(D3, INPUT);
    pinMode(D4, INPUT);
    pinMode(D5, INPUT);
    pinMode(D6, INPUT);
    pinMode(D7, INPUT);
    pinMode(D8, INPUT);
    Serial.write("\r\nTesting GPIO pins\r\n\r\n");
}

void loop()
{
    show_pins();

    Serial.write("\r\n  0. Select pin 0");
    Serial.write("\r\n  1. Select pin 1");
    Serial.write("\r\n  2. Select pin 2");
    Serial.write("\r\n  3. Select pin 3");
    Serial.write("\r\n  4. Select pin 4");
    Serial.write("\r\n  5. Select pin 5");
    Serial.write("\r\n  6. Select pin 6");
    Serial.write("\r\n  7. Select pin 7");
    Serial.write("\r\n  8. Select pin 8");
    Serial.write("\r\n\n");
    for (;;) {
        Serial.write("Command: ");
        int cmd = wait_input();
        Serial.write("\r\n");

        if (cmd == '\n' || cmd == '\r')
            break;

        if (cmd == '0') {
            menu_pin(0);
            break;
        }
        if (cmd == '1') {
            menu_pin(1);
            break;
        }
        if (cmd == '2') {
            menu_pin(2);
            break;
        }
        if (cmd == '3') {
            menu_pin(3);
            break;
        }
        if (cmd == '4') {
            menu_pin(4);
            break;
        }
        if (cmd == '5') {
            menu_pin(5);
            break;
        }
        if (cmd == '6') {
            menu_pin(6);
            break;
        }
        if (cmd == '7') {
            menu_pin(7);
            break;
        }
        if (cmd == '8') {
            menu_pin(8);
            break;
        }
    }
}
