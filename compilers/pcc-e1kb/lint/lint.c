/*	@(#)lint.c	1.7	*/
/*	3.0 SID #	1.2	*/
/* lint.c
 *	This file contains the major functions for the first pass of lint
 *
 *	There are basically two types of functions in this file:
 *	functions called directly by the portable C compiler and other
 *	(lint) functions.
 *
 *	lint functions:
 *	===============
 *		astype	hashes a struct/union line number to ensure uniqueness
 *		ctargs	count arguments of a function
 *		fldcon	check assignment of a constant to a field
 *		fsave	write out a new filename to the intermediate file
 *		lmerge
 *		lprt
 *		lpta
 *		main	driver routine for the first pass (lint1)
 *		outdef	write info out to the intermediate file
 *		strip	strip a filename down to its basename
 *		where	print location of an error
 *
 *	pcc interface routines:
 *	=======================
 *		andable		returns 1 if a node can accept the & operator
 *		aocode		called when an automatic is removed from symbol table
 *		bfcode		emit code for beginning of function
 *		branch		dummy, unused by lint
 *		bycode		dummy, unused by lint
 *		cinit		
 *		cisreg		dummy for lint; returns 1
 *		clocal		do local transformations on the tree
 *		commdec		put out a common declaration
 *		ctype		dummy for lint
 *		defalign	dummy, unused by lint
 *		deflab		dummy, unused by lint
 *		defname		
 *		ecode		emit code for tree (lint processing for whole tree)
 *		efcode		emit code for end of function
 *		ejobcode	end of job processing
 *		exname		create external name
 *		fldal		check field alignment
 *		fldty		check field type
 *		isitfloat
 *		noinit		return storage class for uninitialized objects
 *		offcon		make structure offset node
 *		zecode		create arg integer words of zero
 */

# include "mfile1"
# include "messages.h"

# include "lerror.h"
# include "lmanifest"

# include <signal.h>

# define VAL 0
# define EFF 1

/* these are appropriate for the -p flag */
/* October 1980 -
 *	It would be nice if these values could be set so as to eliminate any
 *	possibility of machine dependency.  When we tried this by making
 *	the sizes relatively prime, several obscure problems were encountered.
 *	PCC apparently makes some sort of assumption(s) that can result in a
 *	compiler error when these sizes are used.
 */

int	SZCHAR	=	8;
int	SZSHORT	=	16;
int	SZINT	=	16;
int	SZLONG	= 	32;
int	SZFLOAT	=	32;
int	SZDOUBLE=	64;
int	SZPOINT	=	16;
int	ALCHAR	=	8;
int	ALSHORT	=	16;
int	ALINT	=	16;
int	ALLONG	=	32;
int	ALFLOAT	=	32;
int	ALDOUBLE=	64;
int	ALPOINT	=	16;
int	ALSTRUCT=	16;

int vflag = 1;  /* tell about unused argments */
/* 28 feb 80  reverse sense of xflag */
int xflag = 1;  /* tell about unused externals */
int argflag = 0;  /* used to turn off complaints about arguments */
int libflag = 0;  /* used to generate library descriptions */
int vaflag = -1;  /* used to signal functions with a variable number of args */
/* 28 feb 80  reverse sense of aflag */
int aflag = 1;  /* used to check precision of assignments */

int wloop_level = LL_TOP;	/* cause no unnecessary set/usage complaints */
int floop_level = LL_TOP;	/* cause no unnecessary set/usage complaints */

#ifdef FLEXNAMES
	char sourcename[ BUFSIZ + 1 ] = "";
#else
	char sourcename[ LFNM ] = "";	/* used to determine name of source file */
#endif

	/* flags for the "outdef" function */
# define USUAL (-101)
# define DECTY (-102)
# define NOFILE (-103)
# define SVLINE (-104)

# define LNAMES 250

struct lnm {
	short lid, flgs;
	}  lnames[LNAMES], *lnp;

/* contx - check context of node
 *	contx is called for each node during tree walk (fwalk);
 *	it complains about nodes that have null effect.
 *	VAL is passed to a child if that child's value is used
 *	EFF is passed to a child if that child is used in an effects context
 *
 *	arguments:
 *		p - node pointer
 *		down - value passed down from ancestor
 *		pl, pr - pointers to values to be passed down to descendants
 */
