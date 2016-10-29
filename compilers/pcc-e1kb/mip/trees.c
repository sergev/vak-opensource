/*	@(#)trees.c	1.4	*/
# include "mfile1"
# include "messages.h"

	    /* corrections when in violation of lint */

/*	some special actions, used in finding the type of nodes */
# define NCVT 01
# define PUN 02
# define TYPL 04
# define TYPR 010
# define TYMATCH 040
# define LVAL 0100
# define CVTO 0200
# define CVTL 0400
# define CVTR 01000
# define PTMATCH 02000
# define OTHER 04000
# define NCVTR 010000

/* node conventions:

	NAME:	rval>0 is stab index for external
		rval<0 is -inlabel number
		lval is offset in bits
	ICON:	lval has the value
		rval has the STAB index, or - label number,
			if a name whose address is in the constant
		rval = NONAME means no name
	REG:	rval is reg. identification cookie

	*/

int bdebug = 0;
extern ddebug;

extern OFFSZ tsize ();

NODE *
buildtree( o, l, r ) register NODE *l, *r; {
	register NODE *p, *q;
	register actions;
	register opty;
	register struct symtab *sp;
	register NODE *lr, *ll;
	int i;
	extern int eprint();

# ifndef BUG1
	if( bdebug ) printf( "buildtree( %s, %lo, %lo )\n", opst[o], l, r );
# endif
	opty = optype(o);

	/* check for constants */

	if( opty == UTYPE && l->in.op == ICON ){

		switch( o ){

		case NOT:
			/* "constant argument to NOT"  */
			if( hflag ) WERROR( MESSAGE( 22 ) );
		case UNARY MINUS:
		case COMPL:
			if( conval( l, o, l ) ) return(l);
			break;

			}
		}

		/* check if cast can be painted down */
		/* cgram has to check what we return here */
	else if (o == CAST && r->in.op == ICON)
	{
		if (bdebug)
			puts("attempting to paint cast down");
		if (paintcast(r, (int) l->in.type))
		{
			l->in.op = FREE;
			return (r);
		}
	}
	else if( o==UNARY MINUS && l->in.op==FCON ){
		l->fpn.dval = -l->fpn.dval;
		return(l);
		}

	else if( o==QUEST && l->in.op==ICON ) {
		l->in.op = FREE;
		r->in.op = FREE;
		if( l->tn.lval || HIGHVAL(l) ){
			tfree( r->in.right );
			return( r->in.left );
			}
		else {
			tfree( r->in.left );
			return( r->in.right );
			}
		}

	else if( (o==ANDAND || o==OROR) && (l->in.op==ICON||r->in.op==ICON) ) goto ccwarn;

	else if( opty == BITYPE && l->in.op == ICON && r->in.op == ICON ){

		switch( o ){

		case ULT:
		case UGT:
		case ULE:
		case UGE:
		case LT:
		case GT:
		case LE:
		case GE:
		case EQ:
		case NE:
		case ANDAND:
		case OROR:
		case CBRANCH:

		ccwarn:
			/* "constant in conditional context" */
			if( hflag ) WERROR( MESSAGE( 24 ) );

		case PLUS:
		case MINUS:
		case MUL:
		case DIV:
		case MOD:
		case AND:
		case OR:
		case ER:
		case LS:
		case RS:
			if( conval( l, o, r ) ) {
				r->in.op = FREE;
				return(l);
				}
			break;
			}
		}

	else if( opty == BITYPE && (l->in.op==FCON||l->in.op==ICON) &&
		(r->in.op==FCON||r->in.op==ICON) ){
		switch(o){
		case PLUS:
		case MINUS:
		case MUL:
		case DIV:
			if( l->in.op == ICON ){
				l->fpn.dval = l->tn.lval;
				}
			if( r->in.op == ICON ){
				r->fpn.dval = r->tn.lval;
				}
			l->in.op = FCON;
			l->in.type = l->fn.csiz = DOUBLE;
			r->in.op = FREE;
			switch(o){
			case PLUS:
				l->fpn.dval += r->fpn.dval;
				return(l);
			case MINUS:
				l->fpn.dval -= r->fpn.dval;
				return(l);
			case MUL:
				l->fpn.dval *= r->fpn.dval;
				return(l);
			case DIV:
				/* "division by 0." */
				if( r->fpn.dval == 0 ) UERROR( MESSAGE( 32 ) );
				else l->fpn.dval /= r->fpn.dval;
				return(l);
				}
			}
		}

	/* its real; we must make a new node */

	p = block( o, l, r, INT, 0, INT );

	actions = opact(p);

	if( actions&LVAL ){ /* check left descendent */
		if( notlval(p->in.left) ) {
			/* "illegal lhs of assignment operator" */
			UERROR( MESSAGE( 62 ) );
			}
		}

	if( actions & NCVTR ){
		p->in.left = pconvert( p->in.left );
		}
	else if( !(actions & NCVT ) ){
		switch( opty ){

		case BITYPE:
			p->in.right = pconvert( p->in.right );
		case UTYPE:
			p->in.left = pconvert( p->in.left );

			}
		}

	if( (actions&PUN) && (o!=CAST||cflag) ){
		chkpun(p);
		}

	if( actions & (TYPL|TYPR) ){

		q = (actions&TYPL) ? p->in.left : p->in.right;

		p->in.type = q->in.type;
		p->fn.cdim = q->fn.cdim;
		p->fn.csiz = q->fn.csiz;
		}

	if( actions & CVTL ) p = convert( p, CVTL );
	if( actions & CVTR ) p = convert( p, CVTR );
	if( actions & TYMATCH ) p = tymatch(p);
	if( actions & PTMATCH ) p = ptmatch(p);

	if( actions & OTHER ){
		l = p->in.left;
		r = p->in.right;

		switch(o){

		case NAME:
			sp = &stab[idname];
			if( sp->stype == UNDEF ){
				/* "%.8s undefined" */
				/* "%s undefined" */
				UERROR( MESSAGE( 4 ), sp->sname );
				/* make p look reasonable */
				p->in.type = p->fn.cdim = p->fn.csiz = INT;
				p->tn.rval = idname;
				ZEROVAL (p);
				defid( p, SNULL );
				break;
				}
			p->in.type = sp->stype;
			p->fn.cdim = sp->dimoff;
			p->fn.csiz = sp->sizoff;
			ZEROVAL (p);
			p->tn.rval = idname;
			/* special case: MOETY is really an ICON... */
			if( p->in.type == MOETY ){
				p->tn.rval = NONAME;
				ZEROVAL (p);
				p->tn.lval = sp->offset;
				p->fn.cdim = 0;
				p->in.type = ENUMTY;
				p->in.op = ICON;
				}
			break;

		case ICON:
			p->in.type = INT;
			p->fn.cdim = 0;
			p->fn.csiz = INT;
			break;

		case STRING:
			p->in.op = NAME;
			p->in.type = CHAR+ARY;
			ZEROVAL (p);
			p->tn.rval = NOLAB;
			p->fn.cdim = curdim;
			p->fn.csiz = CHAR;
			break;

		case FCON:
			ZEROVAL (p);
			p->tn.rval = 0;
			p->in.type = DOUBLE;
			p->fn.cdim = 0;
			p->fn.csiz = DOUBLE;
			break;

		case STREF:
			/* p->x turned into *(p+offset) */
			/* rhs must be a name; check correctness */

			i = r->tn.rval;
			if( i<0 || ((sp= &stab[i])->sclass != MOS && sp->sclass != MOU && !(sp->sclass&FIELD)) ){
				/* "member of structure or union required" */
				UERROR( MESSAGE( 76 ) );
				}else
			/* if this name is non-unique, find right one */
			if( stab[i].sflags & SNONUNIQ &&
				(l->in.type==PTR+STRTY || l->in.type == PTR+UNIONTY) &&
				(l->fn.csiz +1) >= 0 ){
				/* nonunique name && structure defined */
				char * memnam, * tabnam;
				register k;
				int j;
				int memi;
				j=dimtab[l->fn.csiz+1];
				for( ; (memi=dimtab[j]) >= 0; ++j ){
					tabnam = stab[memi].sname;
					memnam = stab[i].sname;
# ifndef BUG1
					if( ddebug>1 ){
#ifdef FLEXNAMES
						printf("member %s==%s?\n",
							memnam, tabnam);
#else
						printf("member %.8s==%.8s?\n",
							memnam, tabnam);
#endif
						}
# endif
					if( stab[memi].sflags & SNONUNIQ ){
#ifdef FLEXNAMES
						if ( memnam != tabnam )
							continue;
						r->tn.rval = i = memi;
						break;
						}
#else
						for( k=0; k<NCHNAM; ++k ){
							if(*memnam++!=*tabnam)
								goto next;
							if(!*tabnam++) break;
							}
						r->tn.rval = i = memi;
						break;
						}
					next: continue;
#endif
					}
				if( memi < 0 )
					/* "illegal member use: %.8s" */
					/* "illegal member use: %s" */
					UERROR(MESSAGE( 63 ), stab[i].sname);
				}
			else {
				register j;
				register align;
				if( l->in.type != PTR+STRTY && l->in.type != PTR+UNIONTY ){
					if( stab[i].sflags & SNONUNIQ ){
						/* "nonunique name demands struct/union or struct/union pointer"  */
						UERROR( MESSAGE( 84 ) );
						}
					else {
						align = talign(BTYPE(l->in.type), l->fn.csiz );
						/* align better be an even multiple of ALSTRUCT */
						if ( (align % ALSTRUCT) == 0 )
							/* "struct/union or struct/union pointer required" */
							WERROR(MESSAGE( 103 ));
						else
							/* "bad structure offset" */
							UERROR( MESSAGE( 5 ) );
						}
					}
				else if( (j=l->fn.csiz+1)<0 ) cerror( "undefined structure or union" );
				else if( !chkstr( i, dimtab[j], DECREF(l->in.type) ) ){
					/* "illegal member use: %.8s" */
					/* "illegal member use: %s" */
					WERROR( MESSAGE( 63 ), stab[i].sname );
					}
				}

			p = stref( p );
			break;

		case UNARY MUL:
			if( l->in.op == UNARY AND ){
				p->in.op = l->in.op = FREE;
				p = l->in.left;
				}
			/* "illegal indirection" */
			if( !ISPTR(l->in.type))UERROR(MESSAGE( 60 ));
			p->in.type = DECREF(l->in.type);
			p->fn.cdim = l->fn.cdim;
			p->fn.csiz = l->fn.csiz;
			break;

		case UNARY AND:
			switch( l->in.op ){

			case UNARY MUL:
				p->in.op = l->in.op = FREE;
				p = l->in.left;
			case NAME:
				p->in.type = INCREF( l->in.type );
				p->fn.cdim = l->fn.cdim;
				p->fn.csiz = l->fn.csiz;
				break;

			case COMOP:
				lr = buildtree( UNARY AND, l->in.right, NIL );
				p->in.op = l->in.op = FREE;
				p = buildtree( COMOP, l->in.left, lr );
				break;

			case QUEST:
				lr = buildtree( UNARY AND, l->in.right->in.right, NIL );
				ll = buildtree( UNARY AND, l->in.right->in.left, NIL );
				p->in.op = l->in.op = l->in.right->in.op = FREE;
				p = buildtree( QUEST, l->in.left, buildtree( COLON, ll, lr ) );
				break;

# ifdef ADDROREG
			case OREG:
				/* OREG was built in clocal()
				 * for an auto or formal parameter
				 * now its address is being taken
				 * local code must unwind it
				 * back to PLUS/MINUS REG ICON
				 * according to local conventions
				 */
				{
				extern NODE * addroreg();
				p->in.op = FREE;
				p = addroreg( l );
				}
				break;

# endif
			default:
				/* "unacceptable operand of &" */
				UERROR( MESSAGE( 110 ) );
				break;
				}
			break;

		case LS:
		case RS:
		case ASG LS:
		case ASG RS:
			if(tsize(p->in.right->in.type, p->in.right->fn.cdim, p->in.right->fn.csiz) > SZINT)
				p->in.right = makety(p->in.right, INT, 0, INT );
			break;

		case RETURN:
		case ASSIGN:
		case CAST:
			/* structure assignment */
			/* take the addresses of the two sides; then make an
			/* operator using STASG and
			/* the addresses of left and right */

			{
				register TWORD t;
				register d, s;

				/* "assignment of different structures" */
				if( l->fn.csiz != r->fn.csiz ) UERROR( MESSAGE( 15 ) );

				r = buildtree( UNARY AND, r, NIL );
				t = r->in.type;
				d = r->fn.cdim;
				s = r->fn.csiz;

				l = block( STASG, l, r, t, d, s );

				if( o == RETURN ){
					p->in.op = FREE;
					p = l;
					break;
					}

				l = clocal( l );
				p->in.op = UNARY MUL;
				p->in.left = l;
				p->in.right = NIL;
				break;
				}
		case COLON:
			/* structure colon */

			/* "type clash in conditional" */
			if( l->fn.csiz != r->fn.csiz ) UERROR( MESSAGE( 109 ) );
			break;

		case CALL:
			p->in.right = r = strargs( p->in.right );
		case UNARY CALL:
			/* "illegal function" */
			if( !ISPTR(l->in.type)) UERROR(MESSAGE( 58 ));
			p->in.type = DECREF(l->in.type);
			/* "illegal function" */
			if( !ISFTN(p->in.type)) UERROR(MESSAGE( 58 ));
			p->in.type = DECREF( p->in.type );
			p->fn.cdim = l->fn.cdim;
			p->fn.csiz = l->fn.csiz;
			if( l->in.op == UNARY AND && l->in.left->in.op == NAME &&
				l->in.left->tn.rval >= 0 && l->in.left->tn.rval != NONAME &&
				( (i=stab[l->in.left->tn.rval].sclass) == FORTRAN || i==UFORTRAN ) ){
				p->in.op += (FORTCALL-CALL);
				}
			if( p->in.type == STRTY || p->in.type == UNIONTY ){
				/* function returning structure */
				/*  make function really return ptr to str., with * */

				p->in.op += STCALL-CALL;
				p->in.type = INCREF( p->in.type );
				p = buildtree( UNARY MUL, p, NIL );

				}
			break;

		default:
			cerror( "other code %d", o );
			}

		}

	if( actions & CVTO ) p = oconvert(p);
	p = clocal(p);

