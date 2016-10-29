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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "global.h"

sym_t stab [MAXSYMS];
sym_t *slast = stab;

node_t *delaytab [MAXDELAYED];
int ndelayed;

node_t nodetab [MAXNODES];

struct {
	int lcont;
	int lbreak;
} looptab [MAXNEST];
int llevel;

int errors;

static int nextlabel;

void output (char *fmt, ...)
{
	va_list ap;
	int width, c;

	va_start (ap, fmt);
	while (*fmt) {
		c = *fmt++;
		switch (c) {
		default:  putchar (c);    continue;
		case ' ': putchar ('\t'); continue;
		case ';': putchar ('\n'); continue;
		case '`': putchar (' ');  continue;
		case '%': break;
		}
		width = 0;
		switch (*fmt++) {
		default:
			putchar (fmt[-1]);
			break;
		case 'c':
			putchar (va_arg (ap, int));
			break;
		case 's':
			printf ("%s", va_arg (ap, char*));
			break;
		case 'D':
			printf ("%0*ld", width, va_arg (ap, long));
			break;
		case 'd':
			printf ("%0*d", width, va_arg (ap, int));
			break;
		case 'O':
			printf ("%0*lo", width, va_arg (ap, long));
			break;
		case 'o':
			printf ("%0*o", width, va_arg (ap, int));
			break;
		case 'n':
			printname (va_arg (ap, node_t*));
			break;
		case 'l':
			printf ("%ld", va_arg(ap,node_t*)->lval & 0xff);
			break;
		case 'h':
			printf ("%ld", va_arg(ap,node_t*)->lval >> 8 & 0xff);
			break;
		case '2':
			printf ("%ld", va_arg(ap,node_t*)->lval >> 16 & 0xff);
			break;
		case '3':
			printf ("%ld", va_arg(ap,node_t*)->lval >> 24 & 0xff);
			break;
		}
	}
	va_end (ap);
}

void outstr (char *s)
{
	int c;

	if (! *s)
		return;
	if (*s != ' ' && *s != '\t')
		putchar ('\t');
	while (*s) {
		c = *s++;
		switch (c) {
		default:  putchar (c);    continue;
		case ' ': putchar ('\t'); continue;
		case ';': putchar ('\n'); continue;
		case '`': putchar (' ');  continue;
		}
	}
	--s;
	if (*s != ';' && *s != '\n')
		putchar ('\n');
}

void pushloop (int mcont, int mbreak)
{
	if (llevel >= MAXNEST) {
		error ("too deeply nested loops");
		return;
	}
	looptab[llevel].lcont = mcont;
	looptab[llevel].lbreak = mbreak;
	++llevel;
}

void poploop ()
{
	if (llevel <= 0) {
		error ("internal loop nest error");
		return;
	}
	--llevel;
}

int loopcont ()
{
	if (llevel <= 0) {
		error ("illegally nested continue");
		return 0;
	}
	return looptab[llevel-1].lcont;
}

int loopbreak ()
{
	if (llevel <= 0) {
		error ("illegally nested break");
		return 0;
	}
	return looptab[llevel-1].lbreak;
}

int newlabel ()
{
	return ++nextlabel;
}

void error (char *fmt, ...)
{
	va_list ap;

	va_start (ap, fmt);
	if (filename)
		fprintf (stderr, "%s:", filename);
	fprintf (stderr, "%d: ", lineno);
	vfprintf (stderr, fmt, ap);
	va_end (ap);
	fprintf (stderr, "\n");
	++errors;
}

int salloc (char *name, int local, int type, int size)
{
	if (slast >= stab+MAXSYMS) {
		error ("too many symbols");
		exit (-1);
	}
	slast->name = malloc (strlen (name) + 1);
	if (! slast->name) {
		error ("no memory for symbol `%s'", name);
		exit (-1);
	}
	strcpy (slast->name, name);
	slast->type = type;
	slast->size = size;
	slast->local = local;
	slast->addr = 0;
	return slast++ - stab;
}

/*
 * Loop up the symbol in the symbol table.
 */
int slookup (char *name, int localonly)
{
	sym_t *s;

	for (s=slast-1; s>=stab; --s) {
		if (localonly && ! s->local)
			continue;
		if (strcmp (s->name, name) == 0)
			return s - stab;
	}
	return -1;
}

/*
 * Free all local symbols.
 */
void sfree ()
{
	while (slast > stab && slast[-1].local)
		free ((--slast)->name);
}

/*
 * Allocate a new tree node.
 */
node_t *nalloc ()
{
	node_t *n;

	for (n=nodetab; n<nodetab+MAXNODES; ++n)
		if (! n->op)
			return n;
	error ("too complex sentence");
	exit (-1);
}

