/*
 * C Compiler for PIC processors.
 *
 * Copyright (C) 1997-2002 Serge Vakulenko <vak@cronyx.ru>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 */
%{
#include "global.h"
#include <string.h>
#define YYSTACKSIZE 2000
%}

%union {
	long lval;
	char *strval;
	node_t *node;
}

%term   NAME STRING CON ANDAND OROR INC DEC EQUOP NEQOP LE GE LSOP RSOP STRUCT
	RETURN GOTO IF ELSE SWITCH BREAK CONTINUE WHILE DO FOR DEFAULT CASE
	SIZEOF ASM CHAR SHORT LONG FAR VOID INTERRUPT

%token <lval>   CON
%token <strval> NAME STRING
%type  <lval>   type o_addr cast
%type  <node>   decl_list declarator arg function data label asg
		fdecl_list stmt_list statement args oasg o_elist elist e term

%left ','
%right '='
%right '?' ':'
%left OROR
%left ANDAND
%left '|'
%left '^'
%left '&'
%left EQUOP NEQOP
%left '<' '>' LE GE
%left RSOP LSOP
%left '+' '-'
%left '*' '/' '%'
%right '~' '!'
%right INC DEC SIZEOF
%left '[' '(' '.'

%start program

%%

program:        program declaration
	      |
	      ;

declaration:    type decl_list ';' ={ node_t *n = rightrec ($2);
				      while (n->op == OP_COMMA) {
					node_t *r = n->right;
					if (n->left->op == OP_NAME)
					  declvar (n->left);
					nfree (n);
					n = r;
				      }
				      if (n->op == OP_NAME)
					declvar (n);
				      nfree (n);
				      sfree (); }
	      | type function o_addr '{'
				    { level = 1;
				      func = stab + $2->left->rval;
				      if (stab[$2->left->rval].size)
					error ("function `%s' redefined",
					  stab[$2->left->rval].name);
				      stab[$2->left->rval].size = 1; }
		fdecl_list stmt_list '}'
				   ={ stab[$2->left->rval].addr = $3;
				      compile ($2, $6, $7);
				      twalk ($2, nfree);
				      twalk ($6, nfree);
				      twalk ($7, nfree);
				      sfree (); level = 0; }
	      | INTERRUPT type function o_addr '{'
				    { level = 1;
				      func = stab + $3->left->rval;
				      if (stab[$3->left->rval].size)
					error ("function `%s' redefined",
					  stab[$3->left->rval].name);
				      stab[$3->left->rval].size = 2; }
		fdecl_list stmt_list '}'
				   ={ stab[$3->left->rval].addr = $4;
				      compile ($3, $7, $8);
				      twalk ($3, nfree);
				      twalk ($7, nfree);
				      twalk ($8, nfree);
				      sfree (); level = 0; }
	      | ASM '(' STRING ')' ';'
				   ={ outstr ($3); }
	      | error
	      ;

decl_list:      declarator %prec ','     ={ $$ = $1; }
	      | decl_list ',' declarator ={ $$ = node (OP_COMMA, $1, $3); }
	      ;

declarator:     data o_addr     ={ $$ = $1; stab[$$->rval].addr = $2; }
	      | function o_addr ={ $$ = $1; stab[$$->left->rval].addr = $2; }
	      ;

data:           '*' NAME        ={ $$ = defvar ($2, type|TPTR, 0); }
	      | FAR '*' NAME    ={ $$ = defvar ($3, type|TPTR|TFAR, 0); }
	      | NAME '[' e ']'  ={ $3 = twalk ($3, evalconst);
				   if ($3->op != OP_CONST)
					error ("invalid array size");
				   $$ = defvar ($1, type, $3->lval);
				   twalk ($3, nfree); }
	      | NAME            ={ $$ = defvar ($1, type, 0); }

function:       '*' NAME '('     { fnode = deffun ($2, type|TPTR); }
		args ')'        ={ $$ = node (OP_FUNC, fnode, $5); }
	      | FAR '*' NAME '(' { fnode = deffun ($3, type|TPTR|TFAR); }
		args ')'        ={ $$ = node (OP_FUNC, fnode, $6); }
	      | NAME '('         { fnode = deffun ($1, type); }
		args ')'        ={ $$ = node (OP_FUNC, fnode, $4); }
	      ;

type:           CHAR            ={ $$ = type = TCHAR; }
	      | SHORT           ={ $$ = type = TSHORT; }
	      | LONG            ={ $$ = type = TLONG; }
	      | VOID            ={ $$ = type = TVOID; }
	      ;

