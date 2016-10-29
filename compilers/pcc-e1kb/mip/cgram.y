/*	@(#)cgram.y	1.7	*/

/******/
/*	Note: In this file there are comments of the form
/*		/*CXREF <line-of-code> */
/*	When this file is used to build cxref, the beginning
/*	of the comment and the CXREF are removed along with
/*	the trailing end of the comment.  Therefore, these
/*	comments cannot be removed with out changing cxref's
/*	version of this file.
/******/

%term NAME  2
%term STRING  3
%term ICON  4
%term FCON  5
%term PLUS   6
%term MINUS   8
%term MUL   11
%term AND   14
%term OR   17
%term ER   19
%term QUEST  21
%term COLON  22
%term ANDAND  23
%term OROR  24

/*	special interfaces for yacc alone */
/*	These serve as abbreviations of 2 or more ops:
	ASOP	=, = ops
	RELOP	LE,LT,GE,GT
	EQUOP	EQ,NE
	DIVOP	DIV,MOD
	SHIFTOP	LS,RS
	ICOP	ICR,DECR
	UNOP	NOT,COMPL
	STROP	DOT,STREF

	*/
%term ASOP  25
%term RELOP  26
%term EQUOP  27
%term DIVOP  28
%term SHIFTOP  29
%term INCOP  30
%term UNOP  31
%term STROP  32

/*	reserved words, etc */
%term TYPE  33
%term CLASS  34
%term STRUCT  35
%term RETURN  36
%term GOTO  37
%term IF  38
%term ELSE  39
%term SWITCH  40
%term BREAK  41
%term CONTINUE  42
%term WHILE  43
%term DO  44
%term FOR  45
%term DEFAULT  46
%term CASE  47
%term SIZEOF  48
%term ENUM 49


/*	little symbols, etc. */
/*	namely,

	LP	(
	RP	)

	LC	{
	RC	}

	LB	[
	RB	]

	CM	,
	SM	;

	*/

%term LP  50
%term RP  51
%term LC  52
%term RC  53
%term LB  54
%term RB  55
%term CM  56
%term SM  57
%term ASSIGN  58
%term ASM 59

/* at last count, there were 7 shift/reduce, 1 reduce/reduce conflicts
/* these involved:
	if/else
	recognizing functions in various contexts, including declarations
	error recovery
	*/

%left CM
%right ASOP ASSIGN
%right QUEST COLON
%left OROR
%left ANDAND
%left OR
%left ER
%left AND
%left EQUOP
%left RELOP
%left SHIFTOP
%left PLUS MINUS
%left MUL DIVOP
%right UNOP
%right INCOP SIZEOF
%left LB LP STROP
%{
# include "mfile1"
# include "messages.h"
%}

	/* define types */
%start ext_def_list

%type <intval> con_e ifelprefix ifprefix doprefix switchpart
		enum_head str_head name_lp
%type <nodep> e .e term attributes oattributes type enum_dcl struct_dcl
		cast_type null_decl funct_idn declarator fdeclarator nfdeclarator
		elist

%token <intval> CLASS NAME STRUCT RELOP CM DIVOP PLUS MINUS SHIFTOP MUL AND OR ER ANDAND OROR
		ASSIGN STROP INCOP UNOP ICON
%token <nodep> TYPE

%%

%{
	extern int wloop_level;	/* specifies while loop code generation */
	extern int floop_level;	/* specifies for loop code generation */
	static int fake = 0;
#ifdef FLEXNAMES
	static char fakename[24];	/* room enough for pattern */
#else
	static char fakename[NCHNAM+1];
#endif
%}

ext_def_list:	   ext_def_list external_def
		|
		{
			beg_file();	/* do implementation dep. stuff now */
			ftnend();
		}
		;
external_def:	   data_def
			={ curclass = SNULL;  blevel = 0; }
		|  error
			={ curclass = SNULL;  blevel = 0; }
		|  ASM SM
			{ asmout();  curclass = SNULL; blevel = 0; }
		;
data_def:
		   oattributes  SM
			={  $1->in.op = FREE; }
		|  oattributes init_dcl_list  SM
			={  $1->in.op = FREE; }
		|  oattributes fdeclarator {
				defid( tymerge($1,$2), curclass==STATIC?STATIC:EXTDEF );
				/*CXREF bbcode(); */
				}  function_body
			={  
			    if( blevel ) cerror( "function level error" );
			    if( reached ) retstat |= NRETVAL; 
			    $1->in.op = FREE;
			    ftnend();
			    }
		;

function_body:	   arg_dcl_list compoundstmt
		;
