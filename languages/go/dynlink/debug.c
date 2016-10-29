/*
 * Scaled down version of printf(3).
 * Based on FreeBSD sources, heavily rewritten.
 *
 * Copyright (C) 2000-2002 Serge Vakulenko, <vak@cronyx.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 *
 * As a special exception to the GPL, permission is granted for additional
 * uses of the text contained in this file.  See the accompanying file
 * "COPY-UOS.txt" for details.
 */
/*
 * Two additional formats:
 *
 * The format %b is supported to decode error registers.
 * Its usage is:
 *
 *	printf("reg=%b\n", regval, "<base><arg>*");
 *
 * where <base> is the output base expressed as a control character, e.g.
 * \10 gives octal; \20 gives hex.  Each arg is a sequence of characters,
 * the first of which gives the bit number to be inspected (origin 1), and
 * the next characters (up to a control character, i.e. a character <= 32),
 * give the name of the register.  Thus:
 *
 *	kvprintf("reg=%b\n", 3, "\10\2BITTWO\1BITONE\n");
 *
 * would produce output:
 *
 *	reg=3<BITTWO,BITONE>
 *
 * The format %D -- Hexdump, takes a pointer. Sharp flag - use `:' as
 * a separator, instead of a space. For example:
 *
 *	("%6D", ptr)       -> XX XX XX XX XX XX
 *	("%#*D", len, ptr) -> XX:XX:XX:XX ...
 */
#include <stdarg.h>
#include <unistd.h>
#include <string.h>

/* Max number conversion buffer length: a long in base 2, plus NUL byte. */
#define MAXNBUF	(sizeof(long) * 8 + 1)

static unsigned char *ksprintn (unsigned char *buf, unsigned long v, unsigned char base,
	int width, unsigned char *lp);
static unsigned char mkhex (unsigned char ch);

static void PUTC (char ch)
{
	if (write (1, &ch, 1) < 0)
		/*ignore*/;
}

