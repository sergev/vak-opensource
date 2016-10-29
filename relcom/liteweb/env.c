#include <string.h>
#include <stdlib.h>

#define QUANT 10
#define MEXPAND(array, type, bound, index)\
	if ((index) >= (bound)) (array) = (type) realloc ((array),\
		(int) ((bound) += QUANT) * sizeof (*(array)))

static int nenv;
static int last;

extern char **environ;

/*
 *	s1 is either name, or name=value
 *	s2 is name=value
 *	if names match, return value of s2, else NULL
 *	used for environment searching: see getenv
 */
static char *nvmatch (char *s1, char *s2)
{
	while (*s1 == *s2++)
		if (*s1++ == '=')
			return (s2);
	if (*s1 == '\0' && *(s2-1) == '=')
		return (s2);
	return (0);
}

static char *findvar (char *name, char ***ptr)
{
	char *v, **p;

	p = environ;
	if (! p)
		return (0);
	for (; *p; ++p)
		if ((v = nvmatch (name, *p))) {
			*ptr = p;
			return (v);
		}
	return (0);
}

static int env_init (char **env)
{
	char **p, **new;
	int i;

	if (nenv && environ) {
		for (p=environ; *p; ++p)
			free (*p);
		free (environ);
		environ = 0;
		nenv = 0;
		last = 0;
	}
	if (! env)
		return (1);
	new = (char**) malloc ((nenv = QUANT) * sizeof (*new));
	if (! new)
		return (0);
	for (i=0, p=env; *p; ++p, ++i) {
		MEXPAND (new, char **, nenv, i);
		if (! new)
			return (0);
		new [i] = malloc (strlen (*p) + 1);
		if (new [i])
			strcpy (new [i], *p);
	}
	MEXPAND (new, char **, nenv, i);
	if (! new)
		return (0);
	last = i;
	new [last] = 0;
	environ = new;
	return (1);
}

void put_env (char *name, char *value)
{
	char *v, **p;

	if (! name)
		return;
	if (! nenv && ! env_init (environ))
		return;
	if (! value)
		value = "";
	v = findvar (name, &p);
	if (v)
		free (*p);
	else {
		++last;
		MEXPAND (environ, char **, nenv, last);
		p = environ + last - 1;
		environ [last] = 0;
	}
	*p = malloc (strlen (name) + 1 + strlen (value) + 1);
	if (*p) {
		strcpy (*p, name);
		strcat (*p, "=");
		strcat (*p, value);
	}
}

void del_env (char *name)
{
	register char *v;
	char **p;

	if (! name)
		return;
	if (! nenv && ! env_init (environ))
		return;
	v = findvar (name, &p);
	if (! v)
		return;
	free (*p);
	for (++p; *p; ++p)
		*(p-1) = *p;
	environ [--last] = 0;
}

char *get_env (char *name)
{
	char **p;

	return (findvar (name, &p));
}
