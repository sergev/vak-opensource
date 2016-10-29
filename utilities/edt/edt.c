# include <stdio.h>
# include <stdlib.h>
# include <sys/types.h>
# include <sys/stat.h>
# include "mem.h"
# include "rec.h"

# define EDTVERSION     "Besm-6 Monitor-86 Text Editor\nCopyright 1990 Demos/*\n\n"
# define TEMPFILE       "/tmp/edtXXXXXX"
# define CMDPROMPT      ": "
# define INPPROMPT      "-\b"
# define PORTION        15

struct cmdtab {
	char *name;
	int (*func) ();
	int (*cont) ();
	char *help;
};

REC *rec;                               /* current file */
int fildes;                             /* file descriptor */
int debug;                              /* -d flag */
int ttyin;                              /* stdin is a tty */
int curline;                            /* current line number */
int linelimit;                          /* line limit for list, etc */
int delim;                              /* current parameter delimiter */
int portion = PORTION;                  /* current portion */
char inputline [256];                   /* input command line */
char *params;                           /* parameters in inputline */
char findstring [100];                  /* string for find */
char replacestring [100];               /* string for replace */
char lastwfile [80];                    /* last written file name */
int lastwlen;                           /* last written file length */
char lastrfile [80];                    /* last read file name */
int lastrlen;                           /* last read file length */
int (*lastfun) ();                      /* action for empty command */
static char tempname [] = TEMPFILE;

extern char *mktemp (), *strcpy ();

static doend (), dolist (), dolength (), doiw (), doir (), doportion ();
static nextlist (), doread (), dowrite (), donew (), dohelp ();
static doinsert (), dodelete (), dochange (), docopy (), doshift ();
static docontext (), dofind (), nextfind (), nextreplace ();

static struct cmdtab table [] = {
	"context",      docontext,      nextreplace,    "Context n1-n2 'aa'bb'  - search and replace",
	"change",       dochange,       0,              "CHange n1-n2           - delete and input lines",
	"copy",         docopy,         0,              "COPy n1-n2-n3          - copy lines",
	"delete",       dodelete,       0,              "Delete n1-n2           - delete lines",
	"end",          doend,          0,              "End                    - exit from edt",
	"find",         dofind,         nextfind,       "Find n1-n2 'aa'        - search",
	"help",         dohelp,         0,              "Help command           - print help",
	"insert",       doinsert,       0,              "Insert n1              - input lines",
	"ir",           doir,           0,              "IR                     - print info about last reading",
	"iw",           doiw,           0,              "IW                     - print info about last writing",
	"list",         dolist,         nextlist,       "List n1-n2             - list lines",
	"length",       dolength,       0,              "LEngth                 - print current length of file",
	"new",          donew,          0,              "NEW                    - clear editor buffer",
	"new ", 0, 0, 0,
	"portion",      doportion,      0,              "PORtion                - change portion for list and find",
	"por ", 0, 0, 0,
	"read",         doread,         0,              "Read file              - read new file",
	"shift",        doshift,        0,              "Shift n1-n2-n3         - move lines",
	"update",       0,              0,              "Update n1              - visual editing of line",
	"write",        dowrite,        0,              "Write file             - write file",
	"quit",         doend,          0,              "Quit                   - exit from edt",
	0,              0,              0,              0,
};

static usage ()
{
	fprintf (stderr, "Usage: edt [-d] [files ...]\n");
}

/* VARARGS1 */

error (s, a)
char *s, *a;
{
	fprintf (stderr, s, a);
	fputc ('\n', stderr);
}

static char *readtemp ()
{
	char buf [1024];
	FILE *fd;
	register nr, nw;

	fd = fopen (tempname, "w");
	if (! fd) {
		perror (tempname);
		return (0);
	}
	for (;;) {
		nr = fread (buf, 1, sizeof (buf), stdin);
		if (nr < 0)
			perror ("stdin");
		if (nr <= 0)
			break;
		nw = fwrite (buf, 1, (unsigned) nr, fd);
		if (nw < nr) {
			perror (tempname);
			break;
		}
	}
	fclose (fd);
	return (tempname);
}