arg_dcl_list:	   arg_dcl_list declaration
		| 	={  blevel = 1; }
		;

stmt_list:	   stmt_list statement
		|  /* empty */
			={  bccode();
			    locctr(PROG);
			    }
		;

dcl_stat_list	:  dcl_stat_list attributes SM
			={  $2->in.op = FREE; }
		|  dcl_stat_list attributes init_dcl_list SM
			={  $2->in.op = FREE; }
		|  /* empty */
		;
declaration:	   attributes declarator_list  SM
			={ curclass = SNULL;  $1->in.op = FREE; }
		|  attributes SM
			={ curclass = SNULL;  $1->in.op = FREE; }
		|  error  SM
			={  curclass = SNULL; }
		;
oattributes:	  attributes
		|  /* VOID */
			={  $$ = mkty(INT,0,INT);  curclass = SNULL; }
		;
attributes:	   class type
			={  $$ = $2; }
		|  type class
		|  class
			={  $$ = mkty(INT,0,INT); }
		|  type
			={ curclass = SNULL ; }
		|  type class type
			={  $1->in.type =
				types( $1->in.type, $3->in.type, UNDEF );
			    $3->in.op = FREE;
			    }
		;


class:		  CLASS
			={  curclass = $1; }
		;

type:		   TYPE
		|  TYPE TYPE
			={  $1->in.type = types( $1->in.type, $2->in.type, UNDEF );
			    $2->in.op = FREE;
			    }
		|  TYPE TYPE TYPE
			={  $1->in.type = types( $1->in.type, $2->in.type, $3->in.type );
			    $2->in.op = $3->in.op = FREE;
			    }
		|  struct_dcl
		|  enum_dcl
		;

enum_dcl:	   enum_head LC moe_list optcomma RC
			={ $$ = dclstruct($1); }
		|  ENUM NAME
			={  $$ = rstruct($2,0);  stwart = instruct;
				/*CXREF ref($2, lineno); */ }
		;

enum_head:	   ENUM
			={  $$ = bstruct(-1,0); stwart = SEENAME; }
		|  ENUM NAME
			={  $$ = bstruct($2,0); stwart = SEENAME;
				/*CXREF ref($2, lineno); */ }
		;

moe_list:	   moe
		|  moe_list CM moe
		;

moe:		   NAME
			={  moedef( $1 ); /*CXREF def($1, lineno); */ }
		|  NAME ASSIGN con_e
			={  strucoff = $3;  moedef( $1 );
				/*CXREF def($1, lineno); */ }
		;

struct_dcl:	   str_head LC type_dcl_list optsemi RC
			={ $$ = dclstruct($1);  }
		|  STRUCT NAME
			={  $$ = rstruct($2,$1); /*CXREF ref($2, lineno); */ }
		;

str_head:	   STRUCT
			={  $$ = bstruct(-1,$1);  stwart=0; }
		|  STRUCT NAME
			={  $$ = bstruct($2,$1);  stwart=0;
				/*CXREF def($2, lineno); */ }
		;

type_dcl_list:	   type_declaration
		|  type_dcl_list SM type_declaration
		;

type_declaration:  type declarator_list
			={ curclass = SNULL;  stwart=0; $1->in.op = FREE; }
		|  type
			={  if( curclass != MOU ){
				curclass = SNULL;
				}
			    else {
				sprintf( fakename, "$%dFAKE", fake++ );
				/*
				* We will not be looking up this name in the
				* symbol table, so there is no reason to save
				* it anywhere.  (I think.)
				*/
				defid( tymerge($1,
					bdty(NAME,NIL,
					lookup( fakename, SMOS ))), curclass );
				/*"structure typed union member must be named"*/
				WERROR(MESSAGE( 106 ));
				}
			    stwart = 0;
			    $1->in.op = FREE;
			    }
		;


declarator_list:   declarator
			={ defid( tymerge($<nodep>0,$1), curclass);  stwart = instruct; }
		|  declarator_list  CM {$<nodep>$=$<nodep>0;}  declarator
			={ defid( tymerge($<nodep>0,$4), curclass);  stwart = instruct; }
		;
