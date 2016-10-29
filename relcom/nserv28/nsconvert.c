/*
 * Преобразование из старого (текстового) формата
 * таблицы подписки в новый (GDBM).
 */

# include <stdio.h>
# include "groups.h"

# define MAXUSERS 10000
# define LINESZ (6*MAXUSERS+256)

char *progname;
int verbose = 1;

long usertab [MAXUSERS];
int numusers;

extern char *malloc ();

usage ()
{
	fprintf (stderr, "Usage: %s [-vs] [txtgroups]\n", progname);
	exit (-1);
}

/* VARARGS1 */
messg (s, a, b, c, d)
char *s, *a, *b, *c, *d;
{
	fprintf (stderr, s, a, b, c, d);
	fprintf (stderr, "\n");
}


/* VARARGS1 */
error (s, a, b, c, d)
char *s, *a, *b, *c, *d;
{
	fprintf (stderr, s, a, b, c, d);
	fprintf (stderr, "\n");
}

quit ()
{
	exit (-1);
}

main (argc, argv)
char **argv;
{
	char *txtfile;

	progname = *(argv++);
	for (--argc; *argv && **argv=='-'; ++argv, --argc) {
		register char *p;

		for (p=1+*argv; *p; ++p)
			switch (*p) {
			case 'v':
				++verbose;
				break;
			case 's':
				verbose = 0;
				break;
			default:
				usage ();
			}
	}
	if (argc > 1)
		usage ();
	if (argc > 0) {
		txtfile = *argv;
		if (! freopen (txtfile, "r", stdin)) {
			perror (txtfile);
			return (-1);
		}
		if (verbose)
			printf ("%s opened\n", txtfile);
	}
loop:	switch (loadgroups (0)) {
	case -1:
		printf ("Groups database locked, waiting...\n");
		sleep (2);
		goto loop;
	case 0:
		return (-1);
	case 1:
		break;
	}
	if (verbose)
		printf ("Groups database opened\n");
	groupslimit (1024);
	convert ();
	groupslimit (0);
	groupssync ();
	savegroups ();
	if (verbose)
		printf ("Groups database closed\n");
	return (0);
}

convert ()
{
	char *line;
	char buf [80];
	int numgroups;

	numusers = 0;
	numgroups = 0;

	/* If groups file does not exist, just return. */

	line = malloc (LINESZ);
	if (! line) {
		error ("out of memory in loadgroups");
		return (0);
	}

	/* Load user table */

	for (;;) {
		int pack;

		if (! fgets (line, LINESZ, stdin)) {
			line [0] = 0;
			break;
		}
		if (line [0] == 0 || line [0] == '#')
			continue;
		if (line [0] != 'u')
			break;
		if (numusers >= MAXUSERS) {
			error ("maximum number of users exceeded");
			goto err;
		}
		sscanf (line, "u %s %d", buf, &pack);
		usertab [numusers] = adduser (buf);
		setuserflags (usertab [numusers], (long) pack);
		if (verbose > 1)
			printf ("User %s added%s\n", buf,
				pack ? " (pack)" : "");
		++numusers;
	}
	if (verbose)
		printf ("%d users loaded\n", numusers);

	/* Load group table */

	for (;;) {
		long g, last;

		if (line [0] == 0 || line [0] == '#')
			goto nextline;
		if (line [0] != 'g')
			break;
		sscanf (line, "g %s %ld", buf, &last);
		g = addgroup (buf, last);
		if (verbose > 1)
			printf ("Group %s ", buf);
		if (! cvtsubscr (g))
			goto err;
		if (verbose > 1)
			printf ("\n", buf);
		++numgroups;
nextline:
		if (! fgets (line, LINESZ, stdin)) {
			line [0] = 0;
			break;
		}
	}
	if (line [0]) {
		error ("bad format of groups file");
		goto err;
	}
	if (verbose)
		printf ("%d groups loaded\n", numgroups);
	free (line);
	return (1);
err:
	if (verbose)
		printf ("%d groups loaded\n", numgroups);
	free (line);
	return (0);
}

cvtsubscr (g)
long g;
{
	short *uindex;
	char *umode;
	struct subscrtab *subscr;
	int n, indx, c, sym;

	if (! numusers) {
		while ((sym = getchar ()) >= 0 && sym != '\n');
		return (1);
	}
	uindex = (short *) malloc ((unsigned) (numusers * sizeof (short)));
	umode = malloc ((unsigned) numusers);
	if (! uindex || !umode) {
		error ("out of memory in parsesubscr");
		while ((sym = getchar ()) >= 0 && sym != '\n');
		return (0);
	}
	n = 0;		/* след. ячейка в uindex */
	indx = 0;	/* номер адресата */
	for (sym=getchar(); sym>=0 && sym!='\n'; sym=getchar()) {
		switch (sym) {
		case 'A': case 'B': case 'C': case 'D': case 'E':
		case 'F': case 'G': case 'H': case 'I': case 'J':
		case 'K': case 'f': case 's':
			if (indx >= numusers) {
				error ("index out of range: group %s, %d > %d", 
					groupname (g), indx, numusers);
				free ((char *) uindex);
				free (umode);
				return(0);
			}
			uindex [n] = indx;
			umode [n] = sym;
			++n;
			++indx;
			break;
		case '.':
			++indx;
			break;
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			c = sym - '0';
			sym = getchar ();
			while (sym>='0' && sym<='9') {
				c = c*10 + sym - '0';
				sym = getchar ();
			}
			ungetc (sym, stdin);
			indx += c;
			break;
		case '\n': case '\t': case ' ':
			break;
		default:
			error ("bad subscription format: %c", sym);
			break;
		}
	}
	subscr = (struct subscrtab *) malloc (n * sizeof (struct subscrtab));
	if (! subscr) {
		error ("out of memory in parsesubscr!");
		free ((char *) uindex);
		free (umode);
		return (0);
	}
	for (c=0; c<n; ++c) {
		subscr[c].tag = usertab[uindex[c]];
		subscr[c].mode = umode[c];
	}
	free ((char *) uindex);
	free (umode);

	setsubscr (g, subscr, n);
	if (verbose>1 && n)
		printf ("- %d users subscribed", n);

	free ((char *) subscr);
	return (1);
}