/*
 * Free the tree node.
 */
node_t *nfree (node_t *n)
{
	n->op = 0;
	return 0;
}

/*
 * Walk the tree left to right, down up,
 * and call the given function for every node.
 */
node_t *twalk (node_t *n, node_t *(*f)())
{
	if (! n)
		return 0;
	if (! LEAF (n->op))
		n->left = twalk (n->left, f);
	if (BINARY (n->op))
		n->right = twalk (n->right, f);
	return (*f) (n);
}

/*
 * Make a recursive copy of the tree.
 */
node_t *tcopy (node_t *n)
{
	node_t *q;

	if (! n)
		return 0;
	q = nalloc ();
	*q = *n;
	if (! LEAF (n->op))
		q->left = tcopy (n->left);
	if (BINARY (n->op))
		q->right = tcopy (n->right);
	return q;
}

/*
 * Create a tree leaf.
 */
node_t *leaf (int op, int type, long lval, int rval)
{
	node_t *n;

	n = nalloc ();
	n->op    = op;
	n->type  = type;
	n->left  = 0;
	n->right = 0;
	n->lval  = lval;
	n->rval  = rval;
	return n;
}

/*
 * Create a tree node.
 */
node_t *node (int op, node_t *l, node_t *r)
{
	node_t *n;

	n = nalloc ();
	n->op    = op;
	n->type  = 0;
	n->lval  = 0;
	n->rval  = 0;
	n->left  = l;
	n->right = r;
	return n;
}

void prtype (int type)
{
	switch (type & TMASK) {
	case 0:      break;
	case TVOID:  printf ("void ");  break;
	case TCHAR:  printf ("char ");  break;
	case TSHORT: printf ("short "); break;
	case TLONG:  printf ("long ");  break;
	default:     printf ("?type%d ", type & TMASK); break;
	}
	switch (type & (TPTR | TFAR)) {
	case 0:           break;
	case TPTR:        printf ("*"); break;
	case TFAR:        printf ("far *"); break;
	case TPTR | TFAR: printf ("far *"); break;
	}
}

/*
 * Print a readable representation of the tree
 * for debugging purposes.
 */
