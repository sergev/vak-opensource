/*
 * Copyright 1990-1994 by Serge Vakulenko.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Serge Vakulenko not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Serge Vakulenko makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * SERGE VAKULENKO DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL SERGE VAKULENKO BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Serge Vakulenko, vak@kiae.su
 */

/*
 *      MemCheckIndex (type& array, type, int& bound, int quant, int index)
 *              Check index of dynamic array.
 *              Enlarge array if needed.
 */

# define MemCheckIndex(array, type, bound, quant, index)\
	if ((index) >= (bound)) (array) = (type) MemRealloc ((mem *) (array),\
		(int) ((bound) += (quant)) * (int) sizeof (*(array)))

# ifdef BCOPY
#    define MemCopy(t,f,n)      bcopy(f,t,n)
#    define MemCompare(a,b,n)   bcmp(a,b,n)
     extern bcopy ();
     extern bcmp ();
# else
#    define MemCopy(t,f,n)      memcpy(t,f,n)
#    define MemCompare(a,b,n)   memcmp(a,b,n)
     extern char *memcpy ();
     extern memcmp ();
# endif

typedef char mem;

# ifdef STDMALLOC
     /* On NeXT ld can't resolve multiply definition of malloc(),... */
     /* We try to use the standart ones. */
#    define MemAlloc(s)         malloc(s)
#    define MemFree(p)          free(p)
#    define MemRealloc(p,n)     realloc(p,n)
#    define MemTrunc(p,n)       realloc(p,n)
# else
extern mem *MemAlloc ();
extern mem *MemRealloc ();
# endif
