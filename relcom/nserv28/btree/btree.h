/*-
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Mike Olson.
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
 *	@(#)btree.h	8.4 (Berkeley) 12/18/93
 */

#include "mpool.h"

#define	DEFMINKEYPAGE	(2)		/* Minimum keys per page */
#define	MINCACHE	(5)		/* Minimum cached pages */
#define	MINPSIZE	(512)		/* Minimum page size */

/*
 * Page 0 of a btree file contains a copy of the meta-data.  This page is also
 * used as an out-of-band page, i.e. page pointers that point to nowhere point
 * to page 0.  Page 1 is the root of the btree.
 */
#define	P_INVALID	 0		/* Invalid tree page number. */
#define	P_META		 0		/* Tree metadata page number. */
#define	P_ROOT		 1		/* Tree root page number. */

/*
 * There are five page layouts in the btree: btree internal pages (BINTERNAL),
 * btree leaf pages (BLEAF), recno internal pages (RINTERNAL), recno leaf pages
 * (RLEAF) and overflow pages.  All five page types have a page header (PAGE).
 * This implementation requires that longs within structures are NOT padded.
 * (ANSI C permits random padding.)  If your compiler pads randomly you'll have
 * to do some work to get this package to run.
 */
typedef struct _page {
	pgno_t	pgno;			/* this page's page number */
	pgno_t	prevpg;			/* left sibling */
	pgno_t	nextpg;			/* right sibling */

#define	P_BINTERNAL	0x01		/* btree internal page */
#define	P_BLEAF		0x02		/* leaf page */
#define	P_OVERFLOW	0x04		/* overflow page */
#define	P_RINTERNAL	0x08		/* recno internal page */
#define	P_RLEAF		0x10		/* leaf page */
#define P_TYPE		0x1f		/* type mask */

#define	P_PRESERVE	0x20		/* never delete this chain of pages */
	unsigned long flags;

	indx_t	lower;			/* lower bound of free space on page */
	indx_t	upper;			/* upper bound of free space on page */
	indx_t	linp[1];		/* long-aligned VARIABLE LENGTH DATA */
} PAGE;

/* First and next index. */
#define	BTDATAOFF	(sizeof(pgno_t) + sizeof(pgno_t) + sizeof(pgno_t) + \
			    sizeof(unsigned long) + sizeof(indx_t) + sizeof(indx_t))
#define	NEXTINDEX(p)	(((p)->lower - BTDATAOFF) / sizeof(indx_t))

/*
 * For pages other than overflow pages, there is an array of offsets into the
 * rest of the page immediately following the page header.  Each offset is to
 * an item which is unique to the type of page.  The h_lower offset is just
 * past the last filled-in index.  The h_upper offset is the first item on the
 * page.  Offsets are from the beginning of the page.
 *
 * If an item is too big to store on a single page, a flag is set and the item
 * is a { page, size } pair such that the page is the first page of an overflow
 * chain with size bytes of item.  Overflow pages are simply bytes without any
 * external structure.
 *
 * The size and page number fields in the items are long aligned so they can be
 * manipulated without copying.
 */
#define LALIGN(n)       (((n) + sizeof(unsigned long) - 1) & ~(sizeof(unsigned long) - 1))
#define NOVFLSIZE       (sizeof(pgno_t) + sizeof(unsigned long))

/*
 * For the btree internal pages, the item is a key.  BINTERNALs are {key, pgno}
 * pairs, such that the key compares less than or equal to all of the records
 * on that page.  For a tree without duplicate keys, an internal page with two
 * consecutive keys, a and b, will have all records greater than or equal to a
 * and less than b stored on the page associated with a.  Duplicate keys are
 * somewhat special and can cause duplicate internal and leaf page records and
 * some minor modifications of the above rule.
 */
typedef struct _binternal {
	unsigned long ksize;            /* key size */
	pgno_t	pgno;			/* page number stored on */
#define	P_BIGDATA	0x01		/* overflow data */
#define	P_BIGKEY	0x02		/* overflow key */
	unsigned char flags;
	char	bytes[1];		/* data */
} BINTERNAL;

