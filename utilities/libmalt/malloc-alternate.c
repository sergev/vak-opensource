/*
 * Copyright (C) 2000-2014 Serge Vakulenko <vak@cronyx.ru>
 *
 * The strategy used within this memory allocator is to try and cause as
 * little waste as possible.  This can make things a little slower than
 * would be ideal, but does give the best chance of things keeping running.
 */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

/*
 * Align data on double-sized boundaries.
 */
#define MEM_ALIGN(x)		(((x) + 7) & -8)
#define PAGESZ                  1024

/*
 * Every memory block has a header.
 */
typedef struct {
	size_t size;			/* Block size including the header */
} mheader_t;

static size_t free_size;                /* Total size of free memory available */
static mheader_t *free_list;            /* List of free blocks */

/*
 * In memory holes (free blocks), the space just after the header
 * is used as a pointer to the next hole (linked free list).
 */
#define NEXT(h)			(*(mheader_t**) ((h) + 1))

/**
 * Allocate a block of memory.
 * The memory may contain garbage.
 */
void *malloc (size_t required)
{
	mheader_t *h, **hprev, *newh;

        /* All allocations need to be several bytes larger than the
         * amount requested by our caller.  They also need to be large enough
         * that they can contain a "mheader_t" and any magic values used in
         * debugging (for when the block gets freed and becomes an isolated
         * hole). */
	if (required < sizeof(void*))
		required = sizeof(void*);
	required = MEM_ALIGN (required + sizeof(mheader_t));

	/* Scan the list of all available memory holes and find the first
	 * one that meets our requirement. */
	h = (mheader_t*) free_list;
	hprev = (mheader_t**) (void*) &free_list;
	while (h) {
        	if (h->size >= required)
        		break;

		hprev = &NEXT(h);
		h = NEXT(h);
	}

        /* Did we find any space available? */
        if (! h) {
                /* Allocate a new chunk of memory, page aligned. */
                size_t size = (required + PAGESZ-1) & ~PAGESZ;
                mheader_t *h = (mheader_t*) sbrk (size);

                /*debug_printf ("mem_init start=0x%x, size %d bytes\n", start, size);*/
                if ((size_t) h == (size_t) -1) {
                    /*debug_printf ("mem_alloc failed, size=%d bytes\n", required);*/
                    errno = ENOMEM;
                    return 0;
                }
                h->size = size;
                NEXT(h) = 0;
                *hprev = h;
                free_size += h->size;
	}

        /* Remove a chunk of space and, if we can, release any of what's left
	 * as a new hole.  If we can't release any then allocate more than was
	 * requested and remove this hole from the hole list. */
	if (h->size >= required + sizeof(mheader_t) + 2*sizeof(void*)) {
		newh = (mheader_t*) ((size_t)h + required);
		newh->size = h->size - required;
		h->size = required;
		NEXT(newh) = NEXT(h);
		*hprev = newh;
	} else {
		*hprev = NEXT(h);
	}
	free_size -= h->size;

	/*debug_printf ("mem %d bytes returned 0x%x\n", h->size, h+1);*/
	return h + 1;
}

/*
 * Add new hole to the free list.
 */
static void make_hole (mheader_t *newh)
{
	mheader_t *h, **hprev;

	free_size += newh->size;

	/*
	 * Walk through the hole list and see if this newly freed block can
	 * be merged with anything else to form a larger space.  Whatever
	 * happens, we still ensure that the list is ordered lowest-addressed
	 * -hole first through to highest-addressed-hole last.
	 */
        h = (mheader_t*) free_list;
        hprev = (mheader_t**) (void*) &free_list;
	for (;;) {
		if (! h) {
			/* At the end of free list */
        		*hprev = newh;
        		NEXT(newh) = 0;
       			break;
        	} else if ((size_t)h > (size_t)newh) {
			/* Insert the new hole before the old one */
        		*hprev = newh;
        		if (((size_t)newh + newh->size) == (size_t)h) {
        			newh->size += h->size;
        			NEXT(newh) = NEXT(h);
        		} else {
	        		NEXT(newh) = h;
        		}
       			break;
		} else if (((size_t)h + h->size) == (size_t)newh) {
			/* Append the new hole at the end of the old one */
        		h->size += newh->size;
        		if (((size_t)h + h->size) == (size_t)NEXT(h)) {
        			h->size += NEXT(h)->size;
        			NEXT(h) = NEXT(NEXT(h));
        		}
       			break;
		}
        	hprev = &NEXT(h);
        	h = NEXT(h);
        }
}

/**
 * Release a block of memory.
 */
void free (void *block)
{
	mheader_t *h;

	if (! block)
		return;

	/* Make the header pointer. */
	h = (mheader_t*) block - 1;

	/* Convert our block into a hole. */
	make_hole (h);
}

void *realloc (void *old_block, size_t bytes)
{
	mheader_t *h;
	size_t old_size;
	void *block;

	if (! old_block)
		return 0;

	/* Make the header pointer. */
	h = (mheader_t*) old_block - 1;
	old_size = h->size - sizeof(mheader_t);
	if (old_size >= bytes)
		return old_block;

	block = malloc (bytes);
	if (! block) {
		make_hole (h);
		return 0;
	}
	memcpy (block, old_block, old_size);
	make_hole (h);
	return block;
}

#if 0
void malloc_truncate (void *block, size_t required)
{
        mheader_t *h, *newh;

	if (! block)
		return;

	/* Add the size of header. */
	if (required < sizeof(void*))
		required = sizeof(void*);
	required = MEM_ALIGN (required + sizeof(mheader_t));

	/* Make the header pointer. */
	h = (mheader_t*) block - 1;

	/* Is there enough space to split? */
	if (h->size >= required + sizeof(mheader_t) + 2*sizeof(void*)) {
		/* Split into two blocks. */
		newh = (mheader_t*) ((size_t)h + required);
		newh->size = h->size - required;

		h->size = required;
		make_hole (newh);
	}
}

/**
 * Return the amount of heap space that's still available.
 */
size_t malloc_available()
{
	return free_size;
}

/*
 * Return the data size of the given block.
 */
size_t malloc_size (void *block)
{
	mheader_t *h;

	if (! block)
		return 0;

	/* Make the header pointer. */
	h = (mheader_t*) block - 1;
	return h->size - sizeof(mheader_t);
}
#endif
