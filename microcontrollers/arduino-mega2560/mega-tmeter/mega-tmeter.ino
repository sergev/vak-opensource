/*
 * Transistor Meter.
 */
#include <CRC32.h>

#define VERSION     "1.0." GITCOUNT

unsigned gate_output = 0;       // control voltage to the Gate pin of transistor
unsigned gate_input;            // measured voltage of the Gate pin
unsigned drain_input;           // measured current of the Drain pin

unsigned gate_mv;               // millivolts at the Gate pin
unsigned long drain_uamp;       // microamperes at the Drain pin

float result_Vg[256];           // gate voltage, V
float result_Id[256];           // drain current, mA
int nresults;

//
// Use Timer3, pins 2, 3 and 5 on Mega 2560 board.
//
static const int gate_output_pin = 2;

//
// Input data buffer for serial packet.
//
#define PKT_INBUFSZ     128
uint8_t pkt_inbuf[PKT_INBUFSZ];
uint8_t *pkt_inptr = pkt_inbuf;

//
// Output data buffer for serial packet.
//
#define PKT_OUTBUFSZ    4096
char pkt_outbuf[PKT_OUTBUFSZ];
char *pkt_outptr = pkt_outbuf;

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

void measure_jfet()
{
    int i;

    digitalWrite(LED_BUILTIN, HIGH);
    nresults = 0;
    for (i=0; i<256; i++) {
        set_gate(255 - i);
        measure_gate();
        measure_drain();
        result_Vg[i] = ((int)gate_mv - 5000) / 1000.0;
        result_Id[i] = drain_uamp / 1000.0;
        nresults++;
#if 0
        Serial1.write(" ");
        Serial1.print(result_Vg[i], 3);
        Serial1.write("  ");
        Serial1.print(result_Id[i], 3);
        Serial1.write("\r\n");
#endif
        // Stop on zero.
        if (drain_input == 0)
            break;
    }
    set_gate(0);
    digitalWrite(LED_BUILTIN, LOW);
}

//
// Send a packet from pkt_ouput[] buffer.
//
void pkt_send()
{
#if 0
    Serial1.write("\r\nSend: ");
    Serial1.print(pkt_outptr - pkt_outbuf);
    Serial1.write(" '");
    Serial1.write(pkt_outbuf, pkt_outptr - pkt_outbuf);
    Serial1.write("'\r\n");
#endif
    // Check transmit size.
    if (pkt_outptr <= pkt_outbuf || pkt_outptr > pkt_outbuf+PKT_OUTBUFSZ-8)
        return;

    // Append checksum.
    int nbytes = pkt_outptr - pkt_outbuf;
    uint32_t checksum = CRC32::calculate(pkt_outbuf, nbytes);
    *pkt_outptr++ = 'a' + (checksum >> 28);
    *pkt_outptr++ = 'a' + ((checksum >> 24) & 0xf);
    *pkt_outptr++ = 'a' + ((checksum >> 20) & 0xf);
    *pkt_outptr++ = 'a' + ((checksum >> 16) & 0xf);
    *pkt_outptr++ = 'a' + ((checksum >> 12) & 0xf);
    *pkt_outptr++ = 'a' + ((checksum >> 8) & 0xf);
    *pkt_outptr++ = 'a' + ((checksum >> 4) & 0xf);
    *pkt_outptr++ = 'a' + (checksum & 0xf);
    *pkt_outptr++ = '\r';
    *pkt_outptr = 0;

    // Send the packet.
    Serial.write(pkt_outbuf);
    pkt_outptr = pkt_outbuf;
}