# ifndef BUG1
	if( bdebug ) fwalk( p, eprint, 0 );
# endif

	return(p);

	}

static
paintcast(p, t)			/* can we paint type t onto ICON *p? */
register NODE *p;
register int t;
{
	register int itype = p->tn.type;

	/* for now just worry about integral->integral conversions */

	if ((ISUNSIGNED(itype) || UNSIGNABLE(itype)) &&
	    (ISUNSIGNED(t) || UNSIGNABLE(t)))
	{
		p->in.type = t;
		return (1);
	}
	else
		return (0);
}

NODE *
strargs( p ) register NODE *p;  { /* rewrite structure flavored arguments */

	if( p->in.op == CM ){
		p->in.left = strargs( p->in.left );
		p->in.right = strargs( p->in.right );
		return( p );
		}

	if( p->in.type == STRTY || p->in.type == UNIONTY ){
		p = block( STARG, p, NIL, p->in.type, p->fn.cdim, p->fn.csiz );
		p->in.left = buildtree( UNARY AND, p->in.left, NIL );
		p = clocal(p);
		}
	return( p );
	}

chkstr( i, j, type ) TWORD type; {
	/* is the MOS or MOU at stab[i] OK for strict reference by a ptr */
	/* i has been checked to contain a MOS or MOU */
	/* j is the index in dimtab of the members... */
	int k, kk;

	extern int ddebug;

# ifndef BUG1
#ifdef FLEXNAMES
	if( ddebug > 1 )
		printf( "chkstr( %s(%d), %d )\n", stab[i].sname, i, j );
#else
	if( ddebug > 1 )
		printf( "chkstr( %.8s(%d), %d )\n", stab[i].sname, i, j );
#endif
# endif
	/* "undefined structure or union" */
	if( (k = j) < 0 ) UERROR( MESSAGE( 112 ) );
	else {
		for( ; (kk = dimtab[k] ) >= 0; ++k ){
			if( kk >= SYMTSZ ){
				cerror( "gummy structure" );
				return(1);
				}
			if( kk == i ) return( 1 );
			switch( stab[kk].stype ){

			case STRTY:
			case UNIONTY:
				if( type == STRTY ) continue;  /* no recursive looking for strs */
				if( hflag && chkstr( i, dimtab[stab[kk].sizoff+1], stab[kk].stype ) ){
					if( stab[kk].sname[0] == '$' ) return(0);  /* $FAKE */
					/* "illegal member use: perhaps %.8s.%.8s?" */
					/* "illegal member use: perhaps %s.%s?" */
					WERROR( MESSAGE( 65 ),
						stab[kk].sname, stab[i].sname );
					return(1);
					}
				}
			}
		}
	return( 0 );
	}

