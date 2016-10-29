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
#include <stdio.h>
#include "global.h"

void header (void)
{
	output ("A0 .equ 0x0a #`WREG;");
	output ("A1 .equ 9 #`FSR1;");
	output ("A2 .equ 0x0d #`TBLPTRL;");
	output ("A3 .equ 0x0e #`TBLPTRH;");
	output ("A4 .data 9;");
	output ("A5 .equ A4+1;");
	output ("A6 .equ A4+2;");
	output ("A7 .equ A4+3;");
	output ("A8 .equ A4+4;");
	output ("A9 .equ A4+5;");
	output ("A10 .equ A4+6;");
	output ("A11 .equ A4+7;");
	output ("A12 .equ A4+8;");
	output ("TBLPTRL .equ 0x0d;");
	output ("TBLPTRH .equ 0x0e;");
	output ("PRODL .equ 0x18;");
	output ("PRODH .equ 0x19;");
	output ("FSR0 .equ 1;");
	output ("INDF0 .equ 0;");
	output ("INDF1 .equ 8;");
}

/*
 * Compute the size of the type in bytes.
 */
int tsize (int type)
{
	if (type & TFAR)
		return 2;               /* far pointers are two bytes long */
	if (type & TPTR)
		return 1;               /* near pointers are one-byte */
	switch (type & TMASK) {
	default:     return 0;
	case TVOID:  return 0;          /* void - nothing */
	case TCHAR:  return 1;          /* char - 1 byte */
	case TSHORT: return 2;          /* short - 2 bytes */
	case TLONG:  return 4;          /* long - 4 bytes */
	}
}

/*
 * Output the assembler name of the symbol.
 * For local symbols the name of the function is appended.
 */
void printname (node_t *n)
{
	if (n->rval) {
		sym_t *s = stab + n->rval;
		if (s->local)
			printf ("%s", func->name);
		printf ("_%s", s->name);
		if (n->lval)
			printf ("+%ld", n->lval);
	} else
		printf ("%ld", n->lval);
}

/*
 * Output the declaration of the symbol.
 */
void declvar (node_t *n)
{
	sym_t *s = stab + n->rval;

	if (s->addr)
		output ("%n .equ %D #`", n, s->addr - 1);
	else {
		int size = tsize (s->type);
		if (s->size)
			size *= s->size;
		output ("%n .data %d #`", n, size);
	}
	nprint (n, 0);
}

/*
 * Output the declaration of the function.
 */
void declfun (node_t *n)
{
	sym_t *s = stab + n->rval;

	if (s->addr)
		output (" .org %D;", s->addr - 1);
	output ("%n:   #`", n);
	nprint (n, 0);
}

/*
 * Initialize the argument.
 */
int initarg (node_t *n, int off)
{
	switch (tsize (stab[n->rval].type)) {
	default:
		error ("bad argument type");
		return 0;
	case 1:
		output (" rtx A%d,%n;", off, n);
		return 2;
	case 2:
		output (" rtx A%d,%n;", off, n);
		output (" rtx A%d,%n+1;", off+1, n);
		return 2;
	case 4:
		output (" rtx A%d,%n;", off, n);
		output (" rtx A%d,%n+1;", off+1, n);
		output (" rtx A%d,%n+2;", off+2, n);
		output (" rtx A%d,%n+3;", off+3, n);
		return 4;
	}
}

/*
 * Initialize arguments.
 */
void initargs (node_t *n)
{
	int off = 0;

	while (n->op == OP_COMMA) {
		node_t *r = n->right;
		if (n->left->op == OP_NAME)
			off += initarg (n->left, off);
		n = r;
	}
	if (n->op == OP_NAME)
		off += initarg (n, off);
	if (off > 4)
		error ("too many arguments");
}

int is_bitmask (long mask)
{
	switch (mask) {
	case 1: case 2: case 4: case 8:
	case 16: case 32: case 64: case 128:
		return 1;
	}
	return 0;
}

int bitmask_to_bitnum (int mask)
{
	switch (mask) {
	case 1:   return 0;
	case 2:	  return 1;
	case 4:   return 2;
	case 8:   return 3;
	case 16:  return 4;
	case 32:  return 5;
	case 64:  return 6;
	case 128: return 7;
	}
	return 0;
}

/*
 * Detect if the operator will be compiled in 1-command expression.
 * Needed for optimizing conditional jumps.
 */
int operword (node_t *n)
{
	if (n->op == OP_ASSIGN && n->left->op == OP_NAME &&
	    n->right->op == OP_NAME) {
		sym_t *l, *r;

		l = stab + n->left->rval;
		r = stab + n->right->rval;
		if (tsize (l->type) == 1 && tsize (r->type) == 1 &&
		    (l->addr || r->addr) &&
		    ((l->addr && l->addr < 0x20) || r->addr < 0x20))
			return 1;
	}
	if (n->op == OP_ASSIGN && n->left->op == OP_NAME &&
	    n->right->op == OP_CONST && ! n->right->lval && ! n->right->rval &&
	    tsize (stab[n->left->rval].type) == 1)
		return 1;
	if (n->op == OP_ASSIGN && n->left->op == OP_NAME &&
	    tsize (stab[n->left->rval].type) == 1 && ! n->right->rval &&
	    n->right->op == OP_CONST && n->right->lval == 0xff)
		return 1;
	if (n->op == OP_ASSIGN && n->left->op == OP_DOT)
		return 1;
	if ((n->op == OP_ORASG || n->op == OP_XORASG || n->op == OP_ANDASG) &&
	    n->left->op == OP_NAME && n->right->op == OP_CONST &&
	    tsize (stab[n->left->rval].type) == 1 &&
	    !n->right->rval && is_bitmask (n->right->lval))
		return 1;
	if (n->op == OP_XORASG && n->left->op == OP_DOT)
		return 1;
	if (n->op == OP_RETVAL && n->left->op == OP_CONST &&
	    tsize (n->left->type) == 1)
		return 1;

	if (n->op == OP_GOTO || n->op == OP_RETURN ||
	    n->op == OP_BREAK || n->op == OP_CONTINUE)
		return 1;
	if (n->op == OP_CALL && ! n->right)
		return 1;
	return 0;
}

/*
 * Try the machine-dependent compilation of the conditional expression.
 * Assume the one-command operator follows.
 * Make the output and return true if successful,
 * otherwise output nothing and return false.
 */
