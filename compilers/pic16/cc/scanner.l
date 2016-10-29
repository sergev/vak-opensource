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
#include "y.tab.h"
static void multistring (void);
static int makechar (void);
static int makename (void);
static void comment (void);
static void commentline (void);
static void badlex (void);
static void line (void);
%}
%%

[ \t\f\r]               { ; }                   /* skip spaces */
\n                      { ++lineno; }
"/*"                    { comment (); }
"//"                    { commentline (); }
"#"                     { commentline (); }
"++"                    { return INC; }
"--"                    { return DEC; }
"&&"                    { return ANDAND; }
"||"                    { return OROR; }
"=="                    { return EQUOP; }
"!="                    { return NEQOP; }
"<<"                    { return LSOP; }
">>"                    { return RSOP; }
"<="                    { return LE; }
">="                    { return GE; }
[+\-&|=!<>%()*,./:?[\]^~@;{}] { return yytext[0]; }
[1-9][0-9]*             { yylval.lval = strtol (yytext, 0, 10); return CON; }
0[0-7]*                 { yylval.lval = strtol (yytext, 0, 8); return CON; }
0[xX][0-9a-fA-F]+       { yylval.lval = strtol (yytext+2, 0, 16); return CON; }
0[bB][01]+              { yylval.lval = strtol (yytext+2, 0, 2); return CON; }
\'                      { yylval.lval = makechar (); return CON; }
[_a-zA-Z\200-\377][_a-zA-Z0-9\200-\377]*  { return makename (); }
\"                      { multistring (); return STRING; }
\#\ [0-9]*\ \"[^"]*\"(\ [0-9]*)?\n { line (); }
.                       { badlex (); }

%%

#include <string.h>

int lineno = 1;
char *filename = 0;

static int escapechar ()
{
	int c, n;

	c = input();
	switch (c) {
	case 'a':  c = 0x07; break;
	case 'b':  c = '\b'; break;
	case 'f':  c = '\f'; break;
	case 'n':  c = '\n'; break;
	case 'r':  c = '\r'; break;
	case 't':  c = '\t'; break;
	case 'v':  c = '\v'; break;
	case '0': case '1': case '2': case '3':
		n = c - '0';
		c = input();
		if (c>='0' && c<='7') {
			n = n*8 + c - '0';
			c = input();
			if (c>='0' && c<='7') {
				n = n*8 + c - '0';
			} else
				unput (c);
		} else
			unput (c);
		c = n;
		break;
	case 'x':
		n = 0;
		c = input();
		if (c>='0' && c<='7')
			n = c - '0';
		else if (c>='a' && c<='f')
			n = c - 'a';
		else if (c>='A' && c<='F')
			n = c - 'A';
		else
			unput (c);
		c = input();
		if (c>='0' && c<='7')
			n = n*16 + c - '0';
		else if (c>='a' && c<='f')
			n = n*16 + c - 'a';
		else if (c>='A' && c<='F')
			n = n*16 + c - 'A';
		else
			unput (c);
		c = n;
		break;
	default:
		if (c == '\n')
			++lineno;
		break;
	}
	return c;
}

static char *gluestrings (char *p, char *s)
{
	int plen, slen;
	char *r;

	plen = strlen (p);
	slen = strlen (s);
	r = malloc (plen+slen+1);
	if (! r) {
		error ("no memory for multistring");
		exit (-1);
	}
	memcpy (r, p, plen);
	memcpy (r+plen, s, slen);
	r [plen+slen] = 0;
	free (p);
	free (s);
	return r;
}

static char *makestring ()
{
	static char *buf;
	static int len;
	char *p;
	int c;

	if (! buf) {
		len = 512;
		buf = malloc (len);
		if (! buf)
			goto nomem;
	}
	p = buf;
	while ((c = input()) >= 0 && c != '"') {
		if (c == '\n')
			++lineno;
		if (c == '\\')
			c = escapechar ();
		*p++ = c;
		if (p >= buf+len) {
			len += 512;
			buf = realloc (buf, len);
			if (! buf)
				goto nomem;
		}
	}
	*p = 0;
	p = strdup (buf);
	if (! p) {
nomem:          error ("no memory for string");
		exit (-1);
	}
	return p;
}

void multistring ()
{
	char *p, *s;
	int c;

	p = makestring ();
	for (;;) {
		/* Skip spaces. */
		while ((c = input()) >= 0 &&
		    (c == ' ' || c == '\t' || c == '\f' || c == '\r' || c == '\n'))
			if (c == '\n')
				++lineno;
		if (c < 0)
			break;
		if (c != '"') {
			unput (c);
			break;
		}
		/* Glue two strings. */
		s = makestring ();
		p = gluestrings (p, s);
	}
	yylval.strval = p;
}

int makechar ()
{
	int c, n;

	c = input();
	if (c < 0) {
		error ("EOF in char constant");
		return 0;
	}
	if (c == '\n') {
		++lineno;
		error ("end of line in char constant");
		return 0;
	}
	if (c == '\\')
		c = escapechar ();
	n = c;
	c = input();
	if (c != '\'')
		error ("unterminated char constant");
	return n;
}

