/*
Modified by Lauren
version 0.3

Any suggestions are welcome.
E-mail: Lauran.pan@gmail.com

Editor     : Lauren from DFRobot
Date       : 06.01.2012

=。=

* Have the back light under control.
* Update the library and sketch to compatible with IDE V1.0 and earlier

*/

#include "myLCD4884.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#include "WConstants.h"
#endif

LCD4884::LCD4884()
{};

LCD4884 lcd = LCD4884();

void LCD4884::backlight(unsigned char dat)
{
    if(dat==1) digitalWrite(LCD_BL,HIGH);
    else       digitalWrite(LCD_BL,LOW);
}

void LCD4884::LCD_init(void)
{
    pinMode(SPI_SCK,  OUTPUT); digitalWrite(SPI_SCK,  LOW);
    pinMode(SPI_MOSI, OUTPUT); digitalWrite(SPI_MOSI, LOW);
    pinMode(LCD_DC,   OUTPUT); digitalWrite(LCD_DC,   LOW);
    pinMode(SPI_CS,   OUTPUT); digitalWrite(SPI_CS,   LOW);
    pinMode(LCD_RST,  OUTPUT); digitalWrite(LCD_RST,  LOW);
    pinMode(LCD_BL,   OUTPUT); digitalWrite(LCD_BL,   LOW);

    digitalWrite(LCD_RST,LOW);
    delayMicroseconds(1);
    digitalWrite(LCD_RST,HIGH);

    digitalWrite(SPI_CS,LOW);  //Chip Select,Slave Transmit Enable(active low,Master Output)低电平有效
    delayMicroseconds(1);
    digitalWrite(SPI_CS,HIGH);
    delayMicroseconds(1);
    digitalWrite(LCD_BL,HIGH);

    //data_type=0, all are command bytes
    LCD_write_byte(0x21, 0);//Function Set:0 0 1 0 0 PD V H=0010 0001;PD=0,V=0,H=1;
    LCD_write_byte(0xc0, 0);//Set Vop:1 Vop6 Vop5 Vop4 Vop3 Vop2 Vop1 Vop0=1100 0000
    LCD_write_byte(0x06, 0);//Set Temperature Coefficient:0 0 0 0 0 1 Tc1 Tc0=0000 0110;Tc1=1,Tc0=0(Vlcd temperature coefficient 2)
    LCD_write_byte(0x13, 0);//Set Bias System (BSx):0 0 0 1 0 BS2 BS1 BS0=0001 0011;BS2=0, BS1=1, BS0=1==>N=4,MUX RATE=1:48

    LCD_write_byte(0x20, 0);//Function Set:0 0 1 0 0 PD V H=0010 0000;PD=0,V=0,H=0;
    LCD_clear();
    LCD_write_byte(0x0c, 0);//Display Control: 0 0 0 0 1 D 0 E=0000 1100 ;D=1,E=0:normal mode

    digitalWrite(SPI_CS,LOW);
}

//读取这一字节的8位数字，并由 data_type 判断这一字节代表读数据还是读指令
void LCD4884::LCD_write_byte(unsigned char dat, unsigned char dat_type)
{
    unsigned int i;
    digitalWrite(SPI_CS,LOW);//Chip Enable:Active LOW

    //判断是读数据还是读指令
    if (dat_type == 0) digitalWrite(LCD_DC,LOW);// D/C=0:the current data byte is interpreted as command byte
    else               digitalWrite(LCD_DC,HIGH);// D/C=1:write data to display RAM

    //依次读取8位data中的每一位数据
    for(i=0;i<8;i++)
    {
        if(dat&0x80) //1000 0000
        {
            digitalWrite(SPI_MOSI,HIGH);//读取的这一位数字是1
        }
        else
        {
            digitalWrite(SPI_MOSI,LOW);//读取的这一位数字是0
        }
        digitalWrite(SPI_SCK,LOW);
        dat = dat << 1;
        digitalWrite(SPI_SCK,HIGH);//在时钟上升沿有效读取这一位数字
    }
    digitalWrite(SPI_CS,HIGH);
}

