/*
 * Transistor Meter.
 */
#include <ArduinoJson.h>
#include <PacketSerial.h>
#include <CRC32.h>

#define VERSION     "1.0." GITCOUNT

unsigned gate_output = 0;       // control voltage to the Gate pin of transistor
unsigned gate_input;            // measured voltage of the Gate pin
unsigned drain_input;           // measured current of the Drain pin

unsigned gate_mv;               // millivolts at the Gate pin
unsigned long drain_uamp;       // microamperes at the Drain pin

//
// Use Timer3, pins 2, 3 and 5 on Mega 2560 board.
//
static const int gate_output_pin = 2;

//
// Use PacketSerial byte stuffing protocol on Serial port.
// Configure 128 byte receive buffer size.
// Packet marker is CR.
//
PacketSerial_<COBS, '\r', 128> cobs;

/*
 * Read a character from the serial port.
 */
int wait_input()
{
    for (;;) {
        int c = Serial1.read();

        if (c >= 0)
            return c;

        // Let PacketSerial protocol to do it's job.
        cobs.update();
    }
}

#if 0
/*
 * Print the current state of the device.
 */
void print_binary(const char *str, int v)
{
    Serial1.write(str);
    Serial1.print((v >> 7) & 1);
    Serial1.print((v >> 6) & 1);
    Serial1.print((v >> 5) & 1);
    Serial1.print((v >> 4) & 1);
    Serial1.print((v >> 3) & 1);
    Serial1.print((v >> 2) & 1);
    Serial1.print((v >> 1) & 1);
    Serial1.print(v & 1);
}
#endif

/*
 * Print volts.
 */
void print_mv(int mv)
{
    Serial1.print(mv / 1000.0, 2);
    Serial1.write("V");
}

/*
 * Print milliamperes.
 */
void print_uamp(int uamp)
{
    Serial1.print(uamp / 1000.0, 2);
    Serial1.write("mA");
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
    Serial1.write("\r\n      Version: ");
    Serial1.write(VERSION);

    Serial1.write("\r\n  Gate output: ");
    Serial1.print(gate_output);

    Serial1.write("\r\n   Gate input: ");
    print_mv(gate_mv);
    Serial1.write(" (");
    Serial1.print(gate_input);
    Serial1.write(")");

    Serial1.write("\r\nDrain current: ");
    print_uamp(drain_uamp);
    Serial1.write(" (");
    Serial1.print(drain_input);
    Serial1.write(")");

    Serial1.write("\r\n");
}

void set_gate(unsigned val)
{
    gate_output = val;
    analogWrite(gate_output_pin, gate_output);
    delay(10);
}

//
// Output data buffer for COBS protocol.
//
#define COBS_BUFSZ 4096
char cobs_outbuf[COBS_BUFSZ];
char *cobs_outptr = cobs_outbuf;

//
// Send a PacketSerial packet from cobs_ouput[] buffer.
//
void cobs_send()
{
    // Check transmit size.
    if (cobs_outptr <= cobs_outbuf || cobs_outptr > cobs_outbuf+COBS_BUFSZ-8)
        return;

    // Append checksum.
    int nbytes = cobs_outptr - cobs_outbuf;
    uint32_t checksum = CRC32::calculate(cobs_outbuf, nbytes);
    *cobs_outptr++ = 'a' + (checksum >> 28);
    *cobs_outptr++ = 'a' + ((checksum >> 24) & 0xf);
    *cobs_outptr++ = 'a' + ((checksum >> 20) & 0xf);
    *cobs_outptr++ = 'a' + ((checksum >> 16) & 0xf);
    *cobs_outptr++ = 'a' + ((checksum >> 12) & 0xf);
    *cobs_outptr++ = 'a' + ((checksum >> 8) & 0xf);
    *cobs_outptr++ = 'a' + ((checksum >> 4) & 0xf);
    *cobs_outptr++ = 'a' + (checksum & 0xf);

    // Send the packet.
    cobs.send((const uint8_t*)cobs_outbuf, nbytes + 8);
    cobs_outptr = cobs_outbuf;
}

//
// Send generic string packet.
//
void send_string(const char *name, const char *value)
{
    StaticJsonBuffer<64> jsonBuffer;

    JsonObject& root = jsonBuffer.createObject();
    root[name] = value;

    cobs_outptr = cobs_outbuf;
    cobs_outptr += root.printTo(cobs_outptr, COBS_BUFSZ);
    cobs_send();
}

//
// Send Version packet.
//
void send_version()
{
    send_string("Version", VERSION);
}

//
// Send Error packet.
//
void send_error(const char *message)
{
    send_string("Error", message);
}

#if 0
//
// Send a Measurement Results packet.
//
void send_measurement()
{
    StaticJsonBuffer<4000> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();

    // Create the "analog" array
    JsonArray& Vg = root.createNestedArray("Vgate");
    for (int i = 0; i < N; i++) {
        Vg.add(value[i]);
    }

    // Create the "digital" array
    JsonArray& Id = root.createNestedArray("Idrain");
    for (int i = 0; i < N; i++) {
        Id.add(value[i]);
    }

    cobs_outptr = cobs_outbuf;
    cobs_outptr += root.printTo(cobs_outptr, COBS_BUFSZ);
    cobs_send();
}
#endif