void nprint (node_t *n, int off)
{
	int i;

	for (i=0; i<off; ++i)
		printf ("    ");
	if (off < 0)
		off = -off;
	if (! n) {
		printf ("(null)\n");
		return;
	}
	switch (n->op) {
	default:
		printf ("op%d", n->op);
		break;
	/*
	 * Leaf ops.
	 */
	case OP_LABEL:    printf ("LABEL %s", stab[n->rval].name);   break;
	case OP_CONST:    printf ("CONST "); printname (n);          break;
	case OP_CASE:     printf ("CASE %ld", n->lval);               break;
	case OP_STRING:   printf ("STRING \"%s\"", (char*) n->lval); break;
	case OP_ASM:      printf ("ASM %s", (char*) n->lval);        break;
	case OP_DEFAULT:  printf ("DEFAULT");                        break;
	case OP_RETURN:   printf ("RETURN");                         break;
	case OP_BREAK:    printf ("BREAK");                          break;
	case OP_CONTINUE: printf ("CONTINUE");                       break;
	case OP_NAME:
		printf ("NAME%s %s", stab[n->rval].local ? " local" : "",
			stab[n->rval].name);
		if (n->type & TFUNC)
			printf ("()");
		else if (stab[n->rval].size)
			printf (" [%d]", stab[n->rval].size);
		if (stab[n->rval].addr)
			printf (" @ %ld", stab[n->rval].addr - 1);
		break;
	/*
	 * Unary ops.
	 */
	case OP_GOTO:    printf ("GOTO");   break;
	case OP_RETVAL:  printf ("RETVAL"); break;
	case OP_INC:     printf ("++");     break;
	case OP_DEC:     printf ("--");     break;
	case OP_POSTINC: printf ("POST++"); break;
	case OP_POSTDEC: printf ("POST--"); break;
	case OP_ADDR:    printf ("&ADDR");  break;
	case OP_REF:     printf ("*REF");   break;
	case OP_NEG:     printf ("-NEG");   break;
	case OP_NOT:     printf ("!");      break;
	case OP_COMPL:   printf ("~");      break;
	case OP_CAST:
		printf ("(");
		switch (n->type & TMASK) {
		case 0:      break;
		case TVOID:  printf ("void");  break;
		case TCHAR:  printf ("char");  break;
		case TSHORT: printf ("short"); break;
		case TLONG:  printf ("long");  break;
		default:     printf ("?type%d", n->type & TMASK); break;
		}
		if (n->type & TFAR)
			printf (" far");
		if (n->type & TPTR)
			printf ("*");
		printf (")");
		break;
	/*
	 * Binary ops.
	 */
	case OP_COMMA:
		printf (",   ");
		if (! LEAF (n->op))
			nprint (n->left, -off-1);
		if (BINARY (n->op))
			nprint (n->right, (n->right->op == OP_COMMA) ?
				off : off+1);
		return;
	case OP_SEMICOLON:
		printf (";   ");
		if (! LEAF (n->op))
			nprint (n->left, -off-1);
		if (BINARY (n->op))
			nprint (n->right, (n->right->op == OP_SEMICOLON) ?
				off : off+1);
		return;
	case OP_IF:        printf ("IF");      break;
	case OP_IFELSE:    printf ("IFELSE");  break;
	case OP_WHILE:     printf ("WHILE");   break;
	case OP_DOUNTIL:   printf ("DOUNTIL"); break;
	case OP_FOR:       printf ("FOR");     break;
	case OP_SWITCH:    printf ("SWITCH");  break;
	case OP_LT:        printf ("<");       break;
	case OP_GT:        printf (">");       break;
	case OP_LE:        printf ("<=");      break;
	case OP_GE:        printf (">=");      break;
	case OP_FUNC:      printf ("FUNC");    break;
	case OP_DIV:       printf ("/");       break;
	case OP_MOD:       printf ("%%");      break;
	case OP_ADD:       printf ("+");       break;
	case OP_SUB:       printf ("-");       break;
	case OP_LSHIFT:    printf ("<<");      break;
	case OP_RSHIFT:    printf (">>");      break;
	case OP_MUL:       printf ("*");       break;
	case OP_EQU:       printf ("==");      break;
	case OP_NEQ:       printf ("!=");      break;
	case OP_AND:       printf ("&");       break;
	case OP_OR:        printf ("|");       break;
	case OP_XOR:       printf ("^");       break;
	case OP_ANDAND:    printf ("&&");      break;
	case OP_OROR:      printf ("||");      break;
	case OP_MULASG:    printf ("*=");      break;
	case OP_DIVASG:    printf ("/=");      break;
	case OP_MODASG:    printf ("%%=");     break;
	case OP_ADDASG:    printf ("+=");      break;
	case OP_SUBASG:    printf ("-=");      break;
	case OP_LSASG:     printf ("<<=");     break;
	case OP_RSASG:     printf (">>=");     break;
	case OP_ANDASG:    printf ("&=");      break;
	case OP_ORASG:     printf ("|=");      break;
	case OP_XORASG:    printf ("^=");      break;
	case OP_QUEST:     printf ("?");       break;
	case OP_COLON:     printf (":");       break;
	case OP_ASSIGN:    printf ("=");       break;
	case OP_DOT:       printf (".");       break;
	case OP_CALL:      printf ("CALL");    break;
	}
	printf (" ");
	prtype (n->type);
	printf ("\n");
	if (! LEAF (n->op))
		nprint (n->left, off+1);
	if (BINARY (n->op))
		nprint (n->right, off+1);
}

int evalsize (node_t *n)
{
	sym_t *s;

	if (! n)
		goto bad;
	switch (n->op) {
	case OP_STRING:
		return 1 + strlen ((char*) n->left);
	case OP_CONST:
		return tsize (n->type);
	case OP_NAME:
		return tsize (stab[n->rval].type);
	case OP_CALL:
		return tsize (stab[n->left->rval].type);
	case OP_ADDR:
		return tsize (TVOID | TPTR);
	case OP_REF:
		n = n->left;
		if (n->op == OP_ADD)
			n = n->left;
		if (n->op != OP_NAME) {
bad:                    error ("bad sizeof() argument");
			return 1;
		}
		s = stab + n->rval;
		if (s->size)
			/* Array. */
			return tsize (s->type);
		if (! (s->type & TPTR))
			goto bad;
		return tsize (s->type & TMASK);
	default:
		goto bad;
	}
}

node_t *rightrec (node_t *n)
{
	node_t *l;

	if (! n)
		return 0;
again:
	switch (n->op) {
	default:
		break;
	case OP_COMMA:
	case OP_SEMICOLON:
	case OP_ADD:
	case OP_MUL:
	case OP_AND:
	case OP_OR:
	case OP_XOR:
	case OP_ANDAND:
	case OP_OROR:
		l = n->left;
		if (l->op != n->op)
			break;
		n->left = l->right;
		l->right = n;
		n = l;
		goto again;
	}
	if (! LEAF (n->op))
		n->left = rightrec (n->left);
	if (BINARY (n->op))
		n->right = rightrec (n->right);
	return n;
}