conval( p, o, q ) register NODE *p, *q; {
	/* apply the op o to the lval part of p; if binary, rhs is val */
	int i, u;
	CONSZ val;
# ifdef BIGCONSTS
	CONSZ val2;
# endif

	val = q->tn.lval;
# ifdef BIGCONSTS
	val2 = q->tn.l2val;
# endif
	u = ISUNSIGNED(p->in.type) || ISUNSIGNED(q->in.type);
	if( u && (o==LE||o==LT||o==GE||o==GT)) o += (UGE-GE);

	if( p->tn.rval != NONAME && q->tn.rval != NONAME ) return(0);
	if( q->tn.rval != NONAME && o!=PLUS ) return(0);
	if( p->tn.rval != NONAME && o!=PLUS && o!=MINUS ) return(0);

	if( SZLONG > SZINT ){
		if( p->in.op == ICON && ( p->in.type == LONG || q->in.type == LONG ) )
				p->in.type = q->in.type = LONG;
		}

	switch( o ){
	default:        return(0);
	case MINUS:     CONMINUS (p, val, val2);        break;
	case MUL:       CONMUL (p, val, val2);          break;
	case AND:       CONAND (p, val, val2);          break;
	case OR:        CONOR (p, val, val2);           break;
	case ER:        CONER (p, val, val2);           break;
	case LS:        CONLS (p, val, val2);           break;
	case RS:        CONRS (p, val, val2);           break;
	case NOT:       CONNOT (p);                     break;
	case LT:        CONLT (p, val, val2);           break;
	case LE:        CONLE (p, val, val2);           break;
	case GT:        CONGT (p, val, val2);           break;
	case GE:        CONGE (p, val, val2);           break;
	case ULT:       CONULT (p, val, val2);          break;
	case ULE:       CONULE (p, val, val2);          break;
	case UGE:       CONUGE (p, val, val2);          break;
	case UGT:       CONUGT (p, val, val2);          break;
	case EQ:        CONEQ (p, val, val2);           break;
	case NE:        CONNE (p, val, val2);           break;
	case PLUS:
		CONPLUS (p, val, val2);
		if( p->tn.rval == NONAME ){
			p->tn.rval = q->tn.rval;
			p->in.type = q->in.type;
			}
		break;
	case DIV:
		/* "division by 0" */
		if( val == 0 ) UERROR( MESSAGE( 31 ) );
		else
			CONDIV (p, val, val2);
		break;
	case MOD:
		/* "division by 0" */
		if( val == 0 ) UERROR( MESSAGE( 31 ) );
		else
			CONMOD (p, val, val2);
		break;
	case UNARY MINUS:
		if( SZLONG > SZINT && p->tn.type == UNSIGNED ) /* its an ICON from sizeof */
			p->tn.lval = (unsigned)-p->tn.lval;
		else
			CONUNMINUS (p);
		break;
	case COMPL:
		if( SZLONG > SZINT && p->tn.type == UNSIGNED ) /* its an ICON from sizeof */
			p->tn.lval = (unsigned)~p->tn.lval;
		else
			CONCOMPL (p);
		break;
		}
	return(1);
	}

