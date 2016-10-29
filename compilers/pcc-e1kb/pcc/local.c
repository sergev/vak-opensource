/*
 *      Сделано в МФТИ.
 *
 *      Чет Фев 11 11:39:15 MSK 1988    Вакуленко С. В.
 */

# include "mfile1"

# define SZCON ((int) sizeof (long) * 8)
# define ulong unsigned long
# define HIBIT (1L << (SZCON-1))

/* This file contains code which is dependent on the target machine. */

NODE *clocal (p)
NODE *p;
{
	/* This is called to do local transformations on
	/* an expression tree preparitory to its being
	/* written out in intermediate code.

	/* The major essential job is rewriting the
	/* automatic variables and arguments in terms of
	/* REG and OREG nodes.
	/* Conversion ops which are not necessary
	/* are also clobbered here.
	/* In addition, any special features (such as rewriting
	/* exclusive or) are easily handled here as well.
	*/

	register struct symtab *q;
	register NODE *r;
	register o;
	register m, ml;

	switch (o = p->in.op) {
	case NAME:
		if (p->tn.rval < 0) return (p); /* already processed; ignore... */
		q = &stab[p->tn.rval];
		switch (q->sclass) {
		case AUTO:
		case PARAM:
			/* fake up a structure reference */
			r = block (REG, NIL, NIL, PTR+STRTY, 0, 0);
			ZEROVAL (p);
			r->tn.rval = (q->sclass==AUTO? STKREG: ARGREG);
			p = stref (block (STREF, r, p, 0, 0, 0));
			break;

		case ULABEL:
		case LABEL:
		case STATIC:
			if (q->slevel == 0) break;
			ZEROVAL (p);
			p->tn.rval = -q->offset;
			break;

		case REGISTER:
			p->in.op = REG;
			ZEROVAL (p);
			p->tn.rval = q->offset;
			break;
		}
		break;

	case PCONV:
		/* Преобразование в тип указателя. */

		if (p->in.left->in.op == ICON) {
			if (isptrchar (p->in.type)) {
				if (isptrchar (p->in.left->in.type)) ;
				else if (ISPTR(p->in.left->in.type))
					p->in.left->tn.lval <<= 3;
				else p->in.left->tn.lval =
					(p->in.left->tn.lval&ADDRMASK)<<3 |
					(p->in.left->tn.lval>>28 & 07);
			} else {
				if (isptrchar (p->in.left->in.type))
					p->in.left->tn.lval >>= 3;
				else p->in.left->tn.lval &= ADDRMASK;
			}
			p->in.left->in.type = p->in.type;
			p->in.op = FREE;
# ifdef BIGCONSTS
			p->in.left->tn.l2val = 0;
# endif
			return (p->in.left);
		}
		if (! ISPTR (p->in.left->in.type)) break;

		/* если это - преобразование
		/* из CHAR, ничего делать не надо:
		/* это есть в таблице */

		if (!isptrchar (p->in.type) &&
			isptrchar (p->in.left->in.type)) break;

		/* все остальные типы указателей имеют
		/* одинаковое представление:
		/* словный адрес */

		p->in.left->in.type = p->in.type;
		p->in.left->fn.cdim = p->fn.cdim;
		p->in.left->fn.csiz = p->fn.csiz;
		p->in.op = FREE;
		return (p->in.left);

	case SCONV:
		/* Преобразование в простой тип. */

		m = p->in.type = ctype (p->in.type);
		ml = p->in.left->in.type = ctype (p->in.left->in.type);

		/* если проебразование из типа 'указатель',
		/* сначала сделаем из него целое */
		if (ml & TMASK)
			ml = p->in.left->in.type = INT;

		/* преобразования в представление
		/* с плав. точкой и наоборот */
		if ((m == DOUBLE) != (ml == DOUBLE)) {
			/* int <-> float есть в таблице */
			if (m == INT || ml == INT) break;
			/* float -> unsign == float -> int */
			if (m == UNSIGNED) {
				p->in.type = INT;
				break;
			}
			/* иначе делаем промежуточное
			/* преобразование в int */
			r = talloc ();
			r->in.op = SCONV;
			r->in.type = INT;
			r->in.left = p->in.left;
			p->in.left = r;
			break;
		}

		/* FLOAT <-> DOUBLE : очень просто */
		if (m == DOUBLE) goto conv;

		/* преобразование констант делаем на месте */
		if (p->in.left->in.op == ICON) {
			if (isptrchar ((TWORD) ml))
				p->in.left->tn.lval = (p->in.left->tn.lval>>3) & ADDRMASK |
					(p->in.left->tn.lval&07) << 28;
			if (m == CHAR) p->in.left->tn.lval &= 0377;
			p->in.left->in.type = m;
# ifdef BIGCONSTS
			p->in.left->tn.l2val = 0;
# endif
			goto conv;
		}

		/*
		 * Преобразования:
		 *
		 * CHR -> CHR   пустое
		 * CHR -> INT   --//--
		 * CHR -> UNS   --//--
		 * INT -> INT   --//--
		 * INT -> UNS   --//--
		 * UNS -> UNS   --//--
		 * INT -> CHR   i & 0377
		 * UNS -> CHR   u & 0377
		 * UNS -> INT   u & 017777777777777
		 */
		if (ml == INT && m == CHAR) break;
		if (ml == UNSIGNED && m == CHAR) break;
		if (ml == UNSIGNED && m == INT) break;
conv:
		p->in.op = FREE;
		return (p->in.left);

	case PVCONV:
	case PMCONV:
		if (p->in.right->in.op != ICON) cerror( "bad conversion", 0);
		p->in.op = FREE;
		return (buildtree (o==PMCONV? MUL: DIV,
			p->in.left, p->in.right));

	case INCR:
	case DECR:
	case ASG PLUS:
	case ASG MINUS:
	case ASG MUL:
	case ASG DIV:
	case ASG MOD:
	case ASG LS:
	case ASG RS:
	case ASG AND:
	case ASG OR:
	case ASG ER:
		if (p->in.right->in.type != CHAR &&
			p->in.right->in.type != UCHAR) break;
		r = talloc ();
		r->in.op = SCONV;
		r->in.type = INT;
		r->in.left = p->in.right;
		p->in.right = clocal (r);
		break;
	}
	return (p);
}

