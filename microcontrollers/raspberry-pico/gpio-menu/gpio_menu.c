/*
 * Test GPIO pins using a text menu on console.
 */
#include <stdio.h>
#include <pico/stdlib.h>

static const int index_to_pin[] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, // 0-15
    16, 17, 18, 19, 20, 21, 22, /*--gap--*/ 26, 27, 28,             // 16-25
};

static int is_output_pin[26];

static int pin_value[26];

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

void show_pins()
{
    // Update input values.
    for (int i=0; i<21; i++)
        if (! is_output_pin[0])
            pin_value[i] = gpio_get(index_to_pin[i]);

    printf("\n");
    printf("  Chip id: RP2040 rev.B%d, ROM v.%d\r\n", rp2040_chip_version(), rp2040_rom_version());
    printf("  Pins GP: %02u %02u %02u %02u %02u %02u %02u %02u %02u %02u %02u %02u %02u\r\n",
        index_to_pin[0], index_to_pin[1], index_to_pin[2], index_to_pin[3], index_to_pin[4],
        index_to_pin[5], index_to_pin[6], index_to_pin[7], index_to_pin[8], index_to_pin[9],
        index_to_pin[10], index_to_pin[11], index_to_pin[12]);

    printf("Direction:  %c  %c  %c  %c  %c  %c  %c  %c  %c  %c  %c  %c  %c\r\n",
        is_output_pin[0] ? 'o' : 'i', is_output_pin[1] ? 'o' : 'i',
        is_output_pin[2] ? 'o' : 'i', is_output_pin[3] ? 'o' : 'i',
        is_output_pin[4] ? 'o' : 'i', is_output_pin[5] ? 'o' : 'i',
        is_output_pin[6] ? 'o' : 'i', is_output_pin[7] ? 'o' : 'i',
        is_output_pin[8] ? 'o' : 'i', is_output_pin[9] ? 'o' : 'i',
        is_output_pin[10] ? 'o' : 'i', is_output_pin[11] ? 'o' : 'i',
        is_output_pin[12] ? 'o' : 'i');

    printf("    Value:  %u  %u  %u  %u  %u  %u  %u  %u  %u  %u  %u  %u  %u\r\n",
        pin_value[0], pin_value[1], pin_value[2], pin_value[3], pin_value[4],
        pin_value[5], pin_value[6], pin_value[7], pin_value[8], pin_value[9],
        pin_value[10], pin_value[11], pin_value[12]);

    printf("  Pins GP: %02u %02u %02u %02u %02u %02u %02u %02u %02u %02u %02u %02u %02u\r\n",
        index_to_pin[13], index_to_pin[14], index_to_pin[15], index_to_pin[16], index_to_pin[17],
        index_to_pin[18], index_to_pin[19], index_to_pin[20], index_to_pin[21], index_to_pin[22],
        index_to_pin[23], index_to_pin[24], index_to_pin[25]);

    printf("Direction:  %c  %c  %c  %c  %c  %c  %c  %c  %c  %c  %c  %c  %c\r\n",
        is_output_pin[13] ? 'o' : 'i', is_output_pin[14] ? 'o' : 'i',
        is_output_pin[15] ? 'o' : 'i', is_output_pin[16] ? 'o' : 'i',
        is_output_pin[17] ? 'o' : 'i', is_output_pin[18] ? 'o' : 'i',
        is_output_pin[19] ? 'o' : 'i', is_output_pin[20] ? 'o' : 'i',
        is_output_pin[21] ? 'o' : 'i', is_output_pin[22] ? 'o' : 'i',
        is_output_pin[23] ? 'o' : 'i', is_output_pin[24] ? 'o' : 'i',
        is_output_pin[25] ? 'o' : 'i');

    printf("    Value:  %u  %u  %u  %u  %u  %u  %u  %u  %u  %u  %u  %u  %u\r\n",
        pin_value[13], pin_value[14], pin_value[15], pin_value[16], pin_value[17],
        pin_value[18], pin_value[19], pin_value[20], pin_value[21], pin_value[22],
        pin_value[23], pin_value[24], pin_value[25]);
}

void menu_pin(int index)
{
    int pin = index_to_pin[index];

again:
    show_pins();

    printf("\r\n  1. Pin %u direction: %s",
        index, is_output_pin[index] ? "Output" : "Input");

    if (is_output_pin[index]) {
        printf("\r\n  2. Pin %u output: %u", index, pin_value[index]);
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
    show_pins();

    printf("\r\n  0. Select pin %u", index_to_pin[0]);
    printf("\r\n  1. Select pin %u", index_to_pin[1]);
    printf("\r\n  2. Select pin %u", index_to_pin[2]);
    printf("\r\n  3. Select pin %u", index_to_pin[3]);
    printf("\r\n  4. Select pin %u", index_to_pin[4]);
    printf("\r\n  5. Select pin %u", index_to_pin[5]);
    printf("\r\n  6. Select pin %u", index_to_pin[6]);
    printf("\r\n  7. Select pin %u", index_to_pin[7]);
    printf("\r\n  8. Select pin %u", index_to_pin[8]);
    printf("\r\n  9. Select pin %u", index_to_pin[9]);
    printf("\r\n  a. Select pin %u", index_to_pin[10]);
    printf("\r\n  b. Select pin %u", index_to_pin[11]);
    printf("\r\n  c. Select pin %u", index_to_pin[12]);
    printf("\r\n  d. Select pin %u", index_to_pin[13]);
    printf("\r\n  e. Select pin %u", index_to_pin[14]);
    printf("\r\n  f. Select pin %u", index_to_pin[15]);
    printf("\r\n  g. Select pin %u", index_to_pin[16]);
    printf("\r\n  h. Select pin %u", index_to_pin[17]);
    printf("\r\n  i. Select pin %u", index_to_pin[18]);
    printf("\r\n  j. Select pin %u", index_to_pin[19]);
    printf("\r\n  k. Select pin %u", index_to_pin[20]);
    printf("\r\n  l. Select pin %u", index_to_pin[21]);
    printf("\r\n  m. Select pin %u", index_to_pin[22]);
    printf("\r\n  n. Select pin %u", index_to_pin[23]);
    printf("\r\n  o. Select pin %u", index_to_pin[24]);
    printf("\r\n  p. Select pin %u", index_to_pin[25]);
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
    }
}

void setup()
{
    stdio_init_all();
    for (int i=0; i<26; i++) {
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