chkpun(p) register NODE *p; {

	/* checks p for the existance of a pun */

	/* this is called when the op of p is ASSIGN, RETURN, CAST, COLON, or relational */

	/* one case is when enumerations are used: this applies only to lint */
	/* in the other case, one operand is a pointer, the other integer type */
	/* we check that this integer is in fact a constant zero... */

	/* in the case of ASSIGN, any assignment of pointer to integer is illegal */
	/* this falls out, because the LHS is never 0 */

	register NODE *q;
	register t1, t2;
	register d1, d2;

	t1 = p->in.left->in.type;
	t2 = p->in.right->in.type;

	if( t1==ENUMTY || t2==ENUMTY ) { /* check for enumerations */
		if( logop( p->in.op ) && p->in.op != EQ && p->in.op != NE ) {
			/* "illegal comparison of enums" */
			UERROR( MESSAGE( 54 ) );
			return;
			}
		if( t1==ENUMTY && t2==ENUMTY && p->in.left->fn.csiz==p->in.right->fn.csiz ) return;
		/* "enumeration type clash, operator %s" */
		WERROR( MESSAGE( 37 ), opst[p->in.op] );
		return;
		}

	if( ISPTR(t1) || ISARY(t1) ) q = p->in.right;
	else q = p->in.left;

	if( !ISPTR(q->in.type) && !ISARY(q->in.type) ){
		if( q->in.op != ICON || q->tn.lval || HIGHVAL(q) ){
			/* "illegal combination of pointer and integer, op %s" */
			WERROR( MESSAGE( 53 ), opst[p->in.op] );
			}
		}
	else {
		d1 = p->in.left->fn.cdim;
		d2 = p->in.right->fn.cdim;
		for( ;; ){
			if( t1 == t2 ) {;
				if( p->in.left->fn.csiz != p->in.right->fn.csiz ) {
					/* "illegal structure pointer combination" */
					WERROR( MESSAGE( 69 ) );
					}
				return;
				}
#ifdef LINT

			/* changes 10/23/80 - complain about pointer casts if cflag
			 *	is set (this implies pflag is also set)
			 */
			if ( p->in.op == CAST ) /* this implies cflag is set */
				if( ISPTR(t1) && ISPTR(t2) ) {
					/* pointer casts may be troublesome */
					WERROR( MESSAGE( 98 ) );
					return;
				}

#endif
			if( ISARY(t1) || ISPTR(t1) ){
				if( !ISARY(t2) && !ISPTR(t2) ) break;
				if( ISARY(t1) && ISARY(t2) && dimtab[d1] != dimtab[d2] ){
					/* "illegal array size combination" */
					WERROR( MESSAGE( 49 ) );
					return;
					}
				if( ISARY(t1) ) ++d1;
				if( ISARY(t2) ) ++d2;
				}
			else break;
			t1 = DECREF(t1);
			t2 = DECREF(t2);
			}
		/* "illegal pointer combination" */
		WERROR( MESSAGE( 66 ) );
		}

	}

