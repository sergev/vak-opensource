/*	@(#)lint.c	1.6	83/07/03	*/
/* Oct 17 79 by NG: SMOS -> SMOS|STAG */
# include "mfile1"
#ifndef FLEXNAMES
#	define LCHNM	8
#	define LFNM	15
#endif
# include "lmanifest"

# include <ctype.h>

# define VAL 0
# define EFF 1

extern int ddebug, idebug, bdebug, tdebug, edebug, xdebug;

int blocknos[20];	/* CXREF */
int blockptr = 0;	/* CXREF */
int nextblock = 1;	/* CXREF */
/* these are appropriate for the -p flag */
int  SZCHAR = 8;
int  SZINT = 16;
int  SZFLOAT = 32;
int  SZDOUBLE = 64;
int  SZLONG = 32;
int  SZSHORT = 16;
int SZPOINT = 16;
int ALCHAR = 8;
int ALINT = 16;
int ALFLOAT = 32;
int ALDOUBLE = 64;
int ALLONG = 32;
int ALSHORT = 16;
int ALPOINT = 16;
int ALSTRUCT = 16;

int vflag = 1;  /* tell about unused argments */
int xflag = 0;  /* tell about unused externals */
int argflag = 0;  /* used to turn off complaints about arguments */
int libflag = 0;  /* used to generate library descriptions */
int vaflag = -1;  /* used to signal functions with a variable number of args */
int aflag = 0;  /* used th check precision of assignments */

char *flabel = "xxx";
int filecount = 0;
int wloop_level = LL_TOP;
int floop_level = LL_TOP;

# define LNAMES 100

struct lnm {
	short lid, flgs;
	}  lnames[LNAMES], *lnp;

beg_file()
{
}

contx( p, down, pl, pr ) register NODE *p; register *pl, *pr; {

	*pl = *pr = VAL;
	switch( p->in.op ){

	case ANDAND:
	case OROR:
	case QUEST:
		*pr = down;
		break;

	case SCONV:
	case PCONV:
	case COLON:
		*pr = *pl = down;
		break;

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
		if( asgop(p->in.op) ) break;
		if( p->in.op == UNARY MUL && ( p->in.type == STRTY || p->in.type == UNIONTY) ) {
			break;  /* the compiler does this... */
			}
		if( down == EFF && hflag ) werror( "null effect" );

		}
	}

ecode( p ) NODE *p; {
	/* compile code for p */
	}

zecode( n ){
	/* n integer words of zeros */
	OFFSZ temp;
	temp = n;
	inoff += temp*SZINT;
	}

andable( p ) NODE *p; {  /* p is a NAME node; can it accept & ? */
	register r;

	if( p->in.op != NAME ) cerror( "andable error" );

	if( (r = p->tn.rval) < 0 ) return(1);  /* labels are andable */

	if( stab[r].sclass == AUTO || stab[r].sclass == PARAM ) return(0); 
	return(1);
	}

