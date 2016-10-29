/*
 * Чтение файла конфигурации сервера.
 *
 * Copyright (C) 1992-1995 Cronyx Ltd.
 * Автор: Сергей Вакуленко, vak@cronyx.ru
 * Wed Feb  8 18:29:31 MSK 1995
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INITCONFIG
#include "config.h"

extern char *strcopy (char*);
extern int getstr (FILE*, char*);

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
	char line [2048];
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