node_t *evalconst (node_t *n)
{
	unsigned long val = 0;
	node_t *l;

	l = n->left;
	if (LEAF (n->op) || l->op != OP_CONST || l->rval)
		return n;
	if (BINARY (n->op)) {
		if (n->right->op != OP_CONST || n->right->rval)
			return n;
		val = n->right->lval;
		nfree (n->right);
	} else if (l->rval)
		return n;
	switch (n->op) {
	default:        break;
	case OP_ADD:    l->lval += val;           break;
	case OP_SUB:    l->lval -= val;           break;
	case OP_MUL:    l->lval *= val;           break;
	case OP_AND:    l->lval &= val;           break;
	case OP_OR:     l->lval |= val;           break;
	case OP_XOR:    l->lval ^= val;           break;
	case OP_LSHIFT: l->lval <<= val;          break;
	case OP_RSHIFT: l->lval >>= val;          break;
	case OP_NEG:    l->lval = - l->lval;      break;
	case OP_COMPL:  l->lval = ~ l->lval;      break;
	case OP_NOT:    l->lval = ! l->lval;      break;
	case OP_LT:     l->lval = l->lval < val;  break;
	case OP_LE:     l->lval = l->lval <= val; break;
	case OP_GT:     l->lval = l->lval > val;  break;
	case OP_GE:     l->lval = l->lval >= val; break;
	case OP_EQU:    l->lval = l->lval == val; break;
	case OP_NEQ:    l->lval = l->lval != val; break;
	case OP_DIV:
		if (! val) {
divzero:                error ("division by 0");
			val = 1;
		}
		l->lval /= val;
		break;
	case OP_MOD:
		if (! val)
			goto divzero;
		l->lval /= val;
		break;
	}
	nfree (n);
	if (l->lval <= 0xff)
		l->type = TCHAR;
	else if (l->lval <= 0xffff)
		l->type = TSHORT;
	else
		l->type = TLONG;
	return l;
}

void compile (node_t *f, node_t *def, node_t *o)
{
	/* Make the function header. */
	declfun (f->left);

	/* Allocate the space for the arguments. */
	if (f->right) {
		node_t *n = f->right = rightrec (f->right);
		while (n->op == OP_COMMA) {
			node_t *r = n->right;
			if (n->left->op == OP_NAME)
				declvar (n->left);
			n = r;
		}
		if (n->op == OP_NAME)
			declvar (n);

		/* Initialize arguments. */
		initargs (f->right);
	}

	/* Define local variables. */
	if (def) {
		node_t *n = rightrec (def);
		while (n->op == OP_COMMA) {
			node_t *r = n->right;
			if (n->left->op == OP_NAME)
				declvar (n->left);
			n = r;
		}
		if (n->op == OP_NAME)
			declvar (n);
	}

	/* Compile the function body. */
	operator (canonconst (o));
	output (func->size > 1 ? ASM_RETINT : ASM_RETURN);
}

node_t *negate (node_t *n)
{
	node_t *q;

	if (n->op == OP_NOT)
		return n->left;
	if (n->op == OP_OROR) {
		n->op = OP_ANDAND;
		n->left = negate (n->left);
		n->right = negate (n->right);
		return n;
	}
	if (n->op == OP_ANDAND) {
		n->op = OP_OROR;
		n->left = negate (n->left);
		n->right = negate (n->right);
		return n;
	}
	q = nalloc ();
	q->op = OP_NOT;
	q->left = n;
	return q;
}

/*
 * Prepare a canonical form of the expression tree.
 */
