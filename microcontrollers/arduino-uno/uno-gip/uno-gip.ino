/*
 * Generate bipolar output 400kHz.
 */
#include <LiquidCrystal.h>

//
// Initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to.
//
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

//
// Rate for generator.
//
#define RATE_DEFAULT    19
#define RATE_MIN        (RATE_DEFAULT - 8)
#define RATE_MAX        (RATE_DEFAULT + 8)

int divisor;

//
// Constants for input buttons.
//
enum {
    BUTTON_NONE,
    BUTTON_RIGHT,
    BUTTON_LEFT,
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_SELECT,
};

//
// Read the buttons.
//
int read_buttons()
{
    // Expected input values are:
    // 0 - right button pressed
    // 131 - up
    // 307 - down
    // 480 - left
    // 721 - select button
    // 1023 - none
    int input = analogRead(0);

    if (input > 872) return BUTTON_NONE;
    if (input > 600) return BUTTON_SELECT;
    if (input > 393) return BUTTON_LEFT;
    if (input > 219) return BUTTON_DOWN;
    if (input > 65)  return BUTTON_UP;

    return BUTTON_RIGHT;
}

int display_frequency()
{
    int khz = 16000 / (1 + divisor) / 2;
    int i;

    // Display seconds elapsed since power-up.
    // Use right side of the second line.
    lcd.setCursor(0, 0);

    lcd.print("Freq ");
    lcd.print(khz);
    lcd.print(" kHz ");

    lcd.setCursor(0, 1);
    for (i=RATE_MAX; i>divisor; i--) {
        lcd.print("\377");
    }
    for (; i>RATE_MIN; i--) {
        lcd.print(" ");
    }
}

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

void generate(int div)
{
    int phase1 = PORTD | B00000100;
    int phase2 = PORTD | B00001000;

    // Use pins D2, D3 as outputs.
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);

    lcd.setCursor(0, 1);
    lcd.print("Generate        ");

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

//
// Initialize the application.
//
void setup()
{
    // Set up the screen width and height.
    lcd.begin(16, 2);

    // Print a message.
    divisor = RATE_DEFAULT;
    display_frequency();
}

//
// Run the application.
//
void loop()
{
    //
    // Read the buttons.
    // Repeat twice to stabilize.
    //
    static int last_key = BUTTON_NONE;
    static int key_depressed = 0;
    int key = read_buttons();
    if (key == last_key) {
        // Perform an action.
        switch (key) {
        case BUTTON_RIGHT:
            if (divisor > RATE_MIN && !key_depressed) {
                key_depressed = 1;
                divisor--;
                display_frequency();
            }
            break;
        case BUTTON_LEFT:
            if (divisor < RATE_MAX && !key_depressed) {
                key_depressed = 1;
                divisor++;
                display_frequency();
            }
            break;
        case BUTTON_NONE:
            key_depressed = 0;
            break;
        case BUTTON_SELECT:
            generate(divisor);
            break;
        }
    }
    last_key = key;
    delay(10);
}