/* Get the page's BINTERNAL structure at index indx. */
#define	GETBINTERNAL(pg, indx) \
	((BINTERNAL *)((char *)(pg) + (pg)->linp[indx]))

/* Get the number of bytes in the entry. */
#define NBINTERNAL(len) \
	LALIGN(sizeof(unsigned long) + sizeof(pgno_t) + sizeof(unsigned char) + (len))

/* Copy a BINTERNAL entry to the page. */
#define	WR_BINTERNAL(p, size, pgno, flags) { \
	*(unsigned long *)p = size; \
	p += sizeof(unsigned long); \
	*(pgno_t *)p = pgno; \
	p += sizeof(pgno_t); \
	*(unsigned char *)p = flags; \
	p += sizeof(unsigned char); \
}

/*
 * For the recno internal pages, the item is a page number with the number of
 * keys found on that page and below.
 */
typedef struct _rinternal {
	recno_t	nrecs;			/* number of records */
	pgno_t	pgno;			/* page number stored below */
} RINTERNAL;

/* Get the page's RINTERNAL structure at index indx. */
#define	GETRINTERNAL(pg, indx) \
	((RINTERNAL *)((char *)(pg) + (pg)->linp[indx]))

/* Get the number of bytes in the entry. */
#define NRINTERNAL \
	LALIGN(sizeof(recno_t) + sizeof(pgno_t))

/* Copy a RINTERAL entry to the page. */
#define	WR_RINTERNAL(p, nrecs, pgno) { \
	*(recno_t *)p = nrecs; \
	p += sizeof(recno_t); \
	*(pgno_t *)p = pgno; \
}

/* For the btree leaf pages, the item is a key and data pair. */
typedef struct _bleaf {
	unsigned long ksize;            /* size of key */
	unsigned long dsize;            /* size of data */
	unsigned char flags;            /* P_BIGDATA, P_BIGKEY */
	char	bytes[1];		/* data */
} BLEAF;

/* Get the page's BLEAF structure at index indx. */
#define	GETBLEAF(pg, indx) \
	((BLEAF *)((char *)(pg) + (pg)->linp[indx]))

/* Get the number of bytes in the entry. */
#define NBLEAF(p)	NBLEAFDBT((p)->ksize, (p)->dsize)

/* Get the number of bytes in the user's key/data pair. */
#define NBLEAFDBT(ksize, dsize) \
	LALIGN(sizeof(unsigned long) + sizeof(unsigned long) + sizeof(unsigned char) + \
	    (ksize) + (dsize))

/* Copy a BLEAF entry to the page. */
#define	WR_BLEAF(p, key, data, flags) { \
	*(unsigned long *)p = key->size; \
	p += sizeof(unsigned long); \
	*(unsigned long *)p = data->size; \
	p += sizeof(unsigned long); \
	*(unsigned char *)p = flags; \
	p += sizeof(unsigned char); \
	memmove(p, key->data, key->size); \
	p += key->size; \
	memmove(p, data->data, data->size); \
}

/* For the recno leaf pages, the item is a data entry. */
typedef struct _rleaf {
	unsigned long dsize;            /* size of data */
	unsigned char flags;            /* P_BIGDATA */
	char	bytes[1];
} RLEAF;

/* Get the page's RLEAF structure at index indx. */
#define	GETRLEAF(pg, indx) \
	((RLEAF *)((char *)(pg) + (pg)->linp[indx]))

/* Get the number of bytes in the entry. */
#define NRLEAF(p)	NRLEAFDBT((p)->dsize)

/* Get the number of bytes from the user's data. */
#define	NRLEAFDBT(dsize) \
	LALIGN(sizeof(unsigned long) + sizeof(unsigned char) + (dsize))

/* Copy a RLEAF entry to the page. */
#define	WR_RLEAF(p, data, flags) { \
	*(unsigned long *)p = data->size; \
	p += sizeof(unsigned long); \
	*(unsigned char *)p = flags; \
	p += sizeof(unsigned char); \
	memmove(p, data->data, data->size); \
}