declarator:	   fdeclarator
		|  nfdeclarator
		|  nfdeclarator COLON con_e
			%prec CM
			/* "field outside of structure" */
			={  if( !(instruct&INSTRUCT) ) UERROR( MESSAGE( 38 ) );
			    if( $3<0 || $3 >= FIELD ){
				/* "illegal field size" */
				UERROR( MESSAGE( 56 ) );
				$3 = 1;
				}
			    defid( tymerge($<nodep>0,$1), FIELD|$3 );
			    $$ = NIL;
			    }
		|  COLON con_e
			%prec CM
			/* "field outside of structure" */
			={  if( !(instruct&INSTRUCT) ) UERROR( MESSAGE( 38 ) );
			    falloc( stab, $2, -1, $<nodep>0 );  /* alignment or hole */
			    $$ = NIL;
			    }
		|  error
			={  $$ = NIL; }
		;

		/* int (a)();   is not a function --- sorry! */
nfdeclarator:	   MUL nfdeclarator		
			={  umul:
				$$ = bdty( UNARY MUL, $2, 0 ); }
		|  nfdeclarator  LP   RP		
			={  uftn:
				$$ = bdty( UNARY CALL, $1, 0 );  }
		|  nfdeclarator LB RB		
			={  uary:
				$$ = bdty( LB, $1, 0 );  }
		|  nfdeclarator LB con_e RB	
			={  bary:
				/* "zero or negative subscript" */
				if( (int)$3 <= 0 ) WERROR( MESSAGE( 119 ) );
				$$ = bdty( LB, $1, $3 );  }
		|  NAME  		
			={  $$ = bdty( NAME, NIL, $1 );
				/*CXREF def($1, lineno); */ }
		|   LP  nfdeclarator  RP 		
			={ $$=$2; }
		;
fdeclarator:	   MUL fdeclarator
			={  goto umul; }
		|  fdeclarator  LP   RP
			={  goto uftn; }
		|  fdeclarator LB RB
			={  goto uary; }
		|  fdeclarator LB con_e RB
			={  goto bary; }
		|   LP  fdeclarator  RP
			={ $$ = $2; }
		|  name_lp
				{if(paramno)uerror("arg list in declaration");}
			name_list  RP
			/* we can't deal with args in declarations of funcs */
			/* this is patch of a botch in the rules: char f(p); */
			={
				/* "function declaration in bad context" */
				if( blevel!=0 ) UERROR(MESSAGE( 44 ));
				$$ = bdty( UNARY CALL, bdty(NAME,NIL,$1), 0 );
				stwart = 0;
				}
		|  name_lp {if(paramno)uerror("arg list in declaration");} RP
			={
				$$ = bdty( UNARY CALL, bdty(NAME,NIL,$1), 0 );
				stwart = 0;
				}
		;

name_lp:	  NAME LP
			={
				/* turn off typedefs for argument names */
				/*CXREF newf($1, lineno); def($1, lineno); */
				stwart = SEENAME;
				if( stab[$1].sclass == SNULL )
				    stab[$1].stype = FTN;
				}
		;

name_list:	   NAME			
			={ ftnarg( $1 );  stwart = SEENAME;
				/*CXREF ref($1, lineno); */ }
		|  name_list  CM  NAME 
			={ ftnarg( $3 );  stwart = SEENAME;
				/*CXREF ref($3, lineno); */ }
		|  error
		;
		/* always preceeded by attributes: thus the $<nodep>0's */
init_dcl_list:	   init_declarator
			%prec CM
		|  init_dcl_list  CM {$<nodep>$=$<nodep>0;}  init_declarator
		;
		/* always preceeded by attributes */
xnfdeclarator:	   nfdeclarator
			={  defid( $1 = tymerge($<nodep>0,$1), curclass);
			    beginit($1->tn.rval);
			    }
		|  error
		;
		/* always preceeded by attributes */
init_declarator:   nfdeclarator
			={  nidcl( tymerge($<nodep>0,$1) ); }
		|  fdeclarator
			={  defid( tymerge($<nodep>0,$1), uclass(curclass) );
			}
/* old init is out
		|  xnfdeclarator optasgn e
			%prec CM
			={  doinit( $3 );
			    endinit(); }
		|  xnfdeclarator optasgn LC init_list optcomma RC
			={  endinit(); }
*/
		|  xnfdeclarator ASSIGN e
			%prec CM
			={  doinit( $3 );
			    endinit(); }
		|  xnfdeclarator ASSIGN LC init_list optcomma RC
			={  endinit(); }
		|  error
		;

init_list:	   initializer
			%prec CM
		|  init_list  CM  initializer
		;
initializer:	   e
			%prec CM
			={  doinit( $1 ); }
		|  ibrace init_list optcomma RC
			={  irbrace(); }
		;

optcomma	:	/* VOID */
		|  CM
		;

optsemi		:	/* VOID */
		|  SM
		;

/* old init is out
optasgn		:	/* VOID */
			/* "old-fashioned initialization: use =" */
