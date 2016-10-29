/*
 * Copyright (C) 2005 Serge Vakulenko, <vak@cronyx.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING" for more details.
 */
#define ROW(a,b,c,d,e,f,g) (a | b<<1 | c<<2 | d<<3 | e<<4 | f<<5 | g<<6)
#define _ 0
#define O 1

/*
 * Двенадцать стандартных рисунков от Мастер Кит.
 */
static unsigned char pic_heart [] __attribute__ ((progmem)) = {
	ROW (_,O,O,_,O,O,_),
	ROW (O,O,O,O,O,O,O),
	ROW (O,O,O,O,O,O,O),
	ROW (O,O,O,O,O,O,O),
	ROW (_,O,O,O,O,O,_),
	ROW (_,_,O,O,O,_,_),
	ROW (_,_,_,O,_,_,_),

	ROW (_,_,_,_,_,_,_),
	ROW (_,_,O,O,O,_,_),
	ROW (_,O,O,O,O,O,_),
	ROW (_,O,O,O,O,O,_),
	ROW (_,_,O,O,O,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,_,_,_,_),

	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,O,O,O,_,_),
	ROW (_,_,O,O,O,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),

	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),

	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),

	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),

	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,O,O,O,_,_),
	ROW (_,_,O,O,O,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),

	ROW (_,_,_,_,_,_,_),
	ROW (_,_,O,O,O,_,_),
	ROW (_,O,O,O,O,O,_),
	ROW (_,O,O,O,O,O,_),
	ROW (_,_,O,O,O,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,_,_,_,_),

	ROW (_,O,O,_,O,O,_),
	ROW (O,O,O,O,O,O,O),
	ROW (O,O,O,O,O,O,O),
	ROW (O,O,O,O,O,O,O),
	ROW (_,O,O,O,O,O,_),
	ROW (_,_,O,O,O,_,_),
	ROW (_,_,_,O,_,_,_),

	0xff
};

static unsigned char pic_bar [] __attribute__ ((progmem)) = {
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,O,_,_,_),

	ROW (_,_,_,_,_,_,O),
	ROW (_,_,_,_,_,O,_),
	ROW (_,_,_,_,O,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,O,_,_,_,_),
	ROW (_,O,_,_,_,_,_),
	ROW (O,_,_,_,_,_,_),

	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (O,O,O,O,O,O,O),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),

	ROW (O,_,_,_,_,_,_),
	ROW (_,O,_,_,_,_,_),
	ROW (_,_,O,_,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,_,O,_,_),
	ROW (_,_,_,_,_,O,_),
	ROW (_,_,_,_,_,_,O),

	0xff
};

static unsigned char pic_coin [] __attribute__ ((progmem)) = {
	ROW (_,_,O,O,O,_,_),
	ROW (_,O,_,O,_,O,_),
	ROW (O,_,_,O,_,_,O),
	ROW (O,_,_,O,_,_,O),
	ROW (O,_,O,_,O,_,O),
	ROW (_,O,_,_,_,O,_),
	ROW (_,_,O,O,O,_,_),

	ROW (_,_,O,O,O,_,_),
	ROW (_,O,_,O,_,O,_),
	ROW (_,O,_,O,_,O,_),
	ROW (_,O,_,O,_,O,_),
	ROW (_,O,O,_,O,O,_),
	ROW (_,O,_,_,_,O,_),
	ROW (_,_,O,O,O,_,_),

	ROW (_,_,_,O,_,_,_),
	ROW (_,_,O,O,O,_,_),
	ROW (_,_,O,O,O,_,_),
	ROW (_,_,O,O,O,_,_),
	ROW (_,_,O,_,O,_,_),
	ROW (_,_,O,_,O,_,_),
	ROW (_,_,_,O,_,_,_),

	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,O,_,_,_),

	0xff
};

static unsigned char pic_cross [] __attribute__ ((progmem)) = {
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (O,O,O,O,O,O,O),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,O,_,_,_),

	ROW (O,_,_,_,_,_,O),
	ROW (_,O,_,_,_,O,_),
	ROW (_,_,O,_,O,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,O,_,O,_,_),
	ROW (_,O,_,_,_,O,_),
	ROW (O,_,_,_,_,_,O),

	0xff
};