o_addr:         /*void*/        ={ $$ = 0; }
	      | '@' CON         ={ $$ = $2 + 1; }
	      ;

args:           /*void*/               ={ $$ = 0; }
	      | type arg %prec ','     ={ $$ = $2; $$->type |= $1; }
	      | type arg ',' type arg  ={ $$ = node (OP_COMMA, $2, $5);
					  $2->type |= $1; $5->type |= $4; }
	      ;

arg:            '*' NAME        ={ $$ = defarg ($2, type|TPTR); }
	      | FAR '*' NAME    ={ $$ = defarg ($3, type|TPTR|TFAR); }
	      | NAME            ={ $$ = defarg ($1, type); }
	      ;

fdecl_list:     fdecl_list type decl_list ';'
			    ={ if ($1) $$ = node (OP_COMMA, $1, $3);
			       else    $$ = $3; }
	      |             ={ $$ = 0; }
	      ;

stmt_list:      stmt_list statement
			    ={ if ($1 && $2) $$ = node (OP_SEMICOLON, $1, $2);
			       else if ($1)  $$ = $1;
			       else          $$ = $2; }
	      | /*void*/    ={ $$ = 0; }
	      ;

statement:      asg ';'                 ={ $$ = $1; }
	      | '{' stmt_list '}'       ={ $$ = $2; }
	      | IF '(' asg ')' statement
					={ $$ = node (OP_IF, $3, $5); }
	      | IF '(' asg ')' statement ELSE statement
					={ $$ = node (OP_IFELSE, $3,
						node (OP_SEMICOLON, $5, $7)); }
	      | WHILE '(' asg ')' statement
					={ $$ = node (OP_WHILE, $3, $5); }
	      | DO statement WHILE '(' asg ')' ';'
					={
					   if ($5->op == OP_NOT)
						$$ = node (OP_DOUNTIL, $2, $5->left);
					   else
						$$ = node (OP_DOUNTIL, $2,
							node (OP_NOT, $5, 0)); }
	      | FOR '(' oasg ';' oasg ';' oasg ')' statement
					={ if ($7)
						$$ = node (OP_SEMICOLON, $3,
						     node (OP_FOR, $5,
						     node (OP_SEMICOLON, $7, $9)));
					   else
						$$ = node (OP_SEMICOLON, $3,
						     node (OP_WHILE, $5, $9)); }
	      | SWITCH '(' asg ')' statement
					={ $$ = node (OP_SWITCH, $3, $5); }
	      | BREAK ';'               ={ $$ = leaf (OP_BREAK, 0, 0, 0); }
	      | CONTINUE ';'            ={ $$ = leaf (OP_CONTINUE, 0, 0, 0); }
	      | RETURN ';'              ={ $$ = leaf (OP_RETURN, 0, 0, 0); }
	      | RETURN asg ';'          ={ $$ = node (OP_RETVAL, $2, 0); }
	      | GOTO NAME ';'           ={ $$ = node (OP_GOTO, refsym ($2), 0); }
	      | label statement         ={ if ($2) $$ = node (OP_SEMICOLON, $1, $2);
					   else    $$ = $1; }
	      | ASM '(' STRING ')' ';'  ={ $$ = leaf (OP_ASM, 0, (long) $3, 0); }
	      | ';'                     ={ $$ = 0; }
	      | error ';'               ={ $$ = 0; }
	      | error '}'               ={ $$ = 0; }
	      ;

asg:            e                       ={ $$ = $1; }
	      | e '=' e                 ={ $$ = node (OP_ASSIGN, $1, $3); }
	      | e '*' '=' e             ={ $$ = node (OP_MULASG, $1, $4); }
	      | e '/' '=' e             ={ $$ = node (OP_DIVASG, $1, $4); }
	      | e '%' '=' e             ={ $$ = node (OP_MODASG, $1, $4); }
	      | e '+' '=' e             ={ $$ = node (OP_ADDASG, $1, $4); }
	      | e '-' '=' e             ={ $$ = node (OP_SUBASG, $1, $4); }
	      | e LSOP '=' e            ={ $$ = node (OP_LSASG, $1, $4); }
	      | e RSOP '=' e            ={ $$ = node (OP_RSASG, $1, $4); }
	      | e '&' '=' e             ={ $$ = node (OP_ANDASG, $1, $4); }
	      | e '|' '=' e             ={ $$ = node (OP_ORASG, $1, $4); }
	      | e '^' '=' e             ={ $$ = node (OP_XORASG, $1, $4); }
	      ;

