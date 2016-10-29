/*
 *      Сделано в МФТИ.
 *
 *      Чет Фев 11 11:39:15 MSK 1988    Вакуленко С. В.
 */

/* a lot of the machine dependent parts of the second pass */

# include "mfile2"

# define BITMASK(n) ((1L<<n)-1)
# define BITSET(h,n) ((h)|=1L<<(n))

struct inline {                 /* таблица встроенных функций */
	char *name;
	int npar;
	char *cmd;
} itab [] = {
	"__flush_",     0,      "\tpop\n",              /* выталкивание БРЗ */
	"__in_",        1,      "\tatc\n\trmod\n",      /* чтение спец. регистра */
	"__out_",       2,      "\twmod\t[,15]\n",      /* запись спец. регистра */
	"__hibit_",     1,      "\tanx\n",              /* номер старшей единицы */
	"__nbits_",     1,      "\tacx\n",              /* количество единиц */
	"__cadd_",      2,      "\tarx\t,15\n",         /* циклическое сложение */
	"__halt_",      1,      "\thalt\n",             /* останов */
	0,
};

static autoincr;
extern Lflag;

extern NODE *dostarg ();

lineid (l, fn)
char *fn;
{
	/* identify line l and file fn */

	printf (";\tline %d, file %s\n", l, fn);
}

eobl2 ()
{
	OFFSZ spoff;	/* offset from stack pointer */

	spoff = maxoff;
	if (spoff >= AUTOINIT) spoff -= AUTOINIT;
	spoff /= SZINT;
	printf ("\tF%d=%ld\n", ftnno, spoff);
}

isptrchar (t)
register TWORD t;
{
	/* является ли указателем на char */

	if (ISPTR (t)) t = DECREF (t);
	return (ischar (t));
}

ischar (t)
register TWORD t;
{
	/* является ли объектом типа char */

	while (ISARY (t)) t=DECREF (t);
	return (t==CHAR || t==UCHAR);
}

char *rnames [] = {
	"Acc", "1",  "2",  "3",  "4",  "5",  "6",  "7",
	  "8", "9", "10", "11", "12", "13", "14", "15",
};


int rstatus [] = {
/*    сумматор    */    SAREG|STAREG,
/*  1,  2,  3,  4 */    SBREG,          SBREG,          SBREG,          SBREG,
/*  5,  6,  7,  8 */    SBREG|STBREG,   SBREG|STBREG,   SBREG|STBREG,   SBREG|STBREG,
/*  9, 10, 11, 12 */    SBREG|STBREG,   SBREG|STBREG,   SBREG|STBREG,   SBREG,
/* 13, 14, 15     */    SBREG,          SBREG,          SBREG,
};

pushstack ()
{
	printf ("\tits\n");
}

norm (c)
register c;
{
	static r = -1;

	switch (c) {
	case 'x':                       /* -1 == неизвестно */
		r = -1;
		return;
	case 'n':                       /* 1 == блокировка переполнения */
		if (r != 1) printf ("\tntr\t200h\n");
		r = 1;
		return;
	case 'o':                       /* 0 == 0 */
		if (r != 0) printf ("\tntr\n");
		r = 0;
		return;
	}
}

zzzcode( p, c )
register NODE *p;
register c;
{
	/* x, n, o, c, m,
	/* N, I, F, C, S, P
	*/
	register m;

	switch (c) {

	case 'x':
	case 'n':
	case 'o':
		norm (c);
		return;

	case 'c':
		cptrput (p->in.right->tn.lval);
		return;

	case 'm':
		cptrput (-p->in.right->tn.lval);
		return;

	case 'N':                       /* logical ops, turned into 0-1 */
		/* работаем на сумматоре */
		cbgen (0, m = getlab(), 'I');
		deflab (p->bn.label);
		printf ("\txta\n");
		deflab (m);
		return;

	case 'I':
	case 'F':
		cbgen (p->in.op, p->bn.label, c);
		return;

	case 'Z':       /* structure size in words */
		printf ("%d", (int) (p->stn.stsize / (SZINT/SZCHAR)));
		break;

	case 'P':
		p = getlr (p, 'L');
		if (p->in.op != UNARY MUL)
			cerror ("illegal zzzcode 'P'");
		upput (p->in.left);
		break;

	default:
		cerror ("illegal zzzcode: %c", c);
	}
}

rmove (rt, rs, t)
TWORD t;
{
	/* пересылка из регистра rs в регистр rt */

	if (rs == ACC)
		printf ("\tati\t%s\n", rnames [rt]);
	else if (rt == ACC)
		printf ("\tita\t%s\n", rnames [rs]);
	else
		printf ("\tmtj\t%s,%s\n", rnames [rt], rnames [rs]);
}