//
// Send generic string packet.
//
void send_string(const char *name, const char *value)
{
    // Build JSON formatted stream like: {"name":"value"}
    pkt_outptr = pkt_outbuf;
    strcpy(pkt_outptr, "{\"");
    strcpy(pkt_outptr += strlen(pkt_outptr), name);
    strcpy(pkt_outptr += strlen(pkt_outptr), "\":\"");
    strcpy(pkt_outptr += strlen(pkt_outptr), value);
    strcpy(pkt_outptr += strlen(pkt_outptr), "\"}");
    pkt_outptr += strlen(pkt_outptr);
    pkt_send();
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

//
// Send a Measurement Results packet.
//
void send_measurement()
{
    char buffer[64];
#if 0
    Serial1.write("Send measurement: ");
    Serial1.print(nresults);
    Serial1.write(" results\r\n");
#endif
    // Build JSON formatted stream like:
    // {"Vgate":[a,b,...],"Idrain":[x,y,...]}
    pkt_outptr = pkt_outbuf;
    strcpy(pkt_outptr, "{\"Vgate\":[");
    for (int i = 0; i < nresults; i++) {
        if (i > 0)
            strcpy(pkt_outptr += strlen(pkt_outptr), ",");
        strcpy(pkt_outptr += strlen(pkt_outptr), dtostrf(result_Vg[i], 0, 3, buffer));
    }
    strcpy(pkt_outptr += strlen(pkt_outptr), "],\"Idrain\":[");
    for (int i = 0; i < nresults; i++) {
        if (i > 0)
            strcpy(pkt_outptr += strlen(pkt_outptr), ",");
        strcpy(pkt_outptr += strlen(pkt_outptr), dtostrf(result_Id[i], 0, 3, buffer));
    }
    strcpy(pkt_outptr += strlen(pkt_outptr), "]}");
    pkt_outptr += strlen(pkt_outptr);
#if 0
    Serial1.write("\r\nSend: ");
    Serial1.print(pkt_outptr - pkt_outbuf);
    Serial1.write(" '");
    Serial1.write(pkt_outbuf);
    Serial1.write("'\r\n");
#endif
    pkt_send();
}

//
// This function is invoked when a new packet is received.
//
void pkt_receive()
{
    unsigned nbytes = pkt_inptr - pkt_inbuf;
#if 0
    Serial1.write("Received packet: ");
    Serial1.print(nbytes);
    Serial1.write(" '");
    Serial1.write(pkt_inbuf, nbytes);
    Serial1.write("'\r\n");
#endif
    if (nbytes <= 8) {
        // Ignore short packets.
        return;
    }

    // Check the sum.
    uint32_t checksum = CRC32::calculate(pkt_inbuf, nbytes-8);
#if 0
    Serial1.write("Checksum: ");
    Serial1.print(checksum, HEX);
    Serial1.write("\r\n");
#endif
    if (pkt_inbuf[nbytes-8] != 'a' + (checksum >> 28) ||
        pkt_inbuf[nbytes-7] != 'a' + ((checksum >> 24) & 0xf) ||
        pkt_inbuf[nbytes-6] != 'a' + ((checksum >> 20) & 0xf) ||
        pkt_inbuf[nbytes-5] != 'a' + ((checksum >> 16) & 0xf) ||
        pkt_inbuf[nbytes-4] != 'a' + ((checksum >> 12) & 0xf) ||
        pkt_inbuf[nbytes-3] != 'a' + ((checksum >> 8) & 0xf) ||
        pkt_inbuf[nbytes-2] != 'a' + ((checksum >> 4) & 0xf) ||
        pkt_inbuf[nbytes-1] != 'a' + ((checksum) & 0xf)) {
        // Incorrect checksum, ignore the packet.
#if 0
        Serial1.write("Bad checksum!\r\n");
#endif
        return;
    }
    nbytes -= 8;

    char *command = (char *) pkt_inbuf;
    command[nbytes] = 0;

    if (strcmp(command, "version") == 0) {
        send_version();
    }
    else if (strcmp(command, "njfet") == 0) {
        measure_jfet();
        send_measurement();
    }
    else {
        send_error("Unknown command");
    }
}

void pkt_update()
{
    while (Serial.available() > 0) {
        uint8_t data = Serial.read();

        if (data == '\r') {
            pkt_receive();
            pkt_inptr = pkt_inbuf;
        } else {
            if (pkt_inptr < &pkt_inbuf[PKT_INBUFSZ-1]) {
                *pkt_inptr++ = data;
            } else {
                // Error, buffer overflow if we write.
            }
        }
    }
}

/*
 * Read a character from the serial port.
 */
int wait_input()
{
    for (;;) {
        int c = Serial1.read();

        if (c >= 0)
            return c;

        // Let packet protocol to do it's job.
        pkt_update();
    }
}

void print_results()
{
    int i;

    Serial1.write("\r\nMeasuring N-JFET:\r\n\r\n");
    Serial1.write(" Vg, V   Id, mA\r\n");
    Serial1.write("---------------\r\n");
    for (i=0; i<nresults; i++) {
        Serial1.write(" ");
        Serial1.print(result_Vg[i], 3);
        Serial1.write("  ");
        Serial1.print(result_Id[i], 3);
        Serial1.write("\r\n");
    }
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
            print_results();
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
    // Main USB serial port: initialize packet protocol.
    Serial.begin(38400);

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
