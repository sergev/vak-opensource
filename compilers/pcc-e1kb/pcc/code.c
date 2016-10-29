/*
 *      Сделано в МФТИ.
 *
 *      Чет Фев 11 11:39:15 MSK 1988    Вакуленко С. В.
 */

# include "mfile1"

int strftn = 0;	/* is the current function one which returns a value */

int Lflag;      /* разрешить встроенные функции */

branch (n)
{
	/* Output a branch to label n.
	   Exception is an ordinary
	   function branching to retlab;
	   then, return.
	*/
	if (n == retlab && !strftn)
	    printf ("\tuj\tcret\n");
	else
	    printf ("\tuj\tL%d\n", n);
}

int lastloc = PROG;

defalign (n)
{
	/* cause the alignment to become a multiple of n */
}

locctr (l)
register l;
{
	register temp;
	/* l is PROG, ADATA, DATA, STRNG, ISTRNG, or STAB */

	temp = lastloc;
	switch (l) {
	case PROG:
		if (lastloc == PROG) return (l);
		printf ("\t.text\n");
		lastloc = PROG;
		break;
	case DATA:
	case ADATA:
		if (lastloc == DATA) return (l);
		printf ("\t.data\n");
		lastloc = DATA;
		break;
	case STRNG:
	case ISTRNG:
		if (lastloc == STRNG) return (l);
		printf ("\t.strng\n");
		lastloc = STRNG;
		break;
	case STAB:
		cerror ("locctr: STAB unused");
		break;
	default:
		cerror ("illegal location counter");
	}
	return (temp);
}

deflab (n)
{
	/* define the current position as label n */

	printf ("L%d:\n", n);
}

beg_file ()
{
	/* do nothing special at beginning of file */
}

int crslab = 10;

getlab ()
{
	/* return a number usable for a label */
	return (++crslab);
}

efcode ()
{
	/* код конца функции */

	if (strftn) {
		/* Признак функции, возвращающей
		   значение типа struct или union.
		   Нужно скопировать результат;
		   его адрес - на сумматоре.
		*/
		register struct symtab *p;
		register int stlab, size;

		p = &stab[curftn];

		deflab (retlab);

		stlab = getlab ();
		size = tsize (DECREF(p->stype),
			p->dimoff, p->sizoff) / SZINT;
		printf ("\t.bss\nL%d:\t.=.+%d\n\t.text\n", stlab, size);
		printf ("\tati\t1\n:\tvtm\tL%d,2\n", stlab);
		printf ("\tdo\t%d\n", size-1);
		printf (":\txta\t,1++\n\tatx\t,2++\n");
		printf ("\txta\t#L%d\n", stlab);
		/* Выключим признак, чтобы больше не повторяться. */
		strftn = 0;
	}
	branch (retlab);
# ifdef ONEPASS
	p2bend ();
# else
	printf ("]\n");
# endif
}

bfcode (a, n)
int a[];
{
	/* code for the beginning of a function; a is an array of
	   indices in stab for the arguments; n is the number */
	register i;
	register temp;
	register struct symtab *p;
	int off;

	locctr (PROG);
	p = &stab[curftn];
	defnam (p);
	temp = p->stype;
	temp = DECREF (temp);
	strftn = (temp==STRTY) || (temp==UNIONTY);

	retlab = getlab();

	/* routine prolog */

	printf ("\tits\t14\n\tvjm\tcsv,14\n");
	/* adjust stack for autos */
	printf ("\tutm\tF%d,15\n", ftnno);

	off = ARGINIT;

	for (i=0; i<n; ++i) {
		p = &stab[a[i]];
		if (p->sclass == REGISTER) {
			temp = p->offset;       /* save register number */
			p->sclass = PARAM;      /* forget that it is a register */
			p->offset = NOOFFSET;
			oalloc (p, &off);
			printf ("\tvtm\t[%d,12],%d\n", p->offset/SZINT, temp);
			p->offset = temp;       /* remember register number */
			p->sclass = REGISTER;   /* remember that it is a register */
		} else {
			if (oalloc (p, &off)) cerror ("bad argument");
		}
	}
}

bccode ()
{
	/* Called just before the first executable statment.
	   By now, the automatics and
	   register variables are allocated.
	*/
	SETOFF (autooff, SZINT);
	/* set aside store area offset */
# ifdef ONEPASS
	p2bbeg (autooff, regvar);
# else
	printf ("[%d\t%d\t%d\t\n", ftnno, autooff, regvar);
# endif
}

