/*
 * C Compiler for PIC16Cxx processors.
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
#define MAXARGLEN	4

#define WREG		0x0a

#define ASM_TRUE        " cta 1;"
#define ASM_FALSE       " cta 0;"
#define ASM_RETURN      " ret;"
#define ASM_RETINT      " reti;"
#define ASM_GOTO        " goto %n;"
#define ASM_CALL        " call %n;"
#define ASM_GOTOLAB     " goto L%d;"
#define ASM_COND        " a|c 0;"
#define ASM_DEFLAB      "L%d:;"