contx( p, down, pl, pr ) register NODE *p; register *pl, *pr;
{
	*pl = *pr = VAL;
	switch( p->in.op ){

		/* left side of ANDAND, OROR, and QUEST always evaluated for value
	 	   (value determines if right side is to be evaluated) */
		case ANDAND:
		case OROR:
		case QUEST:
			*pr = down; break;

		/* left side and right side treated identically */
		case SCONV:
		case PCONV:
		case COLON:
			*pr = *pl = down; break;

		/* comma operator uses left side for effect */
		case COMOP:
			*pl = EFF;
			*pr = down;

		case FORCE:
		case INIT:
		case UNARY CALL:
		case STCALL:
		case UNARY STCALL:
		case CALL:
		case UNARY FORTCALL:
		case FORTCALL:
		case CBRANCH:
			break;

		default:
			/* assignment ops are OK */
			if( asgop(p->in.op) ) break;

			/* struct x f( );  main( ) {  (void) f( ); }
		 	 *  the the cast call appears as U* UNDEF 
			 */
			if( p->in.op == UNARY MUL &&
				( p->in.type == STRTY
				|| p->in.type == UNIONTY
				|| p->in.type == UNDEF) )
				break;  /* the compiler does this... */

			/* found a null effect ... */
			if( down == EFF && hflag ) WERROR( MESSAGE( 86 ) );
	}
}
/* ecode - compile code for node */
ecode( p ) NODE *p;
{
	fwalk( p, contx, EFF );	/* do a preorder tree walk */
	lnp = lnames;		/* initialize pointer to start of array */
	lprt( p, EFF, 0 );
}

ejobcode( flag )
{	/* called after processing each job */
	/* flag is nonzero if errors were detected */
	register k;
	register struct symtab *p;
	extern		hdrclose( ),
			unbuffer( );

	for( p=stab; p< &stab[SYMTSZ]; ++p ){

		if( p->stype != TNULL ) {

			if( p->stype == STRTY || p->stype == UNIONTY )
				if( dimtab[p->sizoff+1] < 0 )
					/* "struct/union %.8s never defined" */
					/* "struct/union %s never defined" */
					if( hflag ) WERROR( MESSAGE( 102 ), p->sname );

			switch( p->sclass ){
			
			case STATIC:
				if( p->suse > 0 ){
					k = lineno;
					lineno = p->suse;
					/* "static %.8s unused" */
					/* "static %s unused" */
					UERROR( MESSAGE( 101 ), p->sname );
					lineno = k;
					break;
					}

			case EXTERN:
			case USTATIC:
				/* with the xflag, worry about externs not used */
				/* the filename may be wrong here... */
				if( xflag && p->suse >= 0 && !libflag )
					outdef( p, LDX, NOFILE );
			
			case EXTDEF:
				if( p->suse < 0 )  /* used */
					outdef( p, LUM, SVLINE );
				break;
			}
		}
	}
	hdrclose( );
	unbuffer( );
	exit( 0 );
}
/* astype - hash a struct/union line number to ensure uniqueness */
astype( t, i ) ATYPE *t;
{ TWORD tt;
	int j, k=0;

	if( (tt=BTYPE(t->aty))==STRTY || tt==UNIONTY ) {
		if( i<0 || i>= DIMTABSZ-3 )
			uerror( "lint's little mind is blown" );
		else {
			j = dimtab[i+3];
			if( j<0 || j>SYMTSZ )
				k = ((-j)<<5)^dimtab[i]|1;
			else
				if( stab[j].suse <= 0 )
#ifdef FLEXNAMES
					uerror( "no line number for %s",
						stab[j].sname );
#else
					uerror( "no line number for %.8s",
						stab[j].sname );
#endif
				else k = (stab[j].suse<<5) ^ dimtab[i];
			}
		
		t->extra = k;
		return( 1 );
	}
	else return( 0 );
}
/* bfcode - handle the beginning of a function */
bfcode( a, n ) int a[];
{	/* code for the beginning of a function; a is an array of
		indices in stab for the arguments; n is the number */
	/* this must also set retlab */
	register i;
	register struct symtab *cfp;
	static ATYPE t;

	retlab = 1;
	cfp = &stab[curftn];

	/* if variable number of arguments,
	  only print the ones which will be checked */
	if( vaflag >= 0 )
		if( n < vaflag )
		  uerror( "declare the VARARGS arguments you want checked!" );
		else n = vaflag;

	fsave( ftitle );
	outdef( cfp, cfp->sclass==STATIC? LDS : (libflag?LIB:LDI),
		vaflag >= 0 ? -(n + 1) : n );
	vaflag = -1;

	/* output the arguments */
	if( n )
		for( i=0; i<n; ++i ) {
			t.aty = stab[a[i]].stype;
			t.extra = 0;
			if( !astype( &t, stab[a[i]].sizoff ) ) 
				switch( t.aty ){

				case ULONG:
					break;

				case CHAR:
				case SHORT:
					t.aty = INT;
					break;

				case UCHAR:
				case USHORT:
				case UNSIGNED:
					t.aty = UNSIGNED;
					break;

				}
			fwrite( (char *)&t, sizeof(ATYPE), 1, stdout );
			}
}
/* ctargs - count arguments; p points to at least one */
ctargs( p ) NODE *p;
{
	/* the arguemnts are a tower of commas to the left */
	register c;
	c = 1; /* count the rhs */
	while( p->in.op == CM ){
		++c;
		p = p->in.left;
		}
	return( c );
}
/* lpta */
lpta( p ) NODE *p;
{
	static ATYPE t;

	if( p->in.op == CM ){
		lpta( p->in.left );
		p = p->in.right;
		}

	t.aty = p->in.type;
	t.extra = (p->in.op==ICON);

	if( !astype( &t, p->in.csiz ) )
		switch( t.aty ){

			case CHAR:
			case SHORT:
				t.aty = INT;
			case LONG:
			case ULONG:
			case INT:
			case UNSIGNED:
				break;

			case UCHAR:
			case USHORT:
				t.aty = UNSIGNED;
				break;

			case FLOAT:
				t.aty = DOUBLE;
				t.extra = 0;
				break;

			default:
				t.extra = 0;
				break;
		}
	fwrite( (char *)&t, sizeof(ATYPE), 1, stdout );
}

