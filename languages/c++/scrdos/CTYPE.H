/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *  This product includes software developed by the University of
 *  California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *  @(#)ctype.h 5.3 (Berkeley) 4/3/91
 *
 * $Id: ctype.h,v 1.6 2008/04/19 17:53:18 ly Exp $
 */
/*
 * Hacked for KOI8-R encoding by Serge Vakulenko, <vak@zebub.msk.su>.
 */

#ifndef _CTYPE_H_
#define _CTYPE_H_

#define _U 0x01 /* uppercase letter */
#define _L 0x02 /* lowercase letter */
#define _N 0x04 /* digit */
#define _S 0x08 /* space */
#define _P 0x10 /* punctuation mark */
#define _C 0x20 /* control character */
#define _X 0x40 /* hexadecimal digit */
#define _B 0x80 /* blank symbol */

extern unsigned char _ctype_[];

unsigned char vak_toupper(unsigned char c);
unsigned char vak_tolower(unsigned char c);

#define isdigit(c)  ((_ctype_ + 1)[(unsigned char)c] & _N)
#define islower(c)  ((_ctype_ + 1)[(unsigned char)c] & _L)
#define isspace(c)  ((_ctype_ + 1)[(unsigned char)c] & _S)
#define ispunct(c)  ((_ctype_ + 1)[(unsigned char)c] & _P)
#define isupper(c)  ((_ctype_ + 1)[(unsigned char)c] & _U)
#define isalpha(c)  ((_ctype_ + 1)[(unsigned char)c] & (_U | _L))
#define isxdigit(c) ((_ctype_ + 1)[(unsigned char)c] & (_N | _X))
#define isalnum(c)  ((_ctype_ + 1)[(unsigned char)c] & (_U | _L | _N))
#define isprint(c)  ((_ctype_ + 1)[(unsigned char)c] & (_P | _U | _L | _N | _B))
#define isgraph(c)  ((_ctype_ + 1)[(unsigned char)c] & (_P | _U | _L | _N))
#define iscntrl(c)  ((_ctype_ + 1)[(unsigned char)c] & _C)
#define isascii(c)  ((unsigned char)(c) <= 0177)
#define toascii(c)  ((c) & 0177)

#endif /* !_CTYPE_H_ */