void
debug_vprintf (unsigned char const *fmt, va_list ap)
{
	unsigned char nbuf [MAXNBUF], padding, *q;
	const unsigned char *s;
	unsigned char c, base, lflag, ladjust, sharpflag, neg, dot, size;
	int n, width, dwidth, uppercase, extrazeros, sign;
	unsigned long ul;

	if (! fmt)
		fmt = (unsigned char*) "(null)\n";
	for (;;) {
		while ((c = *(fmt++)) != '%') {
			if (! c)
				return;
			PUTC (c);
		}
		padding = ' ';
		width = 0; extrazeros = 0;
		lflag = 0; ladjust = 0; sharpflag = 0; neg = 0;
		sign = 0; dot = 0; uppercase = 0; dwidth = -1;
reswitch:	switch (c = *(fmt++)) {
		case '.':
			dot = 1;
			padding = ' ';
			dwidth = 0;
			goto reswitch;

		case '#':
			sharpflag = 1;
			goto reswitch;

		case '+':
			sign = -1;
			goto reswitch;

		case '-':
			ladjust = 1;
			goto reswitch;

		case '%':
			PUTC (c);
			break;

		case '*':
			if (! dot) {
				width = va_arg (ap, int);
				if (width < 0) {
					ladjust = !ladjust;
					width = -width;
				}
			} else {
				dwidth = va_arg (ap, int);
			}
			goto reswitch;

		case '0':
			if (! dot) {
				padding = '0';
				goto reswitch;
			}
		case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			for (n=0; ; ++fmt) {
				n = n * 10 + c - '0';
				c = *(fmt);
				if (c < '0' || c > '9')
					break;
			}
			if (dot)
				dwidth = n;
			else
				width = n;
			goto reswitch;

		case 'b':
			ul = va_arg (ap, int);
			s = va_arg (ap, const unsigned char*);
			q = ksprintn (nbuf, ul, *s++, -1, 0);
			while (*q)
				PUTC (*q--);

			if (! ul)
				break;
			size = 0;
			while (*s) {
				n = *s++;
				if ((char) (ul >> (n-1)) & 1) {
					PUTC (size ? ',' : '<');
					for (; (n = *s) > ' '; ++s)
						PUTC (n);
					size = 1;
				} else
					while (*s > ' ')
						++s;
			}
			if (size)
				PUTC ('>');
			break;

		case 'c':
			if (! ladjust && width > 0)
				while (width--)
					PUTC (' ');

			PUTC (va_arg (ap, int));

			if (ladjust && width > 0)
				while (width--)
					PUTC (' ');
			break;

		case 'D':
			s = va_arg (ap, const unsigned char*);
			if (! width)
				width = 16;
			if (sharpflag)
				padding = ':';
			while (width--) {
				c = *s++;
				PUTC (mkhex (c >> 4));
				PUTC (mkhex (c));
				if (width)
					PUTC (padding);
			}
			break;

		case 'd':
			ul = lflag ? va_arg (ap, long) : va_arg (ap, int);
			if (! sign) sign = 1;
			base = 10;
			goto number;

		case 'l':
			lflag = 1;
			goto reswitch;

		case 'o':
			ul = lflag ? va_arg (ap, unsigned long) :
				va_arg (ap, unsigned int);
			base = 8;
			goto nosign;

		case 'p':
			ul = (size_t) va_arg (ap, void*);
			if (! ul) {
				s = (const unsigned char*) "(nil)";
				goto const_string;
			}
			base = 16;
			sharpflag = (width == 0);
			goto nosign;

		case 'n':
			ul = lflag ? va_arg (ap, unsigned long) :
				sign ? (unsigned long) va_arg (ap, int) :
				va_arg (ap, unsigned int);
			base = 10;
			goto number;

		case 'S':
#if __AVR__
			s = va_arg (ap, const unsigned char*);
			if (! s)
				s = (const unsigned char*) "(null)";
const_string:
			n = 0;
			if (! dot)
				while (*(s+n))
					++n;
			else
				while (n<dwidth && *(s+n))
					++n;

			width -= n;

			if (! ladjust && width > 0)
				while (width--)
					PUTC (' ');
			while (n--)
				PUTC (*(s++));
			if (ladjust && width > 0)
				while (width--)
					PUTC (' ');
			break;
#endif /* LY: AVR, and no break here! */

		case 's':
			s = va_arg (ap, unsigned char*);
#ifdef __AVR__
			if (! s) {
				s = (const unsigned char*) "(null)";
				goto const_string;
			}
#else
			if (! s)
				s = (const unsigned char*) "(null)";
const_string:
#endif
			if (! dot)
				n = strlen ((char*) s);
			else
				for (n=0; n<dwidth && s[n]; n++)
					continue;

			width -= n;

			if (! ladjust && width > 0)
				while (width--)
					PUTC (' ');
			while (n--)
				PUTC (*s++);
			if (ladjust && width > 0)
				while (width--)
					PUTC (' ');
			break;

		case 'r':
			/* Saturated counters. */
			base = 10;
			if (lflag) {
				ul = va_arg (ap, unsigned long);
				if (ul == -1) {
cnt_unknown:				if (ladjust)
						PUTC ('-');
					while (--width > 0)
						PUTC (' ');
					if (! ladjust)
						PUTC ('-');
					break;
				}
				if (ul >= -2) {
					ul = -3;
					neg = '>';
					goto nosign;
				}
			} else {
				ul = va_arg (ap, unsigned int);
				if (ul == (unsigned short) -1)
					goto cnt_unknown;
				if (ul >= (unsigned short) -2) {
					ul = (unsigned short) -3;
					neg = '>';
					goto nosign;
				}
			}
			goto nosign;

		case 'u':
			ul = lflag ? va_arg (ap, unsigned long) :
				va_arg (ap, unsigned int);
			base = 10;
			goto nosign;

		case 'x':
		case 'X':
			ul = lflag ? va_arg (ap, unsigned long) :
				va_arg (ap, unsigned int);
			base = 16;
			uppercase = (c == 'X');
			goto nosign;
		case 'z':
		case 'Z':
			ul = lflag ? va_arg (ap, unsigned long) :
				sign ? (unsigned long) va_arg (ap, int) :
				va_arg (ap, unsigned int);
			base = 16;
			uppercase = (c == 'Z');
			goto number;

nosign:			sign = 0;
number:		if (sign && ((long) ul != 0L)) {
				if ((long) ul < 0L) {
					neg = '-';
					ul = -(long) ul;
				} else if (sign < 0)
					neg = '+';
			}
			if (dwidth >= (int) sizeof(nbuf)) {
				extrazeros = dwidth - sizeof(nbuf) + 1;
				dwidth = sizeof(nbuf) - 1;
			}
			s = ksprintn (nbuf, ul, base, dwidth, &size);
			if (sharpflag && ul != 0) {
				if (base == 8)
					size++;
				else if (base == 16)
					size += 2;
			}
			if (neg)
				size++;

			if (! ladjust && width && padding == ' ' &&
			    (width -= size) > 0)
				do {
					PUTC (' ');
				} while (--width > 0);

			if (neg)
				PUTC (neg);

			if (sharpflag && ul != 0) {
				if (base == 8) {
					PUTC ('0');
				} else if (base == 16) {
					PUTC ('0');
					PUTC (uppercase ? 'X' : 'x');
				}
			}

			if (extrazeros)
				do {
					PUTC ('0');
				} while (--extrazeros > 0);

			if (! ladjust && width && (width -= size) > 0)
				do {
					PUTC (padding);
				} while (--width > 0);

			for (; *s; --s) {
				if (uppercase && *s>='a' && *s<='z') {
					PUTC (*s + 'A' - 'a');
				} else {
					PUTC (*s);
				}
			}

			if (ladjust && width && (width -= size) > 0)
				do {
					PUTC (' ');
				} while (--width > 0);
			break;
		default:
			PUTC ('%');
			if (lflag)
				PUTC ('l');
			PUTC (c);
			break;
		}
	}
}

/*
 * Put a NUL-terminated ASCII number (base <= 16) in a buffer in reverse
 * order; return an optional length and a pointer to the last character
 * written in the buffer (i.e., the first character of the string).
 * The buffer pointed to by `nbuf' must have length >= MAXNBUF.
 */
static unsigned char *
ksprintn (unsigned char *nbuf, unsigned long ul, unsigned char base, int width,
	unsigned char *lenp)
{
	unsigned char *p;

	p = nbuf;
	*p = 0;
	for (;;) {
		*++p = mkhex (ul % base);
		ul /= base;
		if (--width > 0)
			continue;
		if (! ul)
			break;
	}
	if (lenp)
		*lenp = p - nbuf;
	return (p);
}

static unsigned char
mkhex (unsigned char ch)
{
	ch &= 15;
	if (ch > 9)
		return ch + 'a' - 10;
	return ch + '0';
}

void
debug_printf (const char *fmt, ...)
{
	va_list	args;

	va_start (args, fmt);
	debug_vprintf ((unsigned char*) fmt, args);
	va_end (args);
}