# define VALSET 1
# define VALUSED 2
# define VALASGOP 4
# define VALADDR 8

lprt( p, down, uses ) register NODE *p;
{
	register struct symtab *q;
	register id;
	register acount;
	register down1, down2;
	register use1, use2;
	register struct lnm *np1, *np2;

	/* first, set variables which are set... */

	use1 = use2 = VALUSED;
	if( p->in.op == ASSIGN ) use1 = VALSET;
	else if( p->in.op == UNARY AND ) use1 = VALADDR;
	else if( asgop( p->in.op ) ) { /* =ops */
		use1 = VALUSED|VALSET;
		if( down == EFF ) use1 |= VALASGOP;
		}


	/* print the lines for lint */

	down2 = down1 = VAL;
	acount = 0;

	switch( p->in.op ){

	case EQ:
	case NE:
		if( ISUNSIGNED(p->in.left->in.type) &&
		    p->in.right->in.op == ICON && p->in.right->tn.lval < 0 &&
		    p->in.right->tn.rval == NONAME &&
		    !ISUNSIGNED(p->in.right->in.type) )
			/* "comparison of unsigned with negative constant" */
			WERROR( MESSAGE( 21 ) );
		goto charchk;

	case GT:
	case GE:
	case LT:
	case LE:
		if (p->in.left->in.type == CHAR && p->in.right->in.op == ICON &&
		    p->in.right->tn.lval == 0)
			/* "nonportable character comparison" */
			WERROR(MESSAGE(82));
	charchk:
		if (p->in.left->in.type == CHAR && p->in.right->in.op==ICON &&
		   p->in.right->tn.lval < 0)
			/* "nonportable character comparison" */
			WERROR( MESSAGE( 82 ) );

		break;

	case UGE:
	case ULT:
		if( p->in.right->in.op == ICON && p->in.right->tn.lval == 0
		    && p->in.right->tn.rval == NONAME ) {
			/* "degenerate unsigned comparison" */
			WERROR( MESSAGE( 30 ) );
			break;
			}

	case UGT:
	case ULE:
	    if ( p->in.right->in.op == ICON && p->in.right->tn.rval == NONAME
		 && !ISUNSIGNED( p->in.right->in.type ) ) {
			if( p->in.right->tn.lval < 0 ) 
				/* "comparison of unsigned with negative constant" */
				WERROR( MESSAGE( 21 ) );

			if ( p->in.right->tn.lval == 0 )
				/* "unsigned comparison with 0?" */
				WERROR( MESSAGE( 115 ) );
	    }
	    break;

	case COMOP:
		down1 = EFF;

	case ANDAND:
	case OROR:
	case QUEST:
		down2 = down;
		/* go recursively left, then right  */
		np1 = lnp;
		lprt( p->in.left, down1, use1 );
		np2 = lnp;
		lprt( p->in.right, down2, use2 );
		lmerge( np1, np2, 0 );
		return;

	case SCONV:
	case PCONV:
	case COLON:
		down1 = down2 = down;
		break;

	case CALL:
	case STCALL:
	case FORTCALL:
		acount = ctargs( p->in.right );
	case UNARY CALL:
	case UNARY STCALL:
	case UNARY FORTCALL:
		if( p->in.left->in.op == ICON && (id=p->in.left->tn.rval) != NONAME ){
		  /* used to be &name */
			struct symtab *sp = &stab[id];
			int lty;
			/*  if a function used in an effects context is
			 *  cast to type  void  then consider its value
			 *  to have been disposed of properly
			 *  thus a call of type  undef  in an effects
			 *  context is construed to be used in a value
			 *  context
			 */
			if ((down == EFF) && (p->in.type != UNDEF)) lty = LUE;
			else if (down == EFF) lty = LUV | LUE;
			else lty = LUV;

			fsave( ftitle );
			outdef(sp, lty, acount);
			if( acount )
				lpta( p->in.right );
			}
		break;

	case ICON:
		/* look for &name case */
		if( (id = p->tn.rval) >= 0 && id != NONAME ){
			q = &stab[id];
			q->sflags |= (SREF|SSET);
		}
		return;

	case NAME:
		if( (id = p->tn.rval) >= 0 && id != NONAME ){
			q = &stab[id];
			if( (uses&VALUSED) && !(q->sflags&SSET) ){
				if( q->sclass == AUTO || q->sclass == REGISTER ){
					if( !ISARY(q->stype ) && !ISFTN(q->stype)
					  && q->stype!=STRTY ){
						/* "%.8s may be used before set" */
						/* "%s may be used before set" */
						WERROR( MESSAGE( 1 ), q->sname );
						q->sflags |= SSET;
					}
				}
			}
			if( uses & VALASGOP ) break;  /* not a real use */
			if( uses & VALSET ) q->sflags |= SSET;
			if( uses & VALUSED ) q->sflags |= SREF;
			if( uses & VALADDR ) q->sflags |= (SREF|SSET);
			if( p->tn.lval == 0 ){
				lnp->lid = id;
				lnp->flgs = (uses&VALADDR)?0:((uses&VALSET)?VALSET:VALUSED);
				if( ++lnp >= &lnames[LNAMES] ) --lnp;
			}
		}
		return;
	}

	/* recurse, going down the right side first if we can */

	switch( optype(p->in.op) ){

	case BITYPE:
		np1 = lnp;
		lprt( p->in.right, down2, use2 );
	case UTYPE:
		np2 = lnp;
		lprt( p->in.left, down1, use1 );
	}

	if( optype(p->in.op) == BITYPE ){
		if( p->in.op == ASSIGN && p->in.left->in.op == NAME )
		  /* special case for a =  .. a .. */
			lmerge( np1, np2, 0 );

		else lmerge( np1, np2, p->in.op != COLON );
		/* look for assignments to fields, and complain */
		if( p->in.op == ASSIGN && p->in.left->in.op == FLD
		  && p->in.right->in.op == ICON ) fldcon( p );
		}
}
/* lmerge */
lmerge( np1, np2, flag ) struct lnm *np1, *np2;
{
	/* np1 and np2 point to lists of lnm members, for the two sides
	 * of a binary operator
	 * flag is 1 if commutation is possible, 0 otherwise
	 * lmerge returns a merged list, starting at np1, resetting lnp
	 * it also complains, if appropriate, about side effects
	 */

	register struct lnm *npx, *npy;

	for( npx = np2; npx < lnp; ++npx ){

		/* is it already there? */
		for( npy = np1; npy < np2; ++npy ){
			if( npx->lid == npy->lid ){ /* yes */
				if( npx->flgs == 0 || npx->flgs == (VALSET|VALUSED) )
					;  /* do nothing */
				else
					if( (npx->flgs|npy->flgs)== (VALSET|VALUSED)
					  || (npx->flgs&npy->flgs&VALSET) )
						/* "%.8s evaluation order undefined" */
						/* "%s evaluation order undefined" */
						if( flag ) WERROR( MESSAGE( 0 ), stab[npy->lid].sname );

				if( npy->flgs == 0 ) npx->flgs = 0;
				else npy->flgs |= npx->flgs;
				goto foundit;
			}
		}

		/* not there: update entry */
		np2->lid = npx->lid;
		np2->flgs = npx->flgs;
		++np2;

		foundit: ;
	}

	/* all finished: merged list is at np1 */
	lnp = np2;
}
/* efcode - handle end of a function */
efcode()
{
	/* code for the end of a function */
	register struct symtab *cfp;

	cfp = &stab[curftn];
	if( retstat & RETVAL ) outdef( cfp, LRV, DECTY );
	if( !vflag ){
		vflag = argflag;
		argflag = 0;
	}
	if( retstat == RETVAL+NRETVAL )
		/* "function %.8s has return(e); and return;" */
		/* "function %s has return(e); and return;" */
		WERROR( MESSAGE( 43 ), cfp->sname);
	/*
	* See if main() falls off its end or has just a return;
	*/
	if (!strcmp(cfp->sname, "main") && (reached || (retstat & NRETVAL)))
		/* "main() returns random value to invocation environment" */
		WERROR(MESSAGE(127));
}
/* aocode - called when automatic p is removed from stab */
aocode(p) struct symtab *p;
{
	register struct symtab *cfs;
	cfs = &stab[curftn];
	if(p->suse>0 && !(p->sflags&(SMOS|STAG)) ){
		if( p->sclass == PARAM ) {
			/* "argument %.8s unused in function %.8s" */
			/* "argument %s unused in function %s" */
			if( vflag ) WERROR( MESSAGE( 13 ), p->sname, cfs->sname );
		}
		else
			/* "%.8s unused in function %.8s" */
			/* "%s unused in function %s" */
			if( p->sclass != TYPEDEF ) WERROR( MESSAGE( 6 ),
			  p->sname, cfs->sname );
	}

	if( p->suse < 0 && (p->sflags & (SSET|SREF|SMOS)) == SSET
	  && !ISARY(p->stype) && !ISFTN(p->stype) )
		/* "%.8s set but not used in function %.8s" */
		/* "%s set but not used in function %s" */
		WERROR( MESSAGE( 3 ), p->sname, cfs->sname );

	if( p->stype == STRTY || p->stype == UNIONTY || p->stype == ENUMTY )
		/* "structure %.8s never defined" */
		/* "structure %s never defined" */
		if( dimtab[p->sizoff+1] < 0 ) WERROR( MESSAGE( 104 ), p->sname );
}