int machif (node_t *n, int trueflag)
{
	/* ! expr */
	while (n->op == OP_NOT) {
		trueflag = !trueflag;
		n = n->left;
	}
	/* var.bit */
	if (n->op == OP_DOT) {
		output (" b%c? %n,%D;", trueflag ? 's' : 'z', n->left,
			n->right->lval & 7);
		return 1;
	}
	/* var & bitmask */
	if (n->op == OP_AND && n->left->op == OP_NAME &&
	    n->right->op == OP_CONST && ! n->right->rval &&
	    is_bitmask (n->right->lval)) {
		output (" b%c? %n,%D;", trueflag ? 's' : 'z', n->left,
			bitmask_to_bitnum (n->right->lval));
		return 1;
	}
	/* var */
	if (n->op == OP_NAME && tsize (n->type) == 1 && trueflag) {
		output (" x? %n;", n);
		return 1;
	}
	/* ! var */
	if (n->op == OP_NAME && tsize (n->type) == 1 && ! trueflag) {
		output (" az; x<=a? %n;", n);
		return 1;
	}
	/* var != 0 */
	if (n->op == OP_NEQ && n->left->op == OP_NAME &&
	    tsize (n->left->type) == 1 && trueflag && n->right->op == OP_CONST &&
	    ! n->right->rval && ! n->right->lval) {
		output (" x? %n;", n->left);
		return 1;
	}
	/* var == 0 */
	if (n->op == OP_EQU && n->left->op == OP_NAME &&
	    tsize (n->left->type) == 1 && n->right->op == OP_CONST &&
	    ! n->right->rval && ! n->right->lval) {
		if (trueflag) output (" az; x<=a? %n;", n->left);
		else	      output (" x? %n;", n->left);
		return 1;
	}
	/* expr ==/!= var/const */
	if ((n->op == OP_EQU || n->op == OP_NEQ) &&
	    (n->right->op == OP_NAME || n->right->op == OP_CONST) &&
	    tsize (n->left->type) == 1 && tsize (n->right->type) == 1) {
		if (n->op == OP_NEQ)
			trueflag = !trueflag;
		expression (n->left, TARG_ACC);
		if (n->right->op == OP_NAME) output (" a^x %n;", n->right);
		else			     output (" a^c %n;", n->right);
		if (trueflag) output (" z?;");
		else          output (" a?;");
		return 1;
	}
	/* var/const ==/!= expr */
	if ((n->op == OP_EQU || n->op == OP_NEQ) &&
	    (n->left->op == OP_NAME || n->left->op == OP_CONST) &&
	    tsize (n->left->type) == 1 && tsize (n->right->type) == 1) {
		if (n->op == OP_NEQ)
			trueflag = !trueflag;
		expression (n->right, TARG_ACC);
		if (n->left->op == OP_NAME) output (" a^x %n;", n->right);
		else			    output (" a^c %n;", n->right);
		if (trueflag) output (" z?;");
		else          output (" a?;");
		return 1;
	}
	/* expr <=/> var/const */
	if ((n->op == OP_LE || n->op == OP_GT) &&
	    (n->right->op == OP_NAME || n->right->op == OP_CONST) &&
	    tsize (n->left->type) == 1 && tsize (n->right->type) == 1) {
		if (n->op == OP_GT)
			trueflag = !trueflag;
		expression (n->left, TARG_ACC);
		if (n->right->op == OP_NAME) output (" a-x %n;", n->right);
		else			     output (" c-a %n;", n->right);
		if (trueflag) output (" c?;");
		else          output (" b?;");
		return 1;
	}
	/* var/const >=/< expr */
	if ((n->op == OP_GE || n->op == OP_LT) &&
	    (n->left->op == OP_NAME || n->left->op == OP_CONST) &&
	    tsize (n->left->type) == 1 && tsize (n->right->type) == 1) {
		if (n->op == OP_LT)
			trueflag = !trueflag;
		expression (n->right, TARG_ACC);
		if (n->left->op == OP_NAME) output (" a-x %n;", n->left);
		else			    output (" c-a %n;", n->left);
		if (trueflag) output (" c?;");
		else          output (" b?;");
		return 1;
	}
	/* expr & var/const */
	if (n->op == OP_AND && tsize (n->right->type) == 1 &&
	    (n->right->op == OP_NAME || n->right->op == OP_CONST)) {
		expression (n->left, TARG_ACC);
		output (" a&%c %n; %c?;", n->right->op == OP_NAME ? 'x' : 'c',
			n->right, trueflag ? 'a' : 'z');
		return 1;
	}
	/* var/const & expr */
	if (n->op == OP_AND && tsize (n->left->type) == 1 &&
	    (n->left->op == OP_NAME || n->left->op == OP_CONST)) {
		expression (n->right, TARG_ACC);
		output (" a&%c %n; %c?;", n->left->op == OP_NAME ? 'x' : 'c',
			n->left, trueflag ? 'a' : 'z');
		return 1;
	}
	/* (expr & var/const) ==/!= var/const */
	if ((n->op == OP_EQU || n->op == OP_NEQ) && n->left->op == OP_AND &&
	    (n->right->op == OP_NAME || n->right->op == OP_CONST) &&
	    (n->left->right->op == OP_NAME || n->left->right->op == OP_CONST) &&
	    tsize (n->left->right->type) == 1 && tsize (n->right->type) == 1) {
		if (n->op == OP_NEQ)
			trueflag = !trueflag;
		expression (n->left->left, TARG_ACC);
		output (" a&%c %n;", n->left->right->op == OP_NAME ? 'x' : 'c',
			n->left->right);
		output (" a^%c %n;", n->right->op == OP_NAME ? 'x' : 'c',
			n->right);
		if (trueflag) output (" z?;");
		else          output (" a?;");
		return 1;
	}

	/* SHORT/LONG var/const >/<= SHORT/LONG var/const */
	if ((n->op == OP_GT || n->op == OP_LE ||
	     n->op == OP_LT || n->op == OP_GE) &&
	    (n->left->op == OP_NAME || n->left->op == OP_CONST) &&
	    (n->right->op == OP_NAME || n->right->op == OP_CONST)) {
		if (n->op == OP_LE) {
			trueflag = !trueflag;
			n->op = OP_GT;
		}
		if (n->op == OP_GE) {
			trueflag = !trueflag;
			n->op = OP_LT;
		}
		machop (n, TARG_ACC);
		if (trueflag) output (" a?;");
		else          output (" a|c 0; z?;");
		return 1;
	}

	/* This mist be the last entry here! */
	/* expr */
	if (n->op != OP_OROR && n->op != OP_ANDAND && tsize (n->type) == 1) {
		if (trueflag) {
			expression (n, TARG_ACC);
			output (" a?;");
		} else {
			expression (n, TARG_COND);
			output (" z?;");
		}
		return 1;
	}
	return 0;
}

/*
 * Try to make the machine-dependent compilation of the operator.
 */
node_t *machoper (node_t *n)
{
	/*
	 * Register to memory and memory to register movs.
	 */
	if (n->op == OP_ASSIGN && n->left->op == OP_NAME &&
	    n->right->op == OP_NAME) {
		sym_t *l, *r;

		l = stab + n->left->rval;
		r = stab + n->right->rval;
		if (tsize (l->type) == 1 && tsize (r->type) == 1 &&
		    (l->addr || r->addr)) {
			if (l->addr == WREG+1) {
				output (" xta %n;", n->right);
				return 0;
			} else if (l->addr && l->addr < 0x20) {
				output (" xtr %n,%n;", n->right, n->left);
				return 0;
			} else if (r->addr == WREG+1) {
				output (" atx %n;", n->left);
				return 0;
			} else if (r->addr < 0x20) {
				output (" rtx %n,%n;", n->right, n->left);
				return 0;
			}
		}
	}

	/*
	 * Zero assignment.
	 */
	if (n->op == OP_ASSIGN && n->left->op == OP_NAME &&
	    n->right->op == OP_CONST && ! n->right->lval && ! n->right->rval) {
		switch (tsize (stab[n->left->rval].type)) {
		case 4:  output (" xz %n+3; xz %n+2;", n->left, n->left);
		case 2:  output (" xz %n+1;", n->left);
		default: output (" xz %n;", n->left);
		}
		return 0;
	}

	/*
	 * 0xFF assignment.
	 */
	if (n->op == OP_ASSIGN && n->left->op == OP_NAME &&
	    tsize (stab[n->left->rval].type) == 1 && ! n->right->rval &&
	    n->right->op == OP_CONST && n->right->lval == 0xff) {
		output (" xs %n;", n->left);
		return 0;
	}

	/*
	 * Left shift assignment of 1.
	 */
	if (n->op == OP_LSASG && n->left->op == OP_NAME &&
	    n->right->op == OP_CONST && !n->right->rval && n->right->lval==1) {
		output (" bz 4,0;");
		switch (tsize (stab[n->left->rval].type)) {
		case 4:  output (" xc<<x %n; xc<<x %n+1; xc<<x %n+2; xc<<x %n+3;",
				n->left, n->left, n->left, n->left); break;
		case 2:  output (" xc<<x %n; xc<<x %n+1;", n->left, n->left); break;
		default: output (" xc<<x %n;", n->left); break;
		}
		return 0;
	}

	/*
	 * Right shift assignment of 1.
	 */
	if (n->op == OP_RSASG && n->left->op == OP_NAME &&
	    n->right->op == OP_CONST && !n->right->rval && n->right->lval==1) {
		output (" bz 4,0;");
		switch (tsize (stab[n->left->rval].type)) {
		case 4:  output (" xc>>x %n+3; xc>>x %n+2;", n->left, n->left);
		case 2:  output (" xc>>x %n+1; xc>>x %n;", n->left, n->left); break;
		default: output (" xc>>x %n;", n->left); break;
		}
		return 0;
	}

	/*
	 * Short/long constant assignment.
	 */
	if (n->op == OP_ASSIGN && n->left->op == OP_NAME &&
	    n->right->op == OP_CONST && ! n->right->rval) {
		switch (tsize (stab[n->left->rval].type)) {
		case 2:
			output (" cta %d; atx %n+1;", n->right->lval >> 8 & 0xff, n->left);
			output (" cta %d; atx %n;", n->right->lval & 0xff, n->left);
			return 0;
		case 4:
			output (" cta %d; atx %n+3;", n->right->lval >> 24 & 0xff, n->left);
			output (" cta %d; atx %n+2;", n->right->lval >> 16 & 0xff, n->left);
			output (" cta %d; atx %n+1;", n->right->lval >> 8 & 0xff, n->left);
			output (" cta %d; atx %n;", n->right->lval & 0xff, n->left);
			return 0;
		}
	}

	/*
	 * Bit assignment.
	 */
	if (n->op == OP_ASSIGN && n->left->op == OP_DOT) {
		if (n->right->op != OP_CONST || n->right->lval > 1) {
			error ("illegal bit value");
			return 0;
		}
		output (" b%c %n,%D;", n->right->lval ? 's' : 'z',
			n->left->left, n->left->right->lval & 7);
		return 0;
	}
	if ((n->op == OP_ORASG || n->op == OP_XORASG || n->op == OP_ANDASG) &&
	    n->left->op == OP_NAME && n->right->op == OP_CONST &&
	    tsize (stab[n->left->rval].type) == 1 &&
	    !n->right->rval && is_bitmask (n->right->lval)) {
		output (" b%c %n,%D;", n->op==OP_ORASG ? 's' :
			n->op==OP_XORASG ? 't' : 'z', n->left,
			bitmask_to_bitnum (n->right->lval));
		return 0;
	}

	/*
	 * Bit toggle.
	 */
	if (n->op == OP_XORASG && n->left->op == OP_DOT) {
		if (n->right->op != OP_CONST || n->right->lval != 1) {
			error ("illegal bit toggle");
			return 0;
		}
		output (" bt %n,%D;", n->left->left,
			n->left->right->lval & 7);
		return 0;
	}

	/*
	 * Return constant.
	 */
	if (n->op == OP_RETVAL && n->left->op == OP_CONST &&
	    tsize (n->left->type) == 1) {
		output (" retc %n;", n->left);
		return 0;
	}

	/*
	 * One-operand IF statement.
	 */
	if (n->op == OP_IF && operword (n->right) && machif (n->left, 1)) {
		operator (n->right);
		return 0;
	}
	return n;
}

