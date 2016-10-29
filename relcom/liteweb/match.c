/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Guido van Rossum.
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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
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
 * Modified by Serge Vakulenko.
 */

/*
 * int match (char *name, char *pattern)
 *
 * Match name with pattern. Returns 1 on success.
 * Pattern may contain wild symbols:
 *
 * !       - at beginning of pattern - any string not matched
 * ^       - at beginning of pattern - the string beginning
 * $       - at end of pattern - the string end
 * *       - any string
 * ?       - any symbol
 * [a-z]   - symbol in range
 * [^a-z]  - symbol out of range
 * [!a-z]  - symbol out of range
 */
#include <string.h>

int match (unsigned char *name, unsigned char *pat);

static int submatch (unsigned char *name, unsigned char *pat)
{
	int ok, negate_range;

	for (;;) {
		switch (*pat++) {
		case '*':
			if (! *pat)
				return (1);
			for (; *name; ++name)
				if (match (name, pat))
					return (1);
			return (0);
		case '?':
			if (! *name++)
				return (0);
			break;
		case '$':
			return (! *name);
		case 0:
			return (1);
		default:
			if (*name++ != pat[-1])
				return (0);
			break;
		case '[':
			ok = 0;
			negate_range = (*pat == '^' || *pat == '!');
			if (negate_range)
				++pat;
			while (*pat++ != ']')
				if (*pat == '-') {
					if (pat[-1] <= *name && *name <= pat[1])
						ok = 1;
					pat += 2;
				} else if (pat[-1] == *name)
					ok = 1;
			if (ok == negate_range)
				return (0);
			++name;
			break;
		}
	}
}

int match (unsigned char *name, unsigned char *pat)
{
	int matched = 1, len;

	if (*pat == '!') {
		matched = 0;
		++pat;
		if (! *pat)
			return (0);
	}
	while ((len = strlen (pat)) > 0 && pat[len-1] == '*')
		pat[len-1] = 0;
	if (*pat == '^')
		return (submatch (name, ++pat) ? matched : !matched);
	while (*pat == '*')
		++pat;
	if (! *pat)
		return (1);
	if (! pat[1])
		switch (*pat) {
		case 0:
		case '*': return (1);
		case '?': return (*name != 0);
		case '$': return (! *name);
		case '[': return (0);
		default:  return (strchr (name, *pat) != 0);
		}
	for (; *name; ++name)
		if (submatch (name, pat))
			return (matched);
	return (! matched);
}