node_t *canon (node_t *n)
{
	node_t *q;

	if (! n || LEAF (n->op))
		return n;

	n->left = canon (n->left);
	if (BINARY (n->op))
		n->right = canon (n->right);

	/* Set types. */
	switch (n->op) {
	case OP_ADDR:           /* address op (&x) */
		if (n->left->op == OP_NAME) {
			n->left->op = OP_CONST;
			n->left->type = stab[n->left->rval].type | TPTR;
			return n->left;
		}
		if (! (n->left->type & TPTR))
			n->type = TPTR | n->left->type;
		else if (n->left->type & TFAR)
			n->type = TPTR | TSHORT;
		else
			n->type = TPTR | TCHAR;
		break;
	case OP_REF:            /* de-reference op (*x) */
		if (n->left->op == OP_CONST) {
			n->left->op = OP_NAME;
			n->left->type = stab[n->left->rval].type;
			return n->left;
		}
		if (! (n->left->type & TPTR))
			error ("dereferencing non-pointer value");
		n->type = n->left->type & ~(TFAR|TPTR);
		break;
	case OP_LT:             /* < */
	case OP_GT:             /* > */
	case OP_LE:             /* <= */
	case OP_GE:             /* >= */
	case OP_EQU:            /* == */
	case OP_NEQ:            /* != */
	case OP_NOT:            /* logical negate op (!x) */
	case OP_DOT:            /* . */
	case OP_ANDAND:         /* && */
	case OP_OROR:           /* || */
		n->type = TCHAR;
		break;
	case OP_CAST:           /* type cast op */
		/* Nothing to do. */
		break;
	case OP_COMMA:          /* list of arguments or comma op */
	case OP_QUEST:          /* ? */
		n->type = n->right->type;
		break;
	case OP_CALL:           /* f(x) */
		n->type = n->left->type;
		break;
	case OP_ASSIGN:         /* = */
	case OP_MULASG:         /* *= */
	case OP_DIVASG:         /* /= */
	case OP_MODASG:         /* %= */
	case OP_ADDASG:         /* += */
	case OP_SUBASG:         /* -= */
	case OP_LSASG:          /* <<= */
	case OP_RSASG:          /* >>= */
	case OP_ANDASG:         /* &= */
	case OP_ORASG:          /* |= */
	case OP_XORASG:         /* ^= */
#if 0
		if (n->right->type != n->left->type) {
			q = nalloc ();
			q->op = OP_CAST;
			q->type = n->left->type;
			q->left = n->right;
			n->right = canon (q);
		}
#endif
		n->type = n->left->type;
		break;
	default:
#if 0
		if (BINARY (n->op) && n->right->type != n->left->type) {
			int lsz = tsize (n->left->type);
			int rsz = tsize (n->right->type);

			if (lsz > rsz) {
				q = nalloc ();
				q->op = OP_CAST;
				q->type = n->left->type;
				q->left = n->right;
				n->right = canon (q);
			} else if (lsz < rsz) {
				q = nalloc ();
				q->op = OP_CAST;
				q->type = n->right->type;
				q->left = n->left;
				n->left = canon (q);
			}
		}
#endif
		if (n->left)
			n->type = n->left->type;
		else if (n->right)
			n->type = n->right->type;
		else
			n->type = TCHAR;
		break;
	}

	/* Reorder the tree. */
	if (BINARY (n->op) && n->left && n->right) {
		if (LEAF (n->right->op)) {
			if (LEAF (n->left->op))
				n->rval = 1;
			else
				n->rval = n->left->rval;
		} else if (LEAF (n->left->op) || n->left->rval < n->right->rval) {
			switch (n->op) {
			case OP_ADD:    /* + */
			case OP_SUB:    /* - */
			case OP_MUL:    /* * */
			case OP_EQU:    /* == */
			case OP_NEQ:    /* != */
			case OP_AND:    /* & */
			case OP_OR:     /* | */
			case OP_XOR:    /* ^ */
swap:                           q = n->left;
				n->left = n->right;
				n->right = q;
				break;
			case OP_LT: n->op = OP_GE; goto swap;
			case OP_GT: n->op = OP_LE; goto swap;
			case OP_LE: n->op = OP_GT; goto swap;
			case OP_GE: n->op = OP_LT; goto swap;
			}
			if (LEAF (n->left->op) || n->left->rval < n->right->rval)
				n->rval = n->right->rval;
			else
				n->rval = n->left->rval;
		} else
			n->rval = n->left->rval;
	} else {
		if (! n->left || LEAF (n->left->op))
			n->rval = 1;
		else
			n->rval = n->left->rval;
	}

	/* Optimize some operators. */
	switch (n->op) {
	case OP_IF:
		if (! n->right)
			n = n->left;
		break;
	case OP_IFELSE:
		if (! n->right->right) {
			if (! n->right->left) {
				n = n->left;
				break;
			}
			n->right = n->right->left;
			n->op = OP_IF;
			break;
		}
		if (! n->right->left) {
			n->right = n->right->right;
			n->op = OP_IF;
			n->left = negate (n->left);
			break;
		}
		break;
	}
	return n;
}

/*
 * Compile the conditional expression, which
 * could contain && and || operations.
 * Make jumps to true and false labels given.
 * Skip the final "goto true" operator.
 */
void condgoto (node_t *n, int true, int false)
{
	int m;
again:
	if (n->op == OP_ANDAND) {
		m = newlabel ();
		condgoto (n->left, m, false);
		output (ASM_DEFLAB, m);
		n = n->right;
		goto again;
	}
	if (n->op == OP_OROR) {
		m = newlabel ();
		condgoto (negate (n->left), m, true);
		output (ASM_DEFLAB, m);
		n = n->right;
		goto again;
	}
	n = machcond (n, true, false);
	if (! n)
		return;
	expr2 (n, TARG_COND);
	printgoto (false, 0);
}

