/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Margo Seltzer.
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
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)ndbm.c	8.2 (Berkeley) 9/11/93";
#endif /* LIBC_SCCS and not lint */

/*
 * This package provides a dbm compatible interface to the new hashing
 * package described in db(3).
 */

#include <stdio.h>
#include <string.h>

#include "ndbm.h"
#include "btree.h"

extern DB *__bt_open ();

int dbm_compare (a, b)
	register DBT *a, *b;
{
	if (a->size != b->size)
		return (a->size > b->size ? 1 : -1);
	if (a->size == sizeof (long)) {
		register unsigned long aa = *(long*) a->data;
		register unsigned long bb = *(long*) b->data;
		if (aa == bb)
			return (0);
		return (aa>bb ? 1 : -1);
	}
	return (memcmp (a->data, b->data, a->size));
}

/*
 * Returns:
 * 	*DBM on success
 *	 NULL on failure
 */
DBM *dbm_open (file, flags, mode)
	char *file;
	int flags, mode;
{
	BTREEINFO info;

	info.flags = 0;
	info.cachesize = 512 * 1024;
	info.maxkeypage = 32;
	info.minkeypage = 3;
	info.psize = 4096;
	info.compare = dbm_compare;
	info.prefix = 0;
	info.lorder = 0;
	return ((DBM*) __bt_open (file, flags, mode, &info, 0));
}

extern void
dbm_close(db)
	DBM *db;
{
	(void)(db->close)(db);
}

/*
 * Returns:
 *	DATUM on success
 *	NULL on failure
 */
extern datum
dbm_fetch(db, key)
	DBM *db;
	datum key;
{
	datum retval;
	int status;

	status = (db->get)(db, (DBT *)&key, (DBT *)&retval, 0);
	if (status) {
		retval.dptr = 0;
		retval.dsize = 0;
	}
	return (retval);
}

/*
 * Returns:
 *	DATUM on success
 *	NULL on failure
 */
extern datum
dbm_firstkey(db)
	DBM *db;
{
	int status;
	datum retdata, retkey;

	status = (db->seq)(db, (DBT *)&retkey, (DBT *)&retdata, R_FIRST);
	if (status)
		retkey.dptr = 0;
	return (retkey);
}

/*
 * Returns:
 *	DATUM on success
 *	NULL on failure
 */
extern datum
dbm_nextkey(db)
	DBM *db;
{
	int status;
	datum retdata, retkey;

	status = (db->seq)(db, (DBT *)&retkey, (DBT *)&retdata, R_NEXT);
	if (status)
		retkey.dptr = 0;
	return (retkey);
}
/*
 * Returns:
 *	 0 on success
 *	<0 failure
 */
extern int
dbm_delete(db, key)
	DBM *db;
	datum key;
{
	int status;

	status = (db->del)(db, (DBT *)&key, 0);
	if (status)
		return (-1);
	else
		return (0);
}

/*
 * Returns:
 *	 0 on success
 *	<0 failure
 *	 1 if DBM_INSERT and entry exists
 */
extern int
dbm_store(db, key, content, flags)
	DBM *db;
	datum key, content;
	int flags;
{
	return ((db->put)(db, (DBT *)&key, (DBT *)&content,
	    (flags == DBM_INSERT) ? R_NOOVERWRITE : 0));
}
