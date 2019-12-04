/*
 * Interface to a b/w OLED display based on SSD1306 controller.
 * Based on sources from Tara Keeling.
 *
 * Copyright (C) 2019 Serge Vakulenko
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. The name of the author may not be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <string.h>
#include <driver/i2c.h>
#include "oled.h"

//
// I2C interface.
//
#define PIN_RESET       16
#define PIN_SCL         15
#define PIN_SDA         4
#define I2C_PORT_INDEX  I2C_NUM_1
#define I2C_FREQ_HZ     1000000     // 1MHz
#define I2C_ADDRESS     0x3C        // OLED address at I2C bus

//
// Screen dimensions.
//
#define OLED_WIDTH      128
#define OLED_HEIGHT     64

#ifndef BIT
#   define BIT(n) (1 << n)
#endif

#define CheckBounds(expr, retexpr) { \
    if (expr) { \
        printf("[%s:%d] %s\n", __FUNCTION__, __LINE__, #expr); \
        retexpr; \
    } \
}

#define COM_Disable_LR_Remap 0
#define COM_Enable_LR_Remap BIT(5)

#define COM_Pins_Sequential 0
#define COM_Pins_Alternative BIT(4)

#define COM_ScanDir_LR 0
#define COM_ScanDir_RL 1

oled_t oled;

//
// SSD1306 commands.
//
enum {
    SSDCmd_Set_Contrast = 0x81,
    SSDCmd_Set_Display_Show_RAM = 0xA4,
    SSDCmd_Set_Display_Ignore_RAM = 0xA5,
    SSDCmd_Set_Normal_Display = 0xA6,
    SSDCmd_Set_Inverted_Display = 0xA7,
    SSDCmd_Set_Display_Off = 0xAE,
    SSDCmd_Set_Display_On = 0xAF,
    SSDCmd_Set_Memory_Addressing_Mode = 0x20,
    SSDCmd_Set_Mux_Ratio = 0xA8,
    SSDCmd_Nop = 0xE3,
    SSDCmd_Set_Display_Offset = 0xD3,
    SSDCmd_Set_Display_Start_Line = 0x40,
    SSDCmd_Set_Display_HFlip_Off = 0xA0,
    SSDCmd_Set_Display_HFlip_On = 0xA1,
    SSDCmd_Set_Display_VFlip_Off = 0xC0,
    SSDCmd_Set_Display_VFlip_On = 0xC8,
    SSDCmd_Set_COM_Pin_Config = 0xDA,
    SSDCmd_Set_Display_CLK = 0xD5,
    SSDCmd_Enable_Charge_Pump_Regulator = 0x8D,
    SSDCmd_Set_Column_Address = 0x21,
    SSDCmd_Set_Page_Address = 0x22,
};

typedef enum {
    AddressMode_Horizontal = 0,
    AddressMode_Vertical,
    AddressMode_Page,
    AddressMode_Invalid
} oled_address_mode_t;

int oled_send_command(uint8_t byte)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    if (!cmd)
        return -1;

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_WRITE, 1);
    i2c_master_write_byte(cmd, 0x80, true);
    i2c_master_write_byte(cmd, byte, true);
    i2c_master_stop(cmd);

    esp_err_t result = i2c_master_cmd_begin(I2C_PORT_INDEX, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    if (result != ESP_OK)
        return -1;
    return 0;
}

int oled_send_data(uint8_t *data, size_t nbytes)
{
    if (!data)
        return -1;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    if (!cmd)
        return -1;

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x40, true);
    i2c_master_write(cmd, data, nbytes, true);
    i2c_master_stop(cmd);

    esp_err_t result = i2c_master_cmd_begin(I2C_PORT_INDEX, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    if (result != ESP_OK)
        return -1;
    return 0;
}

void oled_set_mux_ratio(uint8_t ratio)
{
    oled_send_command(SSDCmd_Set_Mux_Ratio);
    oled_send_command(ratio);
}

void oled_set_display_offset(uint8_t offset)
{
    oled_send_command(SSDCmd_Set_Display_Offset);
    oled_send_command(offset);
}

void oled_set_display_start_line(uint32_t Line)
{
    oled_send_command(SSDCmd_Set_Display_Start_Line + (Line & 0x1F));
}

/*
 * This is all a big giant mystery that I have yet to figure out.
 * Beware all ye who enter.
 */
static void SetCOMPinConfiguration(uint32_t RemapCFG, uint32_t PinCFG, int ScanDir)
{
    oled_send_command(SSDCmd_Set_COM_Pin_Config);
    oled_send_command((uint8_t) (RemapCFG | PinCFG | BIT(1)));

    oled_send_command((ScanDir == COM_ScanDir_LR) ?
        SSDCmd_Set_Display_VFlip_Off : SSDCmd_Set_Display_VFlip_On);
}

void oled_set_contrast(uint8_t Contrast)
{
    oled_send_command(SSDCmd_Set_Contrast);
    oled_send_command(Contrast);
}

void oled_enable_display_ram()
{
    oled_send_command(SSDCmd_Set_Display_Show_RAM);
}

void oled_disable_display_ram()
{
    oled_send_command(SSDCmd_Set_Display_Ignore_RAM);
}

void oled_set_inverted(bool Inverted)
{
    oled_send_command(Inverted ? SSDCmd_Set_Inverted_Display : SSDCmd_Set_Normal_Display);
}

void oled_set_display_clocks(uint32_t DisplayClockDivider, uint32_t OSCFrequency)
{
    DisplayClockDivider&= 0x0F;
    OSCFrequency&= 0x0F;

    oled_send_command(SSDCmd_Set_Display_CLK);
    oled_send_command(((OSCFrequency << 4) | DisplayClockDivider));
}

