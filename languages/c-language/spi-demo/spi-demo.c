#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "gpio.h"
#include "spi.h"

/*
 * Hardware pins on GPIO connector.
 */
#ifdef MODE_INPUT
//
// Raspberry Pi
//
// Outputs
#define PIN_LCD_CS      8   // Chip Select
#define PIN_LCD_RST     27  // Reset
#define PIN_LCD_DC      25  // Data/Cmd
#define PIN_LCD_BL      24  // Backlight

// Inputs
#define PIN_KEY_UP      6
#define PIN_KEY_DOWN    19
#define PIN_KEY_LEFT    5
#define PIN_KEY_RIGHT   26
#define PIN_KEY_PRESS   13
#define PIN_KEY1        21
#define PIN_KEY2        20
#define PIN_KEY3        16
#else
//
// PIC32MZ DA Starter Kit
//
// Outputs
#define PIN_LCD_CS      GPIO_PIN('D', 0)    // p8  - RD0 - j24
#define PIN_LCD_RST     GPIO_PIN('B', 8)    // p27 - RB8 - j13
#define PIN_LCD_DC      GPIO_PIN('H', 6)    // p25 - RH6 - j22
#define PIN_LCD_BL      GPIO_PIN('H', 4)    // p24 - RH4 - j18

// Inputs
#define PIN_KEY_UP      GPIO_PIN('K', 2)    // p6  - RK2  - j31
#define PIN_KEY_DOWN    GPIO_PIN('B', 0)    // p19 - RB0  - j35
#define PIN_KEY_LEFT    GPIO_PIN('K', 1)    // p5  - RK1  - j29
#define PIN_KEY_RIGHT   GPIO_PIN('H', 7)    // p26 - RH7  - j37
#define PIN_KEY_PRESS   GPIO_PIN('G', 9)    // p13 - RG9  - j33
#define PIN_KEY1        GPIO_PIN('D', 15)   // p21 - RD15 - j40
#define PIN_KEY2        GPIO_PIN('H', 12)   // p20 - RH12 - j38
#define PIN_KEY3        GPIO_PIN('B', 15)   // p16 - RB15 - j36
#endif

/*
 * Initialize the pins and SPI protocol.
 */
void lcd_init()
{
#ifdef MODE_INPUT
    // Raspberry Pi
    gpio_export(PIN_LCD_CS);
    gpio_export(PIN_LCD_RST);
    gpio_export(PIN_LCD_DC);
    gpio_export(PIN_LCD_BL);
    gpio_export(PIN_KEY_UP);
    gpio_export(PIN_KEY_DOWN);
    gpio_export(PIN_KEY_LEFT);
    gpio_export(PIN_KEY_RIGHT);
    gpio_export(PIN_KEY_PRESS);
    gpio_export(PIN_KEY1);
    gpio_export(PIN_KEY2);
    gpio_export(PIN_KEY3);
#else
    // PIC32MZ DA Starter Kit
    // Re-map U2RX pin from p19 to p3, temporarily.
    gpio_set_mode(GPIO_PIN('F', 8), MODE_U2RX);
#endif

    // Output pins.
    gpio_set_mode(PIN_LCD_CS,  MODE_OUTPUT);
    gpio_set_mode(PIN_LCD_RST, MODE_OUTPUT);
    gpio_set_mode(PIN_LCD_DC,  MODE_OUTPUT);
    gpio_set_mode(PIN_LCD_BL,  MODE_OUTPUT);
    gpio_write(PIN_LCD_CS, 1);

    // Input pins.
    gpio_set_mode(PIN_KEY_UP,    MODE_INPUT);
    gpio_set_mode(PIN_KEY_DOWN,  MODE_INPUT);
    gpio_set_mode(PIN_KEY_LEFT,  MODE_INPUT);
    gpio_set_mode(PIN_KEY_RIGHT, MODE_INPUT);
    gpio_set_mode(PIN_KEY_PRESS, MODE_INPUT);
    gpio_set_mode(PIN_KEY1,      MODE_INPUT);
    gpio_set_mode(PIN_KEY2,      MODE_INPUT);
    gpio_set_mode(PIN_KEY3,      MODE_INPUT);

    //
    // Open the SPI port.
    //
#ifdef MODE_INPUT
    // Raspberry Pi
    spi_init("/dev/spidev0.0", 20000000);
#else
    // PIC32MZ DA Starter Kit
    spi_init("/dev/spidev2.0", 20000000);
#endif
}

/*
 * Hardware reset
 */
static void lcd_reset()
{
    gpio_write(PIN_LCD_RST, 1);
    usleep(100000);
    gpio_write(PIN_LCD_RST, 0);
    usleep(100000);
    gpio_write(PIN_LCD_RST, 1);
    usleep(100000);
}

/*
 * Send command
 */
static void lcd_send_command(uint8_t reg)
{
    gpio_write(PIN_LCD_DC, 0);
    //gpio_write(PIN_LCD_CS, 0);
    spi_transfer(reg);
    //gpio_write(PIN_LCD_CS, 1);
}

