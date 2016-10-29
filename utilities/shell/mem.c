/*
 *      mem *MemAlloc (int size)
 *              Allocate block of memory of specified size.
 *              Size cannot be greater than 32000 bytes.
 *              Address of block is
 *              proprietary aligned to be casted to any type.
 *              Return address of block (never 0 if size>0).
 *              If size is 0, just return 0.
 *              If size is <0 or run out of memory,
 *              fatal message is printed to stderr and exit.
 *
 *      mem *MemZeroAlloc (int size)
 *              The same as MemAlloc, but memory is filled
 *              by zeroes.
 *
 *      mem *MemRealloc (mem *pointer, int size)
 *              Reallocate a block, obtained previosly from
 *              MemAlloc. Make it be size bytes long.
 *              Contents of block is saved, address may change.
 *              If pointer is 0, just allocate new block.
 *              If size is 0, just free block and return 0.
 *              Return address of block (never 0 if size>0).
 *
 *      MemTrunc (mem *pointer, int size)
 *              Truncate block, obtained previosly from
 *              MemAlloc, to smaller size. Don't change address.
 *              If size is 0, just free block.
 *
 *      MemFree (mem *pointer)
 *              Free previosly allocated block.
 *              Memory is guaranteed to be not destructed.
 *              If size is <=0, nothing is done.
 *
 *      C storage allocator with linear first-fit strategy.
 *      Designed for use on machines with segmented memory.
 *
 *      Works with noncontiguous, but monotonically linked pool.
 *      Each block is preceded by a ptr to the next
 *      following block. Blocks are exact number of words.
 *      Field length is <0 for busy, >0 for idle block.
 *      Gaps in pool are merely noted as busy blocks.
 *      Last block of pool (pointed to by bottom) is always
 *      marked busy and is never allocated.
 *      Idle blocks are coalesced during space search.
 *
 *      This pachadge works in assumption that
 *      size of int is eqoal to that of int *.
 *      If int is shorter than int *,
 *      add -DLONGPTR to CFLAGS.
 */

# include "mem.h"

# define BLOCK          1024                    /* a multiple of WORD */
# define WORD           (int) sizeof (cell)     /* always power of two */

# ifdef LONGPTR
#    define PTRINT long
# else
#    define PTRINT int
# endif

typedef struct cell {
	struct cell *ptr;
	PTRINT length;
} cell;

static cell *top;               /* pool top */
static cell *firstfree;         /* first free block in pool */
static cell *bottom;            /* the bottom of pool */

extern mem *sbrk ();

static fatal (s)
char *s;
{
	write (2, s, (unsigned) strlen (s));
	exit (-1);
}

static init ()
{
	/* allocate 1 cell for bottom ptr */
	bottom = (cell *) sbrk (WORD);
	if (bottom == (cell *) -1)
		fatal ("bad MemAlloc init\n");
	bottom->length = -1;
	bottom->ptr = bottom;
	firstfree = top = bottom;
}

static expand (nwords)
{
	register cell *b;

	/* must alloc only integer amounts of BLOCKs */
	nwords = ((nwords + BLOCK/WORD - 1) / (BLOCK/WORD)) * (BLOCK/WORD);
	b = (cell *) sbrk (nwords * WORD);
	if (b == (cell *) -1)
		fatal ("no MemAlloc memory\n");
	bottom->length = 1;
	bottom->ptr = b;
	b->length = nwords-1;
	b->ptr = b + b->length;
	bottom = b->ptr;
	bottom->length = -1;
	bottom->ptr = bottom;
}

static cell *findfree (nwords)
{
	register cell *q, *p, *lastfree;
again:
	lastfree = bottom;
	for (p=firstfree; p<bottom; p=p->ptr)
		if (p->length > 0) {
			/* free block found */
			if (p->ptr == bottom)
				lastfree = p;
			/* If there are another free blocks in chain,
			 * cancatenate them, making one biggest block.
			 * Check if there is no hole between blocks.
			 */
			for (q=p->ptr; q==p+p->length && q->length>0; q=p->ptr) {
				p->length += q->length;
				p->ptr = q->ptr;
			}
			/* Now q points to next busy block.
			 * Check if this free block is large enough.
			 */
			if (nwords < p->length)
				return (p);
		}
	/* expand pool by nesessary amount of words */
	if (lastfree->length > 0)
		/* we have some space from last block */
		expand (nwords + 1 - lastfree->length);
	else
		/* no last free block */
		expand (nwords + 1);
	/* now we must repeat the search through the chain
	 * to collect adjacent free blocks properly
	 */
	goto again;
}

static mem *makebusy (p, nwords)
register cell *p;
{
	register cell *q;

	/* Free block found. If it is too large,
	 * break it into two blocks
	 */
	if (p->length > nwords+1) {
		q = p + nwords + 1;
		q->ptr = p->ptr;
		q->length = p->length - nwords - 1;
		p->ptr = q;
		p->length = nwords + 1;
	}
	/* mark it busy - just negate length */
	p->length = - p->length;
	if (p == firstfree) {
		/* find first free block */
		while (firstfree < bottom && firstfree->length < 0)
			firstfree = firstfree->ptr;
	}
	return ((mem *) (p + 1));
}

MemFree (ap)
register mem *ap;
{
	register cell *p;

	if (! ap)
		return;
	p = (cell *) ap - 1;
	if (p->length < 0)
		p->length = - p->length;
	if (p < firstfree)
		firstfree = p;
}

mem *MemAlloc (nbytes)
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

mem *MemZeroAlloc (nbytes)
{
	mem *p;
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

mem *MemRealloc (cp, nbytes)
mem *cp;
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
	if (q->length < 0)
		q->length = - q->length;
	if (q < firstfree)
		firstfree = q;
	nwords = (nbytes + WORD-1) / WORD;
	oldnwords = q->length - 1;
	if (nwords < oldnwords)
		oldnwords = nwords;
	p = findfree (nwords);

	/* Save old contents of block */
	if (p != q && oldnwords)
		MemCopy ((char *) (p+1), (char *) cp, oldnwords * WORD);
	return (makebusy (p, nwords));
}

MemTrunc (cp, nbytes)
mem *cp;
{
	register cell *p;
	register oldnbytes;

	if (! cp)
		fatal ("bad MemTrunc ptr\n");
	if (nbytes == 0) {
		MemFree (cp);
		return;
	}
	p = (cell *) cp - 1;
	if (p->length >= 0)
		fatal ("bad MemTrunc length\n");
	/* block is busy, mark it free */
	p->length = - p->length;
	oldnbytes = (p->length - 1) * WORD;
	if (nbytes<=0 || nbytes>oldnbytes)
		fatal ("bad MemRealloc arg\n");
	makebusy (p, (nbytes + WORD-1) / WORD);
}

# ifndef lint

char *malloc (n)
unsigned n;
{
	return ((char *) MemAlloc ((int) n));
}

char *realloc (p, n)
char *p;
unsigned n;
{
	return ((char *) MemRealloc ((mem *) p, (int) n));
}

free (p)
char *p;
{
	MemFree ((mem *) p);
}

char *calloc (num, size)
unsigned num, size;
{
	return ((char *) MemZeroAlloc ((int) (num * size)));
}

/*ARGSUSED*/
cfree (p, num, size)
char *p;
unsigned num, size;
{
	MemFree ((mem *) p);
}

# endif /* not lint */