static unsigned char pic_bird [] __attribute__ ((progmem)) = {
	ROW (_,_,_,_,_,_,_),
	ROW (O,_,_,_,_,_,O),
	ROW (_,O,_,_,_,O,_),
	ROW (_,_,O,_,O,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),

	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (O,_,_,_,_,_,O),
	ROW (_,O,O,_,O,O,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),

	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (O,O,O,_,O,O,O),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),

	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,O,O,_,O,O,_),
	ROW (O,_,_,O,_,_,O),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),

	0xff
};

static unsigned char pic_asterisk [] __attribute__ ((progmem)) = {
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),

	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,O,O,O,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),

	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,O,O,O,_,_),
	ROW (_,_,O,O,O,_,_),
	ROW (_,_,O,O,O,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),

	ROW (_,_,_,_,_,_,_),
	ROW (_,O,_,O,_,O,_),
	ROW (_,_,O,O,O,_,_),
	ROW (_,O,O,O,O,O,_),
	ROW (_,_,O,O,O,_,_),
	ROW (_,O,_,O,_,O,_),
	ROW (_,_,_,_,_,_,_),

	ROW (O,_,_,O,_,_,O),
	ROW (_,O,_,O,_,O,_),
	ROW (_,_,O,O,O,_,_),
	ROW (O,O,O,O,O,O,O),
	ROW (_,_,O,O,O,_,_),
	ROW (_,O,_,O,_,O,_),
	ROW (O,_,_,O,_,_,O),

	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),

	0xff
};

static unsigned char pic_snow [] __attribute__ ((progmem)) = {
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (O,O,O,O,O,O,O),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,O,_,_,_),

	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,O,O,O,_,_),
	ROW (O,O,O,_,O,O,O),
	ROW (_,_,O,O,O,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,O,_,_,_),

	ROW (_,_,_,O,_,_,_),
	ROW (_,O,O,O,O,O,_),
	ROW (_,O,_,_,_,O,_),
	ROW (O,O,_,_,_,O,O),
	ROW (_,O,_,_,_,_,_),
	ROW (_,O,O,O,O,O,_),
	ROW (_,_,_,O,_,_,_),

	ROW (O,O,O,O,O,O,O),
	ROW (O,_,_,_,_,_,O),
	ROW (O,_,_,_,_,_,O),
	ROW (O,_,_,_,_,_,O),
	ROW (O,_,_,_,_,_,O),
	ROW (O,_,_,_,_,_,O),
	ROW (O,O,O,O,O,O,O),

	0xff
};

static unsigned char pic_fire [] __attribute__ ((progmem)) = {
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,O,_,_,_,_,_),
	ROW (_,O,_,_,O,_,_),
	ROW (_,O,_,O,O,_,O),
	ROW (O,O,O,O,O,O,O),
	ROW (O,O,O,O,O,O,O),

	ROW (_,_,_,_,_,_,_),
	ROW (O,_,_,_,_,_,_),
	ROW (O,_,_,_,_,_,_),
	ROW (O,_,O,_,O,_,_),
	ROW (O,O,O,O,O,O,_),
	ROW (O,O,O,O,O,O,O),
	ROW (O,O,O,O,O,O,O),

	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,O,_,_,O),
	ROW (_,_,_,O,_,O,O),
	ROW (_,O,_,O,O,O,O),
	ROW (O,O,O,O,O,O,O),
	ROW (O,O,O,O,O,O,O),

	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,O,_,_,O),
	ROW (_,O,_,O,_,O,O),
	ROW (O,O,O,O,O,O,O),
	ROW (O,O,O,O,O,O,O),

	0xff
};

static unsigned char pic_square [] __attribute__ ((progmem)) = {
	ROW (O,O,O,O,O,O,O),
	ROW (O,_,_,_,_,_,O),
	ROW (O,_,_,_,_,_,O),
	ROW (O,_,_,_,_,_,O),
	ROW (O,_,_,_,_,_,O),
	ROW (O,_,_,_,_,_,O),
	ROW (O,O,O,O,O,O,O),

	ROW (_,_,_,_,_,_,_),
	ROW (_,O,O,O,O,O,_),
	ROW (_,O,_,_,_,O,_),
	ROW (_,O,_,_,_,O,_),
	ROW (_,O,_,_,_,O,_),
	ROW (_,O,O,O,O,O,_),
	ROW (_,_,_,_,_,_,_),

	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,O,O,O,_,_),
	ROW (_,_,O,_,O,_,_),
	ROW (_,_,O,O,O,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),

	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),

	0xff
};