/*
			={  WERROR( MESSAGE( 88 ) ); }
		|  ASSIGN
		;
*/

ibrace		: LC
			={  ilbrace(); }
		;

/*	STATEMENTS	*/

compoundstmt:	   begin dcl_stat_list stmt_list RC
			={  --blevel;
			    /*CXREF becode(); */
			    if( blevel == 1 ) blevel = 0;
			    clearst( blevel );
			    cleardim();
			    checkst( blevel );
			    autooff = *--psavbc;
			    regvar = *--psavbc;
			    }
		;

begin:		  LC
			={  setdim();
			    if( blevel == 1 ) dclargs();
			    /*CXREF else if (blevel > 1) bbcode(); */
			    ++blevel;
			    if( psavbc > &asavbc[BCSZ-2] ) cerror( "nesting too deep" );
			    *psavbc++ = regvar;
			    *psavbc++ = autooff;
			    }
		;

statement:	   e   SM
			={ ecomp( $1 ); }
		|  ASM SM
			{ asmout(); }
		|  compoundstmt
		|  ifprefix statement
			={ deflab($1);
			   reached = 1;
			   }
		|  ifelprefix statement
			={  if( $1 != NOLAB ){
				deflab( $1 );
				reached = 1;
				}
			    }
		|  WHILE LP e RP
			{
				savebc();
				if (!reached)
					/* "loop not entered at top" */
					WERROR(MESSAGE(75));
				reached = 1;
				brklab = getlab();
				contlab = getlab();
				switch (wloop_level)
				{
				default:
					cerror("bad while loop code gen value");
					/*NOTREACHED*/
				case LL_TOP:	/* test at loop top */
					deflab(contlab);
					if ($3->in.op == ICON &&
						$3->tn.lval != 0)
					{
						flostat = FLOOP;
						tfree($3);
					}
					else
						ecomp(buildtree(CBRANCH, $3,
							bcon((CONSZ) brklab)));
					break;
				case LL_BOT:	/* test at loop bottom */
					if ($3->in.op == ICON &&
						$3->tn.lval != 0)
					{
						flostat = FLOOP;
						tfree($3);
						deflab(contlab);
					}
					else
					{
						branch(contlab);
						deflab($<intval>$ = getlab());
					}
					break;
				case LL_DUP:	/* dup. test at top & bottom */
					if ($3->in.op == ICON &&
						$3->tn.lval != 0)
					{
						flostat = FLOOP;
						tfree($3);
						deflab($<intval>$ = contlab);
					}
					else
					{
#ifndef LINT
						register NODE *sav;
						extern NODE *tcopy();

						sav = tcopy($3);
						ecomp(buildtree(CBRANCH,$3,
							bcon((CONSZ) brklab)));
						$3 = sav;
#endif
						deflab($<intval>$ = getlab());
					}
					break;
				}
			}
			statement
		{
			switch (wloop_level)
			{
			default:
				cerror("bad while loop code gen. value");
				/*NOTREACHED*/
			case LL_TOP:	/* test at loop top */
				branch(contlab);
				break;
			case LL_BOT:	/* test at loop bottom */
				if (flostat & FLOOP)
					branch(contlab);
				else
				{
					reached = 1;
					deflab(contlab);
					ecomp(buildtree(CBRANCH,
						buildtree(NOT, $3, NIL),
						bcon((CONSZ) $<intval>5)));
				}
				break;
			case LL_DUP:	/* dup. test at top & bottom */
				if (flostat & FLOOP)
					branch(contlab);
				else
				{
					if (flostat & FCONT)
					{
						reached = 1;
						deflab(contlab);
					}
					ecomp(buildtree(CBRANCH,
						buildtree(NOT, $3, NIL),
						bcon((CONSZ) $<intval>5)));
				}
				break;
			}
			if ((flostat & FBRK) || !(flostat & FLOOP))
				reached = 1;
			else
				reached = 0;
			deflab(brklab);
			resetbc(0);
		}
		|  doprefix statement WHILE  LP  e  RP   SM
			={  deflab( contlab );
			    if( flostat & FCONT ) reached = 1;
			    ecomp( buildtree( CBRANCH, buildtree( NOT, $5, NIL ), bcon( (CONSZ) $1 ) ) );
			    deflab( brklab );
			    reached = 1;
			    resetbc(0);
			    }
		|  FOR LP .e SM .e SM
			{
				if ($3)
					ecomp($3);
				else if (!reached)
					/* "loop not entered at top" */
					WERROR(MESSAGE(75));
				savebc();
				contlab = getlab();
				brklab = getlab();
				reached = 1;
				switch (floop_level)
				{
				default:
					cerror("bad for loop code gen. value");
					/*NOTREACHED*/
				case LL_TOP:	/* test at loop top */
					deflab($<intval>$ = getlab());
					if (!$5)
						flostat |= FLOOP;
					else if ($5->in.op == ICON &&
						$5->tn.lval != 0)
					{
						flostat |= FLOOP;
						tfree($5);
						$5 = (NODE *)0;
					}
					else
						ecomp(buildtree(CBRANCH, $5,
							bcon((CONSZ) brklab)));
					break;
				case LL_BOT:	/* test at loop bottom */
					if (!$5)
						flostat |= FLOOP;
					else if ($5->in.op == ICON &&
						$5->tn.lval != 0)
					{
						flostat |= FLOOP;
						tfree($5);
						$5 = (NODE *)0;
					}
					else
						branch($<intval>1 = getlab());
					deflab($<intval>$ = getlab());
					break;
				case LL_DUP:	/* dup. test at top & bottom */
					if (!$5)
						flostat |= FLOOP;
					else if ($5->in.op == ICON &&
						$5->tn.lval != 0)
					{
						flostat |= FLOOP;
						tfree($5);
						$5 = (NODE *)0;
					}
					else
					{
#ifndef LINT
						register NODE *sav;
						extern NODE *tcopy();

						sav = tcopy($5);
						ecomp(buildtree(CBRANCH, $5,
							bcon((CONSZ) brklab)));
						$5 = sav;
#endif
					}
					deflab($<intval>$ = getlab());
					break;
				}
			}
			.e RP statement
		{
			if (flostat & FCONT)
			{
				deflab(contlab);
				reached = 1;
			}
			if ($8)
				ecomp($8);
			switch (floop_level)
			{
			default:
				cerror("bad for loop code gen. value");
				/*NOTREACHED*/
			case LL_TOP:	/* test at loop top */
				branch($<intval>7);
				break;
			case LL_BOT:	/* test at loop bottom */
				if ($5)
					deflab($<intval>1);
				/*FALLTHROUGH*/
			case LL_DUP:	/* dup. test at top & bottom */
				if ($5)
				{
					ecomp(buildtree(CBRANCH,
						buildtree(NOT, $5, NIL),
						bcon((CONSZ) $<intval>7)));
				}
				else
					branch($<intval>7);
				break;
			}
			deflab(brklab);
			if ((flostat & FBRK) || !(flostat & FLOOP))
				reached = 1;
			else
				reached = 0;
			resetbc(0);
		}
		| switchpart statement
			={  if( reached ) branch( brklab );
			    deflab( $1 );
			   swend();
			    deflab(brklab);
			    if( (flostat&FBRK) || !(flostat&FDEF) ) reached = 1;
			    resetbc(FCONT);
			    }
		|  BREAK  SM
			/* "illegal break" */
			={  if( brklab == NOLAB ) UERROR( MESSAGE( 50 ));
			    else if(reached) branch( brklab );
			    flostat |= FBRK;
			    if( brkflag ) goto rch;
			    reached = 0;
			    }
		|  CONTINUE  SM
			/* "illegal continue" */
			={  if( contlab == NOLAB ) UERROR( MESSAGE( 55 ));
			    else branch( contlab );
			    flostat |= FCONT;
			    goto rch;
			    }
		|  RETURN  SM
			={  retstat |= NRETVAL;
			    branch( retlab );
			rch:
			    /* "statement not reached" */
			    if( !reached && !reachflg ) WERROR( MESSAGE( 100 ));
			    reached = 0;
			    reachflg = 0;
			    }
		|  RETURN e  SM
			={  register NODE *temp;
			    idname = curftn;
			    temp = buildtree( NAME, NIL, NIL );
			    if(temp->in.type == TVOID)
				/* "void function %s cannot return value" */
				UERROR(MESSAGE( 116 ),
					stab[idname].sname);
			    temp->in.type = DECREF( temp->in.type );
			    temp = buildtree( RETURN, temp, $2 );
			    /* now, we have the type of the RHS correct */
			    temp->in.left->in.op = FREE;
			    temp->in.op = FREE;
			    ecomp( buildtree( FORCE, temp->in.right, NIL ) );
			    retstat |= RETVAL;
			    branch( retlab );
			    reached = 0;
			    reachflg = 0;
			    }
		|  GOTO NAME SM
			={  register NODE *q;
			    q = block( FREE, NIL, NIL, INT|ARY, 0, INT );
			    q->tn.rval = idname = $2;
			    defid( q, ULABEL );
			    stab[idname].suse = -lineno;
			    branch( stab[idname].offset );
			    /*CXREF ref($2, lineno); */
			    goto rch;
			    }
		|   SM
		|  error  SM
		|  error RC
		|  label statement
		;