/*
 * Machine-dependent conditional expression.
 * Make jumps to true and false labels given.
 * Skip the final "goto true" operator.
 */
node_t *machcond (node_t *n, int true, int false)
{
	if (machif (n, 0)) {
		printgoto (false, -1);
		return 0;
	}
	return n;
}

/*
 * Try to make the machine-dependent compilation of the expression.
 */
node_t *machexpr (node_t *n, int target)
{
	switch (n->op) {
	case OP_NOT:
		if (n->left->op == OP_DOT) {
			output (" cta 0; bz? %n,%D; cta 1;", n->left->left,
				n->left->right->lval & 7);
			return 0;
		}
		break;

	case OP_DOT:
		output (" cta 0; bs? %n,%D; cta 1;",
			n->left, n->right->lval & 7);
		return 0;
	}
	return n;
}

/*
 * Compile the assignment and assignment operations
 * when the left argument is a pointer reference:
 *
 *      *N OP= WREG
 *
 * N is the left hand side, OP is the operation.
 * The value is on the accumulator.
 */
void storeref (node_t *n, int op)
{
	output (" xtr %n,FSR0;", n);

	switch (tsize (stab[n->rval].type)) {
	default:
	case 0:
		error ("invalid pointer type at left hand side of the assignment");
		return;
	case 1:
		switch (op) {
		default:        error ("internal assignment error");
		case OP_ASSIGN: output (" atx INDF0;");   break;
		case OP_ADDASG: output (" x+a INDF0;");   break;
		case OP_SUBASG: output (" x-a INDF0;");   break;
		case OP_ANDASG: output (" x&a INDF0;");   break;
		case OP_ORASG:  output (" x|a INDF0;");   break;
		case OP_XORASG: output (" x^a INDF0;");   break;
		case OP_LSASG:  output (" call ls1p;");   break;
		case OP_RSASG:  output (" call rs1p;");   break;
		case OP_MODASG: output (" call divmod11p; xtr A3,INDF0;"); break;
		case OP_MULASG: output (" a*x INDF0; xtr PRODL,INDF0;"); break;
		case OP_DIVASG:	output (" call divmod11p;"); break;
		}
		break;
	case 2:
		switch (op) {
		case OP_ASSIGN:
			output (" atx INDF0; x++ FSR0; xtr A1,INDF0;");
			break;
		case OP_MULASG:
		case OP_DIVASG:
		case OP_MODASG:
		case OP_ADDASG:
		case OP_SUBASG:
		case OP_LSASG:
		case OP_RSASG:
		case OP_ANDASG:
		case OP_ORASG:
		case OP_XORASG:
		default:
			error ("short pointer assignments not implemented yet");
			return;
		}
		break;
	case 4:
		switch (op) {
		case OP_ASSIGN:
			output (" atx INDF0;");
			output (" x++ FSR0; xtr A1,INDF0;");
			output (" x++ FSR0; xtr A2,INDF0;");
			output (" x++ FSR0; xtr A3,INDF0;");
			break;
		case OP_MULASG:
		case OP_DIVASG:
		case OP_MODASG:
		case OP_ADDASG:
		case OP_SUBASG:
		case OP_LSASG:
		case OP_RSASG:
		case OP_ANDASG:
		case OP_ORASG:
		case OP_XORASG:
		default:
			error ("long pointer assignments not implemented yet");
			return;
		}
		break;
	}
}

/*
 * Compile the assignment-divide by constant.
 * `l' is the left hand side, `r' - the right hand side.
 */
void divasg_by_const (node_t *l, node_t *r)
{
	if (r->lval <= 1)
		return;
	if (l->op == OP_REF) {
		/* Left hand side is the pointer reference (*ptr := val).
		 * It must be the near pointer. */
		if (l->left->op != OP_NAME ||
		    ! (stab[l->left->rval].type & TPTR) ||
		    (stab[l->left->rval].type & TFAR)) {
			error ("bad pointer at left hand side of the assignment");
			return;
		}
		output (" xtr %n,FSR0;", l->left);
		switch (tsize (stab[l->left->rval].type)) {
		default:
		case 0:
			error ("invalid pointer type at left hand side of the div-assignment");
			return;
		case 1:
			output (" xta INDF0;");
			if (is_bitmask (r->lval)) {
				output (" a*c %d; rtx PRODH,INDF0;", 256/r->lval);
				break;
			}
			output (" atx A1; a*c %d; cta %d; call divcorr1; atx INDF0;",
				1+256/r->lval, r->lval);
			break;
		case 2:
			error ("short pointer div-assignment not implemented yet");
			break;
		case 4:
			error ("long pointer div-assignment not implemented yet");
			break;
		}
		return;
	}
	if (l->op != OP_NAME) {
		error ("invalid left hand side of the div-assignment");
		return;
	}
	switch (tsize (stab[l->rval].type)) {
	default:
	case 0:
		error ("invalid type of left hand side of the div-assignment");
		break;
	case 1:
		output (" xta %n;", l);
		if (is_bitmask (r->lval)) {
			output (" a*c %d; rtx PRODH,%n;", 256/r->lval, l);
			break;
		}
		output (" atx A1; a*c %d; cta %d; call divcorr1; atx %n;",
			1+256/r->lval, r->lval, l);
		break;
	case 2:
		error ("short div-assignment not implemented yet");
		break;
	case 4:
		error ("long div-assignment not implemented yet");
		break;
	}
}