/* ARGSUSED */

main (argc, argv)
char **argv;
{
	register char *p;

	ttyin = isatty (0);
	if (ttyin)
		fprintf (stderr, EDTVERSION);
	for (++argv; **argv=='-'; ++argv) {
		for (p=1+*argv; *p; ++p) {
			switch (*p) {
			case 'd':
				++debug;
				break;
			default:
				usage ();
				return (1);
			}
		}
	}
	mktemp (tempname);
	if (*argv)
		for (; *argv; ++argv)
			edt (*argv);
	else if (ttyin) {
		close (creat (tempname, 0664));
		edt (tempname);
	} else
		edt (readtemp ());
	return (0);
}

edt (name)
char *name;
{
	if (! name)
		return;
	fildes = open (name, 2);
	if (fildes < 0) {
		perror (name);
		return;
	}
	rec = RecOpen (fildes, 1);
	if (rec) {
		strcpy (lastrfile, name);
		lastrlen = rec->len;
		curline = 0;
		if (name != tempname)
			doir ();
		commandloop ();
		RecClose (rec);
	}
	close (fildes);
}

getline ()
{
	register char *p, *q;
	register begline, c;

	if (! gets (inputline)) {
		inputline [0]= 0;
		return (0);
	}
	begline = 1;
	for (p=q=inputline; *p; ++p) {
		if ((*p == ' ' || *p == '\t') && begline)
			continue;
		begline = 0;
		if (*p == '\\') {
			++p;
			switch (*p) {
			case 'n':       *p = '\n';      break;
			case 'r':       *p = '\t';      break;
			case 't':       *p = '\t';      break;
			case 'e':       *p = '\e';      break;
			case 'f':       *p = '\f';      break;
			case '0': case '1': case '2': case '3':
			case '4': case '5': case '6': case '7':
				c = *p & 7;
				if (*(p+1)>='0' && *(p+1)<='7') {
					++p;
					c = c<<3 | *p & 7;
					if (*(p+1)>='0' && *(p+1)<='7') {
						++p;
						c = c<<3 | *p & 7;
					}
				}
				break;
			}
		}
		*q++ = *p;
	}
	*q = 0;
	return (1);
}

char *getcmd ()
{
	static char buf [20];
	register char *p, *q;

	for (p=inputline, q=buf; *p>='a' && *p<='z'; ++p)
		if (q < buf + sizeof (buf) - 1)
			*q++ = *p;
	*q = 0;
	params = p;
	getdelim ();
	return (buf);
}

char *getname ()
{
	static char buf [80];
	register char *q;
	register c;

	while (*params==' ' || *params=='\t')
		++params;
	if (! *params)
		return (0);
	for (q=buf; (c= *params)>' ' || c<0; ++params)
		if (q < buf + sizeof (buf) - 1)
			*q++ = c;
	*q = 0;
	getdelim ();
	return (buf);
}

char *getstr ()
{
	register delimiter;
	char *p;

	while (*params==' ' || *params=='\t')
		++params;
	if (! *params)
		return (0);
	delimiter = *params++;
	p = params;
	for (; *params != delimiter; ++params)
		if (! *params)
			return (0);
	*params++ = 0;
	getdelim ();
	return (p);
}

char *get2str (r)
char **r;
{
	register delimiter;
	char *p;

	while (*params==' ' || *params=='\t')
		++params;
	if (! *params)
		return (0);
	delimiter = *params++;
	p = params;
	for (; *params != delimiter; ++params)
		if (! *params)
			return (0);
	*params++ = 0;
	*r = params;
	for (; *params != delimiter; ++params)
		if (! *params)
			return (0);
	*params++ = 0;
	getdelim ();
	return (p);
}

getint (p)
int *p;
{
	register c;

	while (*params==' ' || *params=='\t')
		++params;
	if (*params>='0' && *params<='9') {
		c = 0;
		while (*params>='0' && *params<='9')
			c = c * 10 + *params++ - '0';
		*p = c;
		getdelim ();
		return (1);
	}
	getdelim ();
	return (0);
}

