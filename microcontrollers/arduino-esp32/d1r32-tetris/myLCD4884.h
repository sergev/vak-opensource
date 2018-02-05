/*
Modified by Lauren
version 0.3

Any suggestions are welcome.
E-mail: Lauran.pan@gmail.com

Editor     : Lauren from DFRobot
Date       : 06.01.2012

=¡£=

* Have the back light under control.
* Update the library and sketch to compatible with IDE V1.0 and earlier

*/

#ifndef LCD4884_h
#define LCD4884_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// SPI Interface --- (using Arduino Digital Pin 2,3,4,5,6)
#define SPI_SCK     26  // D2 - Serial Clock(Master Output)
#define SPI_MOSI    25  // D3 - Master Output,Slave Input
#define LCD_DC      17  // D4 - Data/Command(command active low)
#define SPI_CS      16  // D5 - Chip Select,Slave Transmit Enable(active low,Master Output)
#define LCD_RST     27  // D6 - One Reset button
#define LCD_BL      14  // D7 - Backlit control (Arduino DIO Pin 7)

//display mode -- normal / highlight
#define MENU_NORMAL 0
#define MENU_HIGHLIGHT 1
#define OFF 0
#define ON 1


class LCD4884
{
public:
LCD4884();
void LCD_init(void);
void backlight(unsigned char dat);
void LCD_write_byte(unsigned char dat, unsigned char dat_type);
void LCD_draw_bmp_pixel(unsigned char X,unsigned char Y,unsigned char *map,unsigned char Pix_x,unsigned char Pix_y);
void LCD_write_string(unsigned char X,unsigned char Y,char *s, char mode);
void LCD_prop_write_string(unsigned char X,unsigned char Y,char *s, char mode); // new function1
void LCD_write_chinese(unsigned char X, unsigned char Y,unsigned char *c,unsigned char ch_with,unsigned char num,unsigned char line,unsigned char row);
void LCD_write_string_big ( unsigned char X,unsigned char Y, char *string, char mode );
void LCD_write_char_big (unsigned char X,unsigned char Y, unsigned char ch, char mode);
void LCD_write_char(unsigned char c, char mode);
unsigned char LCD_prop_write_char(unsigned char c, char mode);//new function2
void LCD_set_XY(unsigned char X, unsigned char Y);
void LCD_clear(void);
};
extern LCD4884 lcd;

#endif   //
