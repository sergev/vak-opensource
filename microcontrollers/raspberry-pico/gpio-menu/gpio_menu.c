/*
 * Test GPIO pins using a text menu on console.
 */
#include <stdio.h>
#include <pico/stdlib.h>

#define NUM_PINS 26

static const int index_to_pin[] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, // 0-15
    16, 17, 18, 19, 20, 21, 22, /*--gap--*/ 26, 27, 28,             // 16-25
};

static int is_output_pin[NUM_PINS];

static int pin_value[NUM_PINS];

//
// Wait for console input.
// Return ASCII keycode.
//
int wait_input()
{
    for (;;) {
#if LIB_PICO_STDIO_USB
        // Make sure console is connected.
        while (!stdio_usb_connected()) {
            sleep_ms(100);
        }
#endif
        // Read one byte.
        int ch = getchar();
        if (ch >= 0) {
            return ch;
        }
    }
}

void poll_pins()
{
    // Update input values.
    for (int i=0; i<NUM_PINS; i++)
        if (! is_output_pin[0])
            pin_value[i] = gpio_get(index_to_pin[i]);
}

void menu_pin(int index)
{
    int pin = index_to_pin[index];

again:
    poll_pins();

    printf("\r\n  1. Pin GP %u direction: %s",
        pin, is_output_pin[index] ? "Output" : "Input");

    if (is_output_pin[index]) {
        printf("\r\n  2. Pin GP %u output: %u", pin, pin_value[index]);
    }
    printf("\r\n\n");
    for (;;) {
        printf("Command: ");
        int cmd = wait_input();
        printf("\r\n");

        if (cmd == '\n' || cmd == '\r')
            break;

        if (cmd == '1') {
            if (is_output_pin[index]) {
                gpio_set_dir(pin, GPIO_IN);
                is_output_pin[index] = false;
            } else {
                gpio_set_dir(pin, GPIO_OUT);
                is_output_pin[index] = true;
            }
            goto again;
        }
        if (cmd == '2') {
            if (is_output_pin[index]) {
                if (pin_value[index] == 0) {
                    gpio_put(pin, 1);
                    pin_value[index] = 1;
                } else {
                    gpio_put(pin, 0);
                    pin_value[index] = 0;
                }
            }
            goto again;
        }
    }
}