NODE *
clocal(p) NODE *p; {

	/* this is called to do local transformations on
	   an expression tree preparitory to its being
	   written out in intermediate code.
	*/

	/* the major essential job is rewriting the
	   automatic variables and arguments in terms of
	   REG and OREG nodes */
	/* conversion ops which are not necessary are also clobbered here */
	/* in addition, any special features (such as rewriting
	   exclusive or) are easily handled here as well */

	register o;
	register unsigned t, tl;

	switch( o = p->in.op ){

	case SCONV:
	case PCONV:
		if( p->in.left->in.type==ENUMTY ){
			p->in.left = pconvert( p->in.left );
			}
		/* assume conversion takes place; type is inherited */
		t = p->in.type;
		tl = p->in.left->in.type;
		if( aflag && (tl==LONG||tl==ULONG) && (t!=LONG&&t!=ULONG) ){
			werror( "long assignment may lose accuracy" );
			}
		if( aflag>=2 && (tl!=LONG&&tl!=ULONG) && (t==LONG||t==ULONG) && p->in.left->in.op != ICON ){
			werror( "assignment to long may sign-extend incorrectly" );
			}
		if( ISPTR(tl) && ISPTR(t) ){
			tl = DECREF(tl);
			t = DECREF(t);
			switch( ISFTN(t) + ISFTN(tl) ){

			case 0:  /* neither is a function pointer */
				if( talign(t,p->fn.csiz) > talign(tl,p->in.left->fn.csiz) ){
					if( hflag||pflag ) werror( "possible pointer alignment problem" );
					}
				break;

			case 1:
				werror( "questionable conversion of function pointer" );

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

NODE *
offcon( off, t, d, s ) OFFSZ off; TWORD t;{  /* make a structure offset node */
	register NODE *p;
	p = bcon(0);
	p->tn.lval = off/SZCHAR;
	return(p);
	}

noinit(){
	/* storage class for such as "int a;" */
	return( pflag ? EXTDEF : EXTERN );
	}


cinit( p, sz ) NODE *p; { /* initialize p into size sz */
	inoff += sz;
	if( p->in.op == INIT ){
		if( p->in.left->in.op == ICON ) return;
		if( p->in.left->in.op == NAME && p->in.left->in.type == MOE ) return;
		}
	uerror( "illegal initialization" );
	}

where(f){ /* print true location of error */
	if( f == 'u' && nerrors>1 ) --nerrors; /* don't get "too many errors" */
	fprintf( stderr, "%s, line %d: ", ftitle, lineno );
	}

	/* a number of dummy routines, unneeded by lint */

branch(n){;}
defalign(n){;}
deflab(n){;}
bycode(t,i){;}
cisreg(t) TWORD t; {return(1);}  /* everyting is a register variable! */

fldty(p) struct symtab *p; {
	; /* all types are OK here... */
	}

fldal(t) unsigned t; { /* field alignment... */
	if( t == ENUMTY ) return( ALCHAR );  /* this should be thought through better... */
	if( ISPTR(t) ){ /* really for the benefit of honeywell (and someday IBM) */
		if( pflag ) uerror( "nonportable field type" );
		}
	else uerror( "illegal field type" );
	return(ALINT);
	}

main( argc, argv ) char *argv[]; {
	/* definitions for CXREF */
	char *p, *cp;
	int n = 1;

	/* handle options */

	for( p=argv[1]; n < argc && *p == '-'; p = argv[++n]){

		switch( *++p ){

		/* the next bunch of cases are for CXREF */
		case 'd':
			++ddebug;
			continue;
		case 'I':
			++idebug;
			continue;
		case 'b':
			++bdebug;
			continue;
		case 't':
			++tdebug;
			continue;
		case 'e':
			++edebug;
			continue;
		case 'x':
			++xdebug;
			continue;
		case 'f':	/* filename sent to cpp */
			p = argv[++n];
			infile[0] = '"';	/* put quotes around name */
			cp = &infile[1];
			while (*cp++ = *p++) ;	/* copy filename */
			*--cp = '"';
			*++cp = '\0';
			continue;

		case 'i':	/* actual input filename */
			p = argv[++n];
			if (freopen(p,"r",stdin) == NULL) {
				fprintf(stderr, "Can't open %s\n",p);
				exit(1);
			}
			continue;

		case 'o':
			p = argv[++n];
			if ((outfp = fopen(p,"a")) == NULL) {
				fprintf(stderr, "Can't Open %s\n",p);
				exit(2);
			}
			continue;

		default:
			uerror( "illegal option: %c", *p );
			continue;

			}
		}

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
		}

	n = mainp1(argc, argv);
	if (feof(stdout) || ferror(stdout))
		perror("cxref.xpass");
	return( mainp1( argc, argv ) );
	}

ctype( type ) unsigned type; { /* are there any funny types? */
	return( type );
	}


isitfloat ( s ) char *s; {
	/* s is a character string;
	   if floating point is implemented, set dcon to the value of s */
	/* lint version
	*/
	dcon = atof( s );
	return( FCON );
	}

bbcode()	/* CXREF */
{
	/* code for beginning a new block */
	blocknos[blockptr] = nextblock++;
	fprintf(outfp, "B%d\t%05d\n", blocknos[blockptr], lineno);
	blockptr++;
}

becode()	/* CXREF */
{
	/* code for ending a block */
	if (--blockptr < 0)
		uerror("bad nesting");
	else
		fprintf( outfp, "E%d\t%05d\n", blocknos[blockptr], lineno);
}
