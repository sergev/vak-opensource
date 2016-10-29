/*
 *      Сделано в МФТИ.
 *
 *      Чет Фев 11 11:39:15 MSK 1988    Вакуленко С. В.
 */

# include "mfile2"

stoasg (p, o)
NODE *p;
{
	/* store the assignment op p,
	   given that it lies as the right operand of o
	   (or the left, if o==UNARY MUL) */
}

deltest (p)
register NODE *p;
{
	/* should we delay the INCR or DECR operation p */

	p = p->in.left;
	if (p->in.op == REG)
		return (0);
loop:
	switch (p->in.op) {
	case NAME:
	case ICON:
	case OREG:
	case REG:
		return (1);
	case PLUS:
		if (p->in.right->in.op != ICON)
			break;
	case UNARY MUL:
		p = p->in.left;
		goto loop;
	}
	return (0);
}

mkadrs (p)
register NODE *p;
{
	if (p->in.right->in.su > p->in.left->in.su) {
		if (p->in.right->in.op == UNARY MUL &&
		    !isptrchar (p->in.right->in.left->in.type))
			SETSTO (p->in.right->in.left, INTEMP);
		else
			SETSTO (p->in.right, INTEMP);
		return;
	}
	if (! asgop (p->in.op) || p->in.left->in.op != UNARY MUL) {
		SETSTO (p->in.left, INTEMP);
		return;
	}
	/* "*x = y" */
	SETSTO (p->in.left->in.left, INTEMP);
}

notoff (t, r, off, cp)
TWORD t;
CONSZ off;
char *cp;
{
	/* Is it legal (0) or not (1) to make an OREG from a register r
	   and offset off if the resulting thing had type t.
	   (cp is the name of operand)
	*/

	return (r==ACC || ischar (t));
}

# define max(x,y) ((x)<(y)?(y):(x))
# define min(x,y) ((x)<(y)?(x):(y))

sucomp (p)
register NODE *p;
{

	/* set the su field in the node to the sethi-ullman
	   number, or local equivalent */

	register o, sul, sur, nr;

	o = p->in.op;
	nr = szty (p->in.type);
	p->in.su = 0;

	switch (optype (o)) {
	case LTYPE:
		break;

	case UTYPE:
		switch (o) {
		case UNARY CALL:
		case UNARY STCALL:
		case UNARY FORTCALL:
			p->in.su = fregs;  /* all regs needed */
			break;
		case UNARY MUL:
			if (optype (p->in.left->in.op) == LTYPE ||
			    shumul (p->in.left))
				return;
		default:
			p->in.su = max (p->in.left->in.su, nr);
			break;
		}
		break;

	case BITYPE:
		/* If rhs needs n, lhs needs m, regular su computation */

		sul = p->in.left->in.su;
		sur = p->in.right->in.su;

		switch (o) {
		case CALL:
		case STCALL:
		case FORTCALL:
			/* in effect, takes all free registers */
			p->in.su = fregs;
			break;

		case ASSIGN:
			p->in.su = max (sul, max (sur, nr));
			break;

		case STASG:
			/* right, then left */
			p->in.su = max (max (sul+nr, sur), fregs);
			break;

		case EQ:
		case NE:
		case LE:
		case LT:
		case GE:
		case GT:
		case ULE:
		case ULT:
		case UGE:
		case UGT:
			nr = szty (p->in.left->in.type);
			p->in.su = min (max (sul, sur+nr), max (sur, sul+nr));
			break;

		case ASG PLUS:
		case ASG MINUS:
		case ASG MUL:
		case ASG AND:
		case ASG DIV:
		case ASG MOD:
		case ASG LS:
		case ASG RS:
		case ASG OR:
		case ASG ER:
		case INCR:
		case DECR:
			p->in.su = max (sul, sur+nr);
			break;

		case ANDAND:
		case OROR:
		case QUEST:
		case COLON:
		case COMOP:
			p->in.su = max (max (sul, sur), nr);
			break;

		case PLUS:
		case MUL:
		case OR:
		case ER:
		case AND:
			/* permute: get the harder on the left */
			if (sur > sul && !istnode (p->in.left)) {
				NODE *s;
				int ssu;

				ssu = sul;
				sul = sur;
				sur = ssu;
				s = p->in.left;
				p->in.left = p->in.right;
				p->in.right = s;
				break;
			}
		default:
			p->in.su = min (max (sul, nr+sur), max (sur, nr+sul));
			break;
		}
		break;
	}
}

int radebug = 0;

