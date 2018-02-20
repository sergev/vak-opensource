/*
 * Transistor Meter.
 *
 * Copyright (C) 2018 Serge Vakulenko
 *
 * This file is distributed under the terms of the GNU General Public License (GPL).
 * See the accompanying file "COPYING" for more details.
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

//
// Measure analog voltage.
// Take three samples and return a sum.
//
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

//
// Measure gate voltage.
//
void measure_gate()
{
    gate_input = measure(A0);
    gate_mv = gate_input * (5000.0 / 1024 / 3);
}

//
// Measure drain voltage.
//
void measure_drain()
{
    drain_input = measure(A1);

    // ZXCT1009 current sense monitor is connected via
    // 51 Ohm resistor as a drain shunt, and 220 Ohm resistor as a load.
    drain_uamp = drain_input * ((5000.0 / 1024 / 3) * (100000.0 / 220 / 51));
}

//
// Set gate voltage.
//
void set_gate(unsigned val)
{
    gate_output = val;
    analogWrite(gate_output_pin, gate_output);
    delay(10);
}

//
// Measure JFET transistor.
//
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
// Send a Measurement Results packet.
//
void send_measurement()
{
    char buffer[64];

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

    pkt_send();
}

//
// This function is invoked when a new packet is received.
//
void pkt_receive()
{
    unsigned nbytes = pkt_inptr - pkt_inbuf;

    if (nbytes <= 8) {
        // Ignore short packets.
        return;
    }

    // Check the sum.
    uint32_t checksum = CRC32::calculate(pkt_inbuf, nbytes-8);
    if (pkt_inbuf[nbytes-8] != 'a' + (checksum >> 28) ||
        pkt_inbuf[nbytes-7] != 'a' + ((checksum >> 24) & 0xf) ||
        pkt_inbuf[nbytes-6] != 'a' + ((checksum >> 20) & 0xf) ||
        pkt_inbuf[nbytes-5] != 'a' + ((checksum >> 16) & 0xf) ||
        pkt_inbuf[nbytes-4] != 'a' + ((checksum >> 12) & 0xf) ||
        pkt_inbuf[nbytes-3] != 'a' + ((checksum >> 8) & 0xf) ||
        pkt_inbuf[nbytes-2] != 'a' + ((checksum >> 4) & 0xf) ||
        pkt_inbuf[nbytes-1] != 'a' + ((checksum) & 0xf)) {
        // Incorrect checksum, ignore the packet.
        return;
    }
    nbytes -= 8;

    char *command = (char *) pkt_inbuf;
    command[nbytes] = 0;

    if (strcmp(command, "version") == 0) {
        // Send Version packet.
        send_string("Version", VERSION);
    }
    else if (strcmp(command, "njfet") == 0) {
        measure_jfet();
        send_measurement();
    }
    else {
        // Send Error packet.
        send_string("Error", "Unknown command");
    }
}

//
// Receive data from the serial port if any.
// Process the received packet.
//
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

void setup()
{
    // Main USB serial port: initialize packet protocol.
    Serial.begin(38400);

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
}

void loop()
{
    // Let packet protocol to do it's job.
    pkt_update();
}
