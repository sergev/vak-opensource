#include "LCD4884.h"
#include "font_6x8.h"
#include "font_big.h"

extern "C"
{
#include <avr/pgmspace.h>
#include <avr/io.h>
#include "WConstants.h"
}

LCD4884::LCD4884()
{
}

LCD4884 lcd = LCD4884();

void LCD4884::LCD_init(void)
{
   // LCD_RST = 0;
   DDRD |= (1 << SPI_CS) |
           (1 << LCD_DC) |
           (1 << LCD_RST) |
           (1 << SPI_MOSI) |
           (1 << SPI_SCK) |
           (1 << LCD_BL);

   PORTD &= ~(1 << LCD_RST);
   delayMicroseconds(1);
   PORTD |= (1 << LCD_RST);

   PORTD &= ~(1 << SPI_CS);
   delayMicroseconds(1);
   PORTD |= (1 << SPI_CS);
   delayMicroseconds(1);

   PORTD |= (1 << LCD_BL);  // turn on backlight

   LCD_write_byte(0x21, 0);
   LCD_write_byte(0xc0, 0);
   LCD_write_byte(0x06, 0);
   LCD_write_byte(0x13, 0);
   LCD_write_byte(0x20, 0);
   LCD_clear();
   LCD_write_byte(0x0c, 0);

   PORTD &= ~(1<<SPI_CS);
}

void LCD4884::LCD_write_byte(unsigned char dat, unsigned char dat_type)
{
    unsigned int i;

    PORTD &= ~(1<<SPI_CS);
    if (dat_type == 0)
       PORTD &= ~(1<<LCD_DC);
    else
       PORTD |= (1<<LCD_DC);

    for (i=0; i<8; i++) {
        if (dat & 0x80) {
            PORTD |= (1<<SPI_MOSI);     // SDIN = 1;
        } else {
            PORTD &= ~(1<<SPI_MOSI);    // SDIN = 0;
        }
        PORTD &= ~(1<<SPI_SCK);         // SCLK = 0;
        dat = dat << 1;
        PORTD |= (1<<SPI_SCK);          // SCLK = 1;
    }
    PORTD |= (1<<SPI_CS);
}

void LCD4884::LCD_draw_bmp_pixel(unsigned char X,unsigned char Y,unsigned char *map,
                  unsigned char Pix_x,unsigned char Pix_y)
{
    unsigned int i,n;
    unsigned char row;

    if (Pix_y %8 == 0)
        row = Pix_y / 8;
    else
        row = Pix_y / 8 + 1;

    for (n=0; n<row; n++) {
      	LCD_set_XY(X, Y);
        for (i=0; i<Pix_x; i++) {
            LCD_write_byte(map[i + n*Pix_x], 1);
        }
        Y++;
    }
}

void LCD4884::LCD_write_string(unsigned char X,unsigned char Y,char *s, char mode)
{
    LCD_set_XY(X,Y);
    while (*s)
      {
	 LCD_write_char(*s, mode);
	 s++;
      }
}

void LCD4884::LCD_write_chinese(unsigned char X, unsigned char Y,unsigned char *c,unsigned char ch_with,unsigned char num,unsigned char line,unsigned char row)
{
    unsigned char i,n;
    LCD_set_XY(X,Y);
    for (i=0;i<num;)
    {
      for (n=0; n<ch_with*2; n++)
      {
          if (n==ch_with)
          {
              if (i==0) LCD_set_XY(X,Y+1);
              else
                {
                  LCD_set_XY((X+(ch_with+row)*i),Y+1);
                }
            }
          LCD_write_byte(c[(i*ch_with*2) + n], 1);
        }
      i++;
      LCD_set_XY((X+(ch_with+row)*i),Y);
    }
}

void LCD4884::LCD_write_string_big (unsigned char X, unsigned char Y, char *string, char mode)
{
    while (*string) {
        LCD_write_char_big(X, Y, *string, mode);

        if (*string++ == '.')
            X += 5;
        else
            X += 12;
    }
}

/*
 * write char in big font
 */
void LCD4884::LCD_write_char_big (unsigned char X,unsigned char Y, unsigned char ch, char mode)
{
    unsigned char i, j;
    unsigned char *pFont;
    unsigned char ch_dat;

    pFont = (unsigned char *) big_number;

    if (ch == '.')
        ch = 10;
    else if (ch == '+')
        ch = 11;
    else if (ch == '-')
        ch = 12;
    else
        ch = ch & 0x0f;

    for (i=0; i<3; i++) {
        LCD_set_XY(X, Y + i);

        for (j=0; j<16; j++) {
            ch_dat = pgm_read_byte(pFont+ch*48 + i*16 +j);
            if (mode != MENU_NORMAL)
                ch_dat ^= 0xff;
            LCD_write_byte(ch_dat, 1);
      }
    }
}

void LCD4884::LCD_write_char(unsigned char c, char mode)
{
    unsigned char line;
    unsigned char *pFont;
    byte ch;

    pFont = (unsigned char *)font6_8;
    c -= 32;

    for (line=0; line<6; line++)    {
        ch = pgm_read_byte(pFont+c*6+line);
        if (mode != MENU_NORMAL)
            ch ^= 0xff;
        LCD_write_byte(ch, 1);
    }
}

void LCD4884::LCD_set_XY(unsigned char X, unsigned char Y)
{
    LCD_write_byte(0x40 | Y, 0);	// column
    LCD_write_byte(0x80 | X, 0);        // row
}

void LCD4884::LCD_clear(void)
{
    unsigned int i;

    LCD_write_byte(0x0c, 0);
    LCD_write_byte(0x80, 0);

    for (i=0; i<504; i++)
        LCD_write_byte(0, 1);
}