struct respref respref [] = {
	/* cookie */            /* shape of rezult */
	INTAREG,                INTAREG,
	INBREG,                 INBREG,
	INTEMP,                 INTEMP|SOREG,   /* SOREG - для char */
	FORARG,                 FORARG,
	FORCC,                  INTAREG,
	0,                      0,
};

setregs ()
{
	/* установка количества временных регистров на входе в блок */
	register i;

	fregs = 1;      /* сумматор */
	for (i=MINRVAR; i<=MAXRVAR; ++i)
		rstatus [i] = SBREG;
	norm ('x');
}

szty (t)
TWORD t;
{
	/* количество регистров (сумматоров),
	/* необходимых под переменную типа t */

	return (1);
}

rewfld (p)
NODE *p;
{
	/* should we rewrite the field op p ? */
	/* p->in.op == FLD */
	return (0);     /* was 1 */
}

callreg (p)
NODE *p;
{
	return (ACC);
}

flshape (p)
register NODE *p;
{
	/* Определить, подходит ли p под SFLD (смотри tshape). */

	return (shltype (p->in.op, p));
}

shtemp (p)
register NODE *p;
{
	/* Определить, подходит ли p
	/* под INTEMP (смотри tshape). */

	if (p->in.type==CHAR || p->in.type==UCHAR)
		return (0);
	if (p->in.op==OREG)
		return (!istreg (p->tn.rval));
	return (p->in.op==NAME || p->in.op==ICON);
}

shltype (o, p)
register o;
register NODE *p;
{
	/* Определить, подходит ли p
	/* под OPLTYPE (смотри match).
	/* Фактически - можно ли из p сделать
	/* wtc - последовательность.
	/* o == p->in.op */

	if (o==NAME || o==ICON || o==OREG)
		return (1);
	if (o == UNARY MUL)
		return (shumul (p->in.left));
	return (0);
}

shumul (p)
register NODE *p;
{
	/* Определить, подходит ли p под UNARY MUL.
	 * Да, если это NAME или OREG, или ICON,
	 * или wtc - последовательность.
	 * Используется в tshape.
	 * Возвращает STARREG, если были '++', '--',
	 * иначе STARNM.
	 */
	if (isptrchar (p->in.type))
		return (0);
	switch (p->in.op) {
	case PLUS:
		if (p->in.right->in.op == ICON &&
		    shltype (p->in.left->in.op, p->in.left))
			return (STARNM);
		break;
	case INCR:
	case DECR:
		if (p->in.left->in.op == REG)
			return (STARREG);
		break;
	case UNARY MUL:
		return (shumul (p->in.left));
	case NAME:
	case ICON:
	case OREG:
		return (STARNM);
	}
	return (0);
}

adrcon (val)
CONSZ val;
{
	printf ("0x%lx", val);
}

cptrput (val)
register CONSZ val;
{
	printf ("0x%x00000000%07lx", (int) (val&07)<<1, val>>3 & ADDRMASK);
}