/* defname - define the current location as the name p->sname */
defnam( p ) register struct symtab *p;
{
	if( p->sclass == STATIC && p->slevel>1 ) return;

	if( !ISFTN( p->stype ) )
		outdef( p, p->sclass==STATIC? LDS : (libflag?LIB:LDI), USUAL );
}

/* zecode - n integer words of zeros */
zecode( n )
{
	OFFSZ temp;
	temp = n;
	inoff += temp*SZINT;
}

/* andable - can the NAME node p accept &  ? */
andable( p ) NODE *p;
{
	register r;

	if( p->in.op != NAME ) cerror( "andable error" );

	if( (r = p->tn.rval) < 0 ) return(1);  /* labels are andable */

	if( stab[r].sclass == AUTO || stab[r].sclass == PARAM ) return(0); 
	/* "cannot take address of %.8s" */
	/* "cannot take address of %s" */
	if( stab[r].sclass == REGISTER ) UERROR( MESSAGE( 18 ), stab[r].sname );
	return(1);
}
/* clocal - do local checking on tree
 *	(pcc uses this routine to do local tree rewriting)
 */
NODE *
clocal(p) NODE *p;
{
	register o;
	register unsigned t, tl;

	switch( o = p->in.op ){

	case SCONV:
	case PCONV:
		if( p->in.left->in.type==ENUMTY )
			p->in.left = pconvert( p->in.left );

		/* assume conversion takes place; type is inherited */
		t = p->in.type;
		tl = p->in.left->in.type;
/* for the future: put aflag in a place where NAME is available; that is, check
 * assignment and arithmetic operators for leftchild NAME and rightchild SCONV
 * so that when message is printed it is possible to name the offending lval
 * note that the lval may be a temporary (NONAME)
 */
		/* 29 may 80  complain about loss of accuracy with long rhs
		 * only if longs are larger than ints
		 * also, no error if lhs is FLOAT or DOUBLE
		 */
		if( SZLONG > SZINT && aflag
		   && ( tl == LONG || tl == ULONG )
		   && t!=LONG && t!=ULONG && t!=UNDEF
		   && t!=FLOAT && t!=DOUBLE ) 
			/* "conversion from long may lose accuracy" */
			WERROR( MESSAGE( 26 ) );

		if( aflag && pflag && tl != LONG && tl != ULONG 
		  && tl != FLOAT && tl != DOUBLE
		  && ( t == LONG || t == ULONG )
		  && p->in.left->in.op != ICON )
			/* conversion to long may sign-extend incorrectly */
			WERROR( MESSAGE( 27 ) );

		if( ISPTR(tl) && ISPTR(t) ){
			tl = DECREF(tl);
			t = DECREF(t);
			switch( ISFTN(t) + ISFTN(tl) ){

			case 0:  /* neither is a function pointer */
				if( talign(t,p->fn.csiz) > talign(tl,p->in.left->fn.csiz) )
					/* "possible pointer alignment problem" */
					if( hflag||pflag ) WERROR( MESSAGE( 91 ) );
				break;

			case 1:
				/* "questionable conversion of function pointer" */
				WERROR( MESSAGE( 95 ) );

			case 2:
				;
			}
		}
		p->in.left->in.type = p->in.type;
		p->in.left->fn.cdim = p->fn.cdim;
		p->in.left->fn.csiz = p->fn.csiz;
		p->in.op = FREE;
		return( p->in.left );

	case PVCONV:
	case PMCONV:
		if( p->in.right->in.op != ICON ) cerror( "bad conversion");
		p->in.op = FREE;
		return( buildtree( o==PMCONV?MUL:DIV, p->in.left, p->in.right ) );
	}
	return(p);
}
/* offcon - make structure offset node */
NODE *
offcon( off, t, d, s ) OFFSZ off; TWORD t;
{
	register NODE *p;
	p = bcon(0);
	p->tn.lval = off/SZCHAR;
	return(p);
}

