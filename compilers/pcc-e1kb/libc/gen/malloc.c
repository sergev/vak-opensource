#include "values.h"
/*      @(#)malloc.c	1.8	*/
/*LINTLIBRARY*/
#define NDEBUG
#ifdef debug
#undef NDEBUG
#endif
#include "assert.h"

/*	avoid break bug */
#if pdp11
#define GRANULE 64
#else
#define GRANULE 0
#endif
#if pdp11 || vax || u3b || u3b5 || besm || svsb
/* this make the assumption that MAXINT is one less than an even block */
#define MAXALLOC (MAXINT-BLOCK+1)
#endif
/*      C storage allocator
 *	circular first-fit strategy
 *	works with noncontiguous, but monotonically linked, arena
 *	each block is preceded by a ptr to the (pointer of) 
 *	the next following block
 *	blocks are exact number of words long 
 *	aligned to the data type requirements of ALIGN
 *	pointers to blocks must have BUSY bit 0
 *	bit in ptr is 1 for busy, 0 for idle
 *	gaps in arena are merely noted as busy blocks
 *	last block of arena (pointed to by alloct) is empty and
 *	has a pointer to first
 *	idle blocks are coalesced during space search
 *
 *	a different implementation may need to redefine
 *	ALIGN, NALIGN, BLOCK, BUSY, INT
 *	where INT is integer type to which a pointer can be cast
*/
#define INT int
#define ALIGN int
#define NALIGN 0
#define WORD sizeof(union store)
#if besm
#define BLOCK 6144      /* a multiple of WORD*/
#else
#if u370
#define BLOCK 4096	/* a multiple of WORD*/
#else
#define BLOCK 1024	/* a multiple of WORD*/
#endif
#endif
#define BUSY 1
#define NULL 0
#define testbusy(p) ((INT)(p)&BUSY)
#define setbusy(p) (union store *)((INT)(p)|BUSY)
#define clearbusy(p) (union store *)((INT)(p)&~BUSY)

union store {
	union store *ptr;
#if NALIGN
	ALIGN dummy[NALIGN];
#endif
	int calloc;		/*calloc clears an array of integers*/
};

extern char *sbrk(), *memcpy();

static union store allocs[2];	/*initial arena*/
static union store *allocp;	/*search ptr*/
static union store *alloct;	/*arena top*/
static union store *allocx;	/*for benefit of realloc*/
static union store *allocend;	/*the last block, if it is free, or
				  *alloct */

