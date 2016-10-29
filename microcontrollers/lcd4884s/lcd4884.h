#ifndef LCD4884_h
#define LCD4884_h

#define SPI_SCK     PORTD2
#define SPI_MOSI    PORTD3
#define LCD_DC      PORTD4
#define SPI_CS      PORTD5
#define LCD_RST     PORTD6
#define LCD_BL      PORTD7

/*
 * Display mode: normal / highlight
 */
#define MENU_NORMAL     0
#define MENU_HIGHLIGHT  1

class LCD4884
{
public:
LCD4884();
void LCD_init(void);
void LCD_write_byte(unsigned char dat, unsigned char dat_type);
void LCD_draw_bmp_pixel(unsigned char X,unsigned char Y,unsigned char *map,unsigned char Pix_x,unsigned char Pix_y);
void LCD_write_string(unsigned char X,unsigned char Y,char *s, char mode);
void LCD_write_chinese(unsigned char X, unsigned char Y,unsigned char *c,unsigned char ch_with,unsigned char num,unsigned char line,unsigned char row);
void LCD_write_string_big ( unsigned char X,unsigned char Y, char *string, char mode );
void LCD_write_char_big (unsigned char X,unsigned char Y, unsigned char ch, char mode);
void LCD_write_char(unsigned char c, char mode);
void LCD_set_XY(unsigned char X, unsigned char Y);
void LCD_clear(void);
};
extern LCD4884 lcd;

#endif
