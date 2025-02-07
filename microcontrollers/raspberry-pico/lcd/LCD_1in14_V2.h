#ifndef __LCD_1IN14_V2_H
#define __LCD_1IN14_V2_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h> //itoa()

#include "DEV_Config.h"

#define LCD_1IN14_V2_HEIGHT 240
#define LCD_1IN14_V2_WIDTH  135

#define HORIZONTAL 0
#define VERTICAL   1

#define LCD_1IN14_V2_SetBacklight(Value) ;

typedef struct {
    UWORD WIDTH;
    UWORD HEIGHT;
    UBYTE SCAN_DIR;
} LCD_1IN14_V2_ATTRIBUTES;
extern LCD_1IN14_V2_ATTRIBUTES LCD_1IN14_V2;

/********************************************************************************
function:
                        Macro definition variable name
********************************************************************************/
void LCD_1IN14_V2_Init(UBYTE Scan_dir);
void LCD_1IN14_V2_Clear(UWORD Color);
void LCD_1IN14_V2_Display(UWORD *Image);
void LCD_1IN14_V2_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image);
void LCD_1IN14_V2_DisplayPoint(UWORD X, UWORD Y, UWORD Color);

void Handler_1IN14_V2_LCD(int signo);
#endif