/* There is no documentation for this command, but it is required during init. */
static void EnableChargePumpRegulator()
{
    oled_send_command(SSDCmd_Enable_Charge_Pump_Regulator);
    oled_send_command(0x14); /* MAGIC NUMBER */
}

void oled_display_on()
{
    oled_send_command(SSDCmd_Set_Display_On);
}

void oled_display_off()
{
    oled_send_command(SSDCmd_Set_Display_Off);
}

void oled_set_display_address_mode(oled_address_mode_t addressMode)
{
    oled_send_command(SSDCmd_Set_Memory_Addressing_Mode);
    oled_send_command(addressMode);
}

//
// Fill whole screen with color.
//
void oled_clear(int color)
{
    memset(oled.image, color ? 0xff : 0, oled.image_size);
}

void oled_update()
{
    oled_send_data(oled.image, oled.image_size);
}

void oled_write_raw_data(uint8_t *data, size_t nbytes)
{
    if (nbytes > oled.image_size) {
        nbytes = oled.image_size;
    }
    if (nbytes > 0) {
        oled_send_data(data, nbytes);
    }
}

void oled_set_h_flip(bool On)
{
    oled_send_command(On ? SSDCmd_Set_Display_HFlip_On : SSDCmd_Set_Display_HFlip_Off);
}

void oled_set_v_flip(bool On)
{
    oled_send_command(On ? SSDCmd_Set_Display_VFlip_On : SSDCmd_Set_Display_VFlip_Off);
}

static void oled_set_column_address(uint8_t Start, uint8_t End)
{
    CheckBounds(Start >= OLED_WIDTH, return);
    CheckBounds(End >= OLED_WIDTH, return);

    oled_send_command(SSDCmd_Set_Column_Address);
    oled_send_command(Start);
    oled_send_command(End);
}

static void oled_set_page_address(uint8_t Start, uint8_t End)
{
    CheckBounds(Start >= (OLED_HEIGHT/8), return);
    CheckBounds(End >= (OLED_HEIGHT/8), return);

    oled_send_command(SSDCmd_Set_Page_Address);
    oled_send_command(Start);
    oled_send_command(End);
}

//
// Reset the chip.
//
static void oled_reset()
{
    // Setup reset pin direction
    gpio_pad_select_gpio(PIN_RESET);
    gpio_set_direction(PIN_RESET, GPIO_MODE_OUTPUT);

    // Set inactive (high) for 10ms
    gpio_set_level(PIN_RESET, 1);
    vTaskDelay(1);

    // Set active (low) for 10ms
    gpio_set_level(PIN_RESET, 0);
    vTaskDelay(1);

    // Bring out of reset
    gpio_set_level(PIN_RESET, 1);
}

//
// Initialize the display.
//
int oled_init(int portrait, int color, int *xsize, int *ysize)
{
    //
    // Initialize I2C interface at pins PIN_SDA and PIN_SCL.
    //
    i2c_config_t config;

    memset(&config, 0, sizeof(i2c_config_t));
    config.mode = I2C_MODE_MASTER;
    config.sda_io_num = PIN_SDA;
    config.scl_io_num = PIN_SCL;
    config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    config.master.clk_speed = I2C_FREQ_HZ;
    if (i2c_param_config(I2C_PORT_INDEX, &config) != ESP_OK) {
        printf("Failed to configure I2C master at pins sda=%d, scl=%d!\n",
            PIN_SDA, PIN_SCL);
        return -1;
    }
    if (i2c_driver_install(I2C_PORT_INDEX, config.mode, 0, 0, 0) != ESP_OK) {
        printf("Failed to install I2C driver!\n");
        return -1;
    }

    //
    // Initialize SSD1306 chip.
    //
    memset(&oled, 0, sizeof(oled));
    oled.image_size = (OLED_WIDTH * OLED_HEIGHT) / 8;

    oled.image = (uint8_t*) malloc(oled.image_size);
    if (!oled.image) {
        printf("Failed allocate OLED frame buffer!\n");
        return -1;
    }
    memset(oled.image, 0, oled.image_size);

    oled_reset();

    /* Init sequence according to SSD1306.pdf */
    oled_set_mux_ratio(0x3F);
    oled_set_display_offset(0x00);
    oled_set_display_start_line(0);
    oled_set_h_flip(false);
    oled_set_v_flip(false);

    if (!portrait) {
        // Landscape mode.
        oled.width = OLED_WIDTH;
        oled.height = OLED_HEIGHT;
        SetCOMPinConfiguration(COM_Disable_LR_Remap, COM_Pins_Alternative, COM_ScanDir_LR);
    } else {
        // Portrait mode.
        oled.width = OLED_HEIGHT;
        oled.height = OLED_WIDTH;
        SetCOMPinConfiguration(COM_Disable_LR_Remap, COM_Pins_Sequential, COM_ScanDir_LR);
    }

    oled_set_contrast(0x7F);
    oled_disable_display_ram();
    oled_set_inverted(false);
    oled_set_display_clocks(0, 8);
    EnableChargePumpRegulator();
    oled_set_display_address_mode(AddressMode_Horizontal);
    oled_set_column_address(0, oled.width - 1);
    oled_set_page_address(0, (oled.height / 8) - 1);
    oled_enable_display_ram();
    oled_display_on();
    oled_update();

    oled_clear(color);

    *xsize = oled.width;
    *ysize = oled.height;
    return 0;
}