/*
 * Send data
 */
static void lcd_send_data(uint8_t data)
{
    gpio_write(PIN_LCD_DC, 1);
    //gpio_write(PIN_LCD_CS, 0);
    spi_transfer(data);
    //gpio_write(PIN_LCD_CS, 1);
}

/*
 * Initialize the display.
 */
void lcd_start(int horiz_flag)
{
    // Turn on the backlight
    gpio_write(PIN_LCD_BL, 1);

    // Hardware reset
    lcd_reset();

    // Set the resolution and scanning method of the screen
    lcd_send_command(0x36);          // MX, MY, RGB mode
    if (horiz_flag) {
        lcd_send_data(0x70);
    } else {
        lcd_send_data(0x00);
    }

    // Initialize the LCD registers
    lcd_send_command(0x3A);
    lcd_send_data(0x05);

    lcd_send_command(0xB2);
    lcd_send_data(0x0C);
    lcd_send_data(0x0C);
    lcd_send_data(0x00);
    lcd_send_data(0x33);
    lcd_send_data(0x33);

    lcd_send_command(0xB7);  // Gate Control
    lcd_send_data(0x35);

    lcd_send_command(0xBB);  // VCOM Setting
    lcd_send_data(0x19);

    lcd_send_command(0xC0);  // LCM Control
    lcd_send_data(0x2C);

    lcd_send_command(0xC2);  // VDV and VRH Command Enable
    lcd_send_data(0x01);
    lcd_send_command(0xC3);  // VRH Set
    lcd_send_data(0x12);
    lcd_send_command(0xC4);  // VDV Set
    lcd_send_data(0x20);

    lcd_send_command(0xC6);  // Frame Rate Control in Normal Mode
    lcd_send_data(0x0F);

    lcd_send_command(0xD0);  // Power Control 1
    lcd_send_data(0xA4);
    lcd_send_data(0xA1);

    lcd_send_command(0xE0);  // Positive Voltage Gamma Control
    lcd_send_data(0xD0);
    lcd_send_data(0x04);
    lcd_send_data(0x0D);
    lcd_send_data(0x11);
    lcd_send_data(0x13);
    lcd_send_data(0x2B);
    lcd_send_data(0x3F);
    lcd_send_data(0x54);
    lcd_send_data(0x4C);
    lcd_send_data(0x18);
    lcd_send_data(0x0D);
    lcd_send_data(0x0B);
    lcd_send_data(0x1F);
    lcd_send_data(0x23);

    lcd_send_command(0xE1);  // Negative Voltage Gamma Control
    lcd_send_data(0xD0);
    lcd_send_data(0x04);
    lcd_send_data(0x0C);
    lcd_send_data(0x11);
    lcd_send_data(0x13);
    lcd_send_data(0x2C);
    lcd_send_data(0x3F);
    lcd_send_data(0x44);
    lcd_send_data(0x51);
    lcd_send_data(0x2F);
    lcd_send_data(0x1F);
    lcd_send_data(0x1F);
    lcd_send_data(0x20);
    lcd_send_data(0x23);

    lcd_send_command(0x21);  // Display Inversion On
    lcd_send_command(0x11);  // Sleep Out
    lcd_send_command(0x29);  // Display On
}

/*
 * Set the start position and size of the display area
 */
void lcd_set_window(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end)
{
    // set the X coordinates
    lcd_send_command(0x2A);
    lcd_send_data((uint8_t) (x_start >> 8));
    lcd_send_data((uint8_t) x_start);
    lcd_send_data((uint8_t) (x_end >> 8));
    lcd_send_data((uint8_t) x_end);

    // set the Y coordinates
    lcd_send_command(0x2B);
    lcd_send_data((uint8_t) (y_start >> 8));
    lcd_send_data((uint8_t) y_start);
    lcd_send_data((uint8_t) (y_end >> 8));
    lcd_send_data((uint8_t) y_end);

    lcd_send_command(0x2c);
}

/*
 * Clear screen
 */
void lcd_clear(unsigned width, unsigned height, unsigned color)
{
    uint16_t image[width*height];
    int j;

    color = ((uint8_t)color << 8) | (uint8_t)(color >> 8);

    for (j = 0; j < height*width; j++) {
        image[j] = color;
    }

    lcd_set_window(0, 0, width-1, height-1);
    gpio_write(PIN_LCD_DC, 1);
    for (j = 0; j < height; j++){
        spi_bulk_rw((uint8_t*) &image[j*width], width*2);
    }
}

int main()
{
    // Initialize the display
    lcd_init();
    printf("SPI demo: clear LCD display.\n");

    // Horizontal orientation.
    lcd_start(1);
    lcd_clear(240, 240, 0xffff);

    spi_close();

    // Restore U2RX, U2TX pins.
    gpio_set_mode(GPIO_PIN('G', 9), MODE_U2TX);
    gpio_set_mode(GPIO_PIN('B', 0), MODE_U2RX);
    return 0;
}
