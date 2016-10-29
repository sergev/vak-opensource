/*
 *      char *MemAlloc (int size)
 *              Allocate block of memory of specified size.
 *              Size cannot be greater than 32000 bytes.
 *              Address of block is
 *              proprietary aligned to be casted to any type.
 *              Return address of block (never 0 if size>0).
 *              If size is 0, just return 0.
 *              If size is <0 or run out of memory,
 *              fatal message is printed to stderr and exit.
 *
 *      char *MemZeroAlloc (int size)
 *              The same as MemAlloc, but memory is filled
 *              by zeroes.
 *
 *      char *MemRealloc (char *pointer, int size)
 *              Reallocate a block, obtained previosly from
 *              MemAlloc. Make it be size bytes long.
 *              Contents of block is saved, address may change.
 *              If pointer is 0, just allocate new block.
 *              If size is 0, just free block and return 0.
 *              Return address of block (never 0 if size>0).
 *
 *      MemTrunc (char *pointer, int size)
 *              Truncate block, obtained previosly from
 *              MemAlloc, to smaller size. Don't change address.
 *              If size is 0, just free block.
 *
 *      MemFree (char *pointer)
 *              Free previosly allocated block.
 *              Memory is guaranteed to be not destructed.
 *              If size is <=0, nothing is done.
 *
 *      C storage allocator with linear first-fit strategy.
 *      Works with noncontiguous, but monotonically linked,
 *      pool. Each block is preceded by a ptr to the next
 *      following block. Blocks are exact number of words.
 *      Field busy is 1 for busy, 0 for idle block.
 *      Gaps in pool are merely noted as busy blocks.
 *      Last block of pool (pointed to by bottom) is always
 *      marked busy and is never allocated.
 *      Idle blocks are coalesced during space search.
 */
# include <stdlib.h>
# include "mem.h"

# define BLOCK          1024    /* a multiple of WORD*/

# define WORD           (int) sizeof (cell)

typedef struct cell {
	struct cell *ptr;
	short busy;
} cell;

static cell *top;               /* pool top */
static cell *firstfree;         /* first free block in pool */
static cell *bottom;            /* the bottom of pool */

extern char *sbrk ();

static fatal (s)
char *s;
{
	write (2, s, (unsigned) strlen (s));
	exit (-1);
}

static init ()
{
	/* allocate 1 cell for bottom ptr */
	firstfree = bottom = top = (cell *) sbrk (WORD);
	if (top == (cell *) -1)
		fatal ("bad MemAlloc init\n");
	bottom->ptr = bottom;
	bottom->busy = 1;
}

static expand (nwords)
{
	/* must alloc only integer amounts of BLOCKs */
	nwords = ((nwords + BLOCK/WORD - 1) /
		(BLOCK/WORD)) * (BLOCK/WORD);
	if (sbrk (nwords * WORD) == (char *) -1)
		fatal ("no MemAlloc memory\n");
	bottom->busy = 0;
	bottom->ptr = bottom + nwords;
	bottom = bottom->ptr;
	bottom->busy = 1;
	bottom->ptr = bottom;
}

static cell *findfree (nwords)
{
	register cell *q, *p, *lastfree;

	lastfree = bottom;
	for (p=firstfree; p<bottom; p=p->ptr)
		if (! p->busy) {
			/* free block found */
			if (p->ptr == bottom)
				lastfree = p;
			/* If there are another free blocks
			 * in chain, cancatenate them, making
			 * one biggest block.
			 */
			for (q=p->ptr; ! q->busy; q=p->ptr)
				p->ptr = q->ptr;
			/* Now q points to next busy block.
			 * Check if this free block is
			 * large enough.
			 */
			if (nwords < q-p)
				return (p);
		}
	/* expand pool by nesessary amount of words */
	expand (nwords + 1 - (bottom - lastfree));
	lastfree->ptr = bottom;
	return (lastfree);
}

static char *makebusy (p, nwords)
register cell *p;
{
	register cell *q;

	/* Free block found. If it is too large,
	 * break it into two blocks
	 */
	q = p + nwords + 1;
	if (p->ptr > q) {
		q->ptr = p->ptr;
		q->busy = 0;
	}
	p->ptr = q;
	p->busy = 1;
	if (p == firstfree) {
		/* find first free block */
		while (firstfree < bottom && firstfree->busy)
			firstfree = firstfree->ptr;
	}
	return ((char *) (p + 1));
}

MemFree (ap)
register char *ap;
{
	register cell *p;

	if (! ap)
		return;
	p = (cell *) ap - 1;
	p->busy = 0;
	if (p < firstfree)
		firstfree = p;
}

char *MemAlloc (nbytes)
{
	register nwords;

	if (nbytes < 0)
		fatal ("bad MemAlloc arg\n");
	if (nbytes == 0)
		return (0);
	if (! top)                      /* first time */
		init ();
	nwords = (nbytes + WORD-1) / WORD;
	return (makebusy (findfree (nwords), nwords));
}

char *MemZeroAlloc (nbytes)
{
	char *p;
	register long *x;
	register nlongs;

	p = MemAlloc (nbytes);
	if (p) {
		nlongs = (nbytes + sizeof (long) - 1) / sizeof (long);
		x = (long *) p;
		while (--nlongs >= 0)
			*x++ = 0;
	}
	return (p);
}

char *MemRealloc (cp, nbytes)
char *cp;
{
	register cell *p, *q;
	register nwords, oldnwords;

	if (nbytes < 0)
		fatal ("bad MemRealloc arg\n");
	if (nbytes == 0) {
		if (cp)
			MemFree (cp);
		return (0);
	}
	if (! cp)
		return (MemAlloc (nbytes));
	q = (cell *) cp - 1;
	q->busy = 0;
	if (q < firstfree)
		firstfree = q;
	nwords = (nbytes + WORD-1) / WORD;
	oldnwords = q->ptr - q - 1;
	if (nwords < oldnwords)
		oldnwords = nwords;
	p = findfree (nwords);

	/* Save old contents of block */
	if (p != q && oldnwords)
		MemCopy ((char *) (p+1), cp, oldnwords * WORD);
	return (makebusy (p, nwords));
}

MemTrunc (cp, nbytes)
char *cp;
{
	register cell *p;
	register oldnbytes;

	if (! cp)
		fatal ("bad MemTrunc ptr\n");
	p = (cell *) cp - 1;
	oldnbytes = (p->ptr - p - 1) * WORD;
	if (nbytes<0 || nbytes>oldnbytes)
		fatal ("bad MemRealloc arg\n");
	if (nbytes == 0) {
		p->busy = 0;
		return;
	}
	makebusy (p, (nbytes + WORD-1) / WORD);
}

#if 0
# ifndef lint

char *malloc (n)
unsigned n;
{
	return (MemAlloc ((int) n));
}

char *realloc (p, n)
char *p;
unsigned n;
{
	return (MemRealloc (p, (int) n));
}

free (p)
char *p;
{
	MemFree (p);
}

char *calloc (num, size)
unsigned num, size;
{
	return (MemZeroAlloc ((int) (num * size)));
}

/*ARGSUSED*/
cfree (p, num, size)
char *p;
unsigned num, size;
{
	free (p);
}

# endif /* not lint */
#endif
