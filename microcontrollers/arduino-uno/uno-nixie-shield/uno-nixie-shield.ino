/*
 * Demo of Nixie Shield simple counter.
 *
 * Nixie Shield uses five digital outputs to drive nixie tube.
 * Pin A0 as on/off line, 13,12,11,10 as an address of nixie tube cathode.
 *
 * This example code is in the public domain.
 *
 * http://www.nixietester.com
 */
const int EN = A0;                  // A0 pin is attached to on/off line
const int A = 13;                   // 13,12,11,10 pins are attached to address lines
const int B = 12;
const int C = 11;
const int D = 10;

//
// Turn the tube on/off.
//
void nixietubePower(int enable)
{
    digitalWrite(EN, !enable);
}

//
// Display the given digit.
//
void nixietube(int digit)
{
    digitalWrite(A, digit & 1);
    digitalWrite(B, (digit >> 1) & 1);
    digitalWrite(C, (digit >> 2) & 1);
    digitalWrite(D, (digit >> 3) & 1);
}

void setup()
{
    // Configure pins as outputs.
    pinMode(EN, OUTPUT);
    pinMode(A, OUTPUT);
    pinMode(B, OUTPUT);
    pinMode(C, OUTPUT);
    pinMode(D, OUTPUT);

    // Turn on the tube.
    nixietubePower(1);
}

void loop()
{
    // Display digits 0 to 9.
    for (int i = 0; i < 10; i++) {
        nixietube(i);
        delay(333);
    }
}