int makename ()
{
	switch (yytext[0]) {
	case 'a': if (! strcmp (yytext, "asm"))       return ASM;       break;
	case 'b': if (! strcmp (yytext, "break"))     return BREAK;     break;
	case 'c': if (! strcmp (yytext, "case"))      return CASE;
		  if (! strcmp (yytext, "char"))      return CHAR;
		  if (! strcmp (yytext, "continue"))  return CONTINUE;  break;
	case 'd': if (! strcmp (yytext, "default"))   return DEFAULT;
		  if (! strcmp (yytext, "do"))        return DO;        break;
	case 'e': if (! strcmp (yytext, "else"))      return ELSE;      break;
	case 'f': if (! strcmp (yytext, "far"))       return FAR;
		  if (! strcmp (yytext, "for"))       return FOR;       break;
	case 'g': if (! strcmp (yytext, "goto"))      return GOTO;      break;
	case 'i': if (! strcmp (yytext, "if"))        return IF;
		  if (! strcmp (yytext, "int"))       return CHAR;
		  if (! strcmp (yytext, "interrupt")) return INTERRUPT; break;
	case 'l': if (! strcmp (yytext, "long"))      return LONG;      break;
	case 'r': if (! strcmp (yytext, "return"))    return RETURN;    break;
	case 's': if (! strcmp (yytext, "short"))     return SHORT;
		  if (! strcmp (yytext, "sizeof"))    return SIZEOF;
		  if (! strcmp (yytext, "struct"))    return STRUCT;
		  if (! strcmp (yytext, "switch"))    return SWITCH;    break;
	case 'v': if (! strcmp (yytext, "void"))      return VOID;      break;
	case 'w': if (! strcmp (yytext, "while"))     return WHILE;     break;
	}
	yylval.strval = strdup (yytext);
	if (! yylval.strval) {
		error ("no memory for name `%s'", yytext);
		exit (-1);
	}
	return NAME;
}

void comment ()
{
	int c;

	for (;;) {
		while ((c = input()) >= 0 && c != '*')
			if (c == '\n')
				++lineno;
		if (c == '*') {
			while ((c = input()) == '*')
				continue;
			if (c == '/')
				break;
			if (c == '\n')
				++lineno;
		}
		if (c < 0) {
			error ("EOF in comment");
			break;
		}
	}
}

void commentline ()
{
	int c;

	while ((c = input()) >= 0 && c != '\n')
		continue;
	++lineno;
}

void line ()
{
	lineno = atoi (yytext + 2);
	if (filename)
		free (filename);
	filename = strchr (yytext, '"');
	if (filename) {
		filename = strdup (filename+1);
		if (filename) {
			char *p = strchr (filename, '"');
			if (p)
				*p = 0;
		}
	}
}

void badlex ()
{
	error ("lexical error");
	exit (-1);
}

void printlex (int lex)
{
	switch (lex) {
	case 0:
		return;

	default:
		printf ("?0%o\n", lex);
		break;

	case '+': case '-': case '&': case '|': case '=': case '!':
	case '<': case '>': case '%': case '(': case ')': case '*':
	case ',': case '.': case '/': case ':': case ';': case '?':
	case '[': case ']': case '{': case '}': case '^': case '~':
	case '@':
		printf ("%c\n", lex);
		break;

	case NAME:
		printf ("NAME \"%s\"\n", stab[yylval.lval].name);
		break;
	case STRING:
		printf ("STRING \"%s\"\n", yylval.strval);
		break;
	case CON:
		printf ("CON %ld\n", yylval.lval);
		break;

	case ANDAND:    printf ("ANDAND\n"); break;
	case OROR:      printf ("OROR\n"); break;
	case INC:       printf ("INC\n"); break;
	case DEC:       printf ("DEC\n"); break;
	case EQUOP:     printf ("EQUOP\n"); break;
	case NEQOP:     printf ("NEQOP\n"); break;
	case LE:        printf ("LE\n"); break;
	case GE:        printf ("GE\n"); break;
	case LSOP:      printf ("LSOP\n"); break;
	case RSOP:      printf ("RSOP\n"); break;
	case STRUCT:    printf ("STRUCT\n"); break;
	case RETURN:    printf ("RETURN\n"); break;
	case GOTO:      printf ("GOTO\n"); break;
	case IF:        printf ("IF\n"); break;
	case ELSE:      printf ("ELSE\n"); break;
	case SWITCH:    printf ("SWITCH\n"); break;
	case BREAK:     printf ("BREAK\n"); break;
	case CONTINUE:  printf ("CONTINUE\n"); break;
	case WHILE:     printf ("WHILE\n"); break;
	case DO:        printf ("DO\n"); break;
	case FOR:       printf ("FOR\n"); break;
	case DEFAULT:   printf ("DEFAULT\n"); break;
	case CASE:      printf ("CASE\n"); break;
	case SIZEOF:    printf ("SIZEOF\n"); break;
	case ASM:       printf ("ASM\n"); break;
	case CHAR:      printf ("CHAR\n"); break;
	case SHORT:     printf ("SHORT\n"); break;
	case LONG:      printf ("LONG\n"); break;
	case FAR:       printf ("FAR\n"); break;
	case VOID:      printf ("VOID\n"); break;
	}
}
