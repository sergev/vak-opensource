/*
 * PIC12C5xx assembler definitions.
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
 * Registers.
 */
#define INDF    0
#define TMR0    1
#define PCL     2
#define STATUS  3
#define FSR     4
#define OSCCAL  5
#define GPIO    6

/*
 * C compiler working registers.
 */
#define A1      0x07
#define A2      0x08
#define A3      0x09

/*
 * STATUS bits.
 */
#define GPWUF   7       /* GPIO reset: 1 - due to wake up */
#define PA0     5       /* program page preselect */
#define TO      4       /* time out: 1 after power up, clrwdt or sleep */
#define PD      3       /* power down: 1 after power up or by clrwdt */
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