NODE *
stref( p ) register NODE *p; {

	TWORD t;
	int d, s, dsc, align;
	OFFSZ off;
	register struct symtab *q;

	/* make p->x */
	/* this is also used to reference automatic variables */

	q = &stab[p->in.right->tn.rval];
	p->in.right->in.op = FREE;
	p->in.op = FREE;
	p = pconvert( p->in.left );

	/* make p look like ptr to x */

	if( !ISPTR(p->in.type)){
		p->in.type = PTR+UNIONTY;
		}

	t = INCREF( q->stype );
	d = q->dimoff;
	s = q->sizoff;

	p = makety( p, t, d, s );

	/* compute the offset to be added */

	off = q->offset;
	dsc = q->sclass;

	if( dsc & FIELD ) {  /* normalize offset */
		align = ALINT;
		s = INT;
		off = (off/align)*align;
		}
	if( off != 0 ) p = clocal( block( PLUS, p, offcon( off, t, d, s ), t, d, s ) );

	p = buildtree( UNARY MUL, p, NIL );

	/* if field, build field info */

	if( dsc & FIELD ){
		p = block( FLD, p, NIL, q->stype, 0, q->sizoff );
		p->tn.rval = PKFIELD( dsc&FLDSIZ, q->offset%align );
		}

	return( clocal(p) );
	}

notlval(p) register NODE *p; {

	/* return 0 if p an lvalue, 1 otherwise */

	again:

	switch( p->in.op ){

	case FLD:
		p = p->in.left;
		goto again;

	case UNARY MUL:
		/* fix the &(a=b) bug, given that a and b are structures */
		if( p->in.left->in.op == STASG ) return( 1 );
		/* and the f().a bug, given that f returns a structure */
		/* x = f().a is not a bug, it is allowed	*/

	case NAME:
	case OREG:
		if( ISARY(p->in.type) || ISFTN(p->in.type) ) return(1);
	case REG:
		return(0);

	default:
		return(1);

		}

	}

NODE *
bcon( i ) CONSZ i; { /* make a constant node with value i */
	register NODE *p;

	p = block( ICON, NIL, NIL, INT, 0, INT );
	ZEROVAL (p);
	p->tn.lval = i;
	p->tn.rval = NONAME;
	return( clocal(p) );
	}

NODE *
bpsize(p) register NODE *p; {
	return( offcon( psize(p), p->in.type, p->fn.cdim, p->fn.csiz ) );
	}

OFFSZ
psize( p ) NODE *p; {
	/* p is a node of type pointer; psize returns the
	   size of the thing pointed to */

	if( !ISPTR(p->in.type) ){
		/* "pointer required" */
		UERROR( MESSAGE( 90 ));
		return( SZINT );
		}
	/* note: no pointers to fields */
	return( tsize( DECREF(p->in.type), p->fn.cdim, p->fn.csiz ) );
	}

NODE *
convert( p, f )  register NODE *p; {
	/*  convert an operand of p
	    f is either CVTL or CVTR
	    operand has type int, and is converted by the size of the other side
	    */

	register NODE *q, *r;

	q = (f==CVTL)?p->in.left:p->in.right;

	r = block( PMCONV,
		q, bpsize(f==CVTL?p->in.right:p->in.left), INT, 0, INT );
	r = clocal(r);
	if( f == CVTL )
		p->in.left = r;
	else
		p->in.right = r;
	return(p);

	}

econvert( p ) register NODE *p; {

	/* change enums to ints, or appropriate types */

	register TWORD ty;

	if( (ty=BTYPE(p->in.type)) == ENUMTY || ty == MOETY ) {
		if( dimtab[ p->fn.csiz ] == SZCHAR ) ty = CHAR;
		else if( dimtab[ p->fn.csiz ] == SZINT ) ty = INT;
		else if( dimtab[ p->fn.csiz ] == SZSHORT ) ty = SHORT;
		else ty = LONG;
		ty = ctype( ty );
		p->fn.csiz = ty;
		MODTYPE(p->in.type,ty);
		if( p->in.op == ICON && ty != LONG ) p->in.type = p->fn.csiz = INT;
		}
	}

NODE *
pconvert( p ) register NODE *p; {

	/* if p should be changed into a pointer, do so */

	if( ISARY( p->in.type) ){
		p->in.type = DECREF( p->in.type );
		++p->fn.cdim;
		return( buildtree( UNARY AND, p, NIL ) );
		}
	if( ISFTN( p->in.type) )
		return( buildtree( UNARY AND, p, NIL ) );

	return( p );
	}

NODE *
oconvert(p) register NODE *p; {
	/* convert the result itself: used for pointer and unsigned */

	switch(p->in.op) {

	case LE:
	case LT:
	case GE:
	case GT:
		if( ISUNSIGNED(p->in.left->in.type) || ISUNSIGNED(p->in.right->in.type) )  p->in.op += (ULE-LE);
	case EQ:
	case NE:
		return( p );

	case MINUS:
		return(  clocal( block( PVCONV,
			p, bpsize(p->in.left), INT, 0, INT ) ) );
		}

	cerror( "illegal oconvert: %d", p->in.op );

	return(p);
	}

NODE *
ptmatch(p)  register NODE *p; {

	/* makes the operands of p agree; they are
	   either pointers or integers, by this time */
	/* with MINUS, the sizes must be the same */
	/* with COLON, the types must be the same */

	TWORD t1, t2, t;
	int o, d2, d, s2, s;

	o = p->in.op;
	t = t1 = p->in.left->in.type;
	t2 = p->in.right->in.type;
	d = p->in.left->fn.cdim;
	d2 = p->in.right->fn.cdim;
	s = p->in.left->fn.csiz;
	s2 = p->in.right->fn.csiz;

	switch( o ){

	case ASSIGN:
	case RETURN:
	case CAST:
		{  break; }

	case MINUS:
		{  if( psize(p->in.left) != psize(p->in.right) ){
			/* "illegal pointer subtraction" */
			UERROR( MESSAGE( 67 ));
			}
		   break;
		   }
	case COLON:
		/* "illegal types in :" */
		{  if( t1 != t2 ) UERROR( MESSAGE( 71 ));
		   break;
		   }
	default:  /* must work harder: relationals or comparisons */

		if( !ISPTR(t1) ){
			t = t2;
			d = d2;
			s = s2;
			break;
			}
		if( !ISPTR(t2) ){
			break;
			}

		/* both are pointers */
		if( talign(t2,s2) < talign(t,s) ){
			t = t2;
			s = s2;
			}
		break;
		}

	p->in.left = makety( p->in.left, t, d, s );
	p->in.right = makety( p->in.right, t, d, s );
	if( o!=MINUS && !logop(o) ){

		p->in.type = t;
		p->fn.cdim = d;
		p->fn.csiz = s;
		}

	return(clocal(p));
	}