ejobcode (flag)
{
	/* called just before final exit */
	/* flag is 1 if errors, 0 if none */
}

aobeg ()
{
	/* called before removing automatics from stab */
}

aocode (p)
struct symtab *p;
{
	/* called when automatic p removed from stab */
}

aoend ()
{
	/* called after removing all automatics from stab */
}

defnam (p)
register struct symtab *p;
{
	/* define the current location as the name p->sname */

	if (p->sclass == EXTDEF)
		printf ("\t.globl\t%s\n", exname (p->sname));
	if (p->sclass == STATIC && p->slevel>1) deflab (p->offset);
	else printf ("%s:\n", exname (p->sname));
}

bycode (t, i)
{
	/* put byte i+1 in a string */

	if (t < 0) {
		printf ("\"\n");        /* end of the string */
		return;
	}
	if (! i) printf ("\t.ascii\t\"");
	if (t == '\n') printf ("\\n");
	else if (t == '\\') printf ("\\\\");
	else if (t == '"') printf ("\\\"");
	else if (t == '\0') printf ("\\0");
	else printf ("%c", t);
}

zecode (n)
{
	/* n integer words of zeros */
	if (n <= 0) return;
	printf (".=.+%d\n", n);
	inoff += (long) n*SZINT;
}

fldal (t)
unsigned t;
{
	/* return the alignment of field of type t */
	uerror ("illegal field type");
	return (ALINT);
}

fldty (p)
struct symtab *p;
{
	/* fix up type of field p */
}

where (c)
{
	/* print location of error  */
	/* c is either 'u', 'c', or 'w' */

	fprintf (stderr, "%s, line %d: ", ftitle, lineno);
}

# ifdef LL_TOP
int wloop_level = LL_TOP;       /* "while" loop test at loop top */
int floop_level = LL_TOP;       /* "for" loop test at loop top */
# endif

main (argc, argv)
register char *argv[];
{
	register char **p, *s, *infile=0, *outfile=0;

	argv[argc] = 0;
	for (p=argv+1; *p; ++p)
		if (**p == '-') {
			for (s=1+*p; *s; ++s) switch (*s) {
			case 'L':       /* разрешить встроенные функции */
				++Lflag;
				*s = 'x';       /* затерли для p2init */
				break;
			}
		} else {
			if (infile)
				if (outfile) cerror ("too many arguments");
				else outfile = *p;
			else infile = *p;
		}
	if (infile && ! freopen (infile, "r", stdin))
		cerror ("can't open %s", infile);
	if (outfile && ! freopen (outfile, "w", stdout))
		cerror ("can't open %s", outfile);
	return (mainp1 (argc, argv));
}

genswitch (p, n)
register struct sw *p;
{
	/* p points to an array of structures, each consisting
	   of a constant value and a label.
	   The first is >=0 if there is a default label;
	   its value is the label number
	   The entries p[1] to p[n] are the nontrivial cases
	*/
	register i;
	register CONSZ j, range;
	register dlab, swlab;

	range = p[n].sval-p[1].sval;

	if (range>0 && range <= 3*n && n>=4) {
		/* делаем прямой switch */

		dlab = p->slab >= 0 ? p->slab : getlab();

		if (p[1].sval) printf ("\tasux\t#%d\n", p[1].sval);
		else printf( "\tasux\n" );

		/* проверка на величину вне диапазона */

		printf ("\tun\tL%d\n", dlab);
		printf ("\tati\t14\n");
		printf ("\txsua\t#%d\n", range);
		printf ("\tun\tL%d\n", dlab);

		printf ("\tuj\tL%d,14\n", swlab = getlab());

		/* output table */

		deflab (swlab);

		for (i=1, j=p[1].sval; i<=n; ++j )
			printf (":\tuj\tL%d\n",
				(j == p[i].sval) ? p[i++].slab : dlab);

		if (p->slab<0) deflab (dlab);
		return;
	}

	/* debugging code */

	/* out for the moment
	if (n >= 4) werror ("inefficient switch: %d, %d", n,
		(int) (range/n));
	*/

	/* simple switch code */

	j = 0;
	for (i=1; i<=n; j = p[i++].sval) {
		/* уже на сумматоре */
		printf ("\taex");
		if (j ^= p[i].sval) printf ("\t#%d", j);
		printf ("\n\tuz\tL%d\n", p[i].slab);
	}

	if (p->slab>=0) branch (p->slab);
}