andable (p)
NODE *p;
{
	/* может ли p стоять в литерале */

	return (1);
}

cendarg ()
{
	/* At the end of the arguments of a ftn,
	/* set the automatic offset. */

	autooff = AUTOINIT;
}

cisreg (t)
register TWORD t;
{
	/* Is an automatic variable of type t
	/* OK for a register variable ?
	/* В регистре можно держать указатель,
	/* но не на CHAR. */

	return (ISPTR(t) && !isptrchar (t));
}

NODE *offcon (off, t, d, s)
OFFSZ off;
TWORD t;
{
	/* Return a node, for structure references,
	/* which is suitable for
	/* being added to a pointer of type t,
	/* in order to be off bits offset
	/* into a structure.
	/* t, d, and s are the type,
	/* dimension offset, and sizeoffset.
	/* In general they are necessary for offcon,
	/* but not on H'well.
	*/
	return (bcon (off / (isptrchar (t) ? SZCHAR : SZINT)));
}

static inwd;            /* текущий свободный бит в слове:
			/* справа налево, 0..63 */
static long wordl;      /* левая половина слова, собираемого из полей */
static long wordr;      /* правая половина слова, собираемого из полей */

incode (p, sz)
register NODE *p;
{
	/* Generate initialization code for assigning a constant c
	/* to a field of width sz.
	/* We assume that the proper alignment has been obtained.
	/* Inoff is updated to have the proper final value.
	/* We also assume sz < SZINT.
	*/

	if (sz <= 0) return;
	if (sz < SZCON)
		p->tn.lval &= (ulong) (1L << sz) - 1;
# ifdef BIGCONSTS
	else
		p->tn.l2val &= (ulong) (1L << (sz - SZCON)) - 1;
# endif
	if ((sz+inwd) > SZINT)
		cerror ("incode: field > int");
	if (inwd < SZINT/2)
		wordr |= p->tn.lval << inwd;
	if (inwd+sz > SZINT/2) {
		wordl |= (inwd > SZINT/2) ?
			p->tn.lval << (inwd - SZINT/2) :
			p->tn.lval >> (SZINT/2 - inwd);
		if (sz >= SZCON)
			wordl |= p->tn.l2val << inwd;
	}
	vfdzero (sz);
}

vfdzero (n)
{
	/* define n bits of zeros in a vfd */

	if (n <= 0) return;

	inwd += n;
	inoff += n;
	if (inoff % SZINT == 0) {
		printf ("\t.word\t0%08lx%08lxh\n", wordl, wordr);
		wordl = wordr = inwd = 0;
	}
}