int tdebug = 0;

NODE *
tymatch(p)  register NODE *p; {

	/* satisfy the types of various arithmetic binary ops */

	/* rules are:
		if assignment, op, type of LHS
		if any float or doubles, make double
		if any longs, make long
		otherwise, make int
		if either operand is unsigned, the result is...
	*/

	register TWORD t1, t2, t, tu;
	register o, u;

	o = p->in.op;

	t1 = p->in.left->in.type;
	t2 = p->in.right->in.type;
	if( (t1==UNDEF || t2==UNDEF) && o!=CAST )
		/* "void type illegal in expression" */
		UERROR(MESSAGE( 118 ));

	u = 0;
	if( ISUNSIGNED(t1) ){
		u = 1;
		t1 = DEUNSIGN(t1);
		}
	if( ISUNSIGNED(t2) ){
		u = 1;
		t2 = DEUNSIGN(t2);
		}

	if( ( t1 == CHAR || t1 == SHORT ) && o!= RETURN && !asgop(o) ) t1 = INT;
	if( t2 == CHAR || t2 == SHORT ) t2 = INT;

	if( t1==DOUBLE || t1==FLOAT || t2==DOUBLE || t2==FLOAT ) t = DOUBLE;
	else if( t1==LONG || t2==LONG ) t = LONG;
	else t = INT;

	if( asgop(o) ){
		tu = p->in.left->in.type;
		t = t1;
		}
	else {
		tu = (u && UNSIGNABLE(t))?ENUNSIGN(t):t;
		}

	/* because expressions have values that are at least as wide
	   as INT or UNSIGNED, the only conversions needed
	   are those involving FLOAT/DOUBLE, and those
	   from LONG to INT and ULONG to UNSIGNED */

	if( t != t1 ) p->in.left = makety( p->in.left, tu, 0, (int)tu );

	if( t != t2 || o==CAST ) p->in.right = makety( p->in.right, tu, 0, (int)tu );

	/* look to see if the value of the assignop is re-used, if so
	* be sure to convert it to an INT.
	* - this, unfortunately causes problems with register variables!
	*
	if ( asgop( p->in.right->in.op ) && ( p->in.right->in.type == CHAR
		|| p->in.right->in.type == SHORT ) )
	{
		p->in.right = makety( p->in.right, INT, 0, INT );
	}
	if ( asgop( p->in.left->in.op ) && ( p->in.left->in.type == CHAR
		|| p->in.left->in.type == SHORT ) )
	{
		p->in.left = makety( p->in.left, INT, 0, INT );
	}
	*/
	if( asgop(o) ){
		p->in.type = p->in.left->in.type;
		p->fn.cdim = p->in.left->fn.cdim;
		p->fn.csiz = p->in.left->fn.csiz;
		}
	else if( !logop(o) ){
		p->in.type = tu;
		p->fn.cdim = 0;
		p->fn.csiz = t;
		}

# ifndef BUG1
	if( tdebug ) printf( "tymatch(%lo): %o %s %o => %o\n",p,t1,opst[o],t2,tu );
# endif

	return(p);
	}

NODE *
makety( p, t, d, s ) register NODE *p; TWORD t; {
	/* make p into type t by inserting a conversion */

	if( p->in.type == ENUMTY && p->in.op == ICON ) econvert(p);
	if( t == p->in.type ){
		p->fn.cdim = d;
		p->fn.csiz = s;
		return( p );
		}

	if( t & TMASK ){
		/* non-simple type */
# ifdef EARLYOPT
		return( clocal( block( PCONV, p, NIL, t, d, s )) );
# else
		return( block( PCONV, p, NIL, t, d, s) );
# endif
		}

	if( p->in.op == ICON ){
		if( t==DOUBLE||t==FLOAT ){
			p->in.op = FCON;
			if( ISUNSIGNED(p->in.type) ){
				p->fpn.dval = /* ??? */ p->tn.lval;
				}
			else {
				p->fpn.dval = p->tn.lval;
				}

			p->in.type = p->fn.csiz = t;
			return( clocal(p) );
			}
		}

# ifdef EARLYOPT
	return( clocal( block( SCONV, p, NIL, t, d, s )) );
# else
	return( block( SCONV, p, NIL, t, d, s) );
# endif

	}

NODE *
block( o, l, r, t, d, s ) register NODE *l, *r; TWORD t; {

	register NODE *p;

	p = talloc();
	p->in.op = o;
	p->in.left = l;
	p->in.right = r;
	p->in.type = t;
	p->fn.cdim = d;
	p->fn.csiz = s;
	return(p);
	}

icons(p) register NODE *p; {
	/* if p is an integer constant, return its value */
	CONSZ val;

	if( p->in.op != ICON ){
		/* "constant expected" */
		UERROR( MESSAGE( 23 ));
		val = 1;
		}
	else {
		val = p->tn.lval;
		/* "constant too big for cross-compiler" */
		if( val != p->tn.lval ) UERROR( MESSAGE( 25 ) );
		}
	tfree( p );
	return(val);
	}

/* 	the intent of this table is to examine the
	operators, and to check them for
	correctness.

	The table is searched for the op and the
	modified type (where this is one of the
	types INT (includes char and short), LONG,
	DOUBLE (includes FLOAT), and POINTER

	The default action is to make the node type integer

	The actions taken include:
		PUN	  check for puns
		CVTL	  convert the left operand
		CVTR	  convert the right operand
		TYPL	  the type is determined by the left operand
		TYPR	  the type is determined by the right operand
		TYMATCH	  force type of left and right to match, by inserting conversions
		PTMATCH	  like TYMATCH, but for pointers
		LVAL	  left operand must be lval
		CVTO	  convert the op
		NCVT	  do not convert the operands
		OTHER	  handled by code
		NCVTR	  convert the left operand, not the right...

	*/