/*
 * A record in the tree is either a pointer to a page and an index in the page
 * or a page number and an index.  These structures are used as a cursor, stack
 * entry and search returns as well as to pass records to other routines.
 *
 * One comment about searches.  Internal page searches must find the largest
 * record less than key in the tree so that descents work.  Leaf page searches
 * must find the smallest record greater than key so that the returned index
 * is the record's correct position for insertion.
 *
 * One comment about cursors.  The cursor key is never removed from the tree,
 * even if deleted.  This is because it is quite difficult to decide where the
 * cursor should be when other keys have been inserted/deleted in the tree;
 * duplicate keys make it impossible.  This scheme does require extra work
 * though, to make sure that we don't perform an operation on a deleted key.
 */
typedef struct _epgno {
	pgno_t	pgno;			/* the page number */
	indx_t	index;			/* the index on the page */
} EPGNO;

typedef struct _epg {
	PAGE	*page;			/* the (pinned) page */
	indx_t	 index;			/* the index on the page */
} EPG;

/*
 * The metadata of the tree.  The m_nrecs field is used only by the RECNO code.
 * This is because the btree doesn't really need it and it requires that every
 * put or delete call modify the metadata.
 */
typedef struct _btmeta {
	unsigned long m_magic;          /* magic number */
	unsigned long m_version;        /* version */
	unsigned long m_psize;          /* page size */
	unsigned long m_free;           /* page number of first free page */
	unsigned long m_nrecs;          /* R: number of records */
#define SAVEMETA (B_NODUPS | R_RECNO)
	unsigned long m_flags;          /* bt_flags & SAVEMETA */
	unsigned long m_unused;         /* unused */
} BTMETA;

/* The in-memory btree/recno data structure. */
typedef struct _btree {
	MPOOL	*bt_mp;			/* memory pool cookie */

	DB	*bt_dbp;		/* pointer to enclosing DB */

	EPG	bt_cur;			/* current (pinned) page */
	PAGE	*bt_pinned;		/* page pinned across calls */

	EPGNO	bt_bcursor;		/* B: btree cursor */
	recno_t	bt_rcursor;		/* R: recno cursor (1-based) */

#define BT_POP(t)       (t->bt_sp ? t->bt_stack + --t->bt_sp : 0)
#define	BT_CLR(t)	(t->bt_sp = 0)
	EPGNO	*bt_stack;		/* stack of parent pages */
	unsigned int bt_sp;             /* current stack pointer */
	unsigned int bt_maxstack;       /* largest stack */

	char	*bt_kbuf;		/* key buffer */
	unsigned long bt_kbufsz;        /* key buffer size */
	char	*bt_dbuf;		/* data buffer */
	unsigned long bt_dbufsz;        /* data buffer size */

	int	bt_fd;			/* tree file descriptor */

	pgno_t	bt_free;		/* next free page */
	unsigned long bt_psize;         /* page size */
	indx_t	bt_ovflsize;		/* cut-off for key/data overflow */
	int	bt_lorder;		/* byte order */
					/* sorted order */
	enum { NOT, BACK, FORWARD } bt_order;
	EPGNO	bt_last;		/* last insert */

	int     (*bt_cmp) ();           /* B: key comparison function */
	int     (*bt_pfx) ();           /* B: prefix comparison function */
	int     (*bt_irec) ();          /* R: recno input function */

	FILE	*bt_rfp;		/* R: record FILE pointer */
	int	bt_rfd;			/* R: record file descriptor */

	void *bt_cmap;                  /* R: current point in mapped space */
	void *bt_smap;                  /* R: start of mapped space */
	void *bt_emap;                  /* R: end of mapped space */
	unsigned long bt_msize;         /* R: size of mapped region. */

	recno_t	bt_nrecs;		/* R: number of records */
	unsigned long bt_reclen;        /* R: fixed record length */
	unsigned char bt_bval;          /* R: delimiting byte/pad character */

/*
 * NB:
 * B_NODUPS and R_RECNO are stored on disk, and may not be changed.
 */
#define	B_DELCRSR	0x00001		/* cursor has been deleted */
#define	B_INMEM		0x00002		/* in-memory tree */
#define	B_METADIRTY	0x00004		/* need to write metadata */
#define	B_MODIFIED	0x00008		/* tree modified */
#define	B_NEEDSWAP	0x00010		/* if byte order requires swapping */
#define	B_NODUPS	0x00020		/* no duplicate keys permitted */
#define	B_RDONLY	0x00040		/* read-only tree */
#define	R_RECNO		0x00080		/* record oriented tree */
#define	B_SEQINIT	0x00100		/* sequential scan initialized */

#define	R_CLOSEFP	0x00200		/* opened a file pointer */
#define	R_EOF		0x00400		/* end of input file reached. */
#define	R_FIXLEN	0x00800		/* fixed length records */
#define	R_MEMMAPPED	0x01000		/* memory mapped file. */
#define	R_INMEM		0x02000		/* in-memory file */
#define	R_MODIFIED	0x04000		/* modified file */
#define	R_RDONLY	0x08000		/* read-only file */

#define	B_DB_LOCK	0x10000		/* DB_LOCK specified. */
#define	B_DB_SHMEM	0x20000		/* DB_SHMEM specified. */
#define	B_DB_TXN	0x40000		/* DB_TXN specified. */

	unsigned long   bt_flags;       /* btree state */
} BTREE;