static unsigned char pic_round [] __attribute__ ((progmem)) = {
	ROW (_,_,O,O,O,_,_),
	ROW (_,O,_,_,_,O,_),
	ROW (O,_,_,_,_,_,O),
	ROW (O,_,_,_,_,_,O),
	ROW (O,_,_,_,_,_,O),
	ROW (_,O,_,_,_,O,_),
	ROW (_,_,O,O,O,_,_),

	ROW (_,_,_,_,_,_,_),
	ROW (_,_,O,O,O,_,_),
	ROW (_,O,_,_,_,O,_),
	ROW (_,O,_,_,_,O,_),
	ROW (_,O,_,_,_,O,_),
	ROW (_,_,O,O,O,_,_),
	ROW (_,_,_,_,_,_,_),

	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,O,_,O,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),

	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),

	0xff
};

static unsigned char pic_romb [] __attribute__ ((progmem)) = {
	ROW (O,O,O,O,O,O,O),
	ROW (O,_,_,_,_,_,O),
	ROW (O,_,_,_,_,_,O),
	ROW (O,_,_,_,_,_,O),
	ROW (O,_,_,_,_,_,O),
	ROW (O,_,_,_,_,_,O),
	ROW (O,O,O,O,O,O,O),

	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),

	ROW (_,_,_,O,_,_,_),
	ROW (_,_,O,_,O,_,_),
	ROW (_,O,_,_,_,O,_),
	ROW (O,_,_,_,_,_,O),
	ROW (_,O,_,_,_,O,_),
	ROW (_,_,O,_,O,_,_),
	ROW (_,_,_,O,_,_,_),

	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),

	0xff
};

static unsigned char pic_ice [] __attribute__ ((progmem)) = {
	ROW (O,O,O,O,O,O,O),
	ROW (O,_,_,_,_,_,O),
	ROW (O,_,_,_,_,_,O),
	ROW (O,_,_,_,_,_,O),
	ROW (O,_,_,_,_,_,O),
	ROW (O,_,_,_,_,_,O),
	ROW (O,O,O,O,O,O,O),

	ROW (_,O,O,O,O,O,_),
	ROW (O,_,_,_,_,_,O),
	ROW (O,_,_,_,_,_,O),
	ROW (O,_,_,_,_,_,O),
	ROW (O,_,_,_,_,_,O),
	ROW (O,_,_,_,_,_,O),
	ROW (_,O,O,O,O,O,_),

	ROW (_,_,O,O,O,_,_),
	ROW (_,O,_,_,_,O,_),
	ROW (O,_,_,_,_,_,O),
	ROW (O,_,_,_,_,_,O),
	ROW (O,_,_,_,_,_,O),
	ROW (_,O,_,_,_,O,_),
	ROW (_,_,O,O,O,_,_),

	ROW (_,_,_,O,_,_,_),
	ROW (_,_,O,_,O,_,_),
	ROW (_,O,O,_,O,O,_),
	ROW (O,_,_,_,_,_,O),
	ROW (_,O,O,_,O,O,_),
	ROW (_,_,O,_,O,_,_),
	ROW (_,_,_,O,_,_,_),

	ROW (_,_,_,_,_,_,_),
	ROW (_,_,O,_,O,_,_),
	ROW (_,O,O,_,O,O,_),
	ROW (_,_,_,O,_,O,_),
	ROW (_,O,O,_,O,O,_),
	ROW (_,_,O,_,O,_,_),
	ROW (_,_,_,_,_,_,_),

	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,O,O,O,_,_),
	ROW (_,_,O,O,O,_,_),
	ROW (_,_,O,O,O,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),

	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,O,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),

	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),
	ROW (_,_,_,_,_,_,_),

	0xff
};

const unsigned char *pic_table [] = {
	pic_heart,
	pic_bar,
	pic_coin,
	pic_cross,
	pic_bird,
	pic_asterisk,
	pic_snow,
	pic_fire,
	pic_square,
	pic_round,
	pic_romb,
	pic_ice,
	0,
};
