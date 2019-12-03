/**
 * Copyright (c) 2017 Tara Keeling
 *
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <stdio.h>
#include <string.h>
#include <esp_timer.h>
#include <driver/spi_master.h>
#include <driver/i2c.h>

#include "ssd1306.h"
#include "font.h"

#include "iface_esp32_i2c.h"
#include "iface_esp32_spi.h"
#include "iface_virtual.h"

const int RSTPin = 16;
const int SCLPin = 15;
const int SDAPin = 4;

struct SSD1306_Device Dev_I2C;

void app_main( void ) {
    printf( "Ready...\n" );

    if ( ESP32_InitI2CMaster( SDAPin, SCLPin ) ) {
        printf( "i2c master initialized.\n" );

        // Setup reset pin direction
        gpio_pad_select_gpio(RSTPin);
        gpio_set_direction(RSTPin, GPIO_MODE_OUTPUT);
        // Set inactive (high) for 10ms
        gpio_set_level(RSTPin, 1);
        vTaskDelay(1);
        // Set active (low) for 10ms
        gpio_set_level(RSTPin, 0);
        vTaskDelay(1);
        // Bring out of reset
        gpio_set_level(RSTPin, 1);

        if ( SSD1306_Init_I2C( &Dev_I2C, 128, 64, 0x3C, 0, ESP32_WriteCommand_I2C, ESP32_WriteData_I2C, NULL ) == 1 ) {
            printf( "i2c display initialized.\n" );

            SSD1306_SetFont( &Dev_I2C, &Font_Comic_Neue_25x28 );
            FontDrawAnchoredString( &Dev_I2C, "Smile!", TextAnchor_Center, true );

            SSD1306_Update( &Dev_I2C );
        }
    }
}