label:          NAME ':'                ={ $$ = leaf (OP_LABEL, 0, 0, deflabel ($1)); }
	      | DEFAULT ':'             ={ $$ = leaf (OP_DEFAULT, 0, 0, 0); }
	      | CASE e ':'              ={ $2 = twalk ($2, evalconst);
					   if ($2->op != OP_CONST)
						error ("invalid array size");
					   $$ = leaf (OP_CASE, 0, $2->lval, 0);
					   twalk ($2, nfree); }

oasg:           /*void*/                ={ $$ = 0; }
	      | asg                     ={ $$ = $1; }
	      ;

o_elist:        /*void*/                ={ $$ = 0; }
	      | elist                   ={ $$ = $1; }
	      ;

elist:          e %prec ','             ={ $$ = $1; }
	      | elist ',' e             ={ $$ = node (OP_COMMA, $1, $3); }
	      ;

e:              e '<' e                 ={ $$ = node (OP_LT, $1, $3); }
	      | e '>' e                 ={ $$ = node (OP_GT, $1, $3); }
	      | e LE e                  ={ $$ = node (OP_LE, $1, $3); }
	      | e GE e                  ={ $$ = node (OP_GE, $1, $3); }
	      | e ',' e                 ={ $$ = node (OP_COMMA, $1, $3); }
	      | e '/' e                 ={ $$ = node (OP_DIV, $1, $3); }
	      | e '%' e                 ={ $$ = node (OP_MOD, $1, $3); }
	      | e '+' e                 ={ $$ = node (OP_ADD, $1, $3); }
	      | e '-' e                 ={ $$ = node (OP_SUB, $1, $3); }
	      | e LSOP e                ={ $$ = node (OP_LSHIFT, $1, $3); }
	      | e RSOP e                ={ $$ = node (OP_RSHIFT, $1, $3); }
	      | e '*' e                 ={ $$ = node (OP_MUL, $1, $3); }
	      | e EQUOP e               ={ $$ = node (OP_EQU, $1, $3); }
	      | e NEQOP e               ={ $$ = node (OP_NEQ, $1, $3); }
	      | e '&' e                 ={ $$ = node (OP_AND, $1, $3); }
	      | e '|' e                 ={ $$ = node (OP_OR, $1, $3); }
	      | e '^' e                 ={ $$ = node (OP_XOR, $1, $3); }
	      | e ANDAND e              ={ $$ = node (OP_ANDAND, $1, $3); }
	      | e OROR e                ={ $$ = node (OP_OROR, $1, $3); }
	      | e '?' e ':' e           ={ $$ = node (OP_QUEST, $1,
						node (OP_COLON, $3, $5)); }
	      | term                    ={ $$ = $1; }
	      ;

term:           term INC                ={ $$ = node (OP_POSTINC, $1, 0); }
	      | term DEC                ={ $$ = node (OP_POSTDEC, $1, 0); }
	      | INC term                ={ $$ = node (OP_INC, $2, 0); }
	      | DEC term                ={ $$ = node (OP_DEC, $2, 0); }
	      | '*' term                ={ $$ = node (OP_REF, $2, 0); }
	      | '&' term                ={ $$ = node (OP_ADDR, $2, 0); }
	      | '-' term                ={ $$ = node (OP_NEG, $2, 0); }
	      | '!' term                ={ $$ = node (OP_NOT, $2, 0); }
	      | '~' term                ={ $$ = node (OP_COMPL, $2, 0); }
	      | NAME '.' CON            ={ $$ = node (OP_DOT, refsym ($1), mkconst ($3)); }
	      | '(' cast ')' term %prec INC
					={ $$ = node (OP_CAST, $4, 0);
					   $$->type = $2; }
	      | NAME '(' o_elist ')'    ={ $$ = node (OP_CALL, refsym ($1), $3); }
	      | NAME '[' e ']'          ={ $$ = node (OP_REF, node (OP_ADD,
						refsym ($1), $3), 0); }
	      | NAME                    ={ $$ = refsym ($1); }
	      | STRING                  ={ $$ = leaf (OP_STRING, 0, (long) $1, 0); }
	      | CON                     ={ $$ = mkconst ($1); }
	      | SIZEOF term             ={ $$ = mkconst (evalsize ($2)); }
	      | SIZEOF '(' cast ')' %prec SIZEOF
					={ $$ = mkconst (tsize ($3)); }
	      | '(' e ')'               ={ $$ = $2; }
	      ;