void LCD4884::LCD_draw_bmp_pixel(unsigned char X,unsigned char Y,unsigned char *map,
                  unsigned char Pix_x,unsigned char Pix_y)
{
    unsigned int i,n;
    unsigned char row;

    if (Pix_y%8==0) row=Pix_y/8;//row from 1 to 6;Pix_y from R0 to R47
      else
        row=Pix_y/8+1;//Quotient+1

    for (n=0;n<row;n++)
    {
        LCD_set_XY(X,Y);
        for(i=0; i<Pix_x; i++)
          {
            LCD_write_byte(map[i+n*Pix_x], 1);// D/C=1:write data to display RAM
          }
        Y++;
      }
}

#if 0
void LCD4884::LCD_write_string(unsigned char X,unsigned char Y,char *s, char mode)
  {
    LCD_set_XY(X,Y);
    while (*s)
      {
     LCD_write_char(*s, mode);
     s++;
      }
  }

/**************************************************************************************/
void LCD4884::LCD_prop_write_string(unsigned char X,unsigned char Y,char *s, char mode)
  {
    LCD_set_XY(X,Y);
    while (*s)
      {
     LCD_prop_write_char(*s, mode);
     s++;
      }
  }
/*************************************************************************************/

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
          LCD_write_byte(c[(i*ch_with*2)+n],1);
        }
      i++;
      LCD_set_XY((X+(ch_with+row)*i),Y);
    }
}


void LCD4884::LCD_write_string_big ( unsigned char X,unsigned char Y, char *string, char mode )
{
    while ( *string ){

        LCD_write_char_big( X, Y, *string , mode );

        if(*string++ == '.')
          X += 5;
        else
          X += 12;
    }

}

/* write char in big font */
void LCD4884::LCD_write_char_big (unsigned char X,unsigned char Y, unsigned char ch, char mode)
{
   unsigned char i, j;
   unsigned char *pFont;
   unsigned char ch_dat;

   pFont = (unsigned char *) big_number;

   if(ch == '.')
    ch = 10;
   else if (ch == '+')
    ch = 11;
   else if (ch == '-')
    ch = 12;
   else
    ch = ch & 0x0f;


   for(i=0;i<3;i++)
   {
     LCD_set_XY ( X, Y+i);

     for(j=0; j<16; j++){
       ch_dat =  pgm_read_byte(pFont+ch*48 + i*16 +j);
       LCD_write_byte( (mode == MENU_NORMAL)? ch_dat : (ch_dat^0xff), 1);
     }
   }


}

void LCD4884::LCD_write_char(unsigned char c, char mode)
{
    unsigned char line;
    unsigned char *pFont;
    byte ch;

    pFont = (unsigned char *)font6_8;//pointer *pFont points at font6_8[][6](取首地址）
    c -= 32;// the ASCII of "SP" is 32

    for (line=0; line<6; line++){
    ch = pgm_read_byte(pFont+c*6+line); //pgm_read_byte(address_short),read c from the font6_8[][6](the detail information is in the "font6x8.h")
    LCD_write_byte( (mode==MENU_NORMAL)? ch: (ch^ 0xff) , 1);//MENU_NORMAL=0,True:return ch;False:return ch^0xff(按位异或）
    }
}

/*******************************************************************/
unsigned char LCD4884::LCD_prop_write_char(unsigned char c, char mode)
{
    int line, line_s=0, line_e=2;
    unsigned char *pFont;
    byte ch;

    pFont = (unsigned char *)font6_8;
    if (c -= 32)
    {
    for (line_s=0; line_s<6; line_s++)
        {
        if(pgm_read_byte(pFont+c*6+line_s))
            break;
    }
    for (line_e=5; line_e<0; line_e--)
        {
        if(pgm_read_byte(pFont+c*6+line_e))
            break;
    }
    }
    for (line=line_s; line<line_e+1; line++)
    {
    ch = pgm_read_byte(pFont+c*6+line);
    LCD_write_byte( (mode==MENU_NORMAL)? ch: (ch^ 0xff) , 1);
    }
    LCD_write_byte( (mode==MENU_NORMAL)? 0:0xff, 1);
    return ((unsigned char)(line_e+2 - line_s));
}
#endif

/******************************************************************/

void LCD4884::LCD_set_XY(unsigned char X, unsigned char Y)
  {
    LCD_write_byte(0x40 | Y, 0);        // column
    LCD_write_byte(0x80 | X, 0);         // row
  }


void LCD4884::LCD_clear(void)
  {
    unsigned int i;

    LCD_write_byte(0x0c, 0);
    LCD_write_byte(0x80, 0);

    for (i=0; i<504; i++)//6*84
    LCD_write_byte(0, 1);
  }
