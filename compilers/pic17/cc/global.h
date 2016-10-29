/*
 * C Compiler for PIC17C4x processors.
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
#include "machdep.h"

#define MAXNODES        4096    /* max function tree size */
#define MAXSYMS         1024    /* max global+local symbols */
#define MAXDELAYED      32      /* max post-increment ops */
#define MAXNEST         32      /* max nested loops */

typedef struct _node node_t;

struct _node {
	int op;
	int type;
	node_t *left;
	node_t *right;
	unsigned long lval;
	int rval;
};

typedef struct {
	char *name;             /* symbol name */
	int type;               /* variable/array type */
	int size;               /* array dimension, 0 for simple variables,
				 * 1 for defined functions,
				 * 2 for interrupt functions */
	int local;              /* local symbol or function argument */
	long addr;              /* @ address */
} sym_t;

#define TPTR    0x10
#define TFAR    0x20
#define TFUNC   0x40
#define TMASK   0x0f

#define TVOID   1
#define TCHAR   2
#define TSHORT  3
#define TLONG   4

#define TVOIDP  (TVOID  | TPTR)
#define TCHARP  (TCHAR  | TPTR)
#define TSHORTP (TSHORT | TPTR)
#define TLONGP  (TLONG  | TPTR)

#define TVOIDFP  (TVOID  | TPTR | TFAR)
#define TCHARFP  (TCHAR  | TPTR | TFAR)
#define TSHORTFP (TSHORT | TPTR | TFAR)
#define TLONGFP  (TLONG  | TPTR | TFAR)

#define LEAF(op)        (! ((op) & 0x300))
#define UNARY(op)       ((op) & 0x100)
#define BINARY(op)      ((op) & 0x200)

/*
 * Leaf ops.
 */
#define OP_NAME         0x001   /* variable or function name, stab[rval] */
#define OP_LABEL        0x002   /* label name, stab[rval] */
#define OP_CASE         0x003   /* case label, lval is the value */
#define OP_DEFAULT      0x004   /* default label */
#define OP_ASM          0x005   /* lvar=string, don't forget to free */
#define OP_RETURN       0x006   /* return operator */
#define OP_BREAK        0x007   /* break operator */
#define OP_CONTINUE     0x008   /* continue operator */
#define OP_STRING       0x009   /* lvar=string, don't forget to free */
#define OP_CONST        0x010   /* integer constant */

/*
 * Unary ops.
 */
#define OP_GOTO         0x101   /* goto operator */
#define OP_RETVAL       0x102   /* return value operator */
#define OP_ADDR         0x103   /* address op (&x) */
#define OP_REF          0x104   /* de-reference op (*x) */
#define OP_NEG          0x105   /* negate op (-x) */
#define OP_NOT          0x106   /* logical negate op (!x) */
#define OP_COMPL        0x107   /* logical complement op (~x) */
#define OP_CAST         0x108   /* type cast op */
#define OP_INC          0x109   /* pre-increment op (++x) */
#define OP_DEC          0x110   /* pre-decrement op (--x) */
#define OP_POSTINC      0x111   /* post-increment op (x++) */
#define OP_POSTDEC      0x112   /* post-decrement op (x--) */

/*
 * Binary ops.
 */
#define OP_COMMA        0x201   /* list of arguments or comma op */
#define OP_SEMICOLON    0x202   /* list of statements */
#define OP_IF           0x203   /* `if' statement */
#define OP_IFELSE       0x204   /* `if-else' statement */
#define OP_WHILE        0x205   /* `while' statement */
#define OP_DOUNTIL      0x206   /* `do-while' statement (negated) */
#define OP_SWITCH       0x207   /* `switch' statement */
#define OP_LT           0x208   /* < */
#define OP_GT           0x209   /* > */
#define OP_LE           0x210   /* <= */
#define OP_GE           0x211   /* >= */
#define OP_FUNC         0x212   /* function declaration */
#define OP_DIV          0x213   /* / */
#define OP_MOD          0x214   /* % */
#define OP_ADD          0x215   /* + */
#define OP_SUB          0x216   /* - */
#define OP_LSHIFT       0x217   /* << */
#define OP_RSHIFT       0x218   /* >> */
#define OP_MUL          0x219   /* * */
#define OP_EQU          0x220   /* == */
#define OP_NEQ          0x221   /* != */
#define OP_AND          0x222   /* & */
#define OP_OR           0x223   /* | */
#define OP_XOR          0x224   /* ^ */
#define OP_ANDAND       0x225   /* && */
#define OP_OROR         0x226   /* || */
#define OP_MULASG       0x227   /* *= */
#define OP_DIVASG       0x228   /* /= */
#define OP_MODASG       0x229   /* %= */
#define OP_ADDASG       0x230   /* += */
#define OP_SUBASG       0x231   /* -= */
#define OP_LSASG        0x232   /* <<= */
#define OP_RSASG        0x233   /* >>= */
#define OP_ANDASG       0x234   /* &= */
#define OP_ORASG        0x235   /* |= */
#define OP_XORASG       0x236   /* ^= */
#define OP_QUEST        0x237   /* ? */
#define OP_COLON        0x238   /* : */
#define OP_ASSIGN       0x239   /* = */
#define OP_DOT          0x240   /* . */
#define OP_CALL         0x241   /* f(x) */
#define OP_FOR          0x242   /* `for' statement */

/*
 * Expression compilation targets.
 */
#define TARG_ACC        1       /* leave the result in accumulator */
#define TARG_COND       2       /* only the condition flags matter */
#define TARG_EFF        3       /* compile for side effects only */

extern int errors;
extern int lineno;
extern char *filename;
extern sym_t *func;
extern sym_t stab [];

void output (char *fmt, ...);
void outstr (char *s);
void error (char *fmt, ...);
void printlex (int lex);

int salloc (char *name, int level, int type, int size);
int slookup (char *name, int level);
void sfree (void);

node_t *nalloc (void);
node_t *nfree (node_t *n);
void nprint (node_t *n, int off);
node_t *leaf (int op, int type, long lval, int rval);
node_t *node (int op, node_t *l, node_t *r);
int tsize (int type);
int evalsize (node_t *n);
node_t *evalconst (node_t *n);
node_t *rightrec (node_t *n);
node_t *twalk (node_t *n, node_t *(*f)());

void header (void);
void compile (node_t *func, node_t *def, node_t *o);
node_t *canonconst (node_t *n);
void operator (node_t *n);
void expression (node_t *n, int target);
void expr2 (node_t *n, int target);
void expr3 (node_t *n, int target);
node_t *machexpr (node_t *n, int target);

void declvar (node_t *n);
void declfun (node_t *n);
void initargs (node_t *n);
void printname (node_t *n);
node_t *machoper (node_t *n);
void printgoto (int m, int cond);
void exprincr (node_t *n, int decr);
void store (node_t *n, int op);
void loadconst (node_t *n);
void loadname (node_t *n);
void loadstring (char *s);
node_t *macharg2 (node_t *n);
int badarg1 (node_t *n);
void call (node_t *name, node_t *a);
int newlabel (void);
node_t *machcond (node_t *n, int true, int false);
void printunary (int op, int type, int argtype);
void machop (node_t *n, int target);
void pusharg (int sz);
void cast (int sz, int asz);
void divasg_by_const (node_t *l, node_t *r);
void makecond (int sz);