rallo (p, down)
register NODE *p;
{
	/* распределение регистров */

	register o, down1, down2, ty;

	if (radebug) printf ("rallo( %o, %o )\n", p, down);

	p->in.rall = down;
	down &= ~MUSTDO;
	ty = optype (o = p->in.op);

	switch( o ) {
	case ASSIGN:	
		down1 = NOPREF;
		down2 = down;
		break;
	case CALL:
	case STASG:
	case EQ:
	case NE:
	case GT:
	case GE:
	case LT:
	case LE:
	case NOT:
	case ANDAND:
	case OROR:
		down1 = NOPREF;
		down2 = NOPREF;
		break;
	case FORCE:	
		down1 = ACC | MUSTDO;
		break;
	case UNARY MUL:
		down1 = NOPREF;
		break;
	default:
		down1 = down;
		down2 = NOPREF;
		break;
	}
	if (ty != LTYPE) rallo (p->in.left, down1);
	if (ty == BITYPE) rallo (p->in.right, down2);
}

offstar (p)
register NODE *p;
{
	/* обработка выражений вида *p */

	order (p, INTAREG|INBREG);
}

setincr (p)
NODE *p;
{
	/* инкремент - декремент */

	if (p->in.type==CHAR || p->in.type==UCHAR)
		p->in.right->in.type = INT;
	return (0);
}

setbin (p)
register NODE *p;
{
	/* бинарная операция */

	register NODE *r, *l;

	r = p->in.right;
	l = p->in.left;

	if (! shtemp (r)) {
		order (r, INTEMP);
		return (1);
	}
	if (! istnode (l)) {
		order (l, INTAREG);
		return (1);
	}
	return (0);                     /* rewrite */
}

setstr (p)
register NODE *p;
{
	/* структурное присваивание */

	if (p->in.right->in.op != ICON &&
	    ! (p->in.right->in.op == REG && p->in.right->tn.rval)) {
		order (p->in.right, INBREG);
		return (1);
	}
	if (optype (p->in.left->in.op) != LTYPE) {
		if (p->in.left->in.op != UNARY MUL)
			cerror ("bad setstr");
		p = p->in.left->in.left;
		if (p->in.op != REG || ! p->tn.rval) {
			order (p, INBREG);
			return (1);
		}
	}
	return (0);
}

setasg (p)
register NODE *p;
{
	/* присваивание */
	register NODE *l, *r;

	l = p->in.left;
	if ((l->in.op == UNARY MUL || l->in.op == FLD) &&
	    optype (l->in.left->in.op) != LTYPE && ! shumul (l->in.left)) {
		order (l->in.left, INTEMP);
		return (1);
	}
	r = p->in.right;
	if (r->in.op != REG || r->tn.rval != ACC) {
		order (r, INTAREG);
		return (1);
	}
	return (0);
}

setasop (p)
register NODE *p;
{
	/* операция вида плюс-присвоить */
	register NODE *l, *r;

	l = p->in.left;
	if ((l->in.op == UNARY MUL || l->in.op == FLD) &&
	    optype (l->in.left->in.op) != LTYPE && ! shumul (l->in.left)) {
		order (l->in.left, INTEMP);
		return (1);
	}
	r = p->in.right;
	if (shtemp (r))
		return (0);
	order (r, INTEMP);
	return (1);
}

getlab ()
{
	static crslab = 10000;

	return (crslab++);
}

deflab (l)
{
	printf ("L%d:\n", l);
	norm ('x');
}

NODE *dostarg (p)
register NODE *p;
{
	if (p->in.left->in.op == ICON) {
		/* make into a name node */
		p->in.op = FREE;
		p = p->in.left;
		p->in.op = NAME;
	} else {
		/* make it look beautiful... */
		p->in.op = UNARY MUL;
		/* turn it into an oreg */
		canon (p);
		if (p->in.op != OREG) {
			offstar (p->in.left);
			canon (p);
			if (p->in.op != OREG)
				cerror ("stuck starg");
		}
	}
	return (p);
}

genargs (p)
register NODE *p;
{
	register size, i;

	/* generate code for the arguments */

	/* first, do the arguments on the right (last->first) */
	while (p->in.op == CM) {
		genargs (p->in.left);
		p->in.op = FREE;
		p = p->in.right;
	}

	if (p->in.op == STARG) { /* structure valued argument */
		size = p->stn.stsize/(SZINT/SZCHAR);
		p = dostarg (p);
		for (i=0; i<size; i++) {
			expand (p, FOREFF, " xts AL;");
			p->tn.lval++;
		}
		reclaim (p, RNULL, 0);
		return;
	}
	/* ordinary case */
	gen1arg (p);
}

gen1arg (p)
register NODE *p;
{
	if (match (p, FORARG) == MDONE)
		return;
	pushstack ();
	order (p, INTAREG);
	reclaim (p, RNULL, 0);
}

argsize (p)
register NODE *p;
{
	register t;

	t = 0;
	if (p->in.op == CM) {
		t = argsize (p->in.left);
		p = p->in.right;
	}
	if (p->in.op == STARG) {
		SETOFF (t, p->stn.stalign);  /* alignment */
		return (t + p->stn.stsize);  /* size */
	} else {
		SETOFF (t, SZINT/SZCHAR);
		return (t + SZINT/SZCHAR);
	}
}

argcnt (p)
register NODE *p;
{
	return (p ? p->in.op==CM ? argcnt (p->in.left) + 1 : 1 : 0);
}
