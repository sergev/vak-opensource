%token STRING NUMBER NAME GLOBAL LOCAL FUNCTION VARIABLE
%{

# include "lex.yy.c

# define FREE   01
# define CHAR   02
# define LB     03
# define NUM    04
# define NAM    05
# define FUN    06

char sym [256];
int stype;
int fix;
int curlab, nextlab;
int cursym;
char *curftn;
int empty;

%}
%%

program:
		definition_list function_list
		{
			register i;

			printf ("/*\n");
			for (i=0; i<snext; i++) {
				printf (" *\t%s\t", stab[i].name);
				if (stab[i].type & SGLOB)
					printf ("global\t");
				else
					printf ("local\t");
				if (stab[i].type & SSYM)
					printf ("variable\n");
				if (stab[i].type & SFUN)
					printf ("fuction\n");
			}
			printf (" */\n");
		}
	;

definition_list:
		definition_list definition
	|       {
			printf ("struct _symbol {\n");
			printf ("\tstruct _chain *top, *tail;\n");
			printf ("};\n");
			printf ("struct _chain {\n");
			printf ("\tstruct _chain *next;\n");
			printf ("\tchar type;\n");
			printf ("\tunion {\n");
			printf ("\t\tchar ch;\n");
			printf ("\t\tint num;\n");
			printf ("\t\tstruct _chain *rb;\n");
			printf ("\t\tstruct _chain (*fun) ();\n");
			printf ("\t\tstruct _symbol *val;\n");
			printf ("\t} v;\n");
			printf ("};\n");
			printf ("extern struct _chain *_rfalloc ();\n");
			printf ("\n");
		}
	;

definition:
		' ' class type name_list '\n'
	;

class:
		GLOBAL  { stype = SGLOB; }
	|       LOCAL   { stype = 0; }
	|               { stype = 0; }
	;

type:
		FUNCTION        { stype |= SFUN; }
	|       VARIABLE        { stype |= SSYM; }
	|                       { stype |= SFUN; }
	;

name_list:
		name_list NAME
		{
			if (stab[$2].type)
				cerror ("%s doubly defined", stab[$2].name);
			stab[$2].type = stype;
			if (! (stype & SGLOB))
				printf ("static ");
			if (stype & SSYM)
				printf ("struct _chain *%s;\n", stab[$2].name);
			else
				printf ("struct _chain *%s ();\n", stab[$2].name);
		}
	|
	;

function_list:
		function_list function
	|
	;

function:
		NAME '\n'
		{
			if (stab[$1].type & SSYM)
				cerror ("%s doubly defined", stab[$1].name);
			stab[$1].type |= SFUN;
			if (! (stab[$1].type & SGLOB))
				printf ("static ");
			curftn = stab[$1].name;
			printf ("struct _chain *%s (_a)\n", curftn);
			printf ("struct _chain *_a;\n{\n");
			printf ("\tstruct _chain *_b, *_c;\n");
		}
		statement_list
		{
			printf ("\t_rfexit (\"%s\");\n", curftn);
			printf ("}\n");
		}
	;

statement_list:
		statement_list statement
	|
	;

statement:
		' '
		{
			register i;

			for (i=0; i<256; i++)
				sym[i] = 0;
			nextlab = curlab++;
			fix = 1;
			printf ("\t_c = _a;\n");
		}
		pattern_list '='
		{
			if (fix) {
				printf ("\tif (_c) goto _%d;\n", nextlab);
			} else {
				printf ("\tif (_c) while (_c->next) _c = _c->next;\n");
				printf ("\t_sym[%d].end = _c;\n", cursym);
			}
			printf ("\twhile (_a) { _a->type = %d; _a = _a->next; }\n", FREE);
			empty = 1;
		}
		replacement_list '\n'
		{
			if (empty)
				printf ("\t_b = _c = 0;\n");
			else
				printf ("\t_c->next = 0;\n");
			printf ("\treturn (_b);\n");
			printf ("_%d:\n", nextlab);
		}
	;

pattern_list:
		pattern_list pattern
	|
	;

pattern:
		NUMBER
	|       NAME
	|       STRING
		{
			if (fix) {
				register char *p;

				for (p=yytext; *p; p++) {
					printf ("\tif (!_c) goto _%d;\n", nextlab);
					printf ("\tif (_c->type != %d) goto _%d;\n", CHAR, nextlab);
					printf ("\tif (_c->v.ch != %d) goto _%d;\n", *p, nextlab);
					printf ("\t_c = _c->next;\n");
				}
			} else {
			}
		}
	|       '*'
	|       '%'
	|       '?'
	|       '#'
	|       '&'
	|       '!'
	|       '(' pattern_list ')'
	;

replacement_list:
		replacement_list replacement
	|
	;

replacement:
		NUMBER
	|       NAME
	|       STRING
		{
			register char *p;

			for (p=yytext; *p; p++) {
				if (empty) printf ("\t_b = _c = _rfalloc ();\n");
				else {
					printf ("\t_c->next = _rfalloc ();\n");
					printf ("\t_c = _c->next;\n");
				}
				printf ("\t_c->type = %d;\n", CHAR);
				printf ("\t_c->v.ch = %d;\n", *p);
				empty = 0;
			}
		}
	|       '*'
	|       '%'
	|       '?'
	|       '#'
	|       '&'
	|       '!'
	|       '('
	|       '['
	;

%%

# include <stdio.h>

yyerror (s, a, b, c)
char *s;
{
	fprintf (stderr, "line %d: ", yylineno);
	fprintf (stderr, s, a, b, c);
	fprintf (stderr, "\n");
}

cerror (s, a, b, c)
char *s;
{
	yyerror (s, a, b, c);
	exit (1);
}