void loop()
{
    poll_pins();
    printf("\r\n  Chip id: RP2040 rev.B%d, ROM v.%d\r\n", rp2040_chip_version(), rp2040_rom_version());

    printf("\r\n  0. Select pin GP %2u: %s %u", index_to_pin[0], is_output_pin[0] ? "out" : " in", pin_value[0]);
    printf("      g. Select pin GP %2u: %s %u", index_to_pin[16], is_output_pin[16] ? "out" : " in", pin_value[16]);

    printf("\r\n  1. Select pin GP %2u: %s %u", index_to_pin[1], is_output_pin[1] ? "out" : " in", pin_value[1]);
    printf("      h. Select pin GP %2u: %s %u", index_to_pin[17], is_output_pin[17] ? "out" : " in", pin_value[17]);

    printf("\r\n  2. Select pin GP %2u: %s %u", index_to_pin[2], is_output_pin[2] ? "out" : " in", pin_value[2]);
    printf("      i. Select pin GP %2u: %s %u", index_to_pin[18], is_output_pin[18] ? "out" : " in", pin_value[18]);

    printf("\r\n  3. Select pin GP %2u: %s %u", index_to_pin[3], is_output_pin[3] ? "out" : " in", pin_value[3]);
    printf("      j. Select pin GP %2u: %s %u", index_to_pin[19], is_output_pin[19] ? "out" : " in", pin_value[19]);

    printf("\r\n  4. Select pin GP %2u: %s %u", index_to_pin[4], is_output_pin[4] ? "out" : " in", pin_value[4]);
    printf("      k. Select pin GP %2u: %s %u", index_to_pin[20], is_output_pin[20] ? "out" : " in", pin_value[20]);

    printf("\r\n  5. Select pin GP %2u: %s %u", index_to_pin[5], is_output_pin[5] ? "out" : " in", pin_value[5]);
    printf("      l. Select pin GP %2u: %s %u", index_to_pin[21], is_output_pin[21] ? "out" : " in", pin_value[21]);

    printf("\r\n  6. Select pin GP %2u: %s %u", index_to_pin[6], is_output_pin[6] ? "out" : " in", pin_value[6]);
    printf("      m. Select pin GP %2u: %s %u", index_to_pin[22], is_output_pin[22] ? "out" : " in", pin_value[22]);

    printf("\r\n  7. Select pin GP %2u: %s %u", index_to_pin[7], is_output_pin[7] ? "out" : " in", pin_value[7]);
    printf("      n. Select pin GP %2u: %s %u", index_to_pin[23], is_output_pin[23] ? "out" : " in", pin_value[23]);

    printf("\r\n  8. Select pin GP %2u: %s %u", index_to_pin[8], is_output_pin[8] ? "out" : " in", pin_value[8]);
    printf("      o. Select pin GP %2u: %s %u", index_to_pin[24], is_output_pin[24] ? "out" : " in", pin_value[24]);

    printf("\r\n  9. Select pin GP %2u: %s %u", index_to_pin[9], is_output_pin[9] ? "out" : " in", pin_value[9]);
    printf("      p. Select pin GP %2u: %s %u", index_to_pin[25], is_output_pin[25] ? "out" : " in", pin_value[25]);

    printf("\r\n  a. Select pin GP %2u: %s %u", index_to_pin[10], is_output_pin[10] ? "out" : " in", pin_value[10]);

    printf("\r\n  b. Select pin GP %2u: %s %u", index_to_pin[11], is_output_pin[11] ? "out" : " in", pin_value[11]);
    printf("      q. Switch all pins to Input");

    printf("\r\n  c. Select pin GP %2u: %s %u", index_to_pin[12], is_output_pin[12] ? "out" : " in", pin_value[12]);
    printf("      r. Switch all pins to Output");

    printf("\r\n  d. Select pin GP %2u: %s %u", index_to_pin[13], is_output_pin[13] ? "out" : " in", pin_value[13]);
    printf("      s. Set all outputs to 1");

    printf("\r\n  e. Select pin GP %2u: %s %u", index_to_pin[14], is_output_pin[14] ? "out" : " in", pin_value[14]);
    printf("      t. Set all outputs to 0");

    printf("\r\n  f. Select pin GP %2u: %s %u", index_to_pin[15], is_output_pin[15] ? "out" : " in", pin_value[15]);

    printf("\r\n\n");
    for (;;) {
        printf("Command: ");
        int cmd = wait_input();
        printf("\r\n");

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
        if (cmd == 'q') {
            // Switch all pins to Input.
            for (int i=0; i<NUM_PINS; i++) {
                gpio_set_dir(index_to_pin[i], GPIO_IN);
                is_output_pin[i] = false;
            }
            break;
        }
        if (cmd == 'r') {
            // Switch all pins to Output.
            for (int i=0; i<NUM_PINS; i++) {
                gpio_set_dir(index_to_pin[i], GPIO_OUT);
                is_output_pin[i] = true;
            }
            break;
        }
        if (cmd == 's') {
            // Set all outputs to 1.
            for (int i=0; i<NUM_PINS; i++) {
                gpio_put(index_to_pin[i], 1);
                pin_value[i] = 1;
            }
            break;
        }
        if (cmd == 't') {
            // Set all outputs to 0.
            for (int i=0; i<NUM_PINS; i++) {
                gpio_put(index_to_pin[i], 0);
                pin_value[i] = 0;
            }
            break;
        }
    }
}

void setup()
{
    stdio_init_all();
    for (int i=0; i<NUM_PINS; i++) {
        gpio_init(index_to_pin[i]);
    }
    sleep_ms(1);
    printf("\r\n-----------------\r\n");
    printf("Testing GPIO pins\r\n");
}

int main()
{
    setup();
    for (;;) {
        loop();
    }
}