fincode (d, sz)
double d;
{
	/* Output code to initialize space of size sz to the value d.
	/* The proper alignment has been obtained.
	/* Inoff is updated to have the proper final value.
	/* On the target machine, write it out in octal!
	*/
	int power=0, sign=0, k;
	long left=0, right=0;

	if (d == 0.0) {                 /* тривиальный случай */
		printf ("\t.word\n");
		inoff += sz;
		return;
	}
	if (d < 0.0) {                  /* установим знак */
		d = -d;
		sign = 1;
	}
	if (d >= 1.0) do {              /* вычислим порядок */
			d /= 2.0;
			power ++;
		} while (d > 1.0);
	else if (d < 0.5) do {
			d += d;
			power --;
		} while (d < 0.5);
	left = (((power + 1024L) << 1) | sign) << 20;
	for (k=19; k>=0; k--, d+=d) if (d >= 0.5) {
		left |= 1L << k;
		d -= 0.5;
	}
	for (k=31; k>=0; k--, d+=d) if (d >= 0.5) {
		right |= 1L << k;
		d -= 0.5;
	}
	if (sign) {
		left ^= 03777777L;
		right ^= 037777777777L;
	}
	printf ("\t.word\t0%08lx%08lxh\n", left, right);
	inoff += sz;
}

cinit (p, sz)
NODE *p;
{
	/* Arrange for the initialization of p
	/* into a space of size sz.
	/* The proper alignment has been opbtained.
	/* Inoff is updated to have
	/* the proper final value.
	*/

	ecode (p);
	inoff += sz;
}

char *exname (p)
char *p;
{
	/* Make a name look like an external
	/* name in the local machine.
	*/

# ifdef FLEXNAMES
	static char text [BUFSIZ+1];
	extern int truncate_flag;

	text [0] = '_';
	strncpy (text+1, p, BUFSIZ);
	text [BUFSIZ] = '\0';
	if (truncate_flag)              /* backward compatibility option */
		text [8] = '\0';
# else
	static char text [NCHNAM+1];
	register i;

	text [0] = '_';
	for (i=1; *p && i<NCHNAM; ++i) text[i] = *p++;
	text [i] = '\0';
	text [NCHNAM] = '\0';  /* truncate */
# endif
	return (text);
}

ctype (type)
register TWORD type;
{
	/* Map types which are not defined on the local machine. */

	switch (BTYPE (type)) {
	case UCHAR:
		MODTYPE (type, CHAR);
		break;
	case SHORT:
	case LONG:
		MODTYPE (type, INT);
		break;
	case USHORT:
	case ULONG:
		MODTYPE (type, UNSIGNED);
		break;
	case FLOAT:
		MODTYPE (type, DOUBLE);
	}
	return (type);
}

noinit ()
{
	/* Curid is a variable which is defined but
	/* is not initialized (and not a function);
	/* this routine returns the stroage class
	/* for an uninitialized declaration.
	*/

	return (EXTERN);
}

commdec (id)
{
	/* Make a common declaration for id, if reasonable. */

	register struct symtab *q;
	OFFSZ off;

	q = &stab[id];
	off = tsize (q->stype, q->dimoff, q->sizoff);
	printf ("\t.comm\t%s,%ld\n", exname (q->sname), (off+SZINT-1)/SZINT);
}

isitfloat (s)
char *s;
{
	extern double atof ();

	dcon = atof (s);
	return (FCON);
}

ecode (p)
NODE *p;
{
	/* walk the tree and write out the nodes.. */

	if (nerrors) return;
# ifdef ONEPASS
	p2tree (p);
	p2compile (p);
# else
	printf (".%d\t%s\n", lineno, ftitle);
	prtree (p);
# endif
}

# ifdef TWOPASS
isptrchar (t)
register TWORD t;
{
	/* является ли указателем на char */

	if (! (t & TMASK)) return (0);
	if (ISPTR (t)) t=DECREF (t);
	while (ISARY (t)) t=DECREF (t);
	return (t==CHAR || t==UCHAR);
}
# endif