void operator (node_t *n)
{
	int mbegin, mcont, melse, mbody, mend;

	if (! n)
		return;
	n = machoper (canon (n));
	if (! n)
		return;
	switch (n->op) {
	default:
		expression (n, TARG_EFF);
		break;
	case OP_SEMICOLON:
		operator (n->left);
		operator (n->right);
		break;
	case OP_ASM:
		outstr ((char*) n->lval);
		break;
	case OP_LABEL:
		printname (n);
		output (":;");
		break;
	case OP_GOTO:
		output (ASM_GOTO, n->left);
		break;
	case OP_RETURN:
		output (func->size > 1 ? ASM_RETINT : ASM_RETURN);
		break;
	case OP_RETVAL:
		expression (n->left, TARG_ACC);
		output (func->size > 1 ? ASM_RETINT : ASM_RETURN);
		break;

	case OP_IF:
		mbegin = newlabel ();
		mend = newlabel ();
		condgoto (n->left, mbegin, mend);
		output (ASM_DEFLAB, mbegin);
		operator (n->right);
		output (ASM_DEFLAB, mend);
		break;
	case OP_IFELSE:
		mbegin = newlabel ();
		melse = newlabel ();
		mend = newlabel ();
		condgoto (n->left, mbegin, melse);
		output (ASM_DEFLAB, mbegin);
		operator (n->right->left);
		printgoto (mend, -1);
		output (ASM_DEFLAB, melse);
		operator (n->right->right);
		output (ASM_DEFLAB, mend);
		break;

	case OP_WHILE:
		mbegin = newlabel ();
		mcont = newlabel ();
		mend = newlabel ();
		pushloop (mcont, mend);
		output (ASM_DEFLAB, mcont);
		if (n->left) {
			condgoto (n->left, mbegin, mend);
			output (ASM_DEFLAB, mbegin);
		}
		operator (n->right);
		printgoto (mcont, -1);
		output (ASM_DEFLAB, mend);
		poploop ();
		break;
	case OP_DOUNTIL:
		mbegin = newlabel ();
		mcont = newlabel ();
		mend = newlabel ();
		pushloop (mcont, mend);
		output (ASM_DEFLAB, mbegin);
		operator (n->left);
		output (ASM_DEFLAB, mcont);
		condgoto (n->right, mend, mbegin);
		output (ASM_DEFLAB, mend);
		poploop ();
		break;
	case OP_FOR:
		mbegin = newlabel ();
		mcont = newlabel ();
		mbody = newlabel ();
		mend = newlabel ();
		pushloop (mcont, mend);
		output (ASM_DEFLAB, mbegin);
		if (n->left) {
#if 1
			condgoto (n->left, mbody, mend);
			output (ASM_DEFLAB, mbody);
#else
			expression (n->left, TARG_COND);
			printgoto (mend, 0);
#endif
		}
		operator (n->right->right);
		output (ASM_DEFLAB, mcont);
		operator (n->right->left);
		printgoto (mbegin, -1);
		output (ASM_DEFLAB, mend);
		poploop ();
		break;

	case OP_BREAK:
		printgoto (loopbreak (), -1);
		break;
	case OP_CONTINUE:
		printgoto (loopcont (), -1);
		break;

	case OP_SWITCH:
	case OP_CASE:
	case OP_DEFAULT:
		error ("operator not implemented yet");
		nprint (n, 1);
		break;
	}
}

node_t *precrement (node_t *n)
{
	node_t *l;

	switch (n->op) {
	default:     return n;
	case OP_INC: break;
	case OP_DEC: break;
	}
	l = n->left;
	exprincr (l, n->op == OP_DEC);
	nfree (n);
	return l;
}

node_t *postcrement (node_t *n)
{
	node_t *l;

	switch (n->op) {
	default:         return n;
	case OP_POSTINC: break;
	case OP_POSTDEC: break;
	}
	l = n->left;
	if (ndelayed >= MAXDELAYED)
		error ("too many post-increment operations");
	else
		delaytab [ndelayed++] = tcopy (n);
	nfree (n);
	return l;
}

void expression (node_t *n, int target)
{
	if (! n)
		return;
	n = machexpr (n, target);
	if (! n)
		return;

	if (n->op == OP_COMMA) {
		expression (n->left, TARG_EFF);
		expression (n->right, target);
		return;
	}

	/* Compile all pre-increment and pre-decrement operations. */
	n = twalk (n, precrement);

	/* Delay all post-increment and post-decrement operations. */
	ndelayed = 0;
	n = twalk (n, postcrement);

	expr2 (n, target);

	while (ndelayed) {
		n = delaytab [--ndelayed];
		switch (n->op) {
		case OP_POSTINC: exprincr (n->left, 0); break;
		case OP_POSTDEC: exprincr (n->left, 1); break;
		}
		twalk (n, nfree);
	}
}

