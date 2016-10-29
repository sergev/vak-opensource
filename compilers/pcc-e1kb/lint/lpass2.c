/*	@(#)lpass2.c	1.8	*/
/*	3.0 SID #	1.2	*/
/* lpass2.c
 *	This file includes the main functions for the second lint pass.
 *
 *	Functions:
 *	==========
 *		chkcompat	check for various sorts of compatibility
 *		chktype		check two types to see if they are compatible
 *		cleanup		do wrapup checking
 *		find		find name in symbol table
 *		lastone		check set/use in a last pass over the symbol table
 *		lread		read in a line from the intermediate file
 *		main		the driver for the second pass (lint2)
 *		mloop		the main loop of the second pass
 *		setfno		set file number (keep track of source file names)
 *		setuse		fix set/use and other information for objects
 *		tget
 */

# define OFFSZ long

# include "manifest"
# include "lerror.h"
# include "lmanifest"
# include "lpass2.h"

# define USED 01
# define VUSED 02
# define EUSED 04
# define RVAL 010
# define VARARGS 0100

# define NSZ 1024
# if pdp11
#	define TYSZ 2500
#	define FSZ 80
# else
#	define TYSZ 3500	/* should be 2500 temp fix for mjs */
#	define FSZ 150		/* used to be 80 */
# endif

STAB stab[NSZ];			/* second pass symbol table */
STAB *find();

STYPE tary[TYSZ];
STYPE *tget();

#ifdef FLEXNAMES
	char *fnm[FSZ];		/* stack of file names */
#else
	char fnm[FSZ][LFNM];	/* stack of file names */
#endif

#ifdef FLEXNAMES
	char *getstr();
#endif

int tfree;  /* used to allocate types */
int ffree;  /* used to save filenames */

struct ty atyp[50];
union rec r;			/* where all the input ends up */

int hflag = 1;			/* 28 feb 80  reverse sense of hflag */
int pflag = 0;
int xflag = 1;			/* 28 feb 80  reverse sense of xflag */
int uflag = 1;

static int idebug = 0;
static int odebug = 0;

int cfno;				/* current file number */
int cmno;				/* current module number */
/* main program for second pass */

main( argc, argv ) char *argv[]; 
{
	int c;
	char		*ifilename = NULL;
	extern char	*htmpname;
	extern char *optarg;
	int Hflag = 0, Tflag = 0;

	while((c=getopt(argc,argv,"abchnpuvxH:LT:X:")) != EOF)
		switch(c) {
		case 'h':
			hflag = 0;
			break;

		case 'p':
			pflag = 1;
			break;

		case 'x':
			xflag = 0;
			break;

		case 'u':
			uflag = 0;
			break;

		case 'H':
			htmpname = optarg;
			Hflag = 1;
			break;

		case 'T':
			ifilename = optarg;
			Tflag = 1;
			break;

			/* debugging options */
		case 'X':
			for(; *optarg; optarg++)
				switch(*optarg) {
				case 'i':
					idebug = 1;
					break;
				case 'o':
					odebug = 1;
					break;
				} /*end switch(*optarg), for*/
			break;

		case 'a':		/* first pass option */
		case 'b':		/* first pass option */
		case 'c':		/* first pass option */
		case 'n':		/* done in shell script */
		case 'v':		/* first pass option */
		case 'L':		/* first pass option */
			break;
		} /*end switch for processing args*/

	tmpopen( );
	if ( Hflag )
		unbuffer( );
	if ( !Tflag )
	{
		lerror( "", CCLOSE | HCLOSE );	/* all done */
		return (0);
	}
	if ( !freopen( ifilename, "r", stdin ) )
		lerror( "cannot open intermediate file", FATAL | CCLOSE | ERRMSG );

	if ( idebug ) pif();
	mloop( LDI|LDS|LIB );
	rewind( stdin );
	mloop( LDC|LDX );
	rewind( stdin );
	mloop( LRV|LUV|LUE|LUM );
	if ( odebug ) pst();
	cleanup();
	un2buffer( );
	return(0);
} /*end main*/
/* mloop - main loop
 *	each pass of the main loop reads in names from the intermediate 
 *	file that have characteristics which overlap with the
 *	characteristics specified as the parameter
 */