getdelim ()
{
	while (*params==' ' || *params=='\t')
		++params;
	switch (*params) {
	case '-': case ':': case ',': case '.': case '=':
		delim = *params++;
		break;
	default:
		delim = 0;
		break;
	}
}

struct cmdtab *findcmd (name)
char *name;
{
	register struct cmdtab *p;

	for (p=table; p->name; ++p)
		if (! strncmp (name, p->name, strlen (name)))
			return (p);
	return (0);
}

commandloop ()
{
	register char *p;
	register struct cmdtab *t;

	for (;;) {
		if (ttyin)
			printf (CMDPROMPT);
		if (! getline () && ! ttyin)
			return;
		if (! inputline [0]) {          /* empty command */
			if (lastfun)
				(*lastfun) ();
			continue;
		}
		p = getcmd ();
		if (! *p) {
			error ("bad command");
			continue;
		}
		t = findcmd (p);
		if (! t) {
			error ("%s: unknown command", p);
			continue;
		}
		if (! t->func) {
			error ("%s: command is not implemented yet", p);
			continue;
		}
		(*t->func) ();
		lastfun = t->cont;
	}
}

printline (p, len)
register char *p;
{
	register c;

	while (--len >= 0) {
		c = *p++ & 0377;
		if (c == '\t')
			putchar (c);
		else if (c < ' ' || c >= 0177 && c < 0300 || c >= 0377)
			printf ("\\%o", c);
		else
			putchar (c);
	}
}

static doend ()
{
	exit (0);
}

list (first, limit)
register limit;
{
	if (first < 0)
		first = 0;
	curline = first;
	if (limit > rec->len)
		limit = rec->len;
	linelimit = limit;
	nextlist ();
}

static nextlist ()
{
	register LINE *l;
	register limit;

	limit = curline + portion;
	if (limit > linelimit)
		limit = linelimit;
	for (; curline<limit; ++curline) {
		l = RecGet (rec, curline);
		printf ("%04d", curline + 1);
		if (l->len) {
			putchar ('\t');
			printline (l->ptr, l->len);
		}
		putchar ('\n');
	}
}

writefile (fd, line, limit)
{
	register LINE *l;

	if (limit > rec->len)
		limit = rec->len;
	for (; line<limit; ++line) {
		l = RecGet (rec, line);
		if (l->len)
			write (fd, l->ptr, (unsigned) l->len);
		write (fd, "\n", (unsigned) 1);
	}
}

static dolist ()
{
	int first, last;

	if (delim)
		if (getint (&last))
			list (curline, last);                   /* -bb */
		else
			list (curline, rec->len);               /* - */
	else if (getint (&first))
		if (getint (&last))
			list (first-1, last);                   /* aa bb */
		else
			list (first-1, rec->len);               /* aa */
	else
		list (curline, rec->len);                       /* */
}

static dolength ()
{
	printf ("%d lines\n", rec->len);
}

static doiw ()
{
	if (lastwfile [0])
		printf ("file \"%s\", %d lines\n", lastwfile, lastwlen);
}

static doir ()
{
	printf ("file \"%s\", %d lines\n", lastrfile, lastrlen);
}

static doportion ()
{
	int c;

	if (delim)
		portion = 9999;
	else if (getint (&c))
		portion = c;
	else
		portion = PORTION;
}

static doread ()
{
	register char *p;
	register fd;

	p = getname ();
	if (p) {
		fd = open (p, 2);
		if (fd < 0) {
			perror (p);
			return;
		}
		RecClose (rec);
		close (fildes);
		fildes = fd;
		strcpy (lastrfile, p);
	} else
		RecClose (rec);
	rec = RecOpen (fildes, 1);
	if (! rec)
		exit (1);
	lastrlen = rec->len;
	curline = 0;
}

samefile (r, w)
char *r, *w;
{
	struct stat rst, wst;

	if (stat (r, &rst) < 0)
		return (0);
	if (stat (w, &wst) < 0)
		return (0);
	return (rst.st_ino==wst.st_ino && rst.st_dev==wst.st_dev);
}