label:		   NAME COLON
			={  register NODE *q;
			    q = block( FREE, NIL, NIL, INT|ARY, 0, LABEL );
			    q->tn.rval = $1;
			    defid( q, LABEL );
			    reached = 1;
			    /*CXREF def($1, lineno); */
			    }
		|  CASE e COLON
			={  addcase($2);
			    reached = 1;
			    }
		|  DEFAULT COLON
			={  reached = 1;
			    adddef();
			    flostat |= FDEF;
			    }
		;
doprefix:	DO
			={  savebc();
			    /* "loop not entered at top" */
			    if( !reached ) WERROR( MESSAGE( 75 ));
			    brklab = getlab();
			    contlab = getlab();
			    deflab( $$ = getlab() );
			    reached = 1;
			    }
		;
ifprefix:	IF LP e RP
			={  ecomp( buildtree( CBRANCH, $3, bcon( (CONSZ) ($$=getlab())) ) ) ;
			    reached = 1;
			    }
		;
ifelprefix:	  ifprefix statement ELSE
			={  if( reached ) branch( $$ = getlab() );
			    else $$ = NOLAB;
			    deflab( $1 );
			    reached = 1;
			    }
		;

switchpart:	   SWITCH  LP  e  RP
			={	int type;
				savebc();
			    brklab = getlab();
				type = BTYPE( $3->in.type );
				if( ( SZLONG > SZINT ) && ( type == LONG || type == ULONG ) )
					/* long in case or switch statement may be truncated */
					WERROR( MESSAGE( 123 ));
			    ecomp( buildtree( FORCE, $3, NIL ) );
			    branch( $$ = getlab() );
			    swstart();
			    reached = 0;
			    }
		;