mloop( m )
{
  STAB *q;
	while( lread(m) ){
		q = find();
		if( q->decflag ) chkcompat(q);
		else setuse(q);
	}
}
/* lread - read a line from intermediate file into r.l */
lread(m)
{
	register n;

	for(;;) {
		/* read in line from intermediate file; exit if at end */
		if( fread( (char *)&r, sizeof(r), 1, stdin ) <= 0 ) return(0);
		if( r.l.decflag & LFN ){
			/* new filename and module number */
#ifdef FLEXNAMES
			r.f.fn = getstr();
#endif
			setfno( r.f.fn );
			/* the purpose of the module number is to handle static scoping
			 *	correctly.  A module is a file with all its include files.
			 *	From a scoping point of view, there is no difference between
			 *	a variable in a file and a variable in an included file.
			 *	The module number itself is not meaningful; it must only
			 *	be unique to ensure that modules are distinguishable
			 */
			cmno = r.f.mno;
			continue;
		}
		/* number of arguments is negative for VARARGS (plus one) */
#ifdef FLEXNAMES
		r.l.name = getstr();
#endif
		n = r.l.nargs;
		if( n<0 ) n = -n - 1;
		/* collect type info for all args */
		if( n )
			fread( (char *)atyp, sizeof(ATYPE), (unsigned) n, stdin );
		/* return with entry only if it has correct characteristics */
		if( ( r.l.decflag & m ) ) return( 1 );
	}
}
/* setfno - set file number */
setfno( s ) char *s;
{
  int i;
	/* look up filename */
	for( i=0; i<ffree; ++i )
#ifdef FLEXNAMES
		if( fnm[i] == s ){
#else
		if( !strncmp( s, fnm[i], LFNM ) ){
#endif
			cfno = i;
			return;
		}
	/* make a new entry */
#ifdef FLEXNAMES
	fnm[ffree] = s;
#else
	strncpy( fnm[ffree], s, LFNM );
#endif
	if ( ( cfno = ffree++ ) >= FSZ )
		lerror( "Too many names", CCLOSE | FATAL | ERRMSG );
}
/* find - find object in the symbol table */
STAB *
find()
{
  register h=0;
	/* hashing scheme */
	/* for this to work, NSZ should be a power of 2 */
#ifdef FLEXNAMES
	h = ( (int) r.l.name ) % NSZ;
#else
	{  register char *p, *q;
		for( h=0,p=r.l.name,q=p+LCHNM; *p&&p<q; ++p) {
			h = (h<<1)+ *p;
			if( h>=NSZ ) h = (h+1)&(NSZ-1);
		}
	}
#endif
	/* symbol table search */
	/* 21 July 1980 - scoping added to symbol table search */
	{  register STAB *p, *q, *maybe = 0;
		for( p=q= &stab[h]; q->decflag; ){
			/* find a name that matches */
#ifdef FLEXNAMES
			if ( r.l.name == q->name )  {
#else
			if( !strncmp( r.l.name, q->name, LCHNM))  {
#endif
				/* match if module id-s are the same */
				if (q->mno == cmno) return(q);
				/* otherwise, possible match if neither is static */
				if( !( q->decflag&LDS || r.l.decflag&LDS ) ) maybe = q;
				/* else there must be another stab entry with same name
				 * (if there is NOT, then we will add an entry) */
				else if (!maybe) q->more = 1;
				/* at end of chain - so use global name */
				if( !(q->more) && maybe ) return(maybe);
			}
			/* no match, so continue looking */
			if( ++q >= &stab[NSZ] ) q = stab;
			if( q == p )
				lerror( "too many names defined", CCLOSE | FATAL | ERRMSG );
		}
#ifdef FLEXNAMES
		q->name = r.l.name;
#else
		strncpy( q->name, r.l.name, LCHNM );
#endif
		/* if we are inserting, the new entry is the last in the "chain" */
		q->more = 0;
		return( q );
	}
}
/* tget */
STYPE *
tget()
{
	if( tfree >= TYSZ )
		lerror( "too many types needed", CCLOSE | FATAL | ERRMSG );
	return( &tary[tfree++] );
}
/* chkcompat - check for compatibility
 *	compare item in symbol table (stab) with item just read in
 *	The work of this routine is not well defined; there are many
 *	checks that might be added or changes
 */

chkcompat(q) STAB *q; 
{
	register int i;
	STYPE *qq;

	setuse(q);

	/* do an argument check only if item is a function; if it is
	*  both a function and a variable, it will get caught later on
	*/

	if( ISFTN(r.l.type.aty) && ISFTN(q->symty.t.aty) )

		/* argument check */

		if( q->decflag & (LDI|LDS|LIB|LUV|LUE) )
			if( r.l.decflag & (LUV|LIB|LUE) ){
				if( q->nargs != r.l.nargs ){
					if( !(q->use&VARARGS) )
						/* "%.8s: variable # of args." */
						/* "%s: variable # of args." */
						buffer( 7, q );
					if( r.l.nargs > q->nargs ) r.l.nargs = q->nargs;
					if( !(q->decflag & (LDI|LDS|LIB) ) ) {
						q->nargs = r.l.nargs;
						q->use |= VARARGS;
					}
				}
				for( i=0,qq=q->symty.next; i<r.l.nargs; ++i,qq=qq->next)
					if( chktype( &qq->t, &atyp[i] ) )
						/* "%.8s, arg. %d used inconsistently" */
						/* "%s, arg. %d used inconsistently" */
						buffer( 6, q, i+1 );
			}

	if( (q->decflag&(LDI|LDS|LIB|LUV)) && r.l.decflag==LUV )
		if( chktype( &r.l.type, &q->symty.t ) )
			/* "%.8s value used inconsistently" */
			/* "%s value used inconsistently" */
			buffer( 4, q );

	/* check for multiple declaration */

	if( (q->decflag & (LDI|LDS) ) && (r.l.decflag&(LDI|LDS|LIB)) )
		/* "%.8s multiply declared" */
		/* "%s multiply declared" */
		buffer( 3, q );

	/* do a bit of checking of definitions and uses... */

	if( (q->decflag & (LDI|LDS|LIB|LDX|LDC|LUM))
	    && (r.l.decflag & (LDX|LDC|LUM))
	    && q->symty.t.aty != r.l.type.aty )
		/* "%.8s value type declared inconsistently" */
		/* "%s value type declared inconsistently" */
		buffer( 5, q );

	/* better not call functions which are declared to be
		structure or union returning */

	if( (q->decflag & (LDI|LDS|LIB|LDX|LDC))
	    && (r.l.decflag & LUE)
	    && q->symty.t.aty != r.l.type.aty ){
		/* only matters if the function returns union or structure */
		TWORD ty;
		ty = q->symty.t.aty;
		if( ISFTN(ty) && ((ty = DECREF(ty))==STRTY || ty==UNIONTY ) )
			/* "%.8s function value type must be declared before use" */
			/* "%s function value type must be declared before use" */
			buffer( 8, q );
	}
} /*end chkcompat*/

/* messages for defintion/use */
int	mess[2][2] = {
	NUM2MSGS,
	0,
	1,
	2
};

lastone(q) STAB *q; 
{
	register nu, nd, uses;
	nu = nd = 0;
	uses = q->use;

	if( !(uses&USED) && q->decflag != LIB )
#ifdef FLEXNAMES
		if( strcmp(q->name,"main") ) nu = 1;
#else
		if( strncmp(q->name,"main",7) ) nu = 1;
#endif

	/* treat functions and variables the same */
	switch( q->decflag ){

	case LIB:
		nu = nd = 0;  /* don't complain about uses on libraries */
		break;
	case LDX:
		if( !xflag ) break;
	case LUV:
	case LUE:
/* 01/04/80 */	case LUV | LUE:
/* or'ed together for void */
	case LUM:
		nd = 1;
	}
	if( uflag && ( nu || nd ) ) buffer( mess[nu][nd], q );

	if( (uses&(RVAL+EUSED)) == (RVAL+EUSED) ){
		if ( uses & VUSED ) 
			/* "%.8s returns value which is sometimes ignored\n" */
			/* "%s returns value which is sometimes ignored\n" */
			buffer( 11, q );
		else 
			/* "%.8s returns value which is always ignored\n" */
			/* "%s returns value which is always ignored\n" */
			buffer( 10, q );
	}

	if( (uses&(RVAL+VUSED)) == (VUSED) && (q->decflag&(LDI|LDS|LIB)) )
		/* "%.8s value is used, but none returned\n" */
		/* "%s value is used, but none returned\n" */
		buffer( 9, q );
}
/* cleanup - call lastone and die gracefully */
cleanup()
{
	STAB *q;
	for( q=stab; q < &stab[NSZ]; ++q )
		if( q->decflag ) lastone(q);
}
/* setuse - check new type to ensure that it is used */
setuse(q) STAB *q;
{
	if( !q->decflag ){ /* new one */
		q->decflag = r.l.decflag;
		q->symty.t = r.l.type;
		if( r.l.nargs < 0 ){
			q->nargs = -r.l.nargs - 1;
			q->use = VARARGS;
		}
		else {
			q->nargs = r.l.nargs;
			q->use = 0;
		}
		q->fline = r.l.fline;
		q->fno = cfno;
		q->mno = cmno;
		if( q->nargs ){
			int i;
			STYPE *qq;
			for( i=0,qq= &q->symty; i<q->nargs; ++i,qq=qq->next ){
				qq->next = tget();
				qq->next->t = atyp[i];
			}
		}
	}

	switch( r.l.decflag ){

	case LRV:
		q->use |= RVAL;
		return;
	case LUV:
		q->use |= VUSED+USED;
		return;
	case LUE:
		q->use |= EUSED+USED;
		return;
	/* 01/04/80 */	case LUV | LUE:
	case LUM:
		q->use |= USED;
		return;

	}
	/* 04/06/80 */
	if ( (q->decflag & LDX) && (r.l.decflag & LDC) ) q->decflag = LDC;
}
/* chktype - check the two type words to see if they are compatible */

chktype( pt1, pt2 ) register ATYPE *pt1, *pt2; 
{
	TWORD t;

	/* currently, enums are turned into ints and should be checked as such */
	if( pt1->aty == ENUMTY ) pt1->aty =  INT;
	if( pt2->aty == ENUMTY ) pt2->aty = INT;

	if( (t=BTYPE(pt1->aty)==STRTY) || t==UNIONTY )
		return( pt1->aty!=pt2->aty || pt1->extra!=pt2->extra );

	if( pt2->extra ){ /* constant passed in */
		if( sizeof(int) == sizeof(long) && !pflag ) return( 0 );
		if( pt1->aty == UNSIGNED && pt2->aty == INT ) return( 0 );
		else if( pt1->aty == ULONG && pt2->aty == LONG ) return( 0 );
	}

	else if( pt1->extra ){ /* for symmetry */
		if( sizeof(int) == sizeof(long) && !pflag ) return( 0 );
		if( pt2->aty == UNSIGNED && pt1->aty == INT ) return( 0 );
		else if( pt2->aty == ULONG && pt1->aty == LONG ) return( 0 );
	}

	return( pt1->aty != pt2->aty );
}

# ifdef DEBUG
/* diagnostic output tools
 *	the following programs are used to print out internal information
 *	for diagnostic purposes
 *	Current options:
 *	-Xi
 *		turns on printing of intermediate file on entry to lint2
 *			(input information)
 *	-Xo
 *		turns on printing of lint symbol table on last pass of lint2
 *			(output information)
 */

struct tb { 
	int m; 
	char * nm };
static struct tb dfs[] = {
	LDI, "LDI",
	LIB, "LIB",
	LDC, "LDC",
	LDX, "LDX",
	LRV, "LRV",
	LUV, "LUV",
	LUE, "LUE",
	LUM, "LUM",
	LDS, "LDS",
	0, "" };

static struct tb us[] = {
	USED, "USED",
	VUSED, "VUSED",
	EUSED, "EUSED",
	RVAL, "RVAL",
	VARARGS, "VARARGS",
	0,	0,
};


/* ptb - print a value from the table */
ptb( v, tp ) struct tb *tp; 
{
	int flag = 0;
	for( ; tp->m; ++tp )
		if( v&tp->m ){
			if( flag++ ) putchar( '|' );
			printf( "%s", tp->nm );
		}
}


/* pif - print intermediate file
 *	prints file written out by first pass of lint
 *  printing turned on by the debug option -Xi
 */
pif()
{
	printf("\n\tintermediate file printout:\n");
	printf("\t===========================\n");
	while( 0 < fread( (char *)&r, sizeof(r), 1, stdin) ) {
		if ( r.l.decflag & LFN )
		{
#ifdef FLEXNAMES
			r.f.fn = getstr();
			printf( "\nFILE NAME: %-15s\n", r.f.fn );
#else
			printf("\nFILE NAME: %-15.15s\n", r.f.fn);
#endif
		}
		else {
#ifdef FLEXNAMES
			r.l.name = getstr();
			printf( "\t%-8s  (", r.l.name );
#else
			printf("\t%-8.8s  (", r.l.name);
#endif
			ptb(r.l.decflag, dfs);
			printf(")\t line= %d", r.l.fline);
			if ( ISFTN(r.l.type.aty) ) printf("\tnargs= %d", r.l.nargs);
			else printf("\t\t\t");
			printf("\t type= ");
			tprint(r.l.type.aty);
			printf("\n");
			if ( r.l.nargs ) {
				int n = 0;
				if ( r.l.nargs < 0 ) r.l.nargs= -r.l.nargs - 1;
				fread( (char *)atyp, sizeof(ATYPE), r.l.nargs, stdin);
				while ( ++n <= r.l.nargs ) {
					printf("\t\t arg(%d)= ",n);
					tprint(atyp[n-1].aty); 
					printf("\n");
				}
			}
		}
	} /*while*/

	rewind( stdin );
} /*end pif*/


/* pst - print lint symbol table
 *	prints symbol table created from intermediate file
 *  printing turned on by the debug option -Xo
 */
pst()
{
  int count = 0;
  STAB *q;
	printf("\n\tsymbol table printout:\n");
	printf("\t======================\n");
	for( q=stab; q < &stab[NSZ]; ++q) {
		if( q->decflag ) {
			count++;
#ifdef FLEXNAMES
			printf( "\t%8s  (", q->name );
#else
			printf("\t%-8.8s  (", q->name);
#endif
			ptb(q->decflag, dfs);
			printf(")\t line= %d", q->fline);
			if ( ISFTN(q->symty.t.aty) ) printf("\tnargs= %d", q->nargs);
			else printf("\t\t\t");
			printf("\t type= ");
			tprint(q->symty.t.aty);
			printf("\t use= ");
			ptb(q->use, us);
			printf("\n");
			if ( q->nargs ) {
				int n = 1;
				STYPE *qq;
				for( qq=q->symty.next; n <= q->nargs; qq=qq->next) {
					printf("\t\t arg(%d)= ",n++);
					tprint(qq->t.aty); 
					printf("\n");
				}
			}
		} /*end if(q->decflag)*/
	} /*end for*/
	printf("\n%d symbol table entries\n",count);
}


/* tprint - print a nice description of a type
 *	borrowed from PCC
 */
tprint( t )  TWORD t; 
{
	static char * tnames[] = {
		"undef",
		"farg",
		"char",
		"short",
		"int",
		"long",
		"float",
		"double",
		"strty",
		"unionty",
		"enumty",
		"moety",
		"uchar",
		"ushort",
		"unsigned",
		"ulong",
		"?", "?"
	};

	for(;; t = DECREF(t) ){

		if( ISPTR(t) ) printf( "PTR " );
		else if( ISFTN(t) ) printf( "FTN " );
		else if( ISARY(t) ) printf( "ARY " );
		else {
			printf( "%s", tnames[t] );
			return;
		}
	}
}
# else
pif()
{ printf("intermediate file dump not available\n"); }
pst()
{ printf("symbol table dump not available\n"); }
# endif

#ifdef FLEXNAMES

char *
getstr()	/* read arb. len. name string from input & return ptr to it */
{
	char buf[BUFSIZ];
	register char *cp = buf;
	register int c;

	while ( ( c = getchar() ) != EOF )
	{
		*cp++ = c;
		if ( c == '\0' )
			break;
	}
	if ( c != '\0' )
	{
		lerror( "Name string format error in intermediate file",
			CCLOSE | FATAL | ERRMSG );
	}
	return ( hash( buf ) );
}

#define NSAVETAB	4096
char	*savetab;
int	saveleft;

char *
savestr( cp )		/* copy string into permanent string storage */
	register char *cp;
{
	register int len;

	len = strlen( cp ) + 1;
	if ( len > saveleft )
	{
		saveleft = NSAVETAB;
		if ( len > saveleft )
			saveleft = len;
		savetab = (char *) malloc( (unsigned) saveleft );
		if ( savetab == 0 )
			lerror( "Ran out of memory [savestr()]",
				CCLOSE | FATAL | ERRMSG );
	}
	(void) strncpy( savetab, cp, len );
	cp = savetab;
	savetab += len;
	saveleft -= len;
	return ( cp );
}

/*
* The segmented hash tables
*/
#define MAXHASH	20
#define HASHINC	1013

struct ht
{
	char	**ht_low;
	char	**ht_high;
	int	ht_used;
};

struct ht htab[MAXHASH];	/* non-filenames */
struct ht fhtab[MAXHASH];	/* filenames */

char *
hash( s )	/* hash and store name in permanent storage area */
	char *s;
{
	register char **h;
	register int i;
	register char *cp;
	struct ht *htp;
	int sh;

	/*
	* Hash on all the characters.
	*/
	cp = s;
	i = 0;
	while ( *cp )
		i = ( i << 1 ) + *cp++;
	sh = ( i & 077777 ) % HASHINC;
	cp = s;
	/*
	* Look through each table for name.  If not found in the current
	* table, skip to the next one.
	*/
	for ( htp = htab; htp < &htab[MAXHASH]; htp++ )
	{
		if ( htp->ht_low == 0 )
		{
			register char **hp = (char **) calloc(
				sizeof (char **), HASHINC );

			if ( hp == 0 )
				lerror( "Ran out of memory [fhash()]",
					CCLOSE | FATAL | ERRMSG );
			htp->ht_low = hp;
			htp->ht_high = hp + HASHINC;
		}
		h = htp->ht_low + sh;
		/*
		* Use quadratic re-hash
		*/
		i = 1;
		do
		{
			if ( *h == 0 )
			{
				if ( htp->ht_used > ( HASHINC * 3 ) / 4 )
					break;
				htp->ht_used++;
				*h = savestr( cp );
				return ( *h );
			}
			if ( **h == *cp && strcmp( *h, cp ) == 0 )
				return ( *h );
			h += i;
			i += 2;
			if ( h >= htp->ht_high )
				h -= HASHINC;
		} while ( i < HASHINC );
	}
	lerror( "Ran out of hash tables", CCLOSE | FATAL | ERRMSG );
}
#endif

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