# define MINT 01  /* integer */
# define MDBI 02   /* integer or double */
# define MSTR 04  /* structure */
# define MPTR 010  /* pointer */
# define MPTI 020  /* pointer or integer */
# define MENU 040 /* enumeration variable or member */

opact( p )  NODE *p; {

	register mt12, mt1, mt2, o;

	mt12 = 0;

	switch( optype(o=p->in.op) ){

	case BITYPE:
		mt12=mt2 = moditype( p->in.right->in.type );
	case UTYPE:
		mt12 &= (mt1 = moditype( p->in.left->in.type ));

		}

	switch( o ){

	case NAME :
	case STRING :
	case ICON :
	case FCON :
	case CALL :
	case UNARY CALL:
	case UNARY MUL:
		{  return( OTHER ); }
	case UNARY MINUS:
		if( mt1 & MDBI ) return( TYPL );
		break;

	case COMPL:
		if( mt1 & MINT ) return( TYPL );
		break;

	case UNARY AND:
		{  return( NCVT+OTHER ); }
	case CBRANCH:
	case NOT:
		if ( !( mt1 & MSTR ) ) return( 0 );
		break;
	case INIT:
	case CM:
		return( 0 );
	case ANDAND:
	case OROR:
		if( (!( mt1 & MSTR ) ) & (!( mt2 & MSTR ) ) )
			return( 0 );
		break;

	case MUL:
	case DIV:
		if( mt12 & MDBI ) return( TYMATCH );
		break;

	case MOD:
	case AND:
	case OR:
	case ER:
		if( mt12 & MINT ) return( TYMATCH );
		break;

	case LS:
	case RS:
		if( mt12 & MINT ) return( TYMATCH+OTHER );
		break;

	case EQ:
	case NE:
	case LT:
	case LE:
	case GT:
	case GE:
		if( (mt1&MENU)||(mt2&MENU) ) return( PTMATCH+PUN+NCVT );
		if( mt12 & MDBI ) return( TYMATCH+CVTO );
		else if( mt12 & MPTR ) return( PTMATCH+PUN );
		else if( mt12 & MPTI ) return( PTMATCH+PUN );
		else break;

	case QUEST:
	case COMOP:
		if( mt2&MENU ) return( TYPR+NCVTR );
		return( TYPR );

	case STREF:
		return( NCVTR+OTHER );

	case FORCE:
		return( TYPL );

	case COLON:
		if( mt12 & MENU ) return( NCVT+PUN+PTMATCH );
		else if( mt12 & MDBI ) return( TYMATCH );
		else if( mt12 & MPTR ) return( TYPL+PTMATCH+PUN );
		else if( (mt1&MINT) && (mt2&MPTR) ) return( TYPR+PUN );
		else if( (mt1&MPTR) && (mt2&MINT) ) return( TYPL+PUN );
		else if( mt12 & MSTR ) return( NCVT+TYPL+OTHER );
		break;

	case ASSIGN:
	case RETURN:
		if( mt12 & MSTR ) return( LVAL+NCVT+TYPL+OTHER );
	case CAST:
		if(o==CAST && mt1==0)return(TYPL+TYMATCH);
		if( mt12 & MDBI ) return( TYPL+LVAL+TYMATCH );
		else if( (mt1&MENU)||(mt2&MENU) ) return( LVAL+NCVT+TYPL+PTMATCH+PUN );
		/* if right is TVOID and looks like a CALL break */
		else if( mt2 == 0 && ( p->in.right->in.op == CALL
				|| p->in.right->in.op == UNARY CALL) ) break;
		else if( mt1 & MPTR ) return( LVAL+PTMATCH+PUN );
		else if( mt12 & MPTI ) return( TYPL+LVAL+TYMATCH+PUN );
		break;

	case ASG LS:
	case ASG RS:
		if( mt12 & MINT ) return( TYPL+LVAL+OTHER );
		break;

	case ASG MUL:
	case ASG DIV:
		if( mt12 & MDBI ) return( LVAL+TYMATCH );
		break;

	case ASG MOD:
	case ASG AND:
	case ASG OR:
	case ASG ER:
		if( mt12 & MINT ) return( LVAL+TYMATCH );
		break;

	case ASG PLUS:
	case ASG MINUS:
	case INCR:
	case DECR:
		if( mt12 & MDBI ) return( TYMATCH+LVAL );
		else if( (mt1&MPTR) && (mt2&MINT) ) return( TYPL+LVAL+CVTR );
		break;

	case MINUS:
		if( mt12 & MPTR ) return( CVTO+PTMATCH+PUN );
		if( mt2 & MPTR ) break;
	case PLUS:
		if( mt12 & MDBI ) return( TYMATCH );
		else if( (mt1&MPTR) && (mt2&MINT) ) return( TYPL+CVTR );
		else if( (mt1&MINT) && (mt2&MPTR) ) return( TYPR+CVTL );

		}
	/* "operands of %s have incompatible types" */
	UERROR( MESSAGE( 89 ), opst[o] );
	return( NCVT );
	}

moditype( ty ) TWORD ty; {

	switch( ty ){

	case TVOID:
	case UNDEF:
		return(0); /* type is void */
	case ENUMTY:
	case MOETY:
		return( MENU );

	case STRTY:
	case UNIONTY:
		return( MSTR );

	case CHAR:
	case SHORT:
	case UCHAR:
	case USHORT:
		return( MINT|MPTI|MDBI );
	case UNSIGNED:
	case ULONG:
	case INT:
	case LONG:
		return( MINT|MDBI|MPTI );
	case FLOAT:
	case DOUBLE:
		return( MDBI );
	default:
		return( MPTR|MPTI );

		}
	}

