/*
 *      int CapInit (buf)
 *      char buf [1024];
 *
 *              - get termcap entry into buffer buf.
 *              Return 1 if ok, else 0.
 *
 *      CapGet (tab, buf)
 *      struct CapTab *tab;
 *      char *buf;
 *
 *              - read terminal properties, described in table tab.
 *              Buffer used as argument to CapInit must be available.
 *              Store strings in buffer buf.
 *
 *      char *CapGoto (movestr, col, line)
 *      char *movestr;
 *
 *              - expand string movestr by column and line numbers.
 */

# ifdef __MSDOS__
CapInit () { return (1); }
# else

# include "cap.h"

# define BUFSIZ         2048            /* max length of termcap entry */
# define MAXHOP         32              /* max number of tc= indirections */
# define MAXRETURNSIZE  64              /* max length of CapGoto string */
# define CTRL(c)        (c & 037)
# define ERR(c)         (c)
# define TPARMERR(c)    { strcpy (outp, (c)); return; }

# define TERMCAP        "/etc/termcap"
# define MYTERMCAP      "/usr/lib/deco/termcap"

# define push(i)        (stack [++top] = (i))
# define pop()          (stack [top--])

static char *tname;                     /* terminal name */
static char *tbuf;                      /* terminal entry buffer */
static char *envtermcap;                /* global variable TERMCAP */
static hopcount;                        /* detect infinite loops */

static char *tskip (), *tdecode ();
extern char *strcpy (), *strcat (), *getenv (), *mallo ();

/*
 * Get an entry for terminal name in buffer bp,
 * from the termcap file.  Parse is very rudimentary;
 * we just notice escaped newlines.
 */

CapInit (bp)
char *bp;
{
	if (tbuf)
		return (1);
	if (! (tname = getenv ("TERM")))
		tname = "unknown";
# ifndef DEMO
	if (! envtermcap)
		envtermcap = getenv ("TERMCAP");
	if (! envtermcap)
		envtermcap = "";
# endif
# ifdef DEMO
	return (tgetent (bp, tname, MYTERMCAP));
# else
	return (tgetent (bp, tname, MYTERMCAP) || tgetent (bp, tname, TERMCAP));
# endif
}

static tgetent (bp, name, termcap)
char *bp, *name, *termcap;
{
	register char *cp;
	register c, i = 0, cnt = 0;
	char *ibuf;
	int tf;

	tbuf = bp;
	tf = -1;
	cp = envtermcap;
	/*
	 * TERMCAP can have one of two things in it. It can be the
	 * name of a file to use instead of /etc/termcap. In this
	 * case it must start with a "/". Or it can be an entry to
	 * use so we don't have to read the file. In this case it
	 * has to already have the newlines crunched out.
	 */
	if (cp && *cp) {
		if (*cp == '/') {
			tf = open (cp, 0);
		} else {
			tbuf = cp;
			c = tnamatch (name);
			tbuf = bp;
			if (c) {
				strcpy (bp,cp);
				return (tnchktc ());
			}
		}
	}
	if (tf < 0)
		tf = open (termcap, 0);
	if (tf < 0)
		return (0);
	ibuf = mallo (BUFSIZ);
	for (;;) {
		cp = bp;
		for (;;) {
			if (i == cnt) {
				cnt = read (tf, ibuf, BUFSIZ);
				if (cnt <= 0) {
					close(tf);
					mfree (ibuf, BUFSIZ);
					return (0);
				}
				i = 0;
			}
			c = ibuf[i++];
			if (c == '\n') {
				if (cp > bp && cp[-1] == '\\'){
					cp--;
					continue;
				}
				break;
			}
			if (cp >= bp+BUFSIZ) {
				outerr ("Termcap entry too long\n");
				break;
			} else
				*cp++ = c;
		}
		*cp = 0;
		mfree (ibuf, BUFSIZ);

		/*
		 * The real work for the match.
		 */
		if (tnamatch (name)) {
			close (tf);
			return (tnchktc ());
		}
	}
}

/*
 * tnchktc: check the last entry, see if it's tc=xxx. If so,
 * recursively find xxx and append that entry (minus the names)
 * to take the place of the tc=xxx entry. This allows termcap
 * entries to say "like an HP2621 but doesn't turn on the labels".
 * Note that this works because of the left to right scan.
 */