void call (node_t *name, node_t *a)
{
	node_t *a1, *a2;

	if (! a) {
		/* No arguments. */
		a1 = 0;
		a2 = 0;
	} else if (a->op == OP_COMMA) {
		/* Two arguments. */
		a1 = a->left;
		a2 = a->right;
		if (tsize (a1->type) + tsize (a2->type) > MAXARGLEN) {
			error ("too long arguments");
			return;
		}
	} else {
		/* One argument. */
		a1 = a;
		a2 = 0;
	}
	if (a2) {
		if (badarg1 (a1)) {
			error ("too complex argument, simplify");
			return;
		}
		a2 = macharg2 (a2);
		if (a2) {
			expr2 (a2, TARG_ACC);
			pusharg (tsize (a2->type));
		}
	}
	if (a1)
		expr2 (a1, TARG_ACC);
	output (ASM_CALL, name);
}

/*
 * Reduce constant sub-expressions.
 */
node_t *canonconst (node_t *n)
{
	node_t *q;

	if (! n || LEAF (n->op))
		return n;
	n->left = canonconst (n->left);
	if (BINARY (n->op))
		n->right = canonconst (n->right);
	if (! n->left || n->left->op != OP_CONST)
		return n;

	if (n->op == OP_QUEST) {
		if (n->left->lval || n->left->rval) {
			q = n->right->left;
			twalk (n->right->right, nfree);
		} else {
			q = n->right->right;
			twalk (n->right->left, nfree);
		}
		nfree (n->left);
		nfree (n->right);
		nfree (n);
		return q;
	} else if (n->op == OP_ANDAND) {
		if (n->left->lval || n->left->rval) {
			q = n->right;
			nfree (n->left);
		} else {
			q = n->left;
			twalk (n->right, nfree);
		}
		nfree (n);
		return q;
	} else if (n->op == OP_OROR) {
		if (n->left->lval || n->left->rval) {
			q = n->left;
			twalk (n->right, nfree);
		} else {
			q = n->right;
			nfree (n->left);
		}
		nfree (n);
		return q;
	} else if (! BINARY (n->op)) {
		if (n->left->rval)
			return n;
		switch (n->op) {
		case OP_NEG:
			q = n->left;
			nfree (n);
			q->lval = - q->lval;
			return q;
		case OP_NOT:
			q = n->left;
			nfree (n);
			q->lval = ! q->lval;
			q->type = TCHAR;
			return q;
		case OP_COMPL:
			q = n->left;
			nfree (n);
			q->lval = ~ q->lval;
			return q;
		case OP_CAST:
			q = n->left;
			q->type = n->type;
			nfree (n);
			return q;
		}
	} else if (n->right && n->right->op == OP_CONST) {
		unsigned long r = n->right->lval;
		q = n->left;

		if (n->right->rval) {
			if (q->rval || n->op != OP_ADD)
				return n;
			q->rval = n->right->rval;
			q->lval += n->right->lval;
			goto done;
		} else if (q->rval) {
			if (n->op != OP_ADD)
				return n;
			q->lval += n->right->lval;
			goto done;
		}
		switch (n->op) {
		case OP_EQU:    q->lval = q->lval == r; goto done;
		case OP_NEQ:    q->lval = q->lval != r; goto done;
		case OP_LT:     q->lval = q->lval < r;  goto done;
		case OP_GT:     q->lval = q->lval > r;  goto done;
		case OP_LE:     q->lval = q->lval <= r; goto done;
		case OP_GE:     q->lval = q->lval >= r; goto done;
		case OP_ADD:    q->lval += r;           goto done;
		case OP_SUB:    q->lval -= r;           goto done;
		case OP_LSHIFT: q->lval <<= r;          goto done;
		case OP_RSHIFT: q->lval >>= r;          goto done;
		case OP_MUL:    q->lval *= r;           goto done;
		case OP_AND:    q->lval &= r;           goto done;
		case OP_OR:     q->lval |= r;           goto done;
		case OP_XOR:    q->lval ^= r;           goto done;
		case OP_DIV:
			if (! r) {
				error ("division by zero");
				r = 1;
			}
			q->lval /= r;
			goto done;
		case OP_MOD:
			if (! r) {
				error ("division by zero");
				r = 1;
			}
			q->lval %= r;
done:                   nfree (n->right);
			nfree (n);
#if 1
			if (q->lval < 0x100)
				q->type = TCHAR;
			else if (q->lval < 0x10000)
				q->type = TSHORT;
			else
				q->type = TLONG;
#endif
			return q;
		}
	}
	return n;
}