static dowrite ()
{
	register char *p;
	register fd;

	p = getname ();
	if (p && ! samefile (lastrfile, p)) {
		fd = creat (p, 0664);
		if (fd < 0) {
			perror (p);
			return;
		}
		writefile (fd, 0, rec->len);
		close (fd);
		strcpy (lastwfile, p);
		lastwlen = rec->len;
		doiw ();
		return;
	}
	RecSave (rec, lastrfile);
	strcpy (lastwfile, lastrfile);
	lastwlen = rec->len;
	doiw ();
}

static donew ()
{
	register fd;

	close (creat (tempname, 0664));
	fd = open (tempname, 2);
	if (fd < 0) {
		perror (tempname);
		return;
	}
	RecClose (rec);
	close (fildes);
	fildes = fd;
	rec = RecOpen (fildes, 1);
	if (! rec)
		exit (1);
	strcpy (lastrfile, tempname);
	lastrlen = rec->len;
	curline = 0;
}

static dohelp ()
{
	register struct cmdtab *p;

	for (p=table; p->name; ++p)
		if (p->func && p->help)
			puts (p->help);
}

static dodelete ()
{
	int first, last;

	if (! getint (&first)) {
		error ("bad parameter");
		return;
	}
	if (--first < 0)
		first = 0;
	if (! getint (&last))
		last = first+1;
	del (first, last-first+1);
}

static dochange ()
{
	int first, last;

	if (! getint (&first)) {
		error ("bad parameter");
		return;
	}
	if (--first < 0)
		first = 0;
	if (! getint (&last))
		last = first+1;
	del (first, last-first+1);
	ins ();
}

del (first, num)
{
	if (first < 0)
		first = 0;
	if (num > rec->len-first)
		num = rec->len-first;
	while (--num >= 0)
		RecDelLine (rec, first);
	curline = first;
}

static doinsert ()
{
	int first;

	if (getint (&first))
		curline = first-1;
	if (curline < 0)
		curline = 0;
	if (curline > rec->len)
		curline = rec->len;
	ins ();
}

ins ()
{
	register LINE *l;

	for (;;) {
		if (ttyin)
			printf (INPPROMPT);
		if (! getline () || inputline[0]=='.' && inputline[1]==0)
			break;
		RecInsLine (rec, curline);
		l = RecGet (rec, curline);
		if (l->len)
			MemFree (l->ptr);
		l->len = strlen (inputline);
		l->ptr = MemAlloc (l->len);
		MemCopy (l->ptr, inputline, l->len);
		RecPut (l, l->len);
		++curline;
	}
}

copy (to, from, n)
{
	register LINE *f, *t;

	for (; n>0; --n, ++to, ++from) {
		f = RecGet (rec, from);
		t = RecGet (rec, to);
		if (t->len)
			MemFree (t->ptr);
		if (f->len) {
			t->ptr = MemAlloc (f->len);
			MemCopy (t->ptr, f->ptr, f->len);
		}
		RecPut (t, f->len);
	}
}

static docopy ()
{
	int n1, n2, n3;
	register n;

	if (! getint (&n1) || ! getint (&n2) || ! getint (&n3)) {
badpar:         error ("bad parameter");
		return;
	}
	if (--n1 < 0)
		n1 = 0;
	if (--n2 < 0)
		n1 = 0;
	if (--n3 < 0)
		n1 = 0;
	if (n2 < n1)
		n = n1, n1 = n2, n2 = n;
	if (n1 >= rec->len)
		goto badpar;
	for (n=n1; n<=n2; ++n)
		RecInsLine (rec, n3);
	n = n2 - n1 + 1;
	if (n3 > n2)
		copy (n3, n1, n);
	else if (n3 <= n1)
		copy (n3, n1+n, n);
	else {
		copy (n3, n1, n3-n1);
		copy (n3+n3-n1, n3+n, n-n1+n3);
	}
}

