/*
 * Copyright (C) 2007 Serge Vakulenko, <vak@cronyx.ru>
 *
 * This file is part of Unicoder utility, which is distributed
 * under the terms of the GNU General Public License (GPL).
 * See the accompanying file "COPYING" for more details.
 */
#include <string.h>
#include <Unicoder.h>

//
// iso8859-1
//
class iso8859_1_engine : public Unicode_engine {
	int get_char (FILE *fd)
		{ return getc (fd); }
	void put_char (unsigned short c, FILE *fd)
		{ putc (c<256 ? c : 0, fd); }
	const char *name (void)
		{ return "iso8859_1"; }
	friend class Unicoder;
};

//
// utf-8
// 00000000.0xxxxxxx -> 0xxxxxxx
// 00000xxx.xxyyyyyy -> 110xxxxx, 10yyyyyy
// xxxxyyyy.yyzzzzzz -> 1110xxxx, 10yyyyyy, 10zzzzzz
//
static inline int utf8_getc (FILE *fd)
{
	int c1, c2, c3;

	c1 = getc (fd);
	if (c1 < 0 || ! (c1 & 0x80))
		return c1;
	c2 = getc (fd);
	if (! (c1 & 0x20))
		return (c1 & 0x1f) << 6 | (c2 & 0x3f);
	c3 = getc (fd);
	return (c1 & 0x0f) << 12 | (c2 & 0x3f) << 6 | (c3 & 0x3f);
}

static inline void utf8_putc (unsigned short c, FILE *fd)
{
	if (c < 0x80) {
		putc (c, fd);
		return;
	}
	if (c < 0x800) {
		putc (c >> 6 | 0xc0, fd);
		putc (c & 0x3f | 0x80, fd);
		return;
	}
	putc (c >> 12 | 0xe0, fd);
	putc ((c >> 6) & 0x3f | 0x80, fd);
	putc (c & 0x3f | 0x80, fd);
}

class utf8_engine : public Unicode_engine {
	int get_char (FILE *fd)
		{ return utf8_getc (fd); }
	void put_char (unsigned short c, FILE *fd)
		{ utf8_putc (c, fd); }
	const char *name (void)
		{ return "utf-8"; }
	friend class Unicoder;
};

//
// ucs-2
//
static inline void ucs2_putc (unsigned short c, FILE *fd)
{
	// Big endian format.
	putc (c >> 8, fd);
	putc (c, fd);
}

static inline int ucs2_getc (FILE *fd)
{
	int c;

	c = getc (fd);
	if (c < 0)
		return c;
	return c << 8 | (unsigned char) getc (fd);
}

class ucs2_engine : public Unicode_engine {
	ucs2_engine ()
		{ swap_bytes = 0; start_of_file = 1; };
	int get_char (FILE *fd);
	void put_char (unsigned short c, FILE *fd);
	const char *name (void)
		{ return "ucs-2"; }
	int start_of_file;
	int swap_bytes;
	friend class Unicoder;
};

int ucs2_engine::get_char (FILE *fd)
{
	int c;
again:
	c = ucs2_getc (fd);
	if (c < 0)
		return c;
	if (c == 0xfffe) {
		swap_bytes = 1;
		goto again;
	}
	if (c == 0xfeff) {
		swap_bytes = 0;
		goto again;
	}
	if (swap_bytes)
		return c >> 8 | ((unsigned char) c) << 8;
	return c;
}

void ucs2_engine::put_char (unsigned short c, FILE *fd)
{
	// Big endian format.
	if (start_of_file) {
		ucs2_putc (0xfeff, fd);
		start_of_file = 0;
	}
	ucs2_putc (c, fd);
}

//
// autodetect utf-8 and ucs-2 on read,
// use utf-8 on write
//
class auto_engine : public Unicode_engine {
	auto_engine ()
		{ use_ucs2 = 0; swap_bytes = 0; start_of_file = 1; };
	int get_char (FILE *fd);
	void put_char (unsigned short c, FILE *fd)
		{ utf8_putc (c, fd); }
	const char *name (void)
		{ return start_of_file ? "autodetect" :
			use_ucs2 ? "ucs-2" : "utf-8"; }
	int use_ucs2;
	int start_of_file;
	int swap_bytes;
	friend class Unicoder;
};

int auto_engine::get_char (FILE *fd)
{
	int c;

	if (start_of_file) {
		c = getc (fd);
		if (c < 0)
			return c;
		if (c == 0xff || c == 0xfe)
			use_ucs2 = 1;
		ungetc (c, fd);
		start_of_file = 0;
	}
	if (! use_ucs2)
		return utf8_getc (fd);
again:
	c = ucs2_getc (fd);
	if (c < 0)
		return c;
	if (c == 0xfffe) {
		swap_bytes = 1;
		goto again;
	}
	if (c == 0xfeff) {
		swap_bytes = 0;
		goto again;
	}
	if (swap_bytes)
		return c >> 8 | ((unsigned char) c) << 8;
	return c;
}

#include "table.h"

int Unicoder::set_format (char *fmt)
{
	if (engine)
		delete engine;
	if (! fmt || strcasecmp (fmt, "autodetect") == 0)
		{ engine = new auto_engine; return 1; }
	if (strcasecmp (fmt, "utf-8") == 0)
		{ engine = new utf8_engine; return 1; }
	if (strcasecmp (fmt, "ucs-2") == 0)
		{ engine = new ucs2_engine; return 1; }
	if (strcasecmp (fmt, "iso8859-1") == 0)
		{ engine = new iso8859_1_engine; return 1; }
#include "table.cpp"
	engine = 0;
	return 0;
}