NODE *
doszof( p )  register NODE *p; {
	/* do sizeof p */
	OFFSZ i;

	/* whatever is the meaning of this if it is a bitfield? */
	i = tsize( p->in.type, p->fn.cdim, p->fn.csiz )/SZCHAR;

	tfree(p);
	/* "sizeof returns 0" */
	if( i <= 0 ) WERROR( MESSAGE( 99 ) );
	p = bcon( i );
		/* sizeof is an unsigned ICON, change the type here and */
		/* look for it when folding constants in conval		*/
	p->in.type = UNSIGNED;
	return( p );
	}

# ifndef BUG2
eprint( p, down, a, b ) register NODE *p; int *a, *b; {
	register ty;

	*a = *b = down+1;
	while( down > 1 ){
		printf( "\t" );
		down -= 2;
		}
	if( down ) printf( "    " );

	ty = optype( p->in.op );

	printf("%lo) %s, ", p, opst[p->in.op] );
	if( ty == LTYPE ){
		printf( CONFMT, p->tn.lval );
		printf( ", %d, ", p->tn.rval );
		}
	tprint( p->in.type );
	printf( ", %d, %d\n", p->fn.cdim, p->fn.csiz );
	}
# endif

prtdcon( p ) register NODE *p; {
	int i;

	if( p->in.op == FCON ){
		if( !SETDCON(p) ){ /* does code generator support FCON */
			locctr( DATA );
			defalign( ALDOUBLE );
			deflab( i = getlab() );
			fincode( p->fpn.dval, SZDOUBLE );
			ZEROVAL (p);
			p->tn.rval = -i;
			p->in.type = DOUBLE;
			p->in.op = NAME;
			}
		}
	}


int edebug = 0;
ecomp( p ) register NODE *p; {
# ifndef BUG2
	if( edebug ) fwalk( p, eprint, 0 );
# endif
	if( !reached && !reachflg ){
		/* "statement not reached"  */
		WERROR( MESSAGE( 100 ) );
		reached = 1;
		}
	p = optim(p);
	walkf( p, prtdcon );
	locctr( PROG );
	ecode( p );
	tfree(p);
	}


# ifdef STDPRTREE
# ifndef ONEPASS

prtree(p) register NODE *p; {

	register struct symtab *q;
	register ty;

# ifdef MYPRTREE
	MYPRTREE(p);  /* local action can be taken here; then return... */
#endif

	ty = optype(p->in.op);

	printf( "%d\t", p->in.op );

	if( ty == LTYPE ) {
		printf( CONFMT, p->tn.lval );
		printf( "\t" );
		}
	if( ty != BITYPE ) {
		if( p->in.op == NAME || p->in.op == ICON ) printf( "0\t" );
		else printf( "%d\t", p->tn.rval );
		}

	printf( "%o\t", p->in.type );

	/* handle special cases */

	switch( p->in.op ){

	case NAME:
	case ICON:
		/* print external name */
		if( p->tn.rval == NONAME ) printf( "\n" );
		else if( p->tn.rval >= 0 ){
			q = &stab[p->tn.rval];
#ifdef FLEXNAMES
			printf(  "%s\n", exname(q->sname) );
#else
			printf(  "%.8s\n", exname(q->sname) );
#endif
			}
		else { /* label */
			printf( LABFMT, -p->tn.rval );
			}
		break;

	case STARG:
	case STASG:
	case STCALL:
	case UNARY STCALL:
		/* print out size */
		/* use lhs size, in order to avoid hassles with the structure `.' operator */

		/* note: p->in.left not a field... */
		printf( CONFMT, (CONSZ) tsize( STRTY, p->in.left->fn.cdim, p->in.left->fn.csiz ) );
		printf( "\t%d\t\n", talign( STRTY, p->in.left->fn.csiz ) );
		break;

	default:
		printf(  "\n" );
		}

	if( ty != LTYPE ) prtree( p->in.left );
	if( ty == BITYPE ) prtree( p->in.right );

	}

# else

p2tree(p) register NODE *p; {
	register ty;

# ifdef MYP2TREE
	MYP2TREE(p);  /* local action can be taken here; then return... */
# endif

	ty = optype(p->in.op);

	switch( p->in.op ){

	case NAME:
	case ICON:
#ifdef FLEXNAMES
		if( p->tn.rval == NONAME ) p->in.name = "";
#else
		if( p->tn.rval == NONAME ) p->in.name[0] = '\0';
#endif
		else if( p->tn.rval >= 0 ){ /* copy name from exname */
			register char *cp;
			register i;
			cp = exname( stab[p->tn.rval].sname );
#ifdef FLEXNAMES
			p->in.name = tstr( cp );
#else
			for( i=0; i<NCHNAM; ++i ) p->in.name[i] = *cp++;
#endif
			}
		else
#ifdef FLEXNAMES
		{
			char temp[32];

			sprintf( temp, LABFMT, -p->tn.rval );
			p->in.name = tstr( temp );
		}
#else
			sprintf( p->in.name, LABFMT, -p->tn.rval );
#endif

	case OREG:
	case FCON:
		break;

	case STARG:
	case STASG:
	case STCALL:
	case UNARY STCALL:
		/* set up size parameters */
		p->stn.stsize = (tsize(STRTY,p->in.left->fn.cdim,p->in.left->fn.csiz)+SZCHAR-1)/SZCHAR;
		p->stn.stalign = talign(STRTY,p->in.left->fn.csiz)/SZCHAR;
		break;

	case REG:
		rbusy( p->tn.rval, p->in.type );

	default:
#ifdef FLEXNAMES
		p->in.name = "";
#else
		p->in.name[0] = '\0';
#endif
		}

	p->in.rall = NOPREF;

	if( ty != LTYPE ) p2tree( p->in.left );
	if( ty == BITYPE ) p2tree( p->in.right );
	}

# endif
# endif