cast:           CHAR            ={ $$ = TCHAR;    }
	      | CHAR '*'        ={ $$ = TCHARP;   }
	      | CHAR FAR '*'    ={ $$ = TCHARFP;  }
	      | SHORT           ={ $$ = TSHORT;   }
	      | SHORT '*'       ={ $$ = TSHORTP;  }
	      | SHORT FAR '*'   ={ $$ = TSHORTFP; }
	      | LONG            ={ $$ = TLONG;    }
	      | LONG '*'        ={ $$ = TLONGP;   }
	      | LONG FAR '*'    ={ $$ = TLONGFP;  }
	      | VOID '*'        ={ $$ = TVOIDP;   }
	      | VOID FAR '*'    ={ $$ = TVOIDFP;  }
	      ;
%%

#include <stdio.h>

static int level, type;
static node_t *fnode;

sym_t *func;

extern int lineno;
extern int yylex (void);
extern int yyparse (void);

void yyerror (char *msg)
{
	error ("%s", msg);
}

#ifdef DEBUG_LEX
void main ()
{
	int lex;

	while ((lex = yylex () != 0)
		printlex (lex);
}
#else /* DEBUG_LEX */
int main (int argc, char **argv)
{
	/* Simulate the GNU cc1 arguments,
	 * to use the 'gcc -Bxxx' as the startup utility.
	 * We nee two arguments here:
	 * -dumpbase <file.c>
	 * -o <file.s> */
	for (++argv; --argc > 0; ++argv)
		if (strcmp ("-dumpbase", *argv) == 0) {
			if (argc < 1)
				continue;
			--argc, ++argv;
			if (filename)
				free (filename);
			filename = strdup (*argv);
		} else if (strcmp ("-o", *argv) == 0) {
			if (argc < 1)
				continue;
			--argc, ++argv;
			if (freopen (*argv, "w", stdout) != stdout) {
				perror (*argv);
				return (-1);
			}
		} else if (**argv == '-')
			continue;
		else {
			if (freopen (*argv, "r", stdin) != stdin) {
				perror (*argv);
				return (-1);
			}
		}

	/* Reserve the stab[0] entry. */
	salloc ("", 0, 0, 0);
	header ();
	yyparse ();
	return (errors ? -1 : 0);
}
#endif /* DEBUG_LEX */

static node_t *refsym (char *name)
{
	int r;

	r = slookup (name, 0);
	if (r < 0) {
		error ("undefined symbol `%s'", name);
		r = salloc (name, 0, TCHAR, 0);
	}
	if (stab[r].size && ! (stab[r].type & TFUNC))
		/* Array - insert '&'. */
		return leaf (OP_CONST, stab[r].type, 0, r);
	return leaf (OP_NAME, stab[r].type, 0, r);
}

static node_t *defvar (char *name, int type, int size)
{
	int r;

	r = slookup (name, level);
	if (r < 0)
		r = salloc (name, level, type, size);
	else if (stab[r].type != type || stab[r].size != size)
		error ("symbol `%s' redefined", stab[r].name);
	return leaf (OP_NAME, type, 0, r);
}

static node_t *defarg (char *name, int type)
{
	int r;

	r = slookup (name, 1);
	if (r < 0)
		r = salloc (name, 1, type, 0);
	else
		error ("argument `%s' redefined", stab[r].name);
	return leaf (OP_NAME, type, 0, r);
}

static node_t *deffun (char *name, int type)
{
	int r;

	type |= TFUNC;
	r = slookup (name, 0);
	if (r < 0)
		r = salloc (name, 0, type, 0);
	else if (stab[r].type != type)
		error ("symbol `%s' redefined", stab[r].name);
	return leaf (OP_NAME, type, 0, r);
}

static int deflabel (char *name)
{
	int r;

	r = slookup (name, 1);
	if (r < 0)
		r = salloc (name, 1, 0, 0);
	else
		error ("symbol `%s' redefined", stab[r].name);
	return r;
}

static node_t *mkconst (unsigned long val)
{
	int type;

	if (val <= 0xff)        type = TCHAR;
	else if (val <= 0xffff) type = TSHORT;
	else                    type = TLONG;
	return leaf (OP_CONST, type, val, 0);
}