static tnchktc ()
{
	register char *p, *q;
	char tcname [16];       /* name of similar terminal */
	char *tcbuf;
	char *holdtbuf = tbuf;
	int l;

	p = tbuf + strlen(tbuf) - 2;	/* before the last colon */
	while (*--p != ':')
		if (p<tbuf) {
			outerr ("Bad termcap entry\n");
			return (0);
		}
	p++;
	/* p now points to beginning of last field */
	if (p[0] != 't' || p[1] != 'c')
		return (1);
	strcpy (tcname,p+3);
	q = tcname;
	while (*q && *q != ':')
		q++;
	*q = 0;
	if (++hopcount > MAXHOP) {
		outerr ("Infinite tc= loop\n");
		return (0);
	}
	tcbuf = mallo (BUFSIZ);
	if (! tgetent (tcbuf, tcname, TERMCAP)) {
		hopcount = 0;		/* unwind recursion */
		mfree (tcbuf, BUFSIZ);
		return (0);
	}
	for (q=tcbuf; *q != ':'; q++);
	l = p - holdtbuf + strlen(q);
	if (l > BUFSIZ) {
		outerr ("Termcap entry too long\n");
		q [BUFSIZ - (p-tbuf)] = 0;
	}
	strcpy (p, q+1);
	tbuf = holdtbuf;
	hopcount = 0;			/* unwind recursion */
	mfree (tcbuf, BUFSIZ);
	return (1);
}

/*
 * Tnamatch deals with name matching.  The first field of the termcap
 * entry is a sequence of names separated by |'s, so we compare
 * against each such name.  The normal : terminator after the last
 * name (before the first field) stops us.
 */

static tnamatch (np)
char *np;
{
	register char *Np, *Bp;

	Bp = tbuf;
	if (*Bp == '#')
		return (0);
	for (;;) {
		for (Np = np; *Np && *Bp == *Np; Bp++, Np++)
			continue;
		if (*Np == 0 && (*Bp == '|' || *Bp == ':' || *Bp == 0))
			return (1);
		while (*Bp && *Bp != ':' && *Bp != '|')
			Bp++;
		if (*Bp == 0 || *Bp == ':')
			return (0);
		Bp++;
	}
}

/*
 * Skip to the next field.  Notice that this is very dumb, not
 * knowing about \: escapes or any such.  If necessary, :'s can be put
 * into the termcap file in octal.
 */

static char *tskip (bp)
register char *bp;
{

	while (*bp && *bp != ':')
		bp++;
	if (*bp == ':')
		bp++;
	return (bp);
}

CapGet (t, area)
struct CapTab *t;
char *area;
{
	register char *bp;
	register struct CapTab *p;
	register i, base;
	char name [2];

	if (! tbuf)
		return;
	bp = tbuf;
	for (;;) {
		bp = tskip(bp);
		if (! bp[0] || ! bp[1])
			return;
		if (bp[0] == ':' || bp[1] == ':')
			continue;
		name[0] = *bp++;
		name[1] = *bp++;
		for (p=t; p->tname[0]; ++p)
			if (p->tname[0] == name[0] && p->tname[1] == name[1])
				break;
		if (! p->tname[0] || p->tdef)
			continue;
		p->tdef = 1;
		if (*bp == '@')
			continue;
		switch (p->ttype) {
		case CAPNUM:
			if (*bp != '#')
				continue;
			bp++;
			base = 10;
			if (*bp == '0')
				base = 8;
			i = 0;
			while (*bp>='0' && *bp<='9')
				i = i * base, i += *bp++ - '0';
			*(p->ti) = i;
			break;
		case CAPFLG:
			if (*bp && *bp != ':')
				continue;
			*(p->tc) = 1;
			break;
		case CAPSTR:
			if (*bp != '=')
				continue;
			bp++;
			*(p->ts) = tdecode (bp, &area);
			break;
		}
	}
}

/*
 * Tdecode does the grung work to decode the
 * string capability escapes.
 */

static char *tdecode (str, area)
register char *str;
char **area;
{
	register char *cp;
	register int c;
	register char *dp;
	int i;

	cp = *area;
	while ((c = *str++) && c != ':') {
		switch (c) {

		case '^':
			c = *str++ & 037;
			break;

		case '\\':
			dp = "E\033^^\\\\::n\nr\rt\tb\bf\f";
			c = *str++;
nextc:
			if (*dp++ == c) {
				c = *dp++;
				break;
			}
			dp++;
			if (*dp)
				goto nextc;
			if (c>='0' && c<='9') {
				c -= '0', i = 2;
				do
					c <<= 3, c |= *str++ - '0';
				while (--i && *str>='0' && *str<='9');
			}
			break;
		}
		*cp++ = c;
	}
	*cp++ = 0;
	str = *area;
	*area = cp;
	return (str);
}

