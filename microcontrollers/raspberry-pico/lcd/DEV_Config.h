#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_

#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"
#include "stdio.h"

/**
 * data
 **/
#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

/**
 * GPIOI config
 **/

#define LCD_RST_PIN 12
#define LCD_DC_PIN  8
#define LCD_BL_PIN  13

#define LCD_CS_PIN   9
#define LCD_CLK_PIN  10
#define LCD_MOSI_PIN 11

#define LCD_SCL_PIN 7
#define LCD_SDA_PIN 6
/*------------------------------------------------------------------------------------------------------*/
void DEV_Digital_Write(UWORD Pin, UBYTE Value);
UBYTE DEV_Digital_Read(UWORD Pin);

void DEV_GPIO_Mode(UWORD Pin, UWORD Mode);
void DEV_KEY_Config(UWORD Pin);
void DEV_Digital_Write(UWORD Pin, UBYTE Value);
UBYTE DEV_Digital_Read(UWORD Pin);

void DEV_SPI_WriteByte(UBYTE Value);
void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len);

void DEV_Delay_ms(UDOUBLE xms);
void DEV_Delay_us(UDOUBLE xus);

void DEV_SET_PWM(uint8_t Value);

UBYTE DEV_Module_Init(void);
void DEV_Module_Exit(void);

#endif
