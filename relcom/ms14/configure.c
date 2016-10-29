#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ut.h"

#define INITCONFIG
#include "config.h"

static void setvalue (char *var, char *value)
{
	int i;

	for (i=0; inttable[i].name; ++i)
		if (! strcasecmp (var, inttable[i].name))
			*inttable[i].value = atoi (value);
	for (i=0; strtable[i].name; ++i)
		if (! strcasecmp (var, strtable[i].name))
			*strtable[i].value = strcopy (value);
}

/*
 * Read config file.
 * Lines must hahe syntax "name = value".
 * Empty lines and lines beginning with # are ignored.
 */
int config (char *name)
{
	FILE *fd;
	char line [512];
	char *var, *value;

	if (name)
		CONFIGFILE = name;
	fd = fopen (CONFIGFILE, "r");
	if (! fd)
		return (0);
	while (getstr (fd, line)) {
		var = line;
		while (*var==' ' || *var == '\t')
			++var;
		if (*var==0 || *var=='#')
			continue;
		value = var;
		while (*value && *value != ' ' && *value != '\t' && *value != '=')
			++value;
		if (! *value)
			continue;
		*value++ = 0;
		while (*value == ' ' || *value == '\t')
			++value;
		if (*value != '=')
			continue;
		++value;
		while (*value == ' ' || *value == '\t')
			++value;
		setvalue (var, value);
	}
	fclose (fd);
	return (1);
}
