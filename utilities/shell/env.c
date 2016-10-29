/*
 *      EnvInit (char **env)
 *              Initialixe Env packadge by environment vector.
 *
 *      char *EnvGet (char *name)
 *              Get variable from environment.
 *
 *      EnvPut (char *name, char *value)
 *              Change value of variable.
 *
 *      char **EnvVector
 *              Current environment vector.
 */

# include "mem.h"

# define QUANT 10

char **EnvVector;

static nenv;
static last;

static char *nvmatch (), *findvar ();
extern char *strcpy (), *strcat ();

EnvInit (env)
char **env;
{
	register char **p;
	register i;

	if (EnvVector) {
		for (p=EnvVector; *p; ++p)
			MemFree ((mem *) *p);
		MemFree ((mem *) EnvVector);
		EnvVector = 0;
		nenv = 0;
		last = nenv = 0;
	}
	if (! env)
		return;
	for (i=0, p=env; *p; ++p, ++i) {
		MemCheckIndex (EnvVector, char **, nenv, QUANT, i);
		EnvVector [i] = (char *) MemAlloc (strlen (*p) + 1);
		strcpy (EnvVector [i], *p);
	}
	MemCheckIndex (EnvVector, char **, nenv, QUANT, i);
	last = i;
	EnvVector [last] = 0;
}

EnvPut (name, value)
char *name, *value;
{
	register char *v;
	char **p;

	if (! name)
		return;
	if (! value)
		value = "";
	v = findvar (name, &p);
	if (v)
		MemFree ((mem *) *p);
	else {
		p = EnvVector + last;
		++last;
		MemCheckIndex (EnvVector, char **, nenv, QUANT, last);
		EnvVector [last] = 0;
	}
	*p = (char *) MemAlloc (strlen (name) + 1 + strlen (value) + 1);
	strcpy (*p, name);
	strcat (*p, "=");
	strcat (*p, value);
}

EnvDelete (name)
char *name;
{
	register char *v;
	char **p;

	if (! name)
		return;
	v = findvar (name, &p);
	if (! v)
		return;
	MemFree ((mem *) *p);
	for (++p; *p; ++p)
		*(p-1) = *p;
	EnvVector [--last] = 0;
}

char *EnvGet (name)
char *name;
{
	char **p;

	return (findvar (name, &p));
}

static char *findvar (name, ptr)
register char *name;
char ***ptr;
{
	register char *v, **p;

	p = EnvVector;
	if (! p)
		return (0);
	for (; *p; ++p)
		if (v = nvmatch (name, *p)) {
			*ptr = p;
			return (v);
		}
	return (0);
}

/*
 *	s1 is either name, or name=value
 *	s2 is name=value
 *	if names match, return value of s2, else NULL
 *	used for environment searching: see getenv
 */

static char *nvmatch (s1, s2)
register char *s1, *s2;
{
	while (*s1 == *s2++)
		if (*s1++ == '=')
			return (s2);
	if (*s1 == '\0' && *(s2-1) == '=')
		return (s2);
	return (0);
}