//
// PacketSerial protocol calls this function when a new packet is received.
//
void cobs_receive(const uint8_t *data, size_t nbytes)
{
#if 1
    Serial1.write("Received packet: ");
    Serial1.print(nbytes);
    Serial1.write(" '");
    Serial1.write(data, nbytes);
    Serial1.write("'\r\n");
#endif
    if (nbytes <= 8) {
        // Ignore short packets.
        return;
    }

    // Check the sum.
    uint32_t checksum = CRC32::calculate(data, nbytes-8);
    if (data[nbytes] != 'a' + (checksum >> 28) ||
        data[nbytes+1] != 'a' + ((checksum >> 24) & 0xf) ||
        data[nbytes+2] != 'a' + ((checksum >> 20) & 0xf) ||
        data[nbytes+3] != 'a' + ((checksum >> 16) & 0xf) ||
        data[nbytes+4] != 'a' + ((checksum >> 12) & 0xf) ||
        data[nbytes+5] != 'a' + ((checksum >> 8) & 0xf) ||
        data[nbytes+6] != 'a' + ((checksum >> 4) & 0xf) ||
        data[nbytes+7] != 'a' + ((checksum) & 0xf)) {
        // Incorrect checksum, ignore the packet.
        return;
    }
    nbytes -= 8;

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(data, nbytes);

    const char *command = root["Command"];
#if 1
    Serial1.write("Command: ");
    Serial1.write(command ? command : "(null)");
    Serial1.write("'\r\n");
#endif

    if (strcmp(command, "version") == 0) {
        send_version();
    }
    else if (strcmp(command, "njfet") == 0) {
        //TODO
        //send_n_jfet();
        send_error("N-JFET not implemented yet");
    }
    else {
        send_error("Unknown command");
    }
}

void measure_jfet()
{
    int i;
    //unsigned v1 = 0, v2 = 0;

    Serial1.write("\r\nMeasuring N-JFET:\r\n\r\n");
    Serial1.write(" Vg, V   Id, mA\r\n");
    Serial1.write("---------------\r\n");
    digitalWrite(LED_BUILTIN, HIGH);
    for (i=255; i>=0; i--) {
        set_gate(i);
        measure_gate();
        measure_drain();

        // Print results.
        Serial1.write(" ");
        Serial1.print(((int)gate_mv - 5000) / 1000.0, 3);
        Serial1.write("  ");
        Serial1.print(drain_uamp / 1000.0, 3);
        Serial1.write("\r\n");

        // Stop on zero.
        if (drain_input == 0)
            break;
    }
    set_gate(0);
    digitalWrite(LED_BUILTIN, LOW);
    Serial1.write("Done\r\n");
}

/*
 * Top level menu.
 */
void menu_top()
{
again:
    show_state();

    Serial1.write("\r\n  M. Measure JFET");
    Serial1.write("\r\n  V. Send Version Packet");
    Serial1.write("\r\n  0. Set Gate = 0V (-5V)");
    Serial1.write("\r\n  1. Set Gate = 1V (-4V)");
    Serial1.write("\r\n  2. Set Gate = 2V (-3V)");
    Serial1.write("\r\n  3. Set Gate = 3V (-2V)");
    Serial1.write("\r\n  4. Set Gate = 4V (-1V)");
    Serial1.write("\r\n  5. Set Gate = 5V (0V)");
    Serial1.write("\r\n\n");
    for (;;) {
        Serial1.write("Command: ");
        int cmd = wait_input();
        Serial1.write("\r\n");

        if (cmd == '\n' || cmd == '\r')
            break;

        if (cmd == 'm' || cmd == 'M') {
            measure_jfet();
            goto again;
        }
        if (cmd == 'v' || cmd == 'v') {
            send_version();
            goto again;
        }
        if (cmd == '0') {
            set_gate(0);
            goto again;
        }
        if (cmd == '1') {
            set_gate(256 - 4.0 * 255/5);
            goto again;
        }
        if (cmd == '2') {
            set_gate(256 - 3.0 * 255/5);
            goto again;
        }
        if (cmd == '3') {
            set_gate(256 - 2.0 * 255/5);
            goto again;
        }
        if (cmd == '4') {
            set_gate(256 - 1.0 * 255/5);
            goto again;
        }
        if (cmd == '5') {
            set_gate(255);
            goto again;
        }
    }
}

void setup()
{
    // Main USB serial port: initialize PacketSerial protocol.
    cobs.begin(38400);
    cobs.setPacketHandler(&cobs_receive);

    // Secondary serial port: provide a text menu interface.
    Serial1.begin(9600);

    // Configure pins.
    // D2 - gate output
    // A0 - gate input
    // A1 - drain current input via ZXCT1009 current sense monitor
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(gate_output_pin, OUTPUT);
    pinMode(A0, INPUT);
    pinMode(A1, INPUT);

    // Configure timer 3 prescale factor as 1:1.
    // Set bits CS32,CS31,CS30 to 0b001.
    TCCR3B = (TCCR3B & ~7) | 1;

    delay(10);
    Serial1.write("\r\n----------------\r\n");
    Serial1.write("Transistor Meter\r\n");
}

void loop()
{
    menu_top();
}