/*	EXPRESSIONS	*/
con_e:		   { $<intval>$=instruct; stwart=instruct=0; } e
			%prec CM
			={  $$ = icons( $2 );  instruct=$<intval>1; }
		;
.e:		   e
		|
			={ $$=0; }
		;
elist:		   e
			%prec CM
		|  elist  CM  e
			={  goto bop; }
		;

e:		   e RELOP e
			={
			preconf:
			    if( yychar==RELOP||yychar==EQUOP||yychar==AND||yychar==OR||yychar==ER ){
			    precplaint:
				/* "precedence confusion possible: parenthesize!" */
				if( hflag ) WERROR( MESSAGE( 92 ) );
				}
			bop:
			    $$ = buildtree( $2, $1, $3 );
			    }
		|  e CM e
			={  $2 = COMOP;
			    goto bop;
			    }
		|  e DIVOP e
			={  goto bop; }
		|  e PLUS e
			={  if(yychar==SHIFTOP) goto precplaint; else goto bop; }
		|  e MINUS e
			={  if(yychar==SHIFTOP ) goto precplaint; else goto bop; }
		|  e SHIFTOP e
			={  if(yychar==PLUS||yychar==MINUS) goto precplaint; else goto bop; }
		|  e MUL e
			={  goto bop; }
		|  e EQUOP  e
			={  goto preconf; }
		|  e AND e
			={  if( yychar==RELOP||yychar==EQUOP ) goto preconf;  else goto bop; }
		|  e OR e
			={  if(yychar==RELOP||yychar==EQUOP) goto preconf; else goto bop; }
		|  e ER e
			={  if(yychar==RELOP||yychar==EQUOP) goto preconf; else goto bop; }
		|  e ANDAND e
			={  goto bop; }
		|  e OROR e
			={  goto bop; }
		|  e MUL ASSIGN e
			={  abop:
				$$ = buildtree( ASG $2, $1, $4 );
				}
		|  e DIVOP ASSIGN e
			={  goto abop; }
		|  e PLUS ASSIGN e
			={  goto abop; }
		|  e MINUS ASSIGN e
			={  goto abop; }
		|  e SHIFTOP ASSIGN e
			={  goto abop; }
		|  e AND ASSIGN e
			={  goto abop; }
		|  e OR ASSIGN e
			={  goto abop; }
		|  e ER ASSIGN e
			={  goto abop; }
		|  e QUEST e COLON e
			={  $$=buildtree(QUEST, $1, buildtree( COLON, $3, $5 ) );
			    }
		|  e ASOP e
			/* "old-fashioned assignment operator"  */
			={  UERROR( MESSAGE( 87 ) );  goto bop; }
		|  e ASSIGN e
			={  goto bop; }
		|  term
		;