/*
 * Compile the assignment and assignment operations.
 * N is the left hand side, OP is the operation.
 * The value is on the accumulator.
 */
void store (node_t *n, int op)
{
	if (n->op == OP_REF) {
		/* Left hand side is the pointer reference (*ptr := val).
		 * It must be the near pointer. */
		if (n->left->op != OP_NAME ||
		    ! (stab[n->left->rval].type & TPTR) ||
		    (stab[n->left->rval].type & TFAR)) {
			error ("bad pointer at left hand side of the assignment");
			return;
		}
		storeref (n->left, op);
		return;
	}
	if (n->op != OP_NAME) {
		error ("invalid left hand side of the assignment");
		return;
	}

	switch (tsize (stab[n->rval].type)) {
	default:
	case 0:
		error ("invalid type of left hand side of the assignment");
		return;
	case 1:
		switch (op) {
		default:        error ("internal assignment error"); break;
		case OP_ASSIGN: output (" atx %n;", n); break;
		case OP_ADDASG: output (" x+a %n;", n); break;
		case OP_SUBASG: output (" x-a %n;", n); break;
		case OP_ANDASG: output (" x&a %n;", n); break;
		case OP_ORASG:  output (" x|a %n;", n); break;
		case OP_XORASG: output (" x^a %n;", n); break;
		case OP_LSASG:
			output (" xtr %n,A2; call ls1; atx %n;", n, n);
			break;
		case OP_RSASG:
			output (" xtr %n,A2; call rs1; atx %n;", n, n);
			break;
		case OP_MULASG:
			output (" a*x %n; xta PRODL; atx %n;", n, n);
			break;
		case OP_DIVASG:
			output (" atx A2; xtr %n,A1; call divmod11;", n);
			output (" rtx A1,%n;", n);
			break;
		case OP_MODASG:
			output (" atx A2; xtr %n,A1; call divmod11;", n);
			output (" rtx A3,%n;", n);
			break;
		}
		break;
	case 2:
		switch (op) {
		case OP_ASSIGN:
			output (" atx %n; rtx A1,%n+1;", n, n);
			break;
		case OP_ANDASG:
			output (" x&a %n; xta A1; x&a %n+1;", n, n);
			break;
		case OP_ORASG:
			output (" x|a %n; xta A1; x|a %n+1;", n, n);
			break;
		case OP_XORASG:
			output (" x^a %n; xta A1; x^a %n+1;", n, n);
			break;
		case OP_LSASG:          /* << */
			output (" xtr %n,A2;", n);
			output (" xtr %n+1,A1;", n);
			output (" call ls2;", n);
			break;
		case OP_RSASG:          /* >> */
			output (" xtr %n,A2;", n);
			output (" xtr %n+1,A1;", n);
			output (" call rs2;", n);
			break;
		case OP_ADDASG:
			output (" x+a %n; xta A1; x+ca %n+1;", n, n);
			break;
		case OP_SUBASG:
			output (" x-a %n; xta A1; x-ba %n+1;", n, n);
			break;
		case OP_MULASG:
		case OP_DIVASG:
		case OP_MODASG:
		default:
			error ("short assignment op %d not implemented yet", op);
			return;
		}
		break;
	case 4:
		switch (op) {
		case OP_ASSIGN:
			output (" atx %n;", n);
			output (" rtx A1,%n+1;", n);
			output (" rtx A2,%n+2;", n);
			output (" rtx A3,%n+3;", n);
			break;
		case OP_LSASG:          /* << */
			output (" atx A5; xta %n; atx A4;", n);
			output (" xtr %n+1,A1;", n);
			output (" xtr %n+2,A2;", n);
			output (" xtr %n+3,A3;", n);
			output (" xta A5; call ls4;");
			output (" atx %n;", n);
			output (" rtx A1,%n+1;", n);
			output (" rtx A2,%n+2;", n);
			output (" rtx A3,%n+3;", n);
			break;
		case OP_RSASG:          /* >> */
			output (" atx A5; xta %n; atx A4;", n);
			output (" xtr %n+1,A1;", n);
			output (" xtr %n+2,A2;", n);
			output (" xtr %n+3,A3;", n);
			output (" xta A5; call rs4;");
			output (" atx %n;", n);
			output (" rtx A1,%n+1;", n);
			output (" rtx A2,%n+2;", n);
			output (" rtx A3,%n+3;", n);
			break;
		case OP_ANDASG:
			output (" x&a %n; xta A1; x&a %n+1;", n, n);
			output (" xta A2; x&a %n+2; xta A3; x&a %n+3;", n, n);
			break;
		case OP_ORASG:
			output (" x|a %n; xta A1; x|a %n+1;", n, n);
			output (" xta A2; x|a %n+2; xta A3; x|a %n+3;", n, n);
			break;
		case OP_XORASG:
			output (" x^a %n; xta A1; x^a %n+1;", n, n);
			output (" xta A2; x^a %n+2; xta A3; x^a %n+3;", n, n);
			break;
		case OP_ADDASG:
			output (" x+a %n; xta A1; x+ca %n+1; xta A2; x+ca %n+2; xta A3; x+ca %n+3;", n, n, n, n);
			break;
		case OP_SUBASG:
			output (" x-a %n; xta A1; x-ba %n+1; xta A2; x-ba %n+2; xta A3; x-ba %n+3;", n, n, n, n);
			break;
		case OP_MULASG:
		case OP_DIVASG:
		case OP_MODASG:
		default:
			error ("long assignments not implemented yet");
			return;
		}
		break;
	}
}

void exprincr (node_t *n, int decr)
{
	if (n->op != OP_NAME) {
		error ("invalid %screment argument", decr ? "de" : "in");
		return;
	}
	switch (tsize (stab[n->rval].type)) {
	default:
		error ("invalid increment/decrement operator");
		break;
	case 1:
		output (" x%s %n;", decr ? "--" : "++", n);
		break;
	case 2:
		if (decr)
			output (" x-- %n; nc?; x-- %n+1;", n, n);
		else
			output (" x++z? %n; x++ %n+1;", n, n);
		break;
	case 4:
		output (" x%s %n;", decr ? "--" : "++", n);
		output (" %sc?; x%s %n+1;", decr ? "n" : "",
			decr ? "--" : "++", n);
		output (" %sc?; x%s %n+2;", decr ? "n" : "",
			decr ? "--" : "++", n);
		output (" %sc?; x%s %n+3;", decr ? "n" : "",
			decr ? "--" : "++", n);
		break;
	}
}

void printgoto (int m, int cond)
{
	if (cond > 0)
		output (" a?;");
	else if (cond == 0)
		output (" z?;");
	output (" goto L%d;", m);
}

void loadconst (node_t *n)
{
	switch (tsize (n->type)) {
	case 1:
		output (" cta %n;", n);
		break;
	case 2:
		output (" cta %D; atx A1;", n->lval >> 8 & 0xff);
		output (" cta %D;", n->lval & 0xff);
		break;
	case 4:
		output (" cta %D; atx A3;", n->lval >> 24 & 0xff);
		output (" cta %D; atx A2;", n->lval >> 16 & 0xff);
		output (" cta %D; atx A1;", n->lval >> 8 & 0xff);
		output (" cta %D;", n->lval & 0xff);
		break;
	default:
		error ("bad constant");
	}
}

void loadname (node_t *n)
{
	switch (tsize (n->type)) {
	case 1:
		output (" xta %n;", n);
		break;
	case 2:
		output (" xtr %n+1,A1; xta %n;", n, n);
		break;
	case 4:
		output (" xtr %n+3,A3;", n);
		output (" xtr %n+2,A2;", n);
		output (" xtr %n+1,A1;", n);
		output (" xta %n;", n);
		break;
	default:
		error ("bad variable");
	}
}

void printstring (char *p)
{
	int delim = '"';

	printf ("%c", delim);
	while (*p) {
		unsigned char c = *p++;

		if (c < ' ' || c == 0177 || c == delim || c == '\\')
			printf ("\\%03o", c);
		else
			printf ("%c", c);
	}
	printf ("%c", delim);
}