#define	SET(t, f)	((t)->bt_flags |= (f))
#define	CLR(t, f)	((t)->bt_flags &= ~(f))
#define	ISSET(t, f)	((t)->bt_flags & (f))

#ifdef __STDC__
int      __bt_close (DB *);
int      __bt_cmp (BTREE *, DBT *, EPG *);
int      __bt_crsrdel (BTREE *, EPGNO *);
int      __bt_defcmp (DBT *, DBT *);
int      __bt_defpfx (DBT *, DBT *);
int      __bt_delete (DB *, DBT *, unsigned int);
int      __bt_dleaf (BTREE *, PAGE *, int);
int      __bt_fd (DB *);
EPG     *__bt_first (BTREE *, DBT *, int *);
int      __bt_free (BTREE *, PAGE *);
int      __bt_get (DB *, DBT *, DBT *, unsigned int);
PAGE    *__bt_new (BTREE *, pgno_t *);
void     __bt_pgin (void *, pgno_t, void *);
void     __bt_pgout (void *, pgno_t, void *);
int      __bt_push (BTREE *, pgno_t, int);
int      __bt_put (DB *dbp, DBT *, DBT *, unsigned int);
int      __bt_ret (BTREE *, EPG *, DBT *, DBT *);
EPG     *__bt_search (BTREE *, DBT *, int *);
int      __bt_seq (DB *, DBT *, DBT *, unsigned int);
int      __bt_split (BTREE *, PAGE *,
	    DBT *, DBT *, unsigned long, unsigned long, unsigned int);
int      __bt_sync (DB *, unsigned int);
int      __ovfl_delete (BTREE *, void *);
int      __ovfl_get (BTREE *, void *, unsigned long *, char **, unsigned long *);
int      __ovfl_put (BTREE *, DBT *, pgno_t *);
#else
int      __bt_close ();
int      __bt_cmp ();
int      __bt_crsrdel ();
int      __bt_defcmp ();
int      __bt_defpfx ();
int      __bt_delete ();
int      __bt_dleaf ();
int      __bt_fd ();
EPG     *__bt_first ();
int      __bt_free ();
int      __bt_get ();
PAGE    *__bt_new ();
void     __bt_pgin ();
void     __bt_pgout ();
int      __bt_push ();
int      __bt_put ();
int      __bt_ret ();
EPG     *__bt_search ();
int      __bt_seq ();
int      __bt_split ();
int      __bt_sync ();
int      __ovfl_delete ();
int      __ovfl_get ();
int      __ovfl_put ();
#endif