/*
 * Routine to perform cursor addressing.
 * CM is a string containing printf type escapes to allow
 * cursor addressing.  We start out ready to print the destination
 * line, and switch each time we print row or column.
 * The following escapes are defined for substituting row/column:
 *
 *	%d	as in printf
 *	%2	like %2d
 *	%3	like %3d
 *	%.	gives %c hacking special case characters
 *	%+x	like %c but adding x first
 *
 *	The codes below affect the state but don't use up a value.
 *
 *	%>xy	if value > x add y
 *	%r	reverses row/column
 *	%i	increments row/column (for one origin indexing)
 *	%%	gives %
 *	%B	BCD (2 decimal digits encoded in one byte)
 *	%D	Delta Data (backwards bcd)
 *
 * all other characters are ``self-inserting''.
 */

char *CapGoto (CM, destcol, destline)
char *CM;
{
	register char *cp, *dp;
	register c, which, oncol;
	static char result [MAXRETURNSIZE];

	cp = CM;
	if (! cp)
toohard:        return ERR ("bad capgoto");
	dp = result;
	oncol = 0;
	which = destline;
	while (c = *cp++) {
		if (c != '%') {
			*dp++ = c;
			continue;
		}
		switch (c = *cp++) {
		case 'n':
			destcol ^= 0140;
			destline ^= 0140;
			goto setwhich;
		case 'd':
			if (which < 10)
				goto one;
			if (which < 100)
				goto two;
			/* fall into... */
		case '3':
			*dp++ = (which / 100) | '0';
			which %= 100;
			/* fall into... */
		case '2':
two:                    *dp++ = which / 10 | '0';
one:                    *dp++ = which % 10 | '0';
swap:                   oncol = 1 - oncol;
setwhich:               which = oncol ? destcol : destline;
			continue;
		case '>':
			if (which > *cp++)
				which += *cp++;
			else
				cp++;
			continue;
		case '+':
			which += *cp++;
			/* fall into... */
		case '.':
			*dp++ = which;
			goto swap;
		case 'r':
			oncol = 1;
			goto setwhich;
		case 'i':
			destcol++;
			destline++;
			which++;
			continue;
		case '%':
			*dp++ = c;
			continue;
		case '/':
			c = *cp;
			*dp++ = which / c | '0';
			which %= *cp++;
			continue;
		case 'B':
			which = (which/10 << 4) + which%10;
			continue;
		case 'D':
			which = which - 2 * (which%16);
			continue;
		case 'p':
		case 'P':
		case '\'':
		case '{':
		case '?':
			/* string is in tparm format... */
			tparm (dp, cp-2, destcol, destline);
			return (result);
		default:
			goto toohard;
		}
	}
	*dp = 0;
	return (result);
}

static char *branchto (cp, to)
register char *cp;
{
	register level = 0;
	register c;

	while (c = *cp++) {
		if (c == '%') {
			if ((c = *cp++) == to || c == ';') {
				if (level == 0) {
					return cp;
				}
			}
			if (c == '?')
				level++;
			if (c == ';')
				level--;
		}
	}
	return ERR("no matching ENDIF");
}

/*
 * Routine to perform parameter substitution.
 * instring is a string containing printf type escapes.
 * The whole thing uses a stack, much like an HP 35.
 * The following escapes are defined for substituting row/column:
 *
 *	%d	print pop() as in printf
 *	%[0]2d	print pop() like %[0]2d
 *	%[0]3d	print pop() like %[0]3d
 *	%c	print pop() like %c
 *      %s (!)  print pop() like %s
 *      %l (!)  pop() a string and push its length.
 *
 *	%p[1-0]	push ith parm
 *	%P[a-z] set variable
 *	%g[a-z] get variable
 *	%'c'	char constant c
 *	%{nn}	integer constant nn
 *
 *	%+ %- %* %/ %m		arithmetic (%m is mod): push(pop() op pop())
 *	%& %| %^		bit operations:		push(pop() op pop())
 *	%= %> %<		logical operations:	push(pop() op pop())
 *	%! %~			unary operations	push(op pop())
 *	%b			unary BCD conversion
 *	%d			unary Delta Data conversion
 *
 *	%? expr %t thenpart %e elsepart %;
 *				if-then-else, %e elsepart is optional.
 *				else-if's are possible ala Algol 68:
 *				%? c1 %t %e c2 %t %e c3 %t %e c4 %t %e %;
 *
 * all other characters are ``self-inserting''.  %% gets % output.
 *
 * (!) not implemented for now
 */