char *
malloc(nbytes)
unsigned nbytes;
{
	register union store *p, *q;
	register int nw;
	register unsigned int temp;
	register unsigned int incr = 0;
	unsigned int sav_temp;

	if(allocs[0].ptr == 0) {		/*first time*/
		allocs[0].ptr = setbusy(&allocs[1]);
		allocs[1].ptr = setbusy(&allocs[0]);
		alloct = &allocs[1];
		allocp = &allocs[0];
		allocend = alloct;
	}
	nw = (nbytes+WORD+WORD-1)/WORD;
	assert(allocp >= allocs && allocp <= alloct);
	assert(allock());
	for(p=allocp; ; ) {
		for(temp=0; ; ) {
			if(!testbusy(p->ptr)) {
				while(!testbusy((q=p->ptr)->ptr)) {
					assert(q > p && q < alloct);
					allocp = p;
					p->ptr = q->ptr;
					if (allocend == q) allocend = p;
				}
				if(q >= p+nw && p+nw >= p)
					goto found;
			}
			q = p;
			p = clearbusy(p->ptr);
			if(p > q)
				assert(p <= alloct);
			else if(q != alloct || p != allocs) {
				assert(q == alloct && p == allocs);
				return(NULL);
			} else if(++temp > 1)
				break;
		}
		/* set block to search next */
		p = allocend;
		q = (union store *)sbrk(0);
		if (q != alloct+1)  {
			/* the addition must be done in words to
			   prevent overflow.  Also, use temporaries,
			   since order of operations may be changed,
			   otherwise. */
			temp = ((nw+BLOCK/WORD - 1)/(BLOCK/WORD));
			temp = temp * BLOCK;
			if (((INT)q%WORD) != 0)  {
				incr = (WORD-(INT)q%WORD);
				q = (union store *)((char *)q + incr);
				temp += incr;
			} 
		}  else  {
			temp = nw - (alloct - allocend);
			temp = ((temp+BLOCK/WORD)/(BLOCK/WORD));
			temp = temp * BLOCK;
		}
		if(((unsigned int)q)+temp+GRANULE < (unsigned int)q) {
			return(NULL);
		}
		
		sav_temp = temp;
		if (temp > MAXALLOC)  {
			if ((INT)sbrk(MAXALLOC) == -1)  {
				return NULL;
			}
			temp -= MAXALLOC;
		}
		if((INT)sbrk(temp) == -1) {
			brk(q);   	/* move brkval back */
			return(NULL);
		}
		allocend = q;
		assert(q > alloct);
		alloct->ptr = q;
		/* must subtract incr, since both q and temp had
		   incr added */
		q->ptr = (union store *)
			   ((unsigned char *)q + sav_temp - incr) - 1;
		if(q != alloct+1)
			alloct->ptr = setbusy(alloct->ptr);
		alloct = q->ptr;
		alloct->ptr = setbusy(allocs);
		q = p;
	}
found:
	allocp = p + nw;
	assert(allocp <= alloct);
	if(q > allocp) {
		allocx = allocp->ptr;
		allocp->ptr = p->ptr;
	}
	p->ptr = setbusy(allocp);
	/* move last block ptr, if necessary */
	if (allocend == p)  allocend = allocp;
	return((char*)(p+1));
}

/*      freeing strategy tuned for LIFO allocation */

void
free(ap)
register char *ap;
{
	register union store *p = (union store *)ap;

	assert(p > clearbusy(allocs[1].ptr) && p <= alloct);
	assert(allock());
	allocp = --p;
	assert(testbusy(p->ptr));
	/* if just freed last block in arena */
	p->ptr = clearbusy(p->ptr);
	if (p->ptr == alloct)  allocend = p;
	assert(p->ptr > allocp && p->ptr <= alloct);
	assert(allocend <= alloct);
}

/*
 *	realloc(p, nbytes) reallocates a block obtained from malloc()
 *	and freed since last call of malloc()
 *	to have new size nbytes, and old content
 *	returns new location, or 0 on failure
 */

char *
realloc(p, nbytes)
char	*p;
unsigned nbytes;
{
	register char *q;
	register union store *ap, *aq;
	register unsigned nw;
	unsigned onw;

	ap = (union store *)p;
	if(testbusy(ap[-1].ptr))
		free(p);
	onw = ap[-1].ptr - ap;
	q = malloc(nbytes);
	if(q == NULL || q == p)
		return(q);
	nw = (nbytes+WORD-1)/WORD;
	if(nw < onw)
		onw = nw;
	aq = (union store *) memcpy(q, p, onw * WORD);
	if(aq < ap && aq+nw >= ap)
		(aq+(aq+nw-ap))->ptr = allocx;
	return(q);
}

#ifdef debug
allock()
{
#ifdef longdebug
	register union store *p;
	int x;
	x = 0;
	for(p= &allocs[0]; clearbusy(p->ptr) > p; p=clearbusy(p->ptr)) {
		if(p == allocp)
			x++;
	}
	assert(p == alloct);
	return(x == 1 || p == allocp);
#else
	return(1);
#endif
}
#endif
/*	For debugging purposes only
/*rstalloc()
/*{
/*	
/*	allocs[0].ptr = 0;
/*	brk(clearbusy(allocs[1].ptr));
/*}
*/