void loadstring (char *s)
{
	int m = newlabel ();

	output ("L%d .const ", m);
	for (; *s; ++s) {
		printf ("%d,", (unsigned char) *s);
	}
	printf ("0");
	output ("; cta @L%d; atx A1; cta L%d;", m, m);
}

/*
 * Push the argument 2, freeing the working register.
 */
void pusharg (int sz)
{
	output (" atx A2;");
	if (sz > 1)
		output (" xtr A1,A3;");
}

/*
 * Try to make the machine-dependent compilation of the second argument.
 */
node_t *macharg2 (node_t *n)
{
	if (n->op == OP_NAME) {
		if (tsize (n->type) > 1)
			output (" xtr %n+1,A3;", n);
		output (" xtr %n,A2;", n);
		return 0;
	}
	if (n->op == OP_CONST && !n->rval && !n->lval) {
		if (tsize (n->type) > 1)
			output (" xz A3;");
		output (" xz A2;");
		return 0;
	}
	if (n->op == OP_CONST && !n->rval && tsize (n->type) == 1 &&
	    n->lval == 0xff) {
		output (" xs A2;");
		return 0;
	}
	return n;
}

int badarg1 (node_t *n)
{
	if (! n)
		return 0;
	if (tsize (n->type) > 2)
		return 1;
	if (LEAF (n->op))
		return 0;
	if (badarg1 (n->left))
		return 1;
	if (! BINARY (n->op))
		return 0;
	if (badarg1 (n->right))
		return 1;
	return 0;
}

/*
 * Convert the operand of size ASZ to size SZ.
 */
void cast (int sz, int asz)
{
	if (sz <= asz)
		return;
	if (asz == 1)
		output (" xz A1;");
	if (sz == 4)
		output (" xz A2; xz A3;");
}

/*
 * Print the unary op code.
 */
void printunary (int op, int type, int argtype)
{
	int sz, asz;

	sz = tsize (type);
	asz = tsize (argtype);
	switch (asz) {
	case 1:
		switch (op) {
		case OP_NOT:            /* logical negate op (!x) */
			output (" a?; cta 1; a^c 1;");
			break;
		case OP_COMPL:          /* logical complement op (~x) */
			output (" ac;");
			break;
		case OP_NEG:            /* negate op (-x) */
			output (" anx A0;");
			break;
		case OP_CAST:           /* type cast op */
			/* Nothing to do. */
			break;
		case OP_REF:            /* de-reference op (*x) */
			if (argtype & TFAR) {
				output (" atx TBLPTRL;");
				output (" xtr A1,TBLPTRH;");
				output (" plx A0;");
				output (" llx A0;");
			} else
				output (" atx FSR0; xta INDF0;");
			break;
		case OP_ADDR:           /* address op (&x) */
			error ("internal error: &wreg");
			break;
		}
		break;
	case 2:
		switch (op) {
		case OP_NOT:            /* logical negate op (!x) */
			output (" a|x A1; a?; cta 1; a^c 1;");
			if (sz > 1)
				output (" xz A1;");
			break;
		case OP_COMPL:          /* logical complement op (~x) */
			output (" ac; xc A1;");
			break;
		case OP_NEG:            /* negate op (-x) */
			output (" xc A1; anx A0; c?; x++ A1;");
			break;
		case OP_CAST:           /* type cast op */
			/* Nothing to do. */
			break;
		case OP_REF:            /* de-reference op (*x) */
			if (argtype & TFAR) {
				output (" atx TBLPTRL;");
				output (" xtr A1,TBLPTRH;");
				output (" plx A0;");
				output (" llx A0;");
				if (sz > 1)
					output (" xz A1;");
			} else {
				output (" atx FSR0; xta INDF0;");
				if (sz > 1)
					output (" xz A1;");
			}
			break;
		case OP_ADDR:           /* address op (&x) */
			error ("internal error: &wreg");
			break;
		}
		break;
	case 4:
		switch (op) {
		case OP_NOT:            /* logical negate op (!x) */
			output (" a|x A1; a|x A2; a|x A3; a?; cta 1; a^c 1;");
			if (sz > 1)
				output (" xz A1;");
			break;
		case OP_COMPL:          /* logical complement op (~x) */
			output (" ac; xc A1; xc A2; xc A3;");
			break;
		case OP_NEG:            /* negate op (-x) */
			output (" xc A3; xc A2; xc A1; anx A0; c?; x++ A1; c?; x++ A2; c?; x++ A3;");
			break;
		case OP_CAST:           /* type cast op */
			/* Nothing to do. */
			break;
		case OP_REF:            /* de-reference op (*x) */
			if (argtype & TFAR)
				error ("internal error: *long");
			output (" atx FSR0; xta INDF0;");
			output (" x++ FSR0; xtr INDF0,A1;");
			output (" x++ FSR0; xtr INDF0,A2;");
			output (" x++ FSR0; xtr INDF0,A3;");
			break;
		case OP_ADDR:           /* address op (&x) */
			error ("internal error: &wreg");
			break;
		}
		break;
	}
	cast (sz, asz);
}

/*
 * Print the binary op code.
 */