# ifdef BIGCONSTS
conop (op, p, v, v2)
register NODE *p;
register CONSZ v, v2;
{
	register i;

	/* операции 64-разрядной кросс-арифметики для констант */
	switch (op) {
	case AND:
logand:         p->tn.lval &= v;
		p->tn.l2val &= v2;
		break;
	case OR:
		p->tn.lval |= v;
		p->tn.l2val |= v2;
		break;
	case ER:
		p->tn.lval ^= v;
		p->tn.l2val ^= v2;
		break;
	case LS:
lshift:         if (v < 0 || v >= SZLONG)
			ZEROVAL (p);
		else if (v < SZCON) {
			p->tn.l2val <<= v;
			p->tn.l2val |= (ulong) p->tn.lval >> (int) (SZCON - v);
			p->tn.lval <<= v;
		} else {
			p->tn.l2val = p->tn.lval << (int) (v - SZCON);
			p->tn.lval = 0;
		}
		break;
	case RS:
rshift:         if (v < 0 || v >= SZLONG)
			ZEROVAL (p);
		else if (v < SZCON) {
			p->tn.lval >>= v;
			p->tn.lval |= p->tn.l2val << (int) (SZCON - v);
			p->tn.l2val >>= v;
		} else {
			p->tn.lval = p->tn.l2val >> (int) (v - SZCON);
			p->tn.l2val = 0;
		}
		break;
	case MUL:                       /* совсем некорректно */
		if ((i = ispow2 (v)) >= 0 && !v2) {
			v = i;
			goto lshift;
		}
		p->tn.l2val = 0;
		p->tn.lval *= v;
		break;
	case DIV:                       /* совсем некорректно */
		if (p->tn.lval>=0 && (i = ispow2 (v)) >= 0 && !v2) {
			v = i;
			goto rshift;
		}
		p->tn.l2val = 0;
		p->tn.lval /= v;
		break;
	case MOD:                       /* совсем некорректно */
		if (p->tn.lval>=0 && (!v2 || ispow2 (v) >= 0 ||
		    !v && ispow2 (v2) >= 0)) {
			if (v2) {
				--v2;
				v = ~0;
			} else
				--v;
			goto logand;
		}
		p->tn.l2val = 0;
		p->tn.lval %= v;
		break;
	case COMPL:
		p->tn.lval = ~p->tn.lval;
		p->tn.l2val = ~p->tn.l2val;
		break;
	case NOT:
		p->tn.lval = !p->tn.l2val && !p->tn.lval;
		p->tn.l2val = 0;
		break;
	case UNARY MINUS:
		if (p->tn.l2val) {
			p->tn.l2val = -p->tn.l2val;
			p->tn.lval = ~(ulong) p->tn.lval + 1;
		} else
			p->tn.lval = -p->tn.lval;
		break;
	case MINUS:                     /* не совсем корректно */
		p->tn.l2val -= v2;
		p->tn.lval -= v;
		break;
	case PLUS:                      /* не совсем корректно */
		p->tn.l2val += v2;
		p->tn.lval += v;
		break;
	case LT:
		if (p->tn.l2val == v2)
			p->tn.lval = p->tn.lval < v;
		else
			p->tn.lval = p->tn.l2val < v2;
		p->tn.l2val = 0;
		break;
	case LE:
		if (p->tn.l2val == v2)
			p->tn.lval = p->tn.lval <= v;
		else
			p->tn.lval = p->tn.l2val <= v2;
		p->tn.l2val = 0;
		break;
	case GT:
		if (p->tn.l2val == v2)
			p->tn.lval = p->tn.lval > v;
		else
			p->tn.lval = p->tn.l2val > v2;
		p->tn.l2val = 0;
		break;
	case GE:
		if (p->tn.l2val == v2)
			p->tn.lval = p->tn.lval >= v;
		else
			p->tn.lval = p->tn.l2val >= v2;
		p->tn.l2val = 0;
		break;
	case ULT:
		if (p->tn.l2val == v2)
			p->tn.lval = p->tn.lval-v < 0;
		else
			p->tn.lval = p->tn.l2val-v2 < 0;
		p->tn.l2val = 0;
		break;
	case ULE:
		if (p->tn.l2val == v2)
			p->tn.lval = p->tn.lval-v <= 0;
		else
			p->tn.lval = p->tn.l2val-v2 <= 0;
		p->tn.l2val = 0;
		break;
	case UGE:
		if (p->tn.l2val == v2)
			p->tn.lval = p->tn.lval-v >= 0;
		else
			p->tn.lval = p->tn.l2val-v2 >= 0;
		p->tn.l2val = 0;
		break;
	case UGT:
		if (p->tn.l2val == v2)
			p->tn.lval = p->tn.lval-v > 0;
		else
			p->tn.lval = p->tn.l2val-v2 > 0;
		p->tn.l2val = 0;
		break;
	case EQ:
		p->tn.lval = p->tn.lval == v && p->tn.l2val == v2;
		p->tn.l2val = 0;
		break;
	case NE:
		p->tn.lval = p->tn.lval != v || p->tn.l2val != v2;
		p->tn.l2val = 0;
		break;
	}
}
# endif