static tparm (outp, instring, p1, p2)
register char *outp;
char *instring;
{
	register char *cp;
	register c, op;
	int vars [26], stack [10], top, sign, onrow, leadzero;
	char *xp;

	if (instring == 0)
		TPARMERR ("null arg");
	top = 0;
	cp = instring;
	onrow = 0;
	leadzero = 0;   /* not having leading zero is unimplemented */
	while (c = *cp++) {
		if (c != '%') {
			*outp++ = c;
			continue;
		}
		op = stack [top];
		if (*cp == '0') {
			leadzero = 1;
			cp++;
		}
		switch (c = *cp++) {

		/* PRINTING CASES */
		case 'd':
			if (op < 10)
				goto one;
			if (op < 100)
				goto two;
			/* fall into... */

		case '3':
three:
			if (c == '3' && *cp++ != 'd')
				TPARMERR ("bad char after %3");
			*outp++ = (op / 100) | '0';
			op %= 100;
			/* fall into... */

		case '2':
			if (op >= 100)
				goto three;
			if (c == '2' && *cp++ != 'd')
				TPARMERR ("bad char after %2");
two:	
			*outp++ = op / 10 | '0';
one:
			*outp++ = op % 10 | '0';
			(void) pop ();
			continue;

		case 'c':
			*outp++ = op;
			(void) pop ();
			break;
# ifdef notdef
		case 'l':
			xp = (char *) pop ();
			push (strlen (xp));
			break;

		case 's':
			xp = (char *) pop ();
			while (*xp)
				*outp++ = *xp++;
			break;
# endif
		case '%':
			*outp++ = c;
			break;

		/*
		 * %i: shorthand for increment first two parms.
		 * Useful for terminals that start numbering from
		 * one instead of zero (like ANSI terminals).
		 */
		case 'i':
			p1++; p2++;
			break;

		/* %pi: push the ith parameter */
		case 'p':
			switch (c = *cp++) {
			case '1': push (p1); break;
			case '2': push (p2); break;
			default: TPARMERR ("bad parm number");
			}
			onrow = (c == '1');
			break;
		
		/* %Pi: pop from stack into variable i (a-z) */
		case 'P':
			vars [*cp++ - 'a'] = pop ();
			break;
		
		/* %gi: push variable i (a-z) */
		case 'g':
			push (vars [*cp++ - 'a']);
			break;
		
		/* %'c' : character constant */
		case '\'':
			push (*cp++);
			if (*cp++ != '\'')
				TPARMERR ("missing closing quote");
			break;
		
		/* %{nn} : integer constant.  */
		case '{':
			op = 0; sign = 1;
			if (*cp == '-') {
				sign = -1;
				cp++;
			} else if (*cp == '+')
				cp++;
			while ((c = *cp++) >= '0' && c <= '9') {
				op = 10*op + c - '0';
			}
			if (c != '}')
				TPARMERR ("missing closing brace");
			push (sign * op);
			break;
		
		/* binary operators */
		case '+': c = pop (); op = pop (); push (op + c); break;
		case '-': c = pop (); op = pop (); push (op - c); break;
		case '*': c = pop (); op = pop (); push (op * c); break;
		case '/': c = pop (); op = pop (); push (op / c); break;
		case 'm': c = pop (); op = pop (); push (op % c); break; /* %m: mod */
		case '&': c = pop (); op = pop (); push (op & c); break;
		case '|': c = pop (); op = pop (); push (op | c); break;
		case '^': c = pop (); op = pop (); push (op ^ c); break;
		case '=': c = pop (); op = pop (); push (op = c); break;
		case '>': c = pop (); op = pop (); push (op > c); break;
		case '<': c = pop (); op = pop (); push (op < c); break;

		/* Unary operators. */
		case '!': stack [top] = ! stack [top]; break;
		case '~': stack [top] = ~ stack [top]; break;
		/* Sorry, no unary minus, because minus is binary. */

		/*
		 * If-then-else.  Implemented by a low level hack of
		 * skipping forward until the match is found, counting
		 * nested if-then-elses.
		 */
		case '?':	/* IF - just a marker */
			break;

		case 't':	/* THEN - branch if false */
			if (! pop ())
				cp = branchto (cp, 'e');
			break;

		case 'e':	/* ELSE - branch to ENDIF */
			cp = branchto (cp, ';');
			break;

		case ';':	/* ENDIF - just a marker */
			break;

		default:
			TPARMERR ("bad % sequence");
		}
	}
	*outp = 0;
}
# endif /* MSDOS */