void printbinary (int op, int type, int ltype, node_t *arg, int target)
{
	int sz, lsz, rsz, i;

	sz = tsize (type);
	lsz = tsize (ltype);
	rsz = tsize (arg->type);
	if (rsz > lsz) {
		cast (rsz, lsz);
		lsz = rsz;
	}
	switch (lsz) {
	case 1:
		switch (op) {
		case OP_SUB:            /* - */
			if (arg->op == OP_NAME)       output (" a-x %n;", arg);
			else if (arg->op == OP_CONST) output (" c-a %n;", arg);
			else error ("bad subtraction");
			break;
		case OP_ADD:            /* + */
			if (arg->op == OP_NAME)       output (" a+x %n;", arg);
			else if (arg->op == OP_CONST) output (" a+c %n;", arg);
			else error ("bad addition");
			break;
		case OP_MUL:            /* * */
			if (arg->op == OP_NAME)       output (" a*x %n; xtr PRODL,A0;", arg);
			else if (arg->op == OP_CONST) output (" a*c %n; xtr PRODL,A0;", arg);
			else error ("bad multiplication");
			break;
		case OP_AND:            /* & */
			if (arg->op == OP_NAME)       output (" a&x %n;", arg);
			else if (arg->op == OP_CONST) output (" a&c %n;", arg);
			else error ("bad logical and");
			break;
		case OP_OR:             /* | */
			if (arg->op == OP_NAME)       output (" a|x %n;", arg);
			else if (arg->op == OP_CONST) output (" a|c %n;", arg);
			else error ("bad logical or");
			break;
		case OP_XOR:            /* ^ */
			if (arg->op == OP_NAME)       output (" a^x %n;", arg);
			else if (arg->op == OP_CONST) output (" a^c %n;", arg);
			else error ("bad logical xor");
			break;
		case OP_EQU:            /* == */
			if (arg->op == OP_NAME)       output (" a^x %n;", arg);
			else if (arg->op == OP_CONST) output (" a^c %n;", arg);
			else error ("bad == op");
			output (" a?; cta 1; a^c 1;");
			break;
		case OP_NEQ:            /* != */
			if (arg->op == OP_NAME)       output (" a^x %n;", arg);
			else if (arg->op == OP_CONST) output (" a^c %n;", arg);
			else error ("bad != op");
			output (" a?; cta 1;");
			break;
		case OP_LE:             /* <= */
			if (arg->op == OP_NAME)       output (" a-x %n;", arg);
			else if (arg->op == OP_CONST) output (" c-a %n;", arg);
			else error ("bad <= op");
			output (" nc?; cta 1; a^c 1;");
			break;
		case OP_GT:             /* > */
			if (arg->op == OP_NAME)       output (" a-x %n;", arg);
			else if (arg->op == OP_CONST) output (" c-a %n;", arg);
			else error ("bad > op");
			output (" c?; cta 1; a^c 1;");
			break;
		case OP_LT:             /* < */
			if (arg->op == OP_NAME)       output (" a-x %n;", arg);
			else if (arg->op == OP_CONST) output (" c-a %n;", arg);
			else error ("bad < op");
			output (" z?; goto 1f; nc?;1: cta 1; a^c 1;");
			break;
		case OP_GE:             /* >= */
			if (arg->op == OP_NAME)       output (" a-x %n;", arg);
			else if (arg->op == OP_CONST) output (" c-a %n;", arg);
			else error ("bad < op");
			output (" z?; goto 1f; c?; cta 1;1: a^c 1;");
			break;
		case OP_DIV:            /* / */
			if (arg->op == OP_CONST) {
				if (arg->lval <= 1)
					break;
				if (is_bitmask (arg->lval)) {
					output (" a*c %d; xta PRODH;", 256/arg->lval);
					break;
				}
				output (" atx A1; a*c %d; cta %d; call divcorr1;",
					1+256/arg->lval, arg->lval);
				break;
			}
			if (arg->op != OP_NAME)
				error ("bad divide");
			output (" atx A1; xtr %n,A2; call divmod11; xtr A1,A0;",
				arg);
			break;
		case OP_MOD:            /* % */
			if (arg->op == OP_CONST) {
				if (is_bitmask (arg->lval)) {
					output (" a&c %d;", arg->lval - 1);
					break;
				}
				output (" atx A1; a*c %d; cta %d; call divcorr1; xta A1;",
					1+256/arg->lval, arg->lval);
				break;
			}
			output (" atx A1;");
			if (arg->op == OP_NAME)       output (" xtr %n,A2;", arg);
			else if (arg->op == OP_CONST) output (" cta %n; atx A2;", arg);
			else error ("bad remainder");
			output (" call divmod11;");
			output (" xtr A3,A0;");
			break;
		case OP_LSHIFT:         /* << */
			if (arg->op == OP_NAME) {
				output (" atx A2; xta %n; call ls1;", arg);
				if (target == TARG_COND)
					output (ASM_COND);
			} else if (arg->op == OP_CONST) {
				i = arg->lval & 7;
				if (i == 1)
					output (" x<<a A0; a&c 0x7f;");
				else if (i == 4)
					output (" aw; a&c 0xf0;");
				else if (i) {
					if (i >= 4) {
						output (" xw A0;");
						i -= 4;
					}
					while (i-- > 0)
						output (" x<<a A0;");
					output (" a&c %d;", (0xff <<
						(arg->lval & 7)) & 0xff);
				}
			} else error ("bad left shift op");
			break;
		case OP_RSHIFT:         /* >> */
			if (arg->op == OP_NAME) {
				output (" atx A2; xta %n; call rs1;", arg);
				if (target == TARG_COND)
					output (ASM_COND);
			} else if (arg->op == OP_CONST) {
				i = arg->lval & 7;
				if (i == 1)
					output (" x>>a A0; a&c 0x7f;");
				else if (i == 4)
					output (" aw; a&c 0x0f;");
				else if (i) {
					if (i >= 4) {
						output (" xw A0;");
						i -= 4;
					}
					while (i-- > 0)
						output (" x>>a A0;");
					output (" a&c %d;", 0xff >>
						(arg->lval & 7));
				}
			} else error ("bad right shift op");
			break;
		}
		break;
	case 2:
		switch (op) {
		case OP_SUB:            /* - */
			error ("short - not implemented yet"); break;
			break;
		case OP_ADD:            /* + */
			if (rsz == 1) {
				/* Add short + char. */
				if (arg->op == OP_NAME)       output (" a+x %n; atx A2;", arg);
				else if (arg->op == OP_CONST) output (" a+c %n; atx A2;", arg);
				else error ("bad add");
				output (" xz A0; x+ca A1;");
			} else {
				if (arg->op == OP_NAME)       output (" a+x %n; atx A2; xta %n+1;", arg, arg);
				else if (arg->op == OP_CONST) output (" a+c %l #`=%n; atx A2; cta %h;", arg, arg, arg);
				else error ("bad add");
				output (" x+ca A1;");
			}
			output (" xta A2;");
			break;
		case OP_AND:            /* & */
			if (arg->op == OP_NAME)       output (" a&x %n;", arg);
			else if (arg->op == OP_CONST) output (" a&c %l #`=%n;", arg, arg);
			else error ("bad short | op");
			if (rsz >= 1) {
				output (" atx A2; xta A1;");
				if (arg->op == OP_NAME)       output (" a&x %n+1;", arg);
				else if (arg->op == OP_CONST) output (" a&c %h;", arg);
				output (" atx A1; xta A2;");
			}
			break;
		case OP_OR:             /* | */
			if (arg->op == OP_NAME)       output (" a|x %n;", arg);
			else if (arg->op == OP_CONST) output (" a|c %l #`=%n;", arg, arg);
			else error ("bad short | op");
			if (rsz >= 1) {
				output (" atx A2; xta A1;");
				if (arg->op == OP_NAME)       output (" a|x %n+1;", arg);
				else if (arg->op == OP_CONST) output (" a|c %h;", arg);
				output (" atx A1; xta A2;");
			}
			break;
		case OP_XOR:            /* ^ */
			if (arg->op == OP_NAME)       output (" a^x %n;", arg);
			else if (arg->op == OP_CONST) output (" a^c %l #`=%n;", arg, arg);
			else error ("bad short | op");
			if (rsz >= 1) {
				output (" atx A2; xta A1;");
				if (arg->op == OP_NAME)       output (" a^x %n+1;", arg);
				else if (arg->op == OP_CONST) output (" a^c %h;", arg);
				output (" atx A1; xta A2;");
			}
			break;
		case OP_LSHIFT:         /* << */
			if (arg->op == OP_NAME) {
				output (" atx A2; xta %n; call ls2;", arg);
			} else if (arg->op == OP_CONST) {
				i = arg->lval & 15;
				if (i == 0)
					/* nothing to do */;
				else if (i == 8)
					output (" atx A1; xz A0;");
				else
					output (" atx A2; cta %n; call ls2;", arg);
			} else error ("bad short left shift op");
			break;
		case OP_RSHIFT:         /* >> */
			if (arg->op == OP_NAME) {
				output (" atx A2; xta %n; call rs2;", arg);
			} else if (arg->op == OP_CONST) {
				i = arg->lval & 15;
				if (i == 0)
					/* nothing to do */;
				else if (i == 8)
					output (" xta A1; xz A1;");
				else
					output (" atx A2; cta %n; call rs2;", arg);
			} else error ("bad short right shift op");
			break;
		case OP_EQU:            /* == */
		case OP_NEQ:            /* != */
			output (" atx A2;");
			if (rsz == 1) {
				/* short == char. */
				if (arg->op == OP_NAME)       output (" xta %n;", arg);
				else if (arg->op == OP_CONST) output (" cta %n;", arg);
				else error ("bad == op");
				output (" xz A3;");
			} else {
				if (arg->op == OP_NAME)       output (" xtr %n,A3; xta %n+1;", arg, arg);
				else if (arg->op == OP_CONST) output (" cta %l #`=%n; atx A3; cta %h;", arg, arg, arg);
				else error ("bad == op");
			}
			output (" call eq22;");
			if (op == OP_NEQ)
				output (" a^c 1;");
			else if (target == TARG_COND)
				output (ASM_COND);
			break;
		case OP_GT:             /* > */
		case OP_LE:             /* <= */
			output (" atx A2;");
			if (rsz == 1) {
				/* short < char. */
				if (arg->op == OP_NAME)       output (" xta %n;", arg);
				else if (arg->op == OP_CONST) output (" cta %n;", arg);
				else error ("bad < op");
				output (" call gt21;");
			} else {
				if (arg->op == OP_NAME)       output (" xtr %n,A3; xta %n+1;", arg, arg);
				else if (arg->op == OP_CONST) output (" cta %l #`=%n; atx A3; cta %h;", arg, arg, arg);
				else error ("bad < op");
				output (" call gt22;");
			}
			if (op == OP_LE)
				output (" a^c 1;");
			else if (target == TARG_COND)
				output (ASM_COND);
			break;
		case OP_LT:             /* < */
		case OP_GE:             /* >= */
			output (" atx A2;");
			if (rsz == 1) {
				/* short < char. */
				if (arg->op == OP_NAME)       output (" xta %n;", arg);
				else if (arg->op == OP_CONST) output (" cta %n;", arg);
				else error ("bad < op");
				output (" call lt21;");
			} else {
				if (arg->op == OP_NAME)       output (" xtr %n,A3; xta %n+1;", arg, arg);
				else if (arg->op == OP_CONST) output (" cta %l #`=%n; atx A3; cta %h;", arg, arg, arg);
				else error ("bad < op");
				output (" call lt22;");
			}
			if (op == OP_GE)
				output (" a^c 1;");
			else if (target == TARG_COND)
				output (ASM_COND);
			break;
		case OP_MUL:            /* * */
			output (" atx A4;");
			if (rsz == 1) {
				/* Mul short * char. */
				if (arg->op == OP_NAME)       output (" xta %n;", arg);
				else if (arg->op == OP_CONST) output (" cta %n;", arg);
				else error ("bad mul");
				output (" atx A5; xz A6;");
			} else {
				if (arg->op == OP_NAME)       output (" xta %n; atx A5; xta %n+1;", arg, arg);
				else if (arg->op == OP_CONST) output (" cta %l #`=%n; atx A5; cta %h;", arg, arg, arg);
				else error ("bad mul");
				output (" atx A6;");
			}
			output (" call mul22;");
			break;
		case OP_DIV:            /* / */
			if (arg->op == OP_CONST) {
				unsigned short divisor;
				if (arg->lval <= 1)
					break;
				divisor = 1 + 256*256 / arg->lval;
				output (" atx A4; atx A8; rtx A1,A9;");
				if (divisor>>8) output (" cta %d; atx A6;", divisor>>8);
				else	        output (" xz A6;");
				output (" cta %d #`=1+65536/%n; atx A5; call mul22;",
					divisor & 0xff, arg);
				output (" cta %h; atx A1; cta %l #`=%n; call divcorr2;",
					arg, arg, arg);
				break;
			}
			output (" atx A2;");
			if (rsz == 1) {
				/* Divide short by char. */
				if (arg->op != OP_NAME)
					error ("bad divide");
				output (" xtr %n,A3;", arg);
				output (" call divmod21; xta A2;");
			} else {
				if (arg->op != OP_NAME)
					error ("bad divide");
				output (" xtr %n,A3; xtr %n+1,A4;", arg, arg);
				output (" call divmod22; xta A2;");
			}
			break;
		case OP_MOD:            /* % */
			if (arg->op == OP_CONST) {
				unsigned short divisor;
				divisor = 1 + 256*256 / arg->lval;
				output (" atx A4; atx A8; rtx A1,A9;");
				if (divisor>>8) output (" cta %d; atx A6;", divisor>>8);
				else	        output (" xz A6;");
				output (" cta %d #`=1+65536/%n; atx A5; call mul22;",
					divisor & 0xff, arg);
				output (" cta %h; atx A1; cta %l #`=%n; call divcorr2;",
					arg, arg, arg);
				output (" xtr A9,A1; xta A8;");
				break;
			}
			output (" atx A2;");
			if (rsz == 1) {
				/* Divide short by char. */
				if (arg->op == OP_NAME)       output (" xtr %n,A3;", arg);
				else if (arg->op == OP_CONST) output (" cta %n; atx A3;", arg);
				else error ("bad divide");
				output (" call divmod21; xta A4;");
				if (sz > 1) output (" xz A1;");
			} else {
				if (arg->op == OP_NAME)       output (" xtr %n,A3; xtr %n+1,A4;", arg, arg);
				else if (arg->op == OP_CONST) output (" cta %l #`=%n; atx A3; cta %h; atx A4;", arg, arg, arg);
				else error ("bad divide");
				output (" call divmod22; xta A5;");
				if (sz > 1) output (" xtr A6,A1;");
			}
			break;
		}
		break;
	case 4:
		switch (op) {
		case OP_SUB:            /* - */
			error ("long - not implemented yet"); break;
		case OP_MUL:            /* * */
			error ("long * not implemented yet"); break;
		case OP_AND:            /* & */
			error ("long & not implemented yet"); break;
		case OP_OR:             /* | */
			error ("long | not implemented yet"); break;
		case OP_XOR:            /* ^ */
			error ("long ^ not implemented yet"); break;
		case OP_ADD:            /* + */
			if (rsz == 1) {
				/* Add long + char. */
				if (arg->op == OP_NAME)       output (" a+x %n; atx A4;", arg);
				else if (arg->op == OP_CONST) output (" a+c %n; atx A4;", arg);
				else error ("bad add");
				output (" xz A0; x+ca A1; x+ca A2; x+ca A3;");
			} else if (rsz == 2) {
				/* Add long + short. */
				if (arg->op == OP_NAME)       output (" a+x %n; atx A4; xta %n+1;", arg, arg);
				else if (arg->op == OP_CONST) output (" a+c %l #`=%n; atx A4; cta %h;", arg, arg, arg);
				else error ("bad add");
				output (" x+ca A1; x+ca A2; x+ca A3;");
			} else {
				if (arg->op == OP_NAME)       output (" a+x %n; atx A4; xta %n+1; x+ca A1; xta %n+2; x+ca A2; xta %n+3; x+ca A3;", arg, arg, arg, arg);
				else if (arg->op == OP_CONST) output (" a+x %l #`=%n; atx A4; cta %h; x+ca A1; cta %2; x+ca A2; cta %3; x+ca A3;", arg, arg, arg, arg, arg);
				else error ("bad add");
			}
			output (" xta A4;");
			break;
		case OP_LSHIFT:         /* << */
			if (arg->op == OP_NAME) {
				output (" atx A4; xta %n; call ls4;", arg);
			} else if (arg->op == OP_CONST) {
				i = arg->lval & 31;
				if (i == 0)
					/* nothing to do */;
				if (i == 1)
					output (" bz ALUSTA,0; xc<<a A0; xc<<x A1; xc<<x A2; xc<<x A3;");
				else if (i == 8)
					output (" xtr A2,A3; xtr A1,A2; atx A1; xz A0;");
				else if (i == 16)
					output (" xtr A1,A3; atx A2; xz A1; xz A0;");
				else if (i == 24)
					output (" atx A3; xz A2; xz A1; xz A0;");
				else
					output (" atx A4; cta %n; call ls4;", arg);
			} else error ("bad long left shift op");
			break;
		case OP_RSHIFT:         /* >> */
			if (arg->op == OP_NAME) {
				output (" atx A4; xta %n; call rs4;", arg);
			} else if (arg->op == OP_CONST) {
				i = arg->lval & 31;
				if (i == 0)
					/* nothing to do */;
				if (i == 1)
					output (" bz ALUSTA,0; xc>>a A0; xc>>x A1; xc>>x A2; xc>>a A3;");
				else if (i == 8)
					output (" xta A1; xtr A2,A1; xtr A3,A2; xz A3;");
				else if (i == 16)
					output (" xta A2; xtr A3,A1; xz A2; xz A3;");
				else if (i == 24)
					output (" xta A3; xz A1; xz A2; xz A3;");
				else
					output (" atx A4; cta %n; call rs4;", arg);
			} else error ("bad long right shift op");
			break;
		case OP_EQU:            /* == */
		case OP_NEQ:            /* != */
			output (" atx A4;");
			if (rsz == 1) {
				/* long == char. */
				if (arg->op == OP_NAME)       output (" xta %n;", arg);
				else if (arg->op == OP_CONST) output (" cta %n;", arg);
				else error ("bad == op");
				output (" xz A5; xz A6; xz A7;");
			} else if (rsz == 2) {
				/* long == short. */
				if (arg->op == OP_NAME)       output (" xta %n; atx A5; xta %n+1;", arg, arg);
				else if (arg->op == OP_CONST) output (" cta %l #`=%n; atx A5; cta %h;", arg, arg, arg);
				else error ("bad == op");
				output (" xz A6; xz A7;");
			} else {
				if (arg->op == OP_NAME)       output (" xta %n; atx A5; xta %n+1; atx A6; xta %n+2; atx A7; xta %n+3;", arg, arg, arg, arg);
				else if (arg->op == OP_CONST) output (" cta %l #`=%n; atx A5; cta %h; atx A6; cta %2; atx A7; cta %3;", arg, arg, arg, arg, arg);
				else error ("bad == op");
			}
			output (" call eq44;");
			if (op == OP_NEQ)
				output (" a^c 1;");
			else if (target == TARG_COND)
				output (ASM_COND);
			break;
		case OP_GT:             /* > */
		case OP_LE:             /* <= */
			output (" atx A4;");
			if (rsz == 1) {
				/* long > char. */
				if (arg->op == OP_NAME)       output (" xta %n;", arg);
				else if (arg->op == OP_CONST) output (" cta %n;", arg);
				else error ("bad < op");
				output (" call gt41;");
			} else if (rsz == 2) {
				/* long > short. */
				if (arg->op == OP_NAME)       output (" xta %n; atx A5; xta %n+1; atx A6;", arg, arg);
				else if (arg->op == OP_CONST) output (" cta %l #`=%n; atx A5; cta %h; atx A6;", arg, arg, arg);
				else error ("bad < op");
				output (" xz A7; az; call gt44;");
			} else {
				if (arg->op == OP_NAME)       output (" xta %n; atx A5; xta %n+1; atx A6; xta %n+2; atx A7; xta %n+3;", arg, arg, arg, arg);
				else if (arg->op == OP_CONST) output (" cta %l #`=%n; atx A5; cta %h; atx A6; cta %2; atx A7; cta %3;", arg, arg, arg, arg, arg);
				else error ("bad < op");
				output (" call gt44;");
			}
			if (op == OP_LE)
				output (" a^c 1;");
			else if (target == TARG_COND)
				output (ASM_COND);
			break;
		case OP_LT:             /* < */
		case OP_GE:             /* >= */
			output (" atx A4;");
			if (rsz == 1) {
				/* long < char. */
				if (arg->op == OP_NAME)       output (" xta %n;", arg);
				else if (arg->op == OP_CONST) output (" cta %n;", arg);
				else error ("bad < op");
				output (" call lt41;");
			} else if (rsz == 2) {
				/* long < short. */
				if (arg->op == OP_NAME)       output (" xta %n; atx A5; xta %n+1; atx A6; xz A7; az;", arg, arg);
				else if (arg->op == OP_CONST) output (" cta %l #`=%n; atx A5; cta %h; atx A6; xz A7; az;", arg, arg, arg);
				else error ("bad < op");
				output (" call lt44;");
			} else {
				if (arg->op == OP_NAME)       output (" xta %n; atx A5; xta %n+1; atx A6; xta %n+2; atx A7; xta %n+3;", arg, arg, arg, arg);
				else if (arg->op == OP_CONST) output (" cta %l #`=%n; atx A5; cta %h; atx A6; cta %2; atx A7; cta %3;", arg, arg, arg, arg, arg);
				else error ("bad < op");
				output (" call lt44;");
			}
			if (op == OP_GE)
				output (" a^c 1;");
			else if (target == TARG_COND)
				output (ASM_COND);
			break;
		case OP_DIV:            /* / */
			output (" atx A4;");
			if (rsz == 1) {
				/* Divide long by char. */
				if (arg->op == OP_NAME)       output (" xta %n; atx A5;", arg);
				else if (arg->op == OP_CONST) output (" cta %n; atx A5;", arg);
				else error ("bad divide");
				output (" xz A6; call divmod42; xta A4;");
			} else if (rsz == 2) {
				/* Divide long by short. */
				if (arg->op == OP_NAME)       output (" xta %n; atx A5; xta %n+1; atx A6;", arg, arg);
				else if (arg->op == OP_CONST) output (" cta %l #`=%n; atx A5; cta %h; atx A6;", arg, arg, arg);
				else error ("bad divide");
				output (" call divmod42; xta A4;");
			} else {
				if (arg->op == OP_NAME)       output (" xta %n; atx A5; xta %n+1; atx A6; xta %n+2; atx A7; xta %n+3; atx A8;", arg, arg, arg, arg);
				else if (arg->op == OP_CONST) output (" cta %l #`=%n; atx A5; cta %h; atx A6; cta %2; atx A7; cta %3; atx A8;", arg, arg, arg, arg, arg);
				else error ("bad divide");
				output (" call divmod44; xta A4;");
			}
			break;
		case OP_MOD:            /* % */
			output (" atx A4;");
			if (rsz == 1) {
				/* Divide long by char. */
				if (arg->op == OP_NAME)       output (" xta %n; atx A5;", arg);
				else if (arg->op == OP_CONST) output (" cta %n; atx A5;", arg);
				else error ("bad divide");
				output (" xz A6; call divmod42; xta A7;");
				if (sz > 1) output (" xtr A8,A1;");
				if (sz > 2) output (" xz A2; xz A3;");
			} else if (rsz == 2) {
				/* Divide long by short. */
				if (arg->op == OP_NAME)       output (" xta %n; atx A5; xta %n+1; atx A6;", arg, arg);
				else if (arg->op == OP_CONST) output (" cta %l #`=%n; atx A5; cta %h; atx A6;", arg, arg, arg);
				else error ("bad divide");
				output (" xz A7; xz A8; call divmod44; xta A9;");
				if (sz > 1) output (" xtr A10,A1;");
				if (sz > 2) output (" xtr A11,A2; xtr A12,A3;");
			} else {
				if (arg->op == OP_NAME)       output (" xta %n; atx A5; xta %n+1; atx A6; xta %n+2; atx A7; xta %n+3; atx A8;", arg, arg, arg, arg);
				else if (arg->op == OP_CONST) output (" cta %l #`=%n; atx A5; cta %h; atx A6; cta %2; atx A7; cta %3; atx A8;", arg, arg, arg, arg, arg);
				else error ("bad divide");
				output (" call divmod44; xta A9;");
				if (sz > 1) output (" xtr A10,A1;");
				if (sz > 2) output (" xtr A11,A2; xtr A12,A3;");
			}
			break;
		}
		break;
	}
	cast (sz, lsz);
}

