/*
 * LED blink demo for ATtiny board.
 */
#include <avr/io.h>
#include <util/delay_basic.h>

const int B_LED1  = 1 << 0;         // PB0 output
const int B_KEY1  = 1 << 1;         // PB1
const int B_KEY2  = 1 << 2;         // PB2
const int B_ADC   = 1 << 3;         // PB3
const int B_LED2  = 1 << 4;         // PB4 output
const int B_RESET = 1 << 5;         // PB5

void mdelay(uint32_t msec)
{
    // Processor clock in KHz.
    const int CPU_KHZ = F_CPU / 1000;

    uint32_t count = msec * CPU_KHZ/1000 * 65536 / 262;

    while (count >= 65536) {
        _delay_loop_2(0);
        count -= 65536;
    }
    if (count > 255) {
        _delay_loop_2((uint16_t) count);
    } else if (count > 0) {
        _delay_loop_1((uint8_t) count);
    }
}

int main()
{
    // Configure LED1 and LED2 pins as output
    DDRB = B_LED1 | B_LED2;

    // Enable pull-up resistors on key1 and key2 pins.
    PORTB = B_KEY1 | B_KEY2;

#if F_CPU > 1200000
    // Disable 1:8 clock divisor: raise from 1.2Mhz tp 9.6MHz.
    CLKPR = 1 << CLKPCE;
    CLKPR = 0;
#endif

    while (1) {
        PORTB |= B_LED1;    // LED1 on
        PORTB &= ~B_LED2;   // LED2 off
        mdelay(100);

        // Buttons work when pull-up resistors are enabled.
        while (!(PINB & B_KEY2)) {
            // Blink LED1 fast, while key2 is depressed.
            PORTB |= B_LED2;
            mdelay(50);
            PORTB &= ~B_LED2;
            mdelay(50);
            continue;
        }

        PORTB &= ~B_LED1;   // LED1 on
        PORTB |= B_LED2;    // LED2 off
        mdelay(100);

        while (!(PINB & B_KEY1)) {
            // Blink LED2 fast, while key1 is depressed.
            PORTB |= B_LED1;
            mdelay(50);
            PORTB &= ~B_LED1;
            mdelay(50);
            continue;
        }
    }

    return 0;
}
