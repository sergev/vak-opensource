/*	@(#)nlist.c	1.9	*/
/*	3.0 SID #	1.4	*/
/*LINTLIBRARY*/
#if vax
#define ISMAGIC(x)	((((unsigned short)x)==(unsigned short)VAXROMAGIC) || \
			  (((unsigned short)x)==(unsigned short)VAXWRMAGIC))
#endif
#if u3b
#define ISMAGIC(x)	((((unsigned short)x)==(unsigned short)N3BMAGIC) || \
			  (((unsigned short)x)==(unsigned short)NTVMAGIC))
#endif
#if u3b5
#define ISMAGIC(x)	((x)==FBOMAGIC)
#endif
#if u3b || vax || u3b5
#define BADMAG(x)	(!ISMAGIC(x))
#endif


/*
 *	When a UNIX aout header is to be built in the optional header,
 *	the following magic numbers can appear in that header:
 *
 *		AOUT1MAGIC : default : readonly sharable text segment
 *		AOUT2MAGIC :	     : writable text segment
 *		PAGEMAGIC  :	     : directly paged object file
 */

#define AOUT1MAGIC 0410
#define AOUT2MAGIC 0407
#define PAGEMAGIC  0413

#define	SGSNAME	""
#define SGS ""
#define RELEASE "Release 6.0 6/1/82"
#include <a.out.h>

#if vax || u3b || u3b5
#	ifndef FLEXNAMES
#		define FLEXNAMES 1
#	endif
#	undef n_name		/* this patch causes problems here */
#endif

#if !(u3b || vax || u3b5)
#define SPACE 100		/* number of symbols read at a time */
#endif

extern long lseek();
extern int open(), read(), close(), strncmp(), strcmp();

int
nlist(name, list)
	char *name;
	struct nlist *list;
{
#if u3b || vax || u3b5
	extern char *malloc();
	extern void free();
	struct	filehdr	buf;
	struct	syment	sym;
	struct	auxent	aux;
	long n;
	int bufsiz=FILHSZ;
#if FLEXNAMES
	char *strtab = (char *)0;
	long strtablen;
#endif
	register struct nlist *p;
	register struct syment *q;
#else
	struct nlist space[SPACE];
	struct exec buf;
	int	nlen=sizeof(space[0].n_name), bufsiz=(sizeof(buf));
	unsigned n, m; 
	register struct nlist *p, *q;
#endif
	long	sa;
	int	fd;

	for (p = list; p->n_name && p->n_name[0]; p++)	/* n_name can be ptr */
	{
		p->n_type = 0;
		p->n_value = 0;
#if u3b || vax || u3b5
		p->n_value = 0L;
		p->n_scnum = 0;
		p->n_sclass = 0;
#endif
	}
	
	if ((fd = open(name, 0)) < 0)
		return(-1);
	(void) read(fd, (char *)&buf, bufsiz);
#if u3b || vax || u3b5
	if (BADMAG(buf.f_magic))
#else
	if (BADMAG(buf))
#endif
	{
		(void) close(fd);
		return (-1);
	}
#if u3b || vax || u3b5
	sa = buf.f_symptr;	/* direct pointer to sym tab */
	lseek(fd, sa, 0);
	q = &sym;
	n = buf.f_nsyms;	/* num. of sym tab entries */
#else
	sa = buf.a_text;
	sa += buf.a_data;
#if u370
	sa += (long)(buf.a_trsize + buf.a_drsize);
#endif
#if pdp11
	if (buf.a_flag != 1)
		sa += sa;
	else if ( buf.a_magic == A_MAGIC5 )
		sa += (long)buf.a_hitext << 16; /* remainder of text size for system overlay a.out */
#endif
	sa += (long)sizeof(buf);
	(void) lseek(fd, sa, 0);
	n = buf.a_syms;
#endif

	while (n)
	{
#if u3b || vax || u3b5
		read(fd, (char *)&sym, SYMESZ);
		n -= (q->n_numaux + 1L);
		/* read past aux ent , if there is one */
		if (q->n_numaux != 0)
			read(fd, (char *) &aux, AUXESZ);
#else
		m = (n < sizeof(space))? n: sizeof(space);
		(void) read(fd, (char*)space, m);
		n -= m;
		for (q=space; (int)(m -= sizeof(space[0])) >= 0; ++q)
		{
#endif
			for (p = list; p->n_name && p->n_name[0]; ++p)
			{
#if u3b || vax || u3b5
				/*
				* For 6.0, the name in an object file is
				* either stored in the eight long character
				* array, or in a string table at the end
				* of the object file.  If the name is in the
				* string table, the eight characters are
				* thought of as a pair of longs, (n_zeroes
				* and n_offset) the first of which is zero
				* and the second is the offset of the name
				* in the string table.
				*/
#if FLEXNAMES
				if (q->n_zeroes == 0L)	/* in string table */
				{
					if (strtab == (char *)0) /* need it */
					{
						long home = lseek(fd, 0L, 1);

						if (lseek(fd, buf.f_symptr +
							buf.f_nsyms * SYMESZ,
							0) == -1 || read(fd,
							(char *)&strtablen,
							sizeof(long)) !=
							sizeof(long) ||
							(strtab = malloc(
							(unsigned)strtablen))
							== (char *)0 ||
							read(fd, strtab +
							sizeof(long),
							strtablen -
							sizeof(long)) !=
							strtablen -
							sizeof(long) ||
							strtab[strtablen - 1]
							!= '\0' ||
							lseek(fd, home, 0) ==
							-1)
						{
							(void) lseek(fd,home,0);
							(void) close(fd);
							if (strtab != (char *)0)
								free(strtab);
							return (-1);
						}
					}
					if (q->n_offset < sizeof(long) ||
						q->n_offset >= strtablen)
					{
						(void) close(fd);
						if (strtab != (char *)0)
							free(strtab);
						return (-1);
					}
					if (strcmp(&strtab[q->n_offset],
						p->n_name))
					{
						continue;
					}
				}
				else
#endif /*FLEXNAMES*/
				{
					if (strncmp(q->_n._n_name,
						p->n_name, SYMNMLEN))
					{
						continue;
					}
				}
#else
				if (strncmp(p->n_name, q->n_name, nlen))
					continue;
#endif
#if u3b || vax || u3b5
				if (p->n_value != 0L)	/* got one already */
					break;
#endif
				p->n_value = q->n_value;
				p->n_type = q->n_type;
#if u3b || vax || u3b5
				p->n_scnum = q->n_scnum;
				p->n_sclass = q->n_sclass;
#endif
				break;
			}
#if !(u3b || vax || u3b5)
		}
#endif
	}
	(void) close(fd);
#if (vax || u3b || u3b5) && FLEXNAMES
	if (strtab != (char *)0)
		free(strtab);
#endif
	return (0);
}