/*
 * Compile the binary operation.
 */
void machop (node_t *n, int target)
{
	switch (n->op) {
	case OP_EQU:            /* == */
	case OP_NEQ:            /* != */
	case OP_LT:             /* < */
	case OP_GT:             /* > */
	case OP_LE:             /* <= */
	case OP_GE:             /* >= */
	case OP_ADD:            /* + */
	case OP_MUL:            /* * */
	case OP_DIV:            /* / */
	case OP_MOD:            /* % */
	case OP_AND:            /* & */
	case OP_OR:             /* | */
	case OP_XOR:            /* ^ */
	case OP_LSHIFT:         /* << */
	case OP_RSHIFT:         /* >> */
		if (! LEAF (n->right->op)) {
			error ("too complex rhs of expression, simplify");
/*
fclose (stdout);
fdopen (2, "w");
dup (2);
nprint (n->right, 0);
*/
			break;
		}
		expr3 (n->left, TARG_ACC);
		printbinary (n->op, n->type, n->left->type, n->right, target);
		break;

	case OP_SUB:            /* - */
		/* Do it right to left. */
		if (! LEAF (n->left->op)) {
			error ("too complex lhs of subtract op, simplify");
			break;
		}
		expr3 (n->right, TARG_ACC);
		printbinary (OP_SUB, n->type, n->right->type, n->left, target);
		break;
	}
}

/*
 * The value is on the accumulator;
 * make the conditional testing (Z-bit).
 */
void makecond (int sz)
{
	if (sz > 2)
		output (" a|x A2; a|x A3;");
	if (sz > 1)
		output (" a|x A1;");
	else
		output (" a|c 0;");
}
