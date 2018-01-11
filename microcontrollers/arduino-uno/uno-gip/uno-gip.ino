/*
 * Generate bipolar output 400kHz.
 */

//
// Rate for generator.
//
#define RATE_DEFAULT    19
#define RATE_MIN        (RATE_DEFAULT - 8)
#define RATE_MAX        (RATE_DEFAULT + 8)

int divisor;

#define NOP5 "nop\nnop\nnop\nnop\nnop\n"

#define NOP9()  asm volatile(NOP5 "nop\nnop\nnop\nnop\n")
#define NOP10() asm volatile(NOP5 NOP5 )
#define NOP11() asm volatile(NOP5 NOP5 "nop\n")
#define NOP12() asm volatile(NOP5 NOP5 "nop\nnop\n")
#define NOP13() asm volatile(NOP5 NOP5 "nop\nnop\nnop\n")
#define NOP14() asm volatile(NOP5 NOP5 "nop\nnop\nnop\nnop\n")
#define NOP15() asm volatile(NOP5 NOP5 NOP5)
#define NOP16() asm volatile(NOP5 NOP5 NOP5 "nop\n")
#define NOP17() asm volatile(NOP5 NOP5 NOP5 "nop\nnop\n")
#define NOP18() asm volatile(NOP5 NOP5 NOP5 "nop\nnop\nnop\n")
#define NOP19() asm volatile(NOP5 NOP5 NOP5 "nop\nnop\nnop\nnop\n")
#define NOP20() asm volatile(NOP5 NOP5 NOP5 NOP5)
#define NOP21() asm volatile(NOP5 NOP5 NOP5 NOP5 "nop\n")
#define NOP22() asm volatile(NOP5 NOP5 NOP5 NOP5 "nop\nnop\n")
#define NOP23() asm volatile(NOP5 NOP5 NOP5 NOP5 "nop\nnop\nnop\n")
#define NOP24() asm volatile(NOP5 NOP5 NOP5 NOP5 "nop\nnop\nnop\nnop\n")
#define NOP25() asm volatile(NOP5 NOP5 NOP5 NOP5 NOP5)
#define NOP26() asm volatile(NOP5 NOP5 NOP5 NOP5 NOP5 "nop\n")
#define NOP27() asm volatile(NOP5 NOP5 NOP5 NOP5 NOP5 "nop\nnop\n")

#define LOOP(delay1, delay2) \
    for(;;) { \
        PORTD = phase1; \
        NOP##delay1(); \
        PORTD = phase2; \
        NOP##delay2(); \
    }

int khz(int div)
{
    return 16000 / (1 + div) / 2;
}

void generate(int div)
{
    int phase1 = PORTD | B00000100;
    int phase2 = PORTD | B00001000;

    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);

    Serial.print("Generate ");
    Serial.print(khz(divisor));
    Serial.print(" kHz...\r\n");

    switch (divisor) {
    case 11: LOOP(11, 9);
    case 12: LOOP(12, 10);
    case 13: LOOP(13, 11);
    case 14: LOOP(14, 12);
    case 15: LOOP(15, 13);
    case 16: LOOP(16, 14);
    case 17: LOOP(17, 15);
    case 18: LOOP(18, 16);
    case 19: LOOP(19, 17);
    case 20: LOOP(20, 18);
    case 21: LOOP(21, 19);
    case 22: LOOP(22, 20);
    case 23: LOOP(23, 21);
    case 24: LOOP(24, 22);
    case 25: LOOP(25, 23);
    case 26: LOOP(26, 24);
    case 27: LOOP(27, 25);
    }
}

void idle()
{
    static int count;
    static int fast;

    //wdt_reset();
    switch (count) {
    case 0:  digitalWrite(5, LOW); digitalWrite(4, HIGH); break;
    case 1:  digitalWrite(4, LOW); digitalWrite(5, HIGH); break;
    case 2:  digitalWrite(5, LOW); digitalWrite(6, HIGH); break;
    case 3:  digitalWrite(6, LOW); digitalWrite(7, HIGH); break;
    case 4:  digitalWrite(7, LOW); digitalWrite(8, HIGH); break;
    case 5:  digitalWrite(8, LOW); digitalWrite(9, HIGH); break;
    case 6:  digitalWrite(9, LOW); digitalWrite(8, HIGH); break;
    case 7:  digitalWrite(8, LOW); digitalWrite(7, HIGH); break;
    case 8:  digitalWrite(7, LOW); digitalWrite(6, HIGH); break;
    case 9:  digitalWrite(6, LOW); digitalWrite(5, HIGH); break;
    }
    if (++fast > 5000) {
        fast = 0;
        if (++count > 9)
            count = 0;
    }
}

int wait_input()
{
    for (;;) {
        int c = Serial.read();
        if (c >= 0)
            return c;
        idle();
    }
}

void show_status()
{
    Serial.write('\n');
    Serial.print("Frequency: ");
    Serial.print(khz(divisor));
    Serial.print(" kHz\r\n");
}

//
// Initialize the application.
//
void setup()
{
    Serial.begin(9600);

    // Use pins D2...D9 as outputs.
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);

    delay(10);
    Serial.write("\r\n------------------------------------------\r\n");
    Serial.write("Generator for BESM-6 Isolated Power Source\r\n");

    divisor = RATE_DEFAULT;
}

//
// Run the application.
//
void loop()
{
    show_status();

    if (divisor > RATE_MIN)
        Serial.write("\r\n  1. Increase frequency");
    if (divisor < RATE_MAX)
        Serial.write("\r\n  2. Decrease frequency");
    Serial.write("\r\n  3. Generate");
    Serial.write("\r\n\n");
    for (;;) {
        Serial.write("Command: ");
        int cmd = wait_input();
        Serial.write("\r\n");

        if (cmd == '\n' || cmd == '\r')
            break;

        if (cmd == '1' && divisor > RATE_MIN) {
            divisor--;
            break;
        }
        if (cmd == '2' && divisor < RATE_MAX) {
            divisor++;
            break;
        }
        if (cmd == '3') {
            generate(divisor);
            break;
        }
    }
}