term:		   term INCOP
			={  $$ = buildtree( $2, $1, bcon((CONSZ) 1) ); }
		|  MUL term
			={ ubop:
			    $$ = buildtree( UNARY $1, $2, NIL );
			    }
		|  AND term
			={  if( ISFTN($2->in.type) || ISARY($2->in.type) ){
				/* "& before array or function: ignored" */
				WERROR( MESSAGE( 7 ) );
				$$ = $2;
				}
			    else goto ubop;
			    }
		|  MINUS term
			={  goto ubop; }
		|  UNOP term
			={
			    $$ = buildtree( $1, $2, NIL );
			    }
		|  INCOP term
			={  $$ = buildtree( $1==INCR ? ASG PLUS : ASG MINUS,
						$2,
						bcon((CONSZ) 1)  );
			    }
		|  SIZEOF term
			={  $$ = doszof( $2 ); }
		|  LP cast_type RP term  %prec INCOP
			={  $$ = buildtree( CAST, $2, $4 );

		/* carefully, now...
		/* paintdown in buildtree returns the right child, thus
		/* we don't have to FREE nodes and return the right child here--
		/* we know this by the cast being tossed and having an ICON in
		/* hand upon the return from buildtree */

			    if ($$->in.op != ICON)
			    {
				    $$->in.left->in.op = FREE;
				    $$->in.op = FREE;
				    $$ = $$->in.right;
			    }
			}
		|  SIZEOF LP cast_type RP  %prec SIZEOF
			={  $$ = doszof( $3 ); }
		|  term LB e RB
			={  $$ = buildtree( UNARY MUL, buildtree( PLUS, $1, $3 ), NIL ); }
		|  funct_idn  RP
			={  $$=buildtree(UNARY CALL,$1,NIL); }
		|  funct_idn elist  RP
			={  $$=buildtree(CALL,$1,$2); }
		|  term STROP NAME
			={  if( $2 == DOT ){
				/* "structure reference must be addressable" */
				if( notlval( $1 ) )UERROR(MESSAGE( 105 ));
				$1 = buildtree( UNARY AND, $1, NIL );
				}
			    idname = $3;
			    $$ = buildtree( STREF, $1, buildtree( NAME, NIL, NIL ) );
			    /*CXREF ref($3, lineno); */
			    }
		|  NAME
			={  idname = $1;
			    /* recognize identifiers in initializations */
			    if( blevel==0 && stab[idname].stype == UNDEF ) {
				register NODE *q;
				/* "undeclared initializer name %.8s" */
				/* "undeclared initializer name %s" */
				WERROR( MESSAGE( 111 ), stab[idname].sname );
				q = block( FREE, NIL, NIL, INT, 0, INT );
				q->tn.rval = idname;
				defid( q, EXTERN );
				}
			    $$=buildtree(NAME,NIL,NIL);
			    stab[$1].suse = -lineno;
			    /*CXREF ref($1, lineno); */
			}
		|  ICON
			={  $$=bcon((CONSZ) 0);
			    $$->tn.lval = lastcon;
# ifdef BIGCONSTS
			    $$->tn.l2val = last2con;
# endif
			    $$->tn.rval = NONAME;
			    if( $1 ) $$->fn.csiz = $$->in.type = ctype(LONG);
			    }
		|  FCON
			={  $$=buildtree(FCON,NIL,NIL);
			    $$->fpn.dval = dcon;
			    }
		|  STRING
			={  $$ = getstr(); /* get string contents */ }
		|   LP  e  RP
			={ $$=$2; }
		;

cast_type:	  type null_decl
			={
			$$ = tymerge( $1, $2 );
			$$->in.op = NAME;
			$1->in.op = FREE;
			}
		;

null_decl:	   /* empty */
			={ $$ = bdty( NAME, NIL, -1 ); }
		|  LP RP
			={ $$ = bdty( UNARY CALL, bdty(NAME,NIL,-1),0); }
		|  LP null_decl RP LP RP
			={  $$ = bdty( UNARY CALL, $2, 0 ); }
		|  MUL null_decl
			={  goto umul; }
		|  null_decl LB RB
			={  goto uary; }
		|  null_decl LB con_e RB
			={  goto bary;  }
		|  LP null_decl RP
			={ $$ = $2; }
		;