/* noinit - return storage class for such as "int a;" */
noinit()
{
	return( pflag ? EXTDEF : EXTERN );
}

/* cinit - initialize p into size sz */
cinit( p, sz ) NODE *p;
{
	inoff += sz;
	if( p->in.op == INIT ){
		if( p->in.left->in.op == ICON ) return;
		if( p->in.left->in.op == NAME && p->in.left->in.type == MOE ) return;
	}
	/* "illegal initialization" */
	UERROR( MESSAGE( 61 ) );
}

/* exname - make a name look like an external name
 *		if checking for portability or if running on gcos or ibm,
 *		truncate name to six characters
 */
char *
exname( p ) char *p;
{
	static char aa[8];
	register int i;

#if !( ibm | gcos )
	if( !pflag ) return(p);
#endif
	for( i=0; i<6; ++i ){
		if( *p>='A' && *p<='Z' ) aa[i] = *p - 'A' + 'a';
		else aa[i] = *p;
		if( *p ) ++p;
	}
	aa[6] = '\0';
	return( aa );
}
/* strip - strip full name to get basename */
char *
strip(s) char *s;
{
#ifdef FLEXNAMES
	static char x[BUFSIZ+1];
#else
	static char x[LFNM+1];
#endif
	register char *p;

	for( p=x; *s; ++s ){
		if( *s == '/' ) p=x;
		else if( *s != '"' ) *p++ = *s;

#ifdef FLEXNAMES
		if ( p > &x[BUFSIZ] ) {
#else
		if ( p > &x[LFNM] ) {
#endif
			/* 5 feb 80:  simulate a call to cerror( ) */
#ifdef FLEXNAMES
			fprintf(stderr, "%s: compiler error: filename too long\n", x);
#else
			fprintf(stderr, "%.15s: compiler error: filename too long\n", x);
#endif
			exit(1);
			/* cannot call cerror( )
			 * because cerror( ) calls where( ) and where( ) calls
			 * strip( ) and this is strip
			 */
		}
	}
	*p = '\0';
	return( x );
}

/* fsave - save file name on intermediate file */
fsave( s ) char *s; {
	static union rec fsname;
#ifdef FLEXNAMES
	static char buf[BUFSIZ+1];
#endif

	s = strip( s );
#ifdef FLEXNAMES
	if( strcmp( s, buf ) ){
#else
	if( strncmp( s, fsname.f.fn, LFNM ) ){
#endif
		/* new one */
#ifdef FLEXNAMES
		strcpy( buf, s );
#else
		strncpy( fsname.f.fn, s, LFNM );
#endif
		fsname.f.decflag = LFN;
		fsname.f.mno = getpid();
		fwrite( (char *)&fsname, sizeof(fsname), 1, stdout );
#ifdef FLEXNAMES
		fwrite( buf, strlen( buf ) + 1, 1, stdout );
#endif
	}
}

/* where  - print the location of an error
 *  if the filename is a C file (the source file) then just print the lineno
 *    the filename is taken care of in a title
 *  if the file is a header file ( unlikely but possible)
 *    then the filename is printed with the line number of the error
 *    where is called by cerror, uerror and werror
 *    (it is not called by luerror or lwerror)
 */
where( f ) char	f;
{
    extern		fprintf( );
    extern enum boolean	iscfile( );
    extern char		*strip( );
    char		*filename;

    if( f == 'u' && nerrors > 1 ) --nerrors; /* don't get "too many errors" */

    filename = strip( ftitle );
    if ( iscfile( filename ) == true )
		fprintf( stderr, "(%d)  ", lineno );
    else
		fprintf( stderr, "%s(%d): ", filename, lineno );
}
/* beg_file() and fpe_catch()
 * Used to do stuff to handle floating point exceptions
 * generated during floating point constant folding
 */
beg_file()
{
	extern int fpe_catch();

	/*
	 * Catch floating exceptions generated by the constant
	 * folding code.
	 */
	(void) signal( SIGFPE, fpe_catch );
}
fpe_catch()
{
	/*
	 * Floating exception generated by constant folding.
	 */
	/* "floating point constant folding causes exception" */
	UERROR( MESSAGE( 125 ) );
	lerror( "", CCLOSE | HCLOSE | FATAL );
	/* note that no error message is printed */
}
/* a number of dummy routines, unneeded by lint */

branch(n){;}
defalign(n){;}
deflab(n){;}
bycode(t,i){;}
cisreg(t) TWORD t; {return(1);}  /* everyting is a register variable! */

/* fldty - check field type (called from pcc) */
fldty(p) struct symtab *p;
{
	/* for a field to be portable, it must be unsigned int */
	/* "the only portable field type is unsigned int" */
	if( pflag && BTYPE(p->stype) != UNSIGNED ) UERROR( MESSAGE( 83 ) );
}

/* fldal - field alignment (called from pcc) 
 *		called for alignment of funny types (e.g. arrays, pointers) 
 */
fldal(t) unsigned t;
{
	if( t == ENUMTY )	/* this should be thought through better... */
		return( ALINT );/* jwf try this */
#ifdef gcos
	if( !ISPTR(t) )	
#endif
		/* "illegal field type" */
		UERROR( MESSAGE( 57 ) );
	return(ALINT);
}
/* main - driver for the first pass */
main( argc, argv ) int	argc; char	*argv[ ];
{
    char	 *p;
    int		i;
    extern 	tmpopen( );
    extern char *htmpname;
	extern char *optarg;
	extern int optind;
	char stdbuf[BUFSIZ];	/*stderr output buffer*/

	setbuf(stderr, stdbuf);
    /* handle options */

    /* 28 feb 80  reverse the sense of hflag and cflag */
    hflag = 1;
	/*
    cflag = 1;
		9/25/80	undo the (28 feb 80) change to cflag */
    /* 31 mar 80  reverse the sense of brkflag */
    brkflag = 1;

	while((i=getopt(argc,argv,"abchnpuvxH:LT:X:")) != EOF)
		switch(i) {
		case 'a':
			aflag = 0;
			break;

		case 'b':
			brkflag = 0;
			break;

		case 'c':
			fprintf(stderr,"lint: -c option ignored - no longer available\n");
			break;

		case 'h':
			hflag = 0;
			break;

		case 'p':
			pflag = 1;
			cflag = 1;	/* added 9/25/80 */
			break;

		case 'v':
			vflag = 0;
			break;

		case 'x':
			xflag = 0;
			break;

		case 'H':
			htmpname = optarg;
			break;

		case 'L':		/*undocumented; make input look like library*/
			libflag = 1;
			vflag = 0;
			break;

		case 'n':		/* done in shell script */
		case 'u':		/* second pass option */
		case 'T':		/* second pass option */
		case 'X':		/* second pass option */
			break;
		} /*end switch*/

	/* process file name */
	if( argv[optind] ) {
		p = strip( argv[optind] );
		strncpy( sourcename, p, LFNM );
	}
    tmpopen( );

	if( !pflag ){  /* set sizes to sizes of target machine */
# ifdef gcos
		SZCHAR = ALCHAR = 9;
# else
		SZCHAR = ALCHAR = 8;
# endif
		SZINT = ALINT = sizeof(int)*SZCHAR;
		SZFLOAT = ALFLOAT = sizeof(float)*SZCHAR;
		SZDOUBLE = ALDOUBLE = sizeof(double)*SZCHAR;
		SZLONG = ALLONG = sizeof(long)*SZCHAR;
		SZSHORT = ALSHORT = sizeof(short)*SZCHAR;
		SZPOINT = ALPOINT = sizeof(int *)*SZCHAR;
		ALSTRUCT = ALINT;
		/* now, fix some things up for various machines (I wish we had "alignof") */

# ifdef pdp11
		ALLONG = ALDOUBLE = ALFLOAT = ALINT;
#endif
# ifdef ibm
		ALSTRUCT = ALCHAR;
#endif
# ifdef vax
		ALDOUBLE=ALFLOAT=ALLONG=ALSHORT=ALPOINT=ALINT=ALSTRUCT=ALCHAR;
#endif
# ifdef mc68
		ALDOUBLE=ALFLOAT=ALLONG=ALPOINT=ALINT=ALSTRUCT=ALSHORT;
#endif
	}
	return( mainp1( argc, argv ) );
}
/* ctype - are there any funny types? */
ctype( type ) unsigned type;
{
	return( type );
}

/* commdec - common declaration */
commdec( i )
{
	/* 10/14/80 - the compiler complains, and so should lint */
/* this check is not quite right, e.g.,		void (*f)();
 *		-- tsize() performs a similar check, but will return
 *		before so if we've got a pointer.
 *	so, let tsize figure it out.
 *
 *	if( dimtab[ stab[i].sizoff ] == 0 )
 */
	register struct symtab *q = &stab[i];

	(void) tsize(q->stype, q->dimoff, q->sizoff);
	/* put out a common declaration */
	outdef( &stab[i], libflag?LIB:LDC, USUAL );
}

isitfloat ( s ) char *s;
{	/* s is a character string;
	   if floating point is implemented, set dcon to the value of s */
	/* lint version */
	dcon = atof( s );
	return( FCON );
}

/* fldcon - check assignment of a constant to a field */
fldcon( p ) register NODE *p;
{
	/* check to see if the assignment is going to overflow,
	  or otherwise cause trouble */
	register s;
	CONSZ v;

	if( !hflag & !pflag ) return;

	s = UPKFSZ(p->in.left->tn.rval);
	v = p->in.right->tn.lval;

	switch( p->in.left->in.type ){

	case CHAR:
	case INT:
	case SHORT:
	case LONG:
	case ENUMTY:
		if( v>=0 && (v>>(s-1))==0 ) return;
		/* "precision lost in assignment to (possibly sign-extended) field" */
		WERROR( MESSAGE( 93 ) );
	default:
		return;

	case UNSIGNED:
	case UCHAR:
	case USHORT:
	case ULONG:
		/* "precision lost in field assignment" */
		if( v<0 || (v>>s)!=0 ) WERROR( MESSAGE( 94 ) );
	}
}
/* outdef - output a definition for the second pass */
outdef( p, lty, mode ) struct symtab *p;
{
	/* if mode is > USUAL, it is the number of args */
	extern char	*strncat( );
	char *fname;
	TWORD t;
	int line;
	static union rec rc;

	if( mode == NOFILE ){
		/*  fname = "???"; */
		fname = strip( ftitle );
#ifdef FLEXNAMES
		fname = strncat( fname, "?", BUFSIZ );
#else
		fname = strncat( fname, "?", LFNM );
#endif
		line = p->suse;
	}
	else if( mode == SVLINE ){
		fname = ftitle;
		line = -p->suse;
		}
	else {
		fname = ftitle;
		line = lineno;
		}
	fsave( fname );
#ifndef FLEXNAMES
	if ( p->sclass != STATIC)
		(void) strncpy( rc.l.name, exname(p->sname), LCHNM );
	else
		/* no need to use exname on statics */
		(void) strncpy( rc.l.name, p->sname, LCHNM );
#endif
	rc.l.decflag = lty;
	t = p->stype;
	if( mode == DECTY ) t = DECREF(t);
	rc.l.type.aty = t;
	rc.l.type.extra = 0;
	astype( &rc.l.type, p->sizoff );
	rc.l.nargs = (mode>USUAL) ? mode : 0;
	rc.l.fline = line;
	fwrite( (char *)&rc, sizeof(rc), 1, stdout );
#ifdef FLEXNAMES
	rc.l.name = ( p->sclass == STATIC ) ? p->sname : exname( p->sname );
	fwrite( rc.l.name, strlen( rc.l.name ) + 1, 1, stdout );
#endif
}

extern int strcmp();
extern char *strchr();

int	opterr = 1;
int	optind = 1;
int	optopt;
char	*optarg;

int
getopt(argc, argv, opts)
int	argc;
char	**argv, *opts;
{
	static int sp = 1;
	register int c;
	register char *cp;

	if(sp == 1)
		if(optind >= argc ||
		   argv[optind][0] != '-' || argv[optind][1] == '\0')
			return(EOF);
		else if(strcmp(argv[optind], "--") == 0) {
			optind++;
			return(EOF);
		}
	optopt = c = argv[optind][sp];
	if(c == ':' || (cp=strchr(opts, c)) == 0) {
		fprintf (stderr, "%s: illegal option -- %c\n", argv[0], c);
		if(argv[optind][++sp] == '\0') {
			optind++;
			sp = 1;
		}
		return('?');
	}
	if(*++cp == ':') {
		if(argv[optind][sp+1] != '\0')
			optarg = &argv[optind++][sp+1];
		else if(++optind >= argc) {
			fprintf (stderr, "%s: option requires an argument -- %c\n", argv[0], c);
			sp = 1;
			return('?');
		} else
			optarg = argv[optind++];
		sp = 1;
	} else {
		if(argv[optind][++sp] == '\0') {
			sp = 1;
			optind++;
		}
		optarg = 0;
	}
	return(c);
}

/*
 * Return the ptr in sp at which the character c appears;
 * NULL if not found
 */

char *
strchr(sp, c)
register char *sp, c;
{
	do {
		if(*sp == c)
			return(sp);
	} while(*sp++);
	return(0);
}