void expr2 (node_t *n, int target)
{
	n = machexpr (n, target);
	if (! n)
		return;

#if 0
	printf ("#EXPR ");
	switch (target) {
	case TARG_ACC:  printf ("ACC");  break;
	case TARG_COND: printf ("COND"); break;
	case TARG_EFF:  printf ("EFF");  break;
	}
	printf ("\n");
	nprint (n, 1);
#endif
	switch (n->op) {
	case OP_DIVASG:         /* /= */
		if (n->right->op == OP_CONST) {
			divasg_by_const (n->left, n->right);
			return;
		}
		/* fall through... */
	case OP_ASSIGN:         /* = */
	case OP_ADDASG:         /* += */
	case OP_SUBASG:         /* -= */
	case OP_MULASG:         /* *= */
	case OP_MODASG:         /* %= */
	case OP_LSASG:          /* <<= */
	case OP_RSASG:          /* >>= */
	case OP_ANDASG:         /* &= */
	case OP_ORASG:          /* |= */
	case OP_XORASG:         /* ^= */
		expr2 (n->right, TARG_ACC);
		cast (tsize (n->left->type), tsize (n->right->type));
		store (n->left, n->op);
		return;
	}
	expr3 (n, target);
}

void expr3 (node_t *n, int target)
{
	int mtrue, mfalse, mend;

	switch (n->op) {
	/*
	 * Leaves.
	 */
	case OP_CONST:          /* integer constant */
		if (target != TARG_EFF)
			loadconst (n);
		break;
	case OP_NAME:           /* variable name, stab[rval] */
		if (target != TARG_EFF)
			loadname (n);
		if (target == TARG_COND)
			makecond (tsize (n->type));
		break;
	case OP_STRING:         /* lvar=string, don't forget to free */
		if (target != TARG_EFF)
			loadstring ((char*) n->lval);
		break;

	/*
	 * Unary ops.
	 */
	case OP_NOT:            /* logical negate op (!x) */
	case OP_ADDR:           /* address op (&x) */
	case OP_REF:            /* de-reference op (*x) */
	case OP_NEG:            /* negate op (-x) */
	case OP_COMPL:          /* logical complement op (~x) */
	case OP_CAST:           /* type cast op */
		if (target == TARG_EFF) {
			expr2 (n->left, TARG_EFF);
			break;
		}
		expr2 (n->left, TARG_ACC);
		printunary (n->op, n->type, n->left->type);
		break;

	/*
	 * Binary ops.
	 */
	case OP_CALL:           /* f(x) */
		call (n->left, n->right);
		if (target == TARG_COND)
			output (ASM_COND);
		return;

	case OP_ANDAND:         /* && */
	case OP_OROR:           /* || */
		mtrue = newlabel ();
		mfalse = newlabel ();
		mend = newlabel ();
		condgoto (n, mtrue, mfalse);
		output (ASM_DEFLAB, mtrue);
		output (ASM_TRUE);
		output (ASM_GOTOLAB, mend);
		output (ASM_DEFLAB, mfalse);
		output (ASM_FALSE);
		output (ASM_DEFLAB, mend);
		return;

	case OP_QUEST:
		mtrue = newlabel ();
		mfalse = newlabel ();
		mend = newlabel ();
		condgoto (n->left, mtrue, mfalse);
		output (ASM_DEFLAB, mtrue);
		expr2 (n->right->left, TARG_ACC);
		printgoto (mend, -1);
		output (ASM_DEFLAB, mfalse);
		expr2 (n->right->right, TARG_ACC);
		output (ASM_DEFLAB, mend);
		return;

	case OP_COLON:
		error ("internal error: COLON op");
		return;

	case OP_DOT:
		error ("bits are not addressable");
		return;

	case OP_EQU:            /* == */
	case OP_NEQ:            /* != */
	case OP_LT:             /* < */
	case OP_GT:             /* > */
	case OP_LE:             /* <= */
	case OP_GE:             /* >= */
	case OP_ADD:            /* + */
	case OP_SUB:            /* - */
	case OP_MUL:            /* * */
	case OP_DIV:            /* / */
	case OP_MOD:            /* % */
	case OP_AND:            /* & */
	case OP_OR:             /* | */
	case OP_XOR:            /* ^ */
	case OP_LSHIFT:         /* << */
	case OP_RSHIFT:         /* >> */
		if (target == TARG_EFF) {
			expr3 (n->left, TARG_EFF);
			expr3 (n->right, TARG_EFF);
		} else
			machop (n, target);
		return;
	}
}