funct_idn:	   NAME  LP 
			={  if( stab[$1].stype == UNDEF ){
				register NODE *q;
				q = block( FREE, NIL, NIL, FTN|INT, 0, INT );
				q->tn.rval = $1;
				defid( q, EXTERN );
				}
			    idname = $1;
			    $$=buildtree(NAME,NIL,NIL);
			    stab[idname].suse = -lineno;
			    /*CXREF ref($1, lineno); */
			}
		|  term  LP 
		;
%%

NODE *
mkty( t, d, s ) unsigned t; {
	return( block( TYPE, NIL, NIL, t, d, s ) );
	}

NODE *
bdty( op, p, v ) NODE *p; {
	register NODE *q;

	q = block( op, p, NIL, INT, 0, INT );

	switch( op ){

	case UNARY MUL:
	case UNARY CALL:
		break;

	case LB:
		q->in.right = bcon((CONSZ) v);
		break;

	case NAME:
		q->tn.rval = v;
		break;

	default:
		cerror( "bad bdty" );
		}

	return( q );
	}

dstash( n ){ /* put n into the dimension table */
	if( curdim >= DIMTABSZ-1 ){
		cerror( "dimension table overflow");
		}
	dimtab[ curdim++ ] = n;
	}

savebc() {
	if( psavbc > & asavbc[BCSZ-4 ] ){
		cerror( "whiles, fors, etc. too deeply nested");
		}
	*psavbc++ = brklab;
	*psavbc++ = contlab;
	*psavbc++ = flostat;
	*psavbc++ = swx;
	flostat = 0;
	}

resetbc(mask){

	swx = *--psavbc;
	flostat = *--psavbc | (flostat&mask);
	contlab = *--psavbc;
	brklab = *--psavbc;

	}

addcase(p) NODE *p; { /* add case to switch */
  int type;
	p = optim( p );  /* change enum to ints */
	if( p->in.op != ICON ){
		/* "non-constant case expression" */
		UERROR( MESSAGE( 80 ));
		return;
		}
	type = BTYPE( p->in.type );
	if( ( SZLONG > SZINT ) && ( type == LONG || type == ULONG ) )
		/* long in case or switch statement may be truncated */
		WERROR( MESSAGE( 123 ));
	if( swp == swtab ){
		/* "case not in switch" */
		UERROR( MESSAGE( 20 ));
		return;
		}
	if( swp >= &swtab[SWITSZ] ){
		cerror( "switch table overflow");
		}
	swp->sval = p->tn.lval;
	deflab( swp->slab = getlab() );
	++swp;
	tfree(p);
	}

adddef(){ /* add default case to switch */
	if( swtab[swx].slab >= 0 ){
		/* "duplicate default in switch" */
		UERROR( MESSAGE( 34 ));
		return;
		}
	if( swp == swtab ){
		/* "default not inside switch" */
		UERROR( MESSAGE( 29 ));
		return;
		}
	deflab( swtab[swx].slab = getlab() );
	}

swstart(){
	/* begin a switch block */
	if( swp >= &swtab[SWITSZ] ){
		cerror( "switch table overflow");
		}
	swx = swp - swtab;
	swp->slab = -1;
	++swp;
	}

swend(){ /* end a switch block */

	register struct sw *swbeg, *p, *q, *r, *r1;
	CONSZ temp;
	int tempi;

	swbeg = &swtab[swx+1];

	/* sort */

	r1 = swbeg;
	r = swp-1;

	while( swbeg < r ){
		/* bubble largest to end */
		for( q=swbeg; q<r; ++q ){
			if( q->sval > (q+1)->sval ){
				/* swap */
				r1 = q+1;
				temp = q->sval;
				q->sval = r1->sval;
				r1->sval = temp;
				tempi = q->slab;
				q->slab = r1->slab;
				r1->slab = tempi;
				}
			}
		r = r1;
		r1 = swbeg;
		}

	/* it is now sorted */

	for( p = swbeg+1; p<swp; ++p ){
		if( p->sval == (p-1)->sval ){
			/* "duplicate case in switch, %d" */
			UERROR( MESSAGE( 33 ), tempi=p->sval );
			return;
			}
		}

	genswitch( swbeg-1, swp-swbeg );
	swp = swbeg-1;
	}

setdim() { /*  store dimtab info on block entry */
	dimptr++;
	if (dimptr >= &dimrec[BNEST]) uerror("block nesting too deep");
	dimptr->index  = curdim;
	dimptr->cextern = 0;
	}

cleardim(){ /* clear dimtab info on block exit */
	if(dimptr->cextern == 0) {
		curdim = dimptr->index;
		dimptr--;
		}
	else {
		dimptr--;
		if (dimptr >= dimrec) dimptr->cextern = 1;
		}
	}