static doshift ()
{
	int n1, n2, n3;
	register n;

	if (! getint (&n1) || ! getint (&n2) || ! getint (&n3)) {
badpar:         error ("bad parameter");
		return;
	}
	if (--n1 < 0)
		n1 = 0;
	if (--n2 < 0)
		n1 = 0;
	if (--n3 < 0)
		n1 = 0;
	if (n2 < n1)
		n = n1, n1 = n2, n2 = n;
	if (n1 >= rec->len)
		goto badpar;
	for (n=n1; n<=n2; ++n)
		RecInsLine (rec, n3);
	n = n2 - n1 + 1;
	if (n3 > n2) {
		copy (n3, n1, n);
		del (n1, n);
	} else if (n3 <= n1) {
		copy (n3, n1+n, n);
		del (n1+n, n);
	} else {
		copy (n3, n1, n3-n1);
		copy (n3+n3-n1, n3+n, n-n1+n3);
		del (n3+n, n-n1+n3);
		del (n1, n3-n1);
	}
}

static dofind ()
{
	int first, last;
	register char *p;

	if (delim) {
		first = 0;
		if (! getint (&last))
			last = rec->len;
	} else if (getint (&first)) {
		if (--first < 0)
			first = 0;
		if (delim) {
			if (! getint (&last))
				last = rec->len;
		} else if (! getint (&last))
			last = first+1;
	} else {
		first = 0;
		last = rec->len;
	}
	p = getstr ();
	if (! p) {
		error ("bad parameter");
		return;
	}
	curline = first;
	linelimit = last;
	strcpy (findstring, p);
	replacestring [0] = 0;
	nextfind ();
}

static docontext ()
{
	int first, last;
	register char *p;
	char *r;

	if (delim) {
		first = 0;
		if (! getint (&last))
			last = rec->len;
	} else if (getint (&first)) {
		if (--first < 0)
			first = 0;
		if (delim) {
			if (! getint (&last))
				last = rec->len;
		} else if (! getint (&last))
			last = first+1;
	} else {
		first = 0;
		last = rec->len;
	}
	p = get2str (&r);
	if (! p) {
		error ("bad parameter");
		return;
	}
	curline = first;
	linelimit = last;
	strcpy (findstring, p);
	strcpy (replacestring, r);
	nextreplace ();
}

static nextfind ()
{
	register LINE *l;
	register n;

	for (n=0; n<portion && curline<linelimit; ++curline) {
		l = RecGet (rec, curline);
		if (! match (l->ptr, l->len, findstring))
			continue;
		printf ("%04d", curline + 1);
		if (l->len) {
			putchar ('\t');
			printline (l->ptr, l->len);
		}
		putchar ('\n');
		++n;
	}
}

static nextreplace ()
{
	register LINE *l;
	register n;
	char *newptr;
	int newlen;

	for (n=0; n<portion && curline<linelimit; ++curline) {
		l = RecGet (rec, curline);
		if (! replace (l->ptr, l->len, findstring, replacestring, &newptr, &newlen))
			continue;
		l->ptr = newptr;
		l->len = newlen;
		RecPut (l, l->len);
		printf ("%04d", curline + 1);
		if (l->len) {
			putchar ('\t');
			printline (l->ptr, l->len);
		}
		putchar ('\n');
		++n;
	}
}

match (ptr, len, str)
char *ptr, *str;
{
	register i, lim, slen;

	slen = strlen (str);
	lim = len - slen + 1;
	for (i=0; i<lim; ++i, ++ptr)
		if (! MemCompare (ptr, str, slen))
			return (1);
	return (0);
}

replace (ptr, len, str, repl, newptr, newlen)
char *ptr, *str, *repl, **newptr;
int *newlen;
{
	register i, lim, slen, rlen, changed;
	char *p;

	slen = strlen (str);
	rlen = strlen (repl);
	lim = len - slen + 1;
	changed = 0;
	for (i=0; i<lim; ++i)
		if (! MemCompare (ptr+i, str, slen)) {
			p = MemAlloc (len + rlen - slen);
			if (i)
				MemCopy (p, ptr, i);
			MemCopy (p+i, repl, rlen);
			if (i < len-slen)
				MemCopy (p+i+rlen, ptr+i+slen, len-i-slen);
			MemFree (ptr);
			ptr = p;
			len += rlen - slen;
			i += rlen - 1;
			changed |= 1;
		}
	*newptr = ptr;
	*newlen = len;
	return (changed);
}
