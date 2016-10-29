/*
 * PIC17C4x assembler definitions.
 *
 * Copyright (C) 1997-2002 Serge Vakulenko <vak@cronyx.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 */

/*
 * Unbanked registers.
 */
#define INDF0   0
#define FSR0    1
#define PCL     2
#define PCLATH  3
#define ALUSTA  4
#define T0STA   5
#define CPUSTA  6
#define INTSTA  7
#define INDF1   8
#define FSR1    9
#define WREG    0x0a
#define TBLPTR  0x0d
#define PROD    0x18
#define PRODH   0x19
#define PRODL   0x18

/*
 * C compiler working registers.
 */
#define A0      0x0a    /* WREG */
#define A1      0x09    /* FSR1 */
#define A2      0x0d    /* TBLPTRL */
#define A3      0x0e    /* TBLPTRH */

/*
 * ALUSTA bits.
 */
#define FS3     7       /* FSR1 do not change */
#define FS2     6       /* FSR1 post auto increment */
#define FS1     5       /* FSR0 do not change */
#define FS0     4       /* FSR1 post auto increment */
#define OV      3       /* arithmetic overflow flag */
#define Z       2       /* zero result flag */
#define DC      1       /* digit carry/not borrow flag */
#define C       0       /* carry/not borrow flag */

#define IF_GE c?
#define IF_LT nc?
#define IF_LE_GOTO c?; z?; goto

A4      .data   1
A5      .data   1
A6      .data   1
A7      .data   1
A8      .data   1
A9      .data   1
A10     .data   1
A11     .data   1
A12     .data   1
A13     .data   1
