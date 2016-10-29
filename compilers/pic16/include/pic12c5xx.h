/*
 * PIC12C5xx registers.
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
char  INDF    @ 0;
char  TMR0    @ 1;
char  PCL     @ 2;
char  STATUS  @ 3;
char  FSR     @ 4;
char  OSCCAL  @ 5;
char  GPIO    @ 6;

/*
 * C compiler working registers.
 */
char  A1      @ 7;
char  A2      @ 8;
char  A3      @ 9;

/*
 * STATUS bits.
 */
#define GPWUF   STATUS.7       /* GPIO reset: 1 - due to wake up */
#define PA0     STATUS.5       /* program page preselect */
#define TO      STATUS.4       /* time out: 1 after power up, clrwdt or sleep */
#define PD      STATUS.3       /* power down: 1 after power up or by clrwdt */
#define Z       STATUS.2       /* zero result flag */
#define DC      STATUS.1       /* digit carry/not borrow flag */
#define C       STATUS.0       /* carry/not borrow flag */

#define awake()  asm (" awake")
