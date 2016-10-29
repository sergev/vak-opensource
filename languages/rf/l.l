%{

# define STSIZE 128

# define SSYM   01
# define SFUN   02
# define SGLOB  04

struct stab {
	char *name;
	char type;
} stab [STSIZE];

int snext;

extern yylval;

%}

L                       [a-zA-ZБ-Сб-с]
D                       [0-9]
LD                      ({L}|{D})
SP                      [ \t]

%%

^{SP}*\n                ;               /* empty line */
^\*.*\n                 ;               /* comment */
^{SP}+                  return (' ');
{SP}+                   ;               /* empty symbols */
"\n+"                   ;
"\n"                    return ('\n');

"global"                return (GLOBAL);
"local"                 return (LOCAL);
"variable"              return (VARIABLE);
"function"              return (FUNCTION);

{L}{LD}*                { yylval = enter (yytext); return (NAME); }
-?{D}+                  { yylval = atoi (yytext); return (NUMBER); }
'(\\.|[^\\'])*'         { yylval = mkstring (); return (STRING); }
[*?%#&!]{LD}            { yylval = yytext[1]; return (yytext[0]); }
.                       return (yytext[0]);

%%

mkstring ()
{
	register char *p, *g;
	register c;

	g = yytext + 1;
	p = yytext;
	for (; *g; g++, p++) {
		switch (*g) {
		default:
			*p = *g;
			break;
		case '\'':
			*p = 0;
			return (p - yytext);
		case '\\':
			switch (*++g) {
			case '\0':
				cerror ("bad string");
			case 'n':
				*p = '\n';
				break;
			case 't':
				*p = '\t';
				break;
			case 'f':
				*p = '\f';
				break;
			case 'e':
				*p = '\e';
				break;
			case '\\':
				*p = '\\';
				break;
			case '\'':
				*p = '\'';
				break;
			default:
				*p = *g;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
				*p = *g - '0';
				c = g[1];
				if (c<='7' && c>='0') {
					*p = (*p<<3) + c - '0';
					g++;
					c = g[1];
					if (c<='7' && c>='0') {
						*p = (*p<<3) + c - '0';
						g++;
					}
				}
				break;
			}
			break;
		}
	}
	cerror ("bad mkstring");
}

enter (name)
char *name;
{
	register struct stab *l, *r, *p;
	register c;
	extern char *malloc ();

	l = stab;
	r = stab + snext;
	while (r > l) {
		p = l + (r-l)/2;
		c = strcmp (name, p->name);
		if (!c) return (p-stab);
		if (c>0) l = p+1; else r = p;
	}
	for (r=stab+snext; r>l; r--)
		r[0] = r[-1];
	l->name = malloc (strlen (name) + 1);
	if (! l->name) cerror ("out of memory");
	strcpy (l->name, name);
	l->type = 0;
	snext++;
	return (l-stab);
}

lookup (name)
char *name;
{
	register struct stab *l, *r, *p;
	register c;

	l = stab;
	r = stab + snext;
	while (r > l) {
		p = l + (r-l)/2;
		c = strcmp (name, p->name);
		if (!c) return (p-stab);
		if (c>0) l = p+1; else r = p;
	}
	return (-1);
}
