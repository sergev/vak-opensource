/*
 *      ОС ДЕМОС СВС-Б.
 *
 *      Создание таблицы symdef для быстрой загрузки.
 *
 *      Автор: Вакуленко С.
 *      Версия от 06.02.90.
 *
 *      Исходные тексты взяты из ОС UNIX 4.3 BSD.
 */

# include <stdio.h>

# ifdef CROSS
#    include "../h/a.out.h"
#    include "../h/ar.h"
#    include "../h/ranlib.h"
# else
#    include <a.out.h>
#    include <ar.h>
#    include <ranlib.h>
# endif

# define W              8       /* sizeof word of Elbrus-B */

# define AR             "/usr/SVSB/bin/ar"

# define TABSZ          1000
# define STRTABSZ       (TABSZ*10)

struct	ar_hdr	archdr;
struct	exec	exp;

long	arsize;
FILE	*fi, *fo;
long	off, oldoff;
int	new;
char    firstname [40];
char    tempnm [] = "__.SYMDEF";
struct  ranlib rantab [TABSZ];
int	tnum;
int     debug;

extern long fgeth (), time ();

main (argc, argv)
char **argv;
{
	char cmdbuf [BUFSIZ];
	register justtouch = 0;

	/* check for the "-t" flag" */
	for (; argc>1 && argv[1][0]=='-'; --argc, ++argv) {
		register char *p;

		for (p=argv[1]+1; *p; ++p) switch (*p) {
		case 't':
			++justtouch;
			break;
		case 'd':
			++debug;
			break;
		default:
			fprintf (stderr, "ranlib: unknown flag '%c'\n", *p);
			exit (1);
		}
	}

	while (--argc > 0) {
		fi = fopen (*++argv, "r");
		if (! fi) {
			fprintf (stderr, "ranlib: cannot open %s\n", *argv);
			continue;
		}
		if (fgeth (fi) != ARMAG) {
			fprintf (stderr, "not an archive: %s\n", *argv);
			continue;
		}
		if (justtouch) {
			register int len;

			fseek (fi, (long) W, 0);
			if (! fgetarhdr (fi, &archdr)) {
				fprintf (stderr, "malformed archive: %s\n",
					*argv);
				continue;
			}
			len = strlen (tempnm);
			if (strncmp (archdr.ar_name, tempnm, len)) {
				fprintf (stderr, "no symbol table: %s\n",
					*argv);
				continue;
			}
			fclose (fi);
			fixdate (*argv);
			continue;
		}
		new = tnum = 0;
		off = W;
		if (nextel (fi) == 0) {
			fclose (fi);
			continue;
		}
		do {
			register n;
			struct nlist sym;

			if (! strncmp (tempnm, archdr.ar_name, sizeof (archdr.ar_name)))
				continue;
			if (! fgethdr (fi, &exp))
				continue;
			if (N_BADMAG (exp))
				continue;
			if (! exp.a_syms) {
				fprintf (stderr,
					"ranlib: warning: %s(%s): no symbol table\n",
					*argv, archdr.ar_name);
				continue;
			}
			fseek (fi, 2*(exp.a_const + exp.a_text + exp.a_data), 1);
			for (;;) {
				n = fgetsym (fi, &sym);
				if (n == 0) {   /* malloc returned 0 */
					fprintf (stderr,
						"ranlib: out of memory\n");
					exit (1);
				}
				if (n == 1)     /* end of symtab */
					break;
				if ((sym.n_type & N_EXT) &&
				    (sym.n_type & N_TYPE) != N_UNDF)
					stash (&sym);
				else
					free (sym.n_name);
			}
		} while (nextel (fi));
		fixsize ();     /* update ran_off by length of __.SYMTAB */
		fclose (fi);
		fo = fopen (tempnm, "w");
		if (! fo) {
			fprintf (stderr, "can't create temporary\n");
			exit (1);
		}
		putrantab (fo);
		fclose (fo);
		if (new)
			sprintf (cmdbuf, "%s rlb %s %s %s",
				AR, firstname, *argv, tempnm);
		else
			sprintf (cmdbuf, "%s rl %s %s", AR, *argv, tempnm);
		if (system (cmdbuf))
			fprintf (stderr, "ranlib: ``%s'' failed\n", cmdbuf);
		else
			fixdate (*argv);
		unlink (tempnm);
	}
	return (0);
}

nextel (af)
FILE *af;
{
	oldoff = off;
	fseek (af, off, 0);
	if (! fgetarhdr (af, &archdr))
		return (0);
	arsize = (archdr.ar_size + W-1) / W * W;
	off = ftell (af) + arsize;
	return (1);
}

fixdate (s)             /* patch time */
char *s;
{
	register fd;

	fd = open (s, 2);
	if (fd < 0) {
		fprintf (stderr, "ranlib: can't reopen %s\n", s);
		return;
	}
	lseek (fd, (long) W, 0);
	getarhdr (fd, &archdr);
	lseek (fd, (long) W, 0);
	archdr.ar_date = time ((long *) 0);
	putarhdr (fd, &archdr);
	close (fd);
}

putrantab (f)
FILE *f;
{
	register struct ranlib *p;
	register n;

	n = 0;
	for (p=rantab; p<rantab+tnum; ++p) {
		if (debug)
			printf ("%08o: %3d  %s\n", p->ran_off,
				p->ran_len, p->ran_name);
		fputran (p, f);
		n += 5 + p->ran_len;
		free (p->ran_name);
	}
	tnum = 0;
	/* pad with nulls */
	do
		putc (0, f);
	while (++n % W);
}

stash (s)
struct nlist *s;
{
	if (tnum >= TABSZ) {
		fprintf (stderr, "ranlib: symbol table overflow\n");
		exit (1);
	}
	rantab[tnum].ran_name = s->n_name;
	rantab[tnum].ran_len = s->n_len;
	rantab[tnum].ran_off = oldoff;
	++tnum;
}

fixsize ()
{
	int i;
	long offdelta;

	offdelta = ARHDRSZ;
	for (i=0; i<tnum; ++i)
		offdelta += rantab[i].ran_len + 5;
	offdelta = (offdelta + W) / W * W;
	off = W;
	nextel (fi);
	if (! strncmp (archdr.ar_name, tempnm, sizeof (archdr.ar_name))) {
		new = 0;
		offdelta -= ARHDRSZ + arsize;
	} else {
		new = 1;
		strncpy (firstname, archdr.ar_name, sizeof (archdr.ar_name));
	}
	for (i=0; i<tnum; ++i)
		rantab[i].ran_off += offdelta;
}