cput (p)
register NODE *p;
{
# ifdef BIGCONSTS
	if (p->tn.lval || p->tn.l2val) {
# else
	if (p->tn.lval) {
# endif
		switch (p->in.type) {
		case INT:
		case SHORT:
		case LONG:
# ifdef BIGCONSTS
			if (p->tn.l2val) {
				if (p->tn.l2val < 0)
					printf ("{0x%lx%08lx}", p->tn.l2val, p->tn.lval);
				else
					printf ("0x%lx%08lx", p->tn.l2val, p->tn.lval);
				break;
			}
# endif
			if (p->tn.lval < 0)
				printf ("{%ld}", p->tn.lval);
			else if (p->tn.lval < 256)
				printf ("%ld", p->tn.lval);
			else
				printf ("0x%lx", p->tn.lval);
			break;
		case UNSIGNED:
		case USHORT:
		case ULONG:
# ifdef BIGCONSTS
			if (p->tn.l2val) {
				printf ("0x%lx%08lx", p->tn.l2val, p->tn.lval);
				break;
			}
# endif
			printf ("0x%lx", p->tn.lval);
			break;
		case CHAR:
		case UCHAR:
			printf ("0x%x", (int) p->tn.lval & 0377);
			break;
		default:
			if (isptrchar (p->in.type))
				cptrput (p->tn.lval);
			else
				printf ("0x%lx", p->tn.lval & ADDRMASK);
			break;
		}
		if (! p->in.name[0])
			return;
		printf ("+");
	}
# ifdef FLEXNAMES
	printf (p->in.name);
# else
	printf ("%.8s", p->in.name);
# endif
}

uput (p)
register NODE *p;
{
	switch (p->in.op) {
	case PLUS:
		printf ("[");
		aput (p->in.left);
		printf ("]");
		conput (p->in.right);
		return;
	case UNARY MUL:
	case NAME:
	case OREG:
		printf ("[");
		aput (p);
		printf ("]");
		return;
	case INCR:
	case DECR:
		printf (",%s", rnames [p->in.left->tn.rval]);
		if (autoincr) {
			printf (p->in.op==INCR ? "++" : "--");
			if (p->in.right->tn.lval != 1)
				printf ("%ld", p->in.right->tn.lval);
		}
		return;
	default:
		cerror ("uput: illegal op");
	}
}

aput (p)
register NODE *p;
{
	/* output an address, with offsets, from p */

	switch (p->in.op) {
	case FLD:
		aput (p->in.left);
		return;
	case UNARY MUL:
		uput (p->in.left);
		return;
	case ICON:              /* addressable value of the constant */
		printf ("#");
		cput (p);
		return;
	case NAME:
		if (p->tn.lval) {
			if (ischar (p->in.type))
				cptrput (p->tn.lval);
			else
				printf ("0x%lx", p->tn.lval & ADDRMASK);
			if (! p->in.name[0])
				return;
			printf ("+");
		}
# ifdef FLEXNAMES
		printf (p->in.name);
# else
		printf ("%.8s", p->in.name);
# endif
		return;
	case REG:
		printf ("%s", rnames[p->tn.rval]);
		return;
	case OREG:
		printf ("%ld", p->tn.lval);
		if (p->tn.rval)
			printf (",%s", rnames [p->tn.rval]);
		return;
	default:
		cerror ("aput: illegal address");
	}
}

conput (p)
register NODE *p;
{
	switch (p->in.op) {
	case ICON:
		cput (p);
		return;
	case REG:
		printf (rnames [p->tn.rval]);
		return;
	default:
		cerror ("illegal conput");
	}
}

upput (p)
NODE *p;
{
	/* выдать адресное выражение */
	/* '++' и '--' не выдавать */

	autoincr = 0;
	aput (p);
}

adrput (p)
register NODE *p;
{
	/* выдать адресное выражение */
	/* допускаются '++' и '--' */

	autoincr = 1;
	aput (p);
}

genscall (p, cookie)
register NODE *p;
{
	/* structure valued call */

	return (gencall (p, cookie));
}

gencall (p, cookie)
register NODE *p;
{
	/* generate the call given by p */

	register temp;
	register m;

	if (Lflag && p->in.left->in.op==ICON &&
	    (m = inlinefunc (p->in.left->tn.name)) >= 0)
		return (inlinecall (p, cookie, m));

	temp = p->in.right ? argsize (p->in.right) : 0;

	if (p->in.right)                /* generate args */
		genargs (p->in.right);

	if ((p->in.left->in.op != UNARY AND ||
	    !shltype (p->in.left->in.left->in.op, p->in.left->in.left)) &&
	    !shltype (p->in.left->in.op, p->in.left) ) {
		printf ("\txts\n");
		order (p->in.left, INTEMP);
		sbegar (temp, 0);
	} else
		sbegar (temp, 1);

	p->in.op = UNARY CALL;
	m = match (p, INTAREG);
	return (m != MDONE);
}

inlinefunc (name)
char *name;
{
	register struct inline *p;

	/* поиск имени в таблице встроенных функций */
	for (p=itab; p->name; ++p)
		if (! strcmp (name, p->name))
			return (p - itab);
	return (-1);
}

inlinecall (p, cookie, n)
register NODE *p;
{
	/* генерация "вызова" встроенной функции */
	register temp, m;
	register struct inline *i;
	register NODE *a1, *a2;

	i = &itab[n];
	temp = p->in.right ? argsize (p->in.right) : 0;
	if (argcnt (p->in.right) != i->npar)
		cerror ("wrong number of arguments in inline function");
	if (temp/8 != i->npar)
		cerror ("wrong type of arguments in inline function");
	a1 = a2 = 0;
	if (i->npar > 1)
		a2 = p->in.right->in.right;
	if (i->npar)
		if (p->in.right->in.op == CM) {
			a1 = p->in.right->in.left;
			p->in.right->in.op = FREE;
		} else
			a1 = p->in.right;
	if (a1) {
		if (a1->in.op == STARG) {
			a1 = dostarg (a1);
			a1->in.type = TINT;
		}
		order (a1, INTAREG);
		reclaim (a1, RNULL, 0);
	}
	if (a2) {
		if (a2->in.op == STARG) {
			a2 = dostarg (a2);
			a2->in.type = TINT;
		}
		gen1arg (a2);
	}
	p->in.op = UNARY CALL;
	m = match (p, INTAREG);
	return (m != MDONE);
}

insput (p)
NODE *p;
{
	register struct inline *i;
	int n;

	n = inlinefunc (p->tn.name);
	if (n < 0) {
		uerror ("bad insput");
		n = 0;
	}
	i = &itab[n];
	printf (i->cmd);
}

hopcode (f, o)
{
	cerror ("hopcode called");
}

sbegar (size, instack)
register size;
{
	/* заслать в стек последний параметр - минус количество аргументов */

	printf ("\txt%c\t#-%d\n", instack ? 's' : 'a', size/8);
}

char *ccbranches [] = {
	"\tuz\tL%d\n",                          /* EQ  */
	"\tui\tL%d\n",                          /* NE  */
	"\tunz\tL%d\n",                         /* LE  */
	"\tun\tL%d\n",                          /* LT  */
	"\tupz\tL%d\n",                         /* GE  */
	"\tup\tL%d\n",                          /* GT  */
	"\tuiv\tL%d\n\tuz\tL%d\n",              /* ULE */
	"\tuiv\tL%d\n",                         /* ULT */
	"\tuzv\tL%d\n",                         /* UGE */
	":\tuz\t.+1\n\tuzv\tL%d\n",             /* UGT */
};

/* logical relations when compared in reverse order (cmp R,L) */

# ifdef ONEPASS
extern short revrel[];
# else
		/*  EQ, NE, LE, LT, GE, GT, ULE, ULT, UGE, UGT */
short revrel [] = { EQ, NE, GE, GT, LE, LT, UGE, UGT, ULE, ULT };
# endif

cbgen (o, lab, mode)
register o;
{
	/* условные и безусловные переходы
	/* o - номер операции :
	/* 0 = безусл. переход на метку lab
	/*      EQ + 0 = EQ
	/*      EQ + 1 = NE
	/*      EQ + 2 = LE
	/*      EQ + 3 = LT
	/*      EQ + 4 = GE
	/*      EQ + 5 = GT
	/*      EQ + 6 = ULE (UNSIGNED LE)
	/*      EQ + 7 = ULT
	/*      EQ + 8 = UGE
	/*      EQ + 9 = UGT

	/* mode=='F' - признак перехода по
	/* инвертированному условию (revrel -
	/* таблица инвертированных операций).
	*/

	if (o == 0) printf ("\tuj\tL%d\n", lab);
	else if (o > UGT)
		cerror ("bad conditional branch: %s", opst[o]);
	else {
		if (mode == 'F') o = revrel [o - EQ];
		printf (ccbranches [o - EQ], lab, lab);
	}
}

nextcook (p, cookie)
NODE *p;
{
	/* we have failed to match p with cookie; try another */

	if (cookie == FORREW) return (0);       /* безнадежно ! */
	if (! (cookie & INTAREG)) return (INTAREG);
	if (! (cookie & INTEMP)) return (INTAREG|INTEMP);
	return (FORREW);
}

lastchance (p, cook)
NODE *p;
{
	return (0);     /* forget it! */
}

myreader (p)
register NODE *p;
{
}

special (p, shape)
register NODE *p;
{
	/* special shape matching routine */

	switch (shape) {
	case SINLINE:   /* встроенные функции */
		if (Lflag && p->in.op == ICON && inlinefunc (p->in.name) >= 0)
			return (1);
		break;
	case SCCON:     /* для сдвигов: -64..63 */
		if (p->in.op == ICON && p->in.name[0] == '\0' &&
		    p->tn.lval >= -64 && p->tn.lval <= 63)
			return (1);
		break;
	case STRRG:
		if (p->in.op == UNARY MUL &&
		    isptrchar (p->in.left->in.type) &&
		    (p->in.left->in.op == NAME || p->in.left->in.op == OREG))
			return (1);
		break;
	default:
		cerror ("bad special shape");
	}
	return (0);
}

# ifndef ONEPASS
int Lflag;

main (argc, argv)
register char *argv[];
{
	register char **p, *s, **save, *outfile;

	/* нужно найти последний параметр - имя файла,
	/* открыть его в качестве стандартного вывода и затереть */

	argv[argc] = 0;
	outfile = 0;
	for (p=argv+1; *p; ++p)
		if (**p == '-') {
			for (s=1+*p; *s; ++s) switch (*s) {
			case 'L':       /* разрешить встроенные функции */
				++Lflag;
				break;
			}
		} else {
			outfile = *p;
			save = p;
		}
	if (outfile) {
		if (! freopen (outfile, "w", stdout))
			cerror ("can't open %s", outfile);
		*save = "-";
	}
	return (mainp2 (argc, argv));
}

where (c)
{
}

# endif
