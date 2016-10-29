/*
 * $Header: configure.c,v 2.0 92/08/05 08:42:41 vak Exp $
 * $Log:	configure.c,v $
 * Revision 2.0  92/08/05  08:42:41  vak
 * 
 * 
 * Revision 1.3  92/06/19  22:19:49  usenet
 * Fix errors. Rudnev
 * 
 * Revision 1.2  92/06/19  22:10:46  usenet
 * 
 * Revision 1.2  92/06/19  22:08:11  usenet
 * News Server
 * 
 */
/*
 * Copyright (C) 1991 Сергей Вакуленко, <vak@kiae.su>.
 * All rights reserved.
 */

# include <stdio.h>

# define INITCONFIG
# include "config.h"

extern char *strlower (), *strcopy ();

/*
 * Чтение файла конфигурации.
 * Пустые строки и начинающиеся с # считаются комментариями.
 * Содержательные строки имеют вид "ИМЯ = ЗНАЧЕНИЕ".
 */

static void setvalue (var, value)
char *var, *value;
{
	int i;

	var = strlower (var);
	for (i=0; inttable[i].name; ++i)
		if (! strcmp (var, inttable[i].name))
			*inttable[i].value = atoi (value);
	for (i=0; strtable[i].name; ++i)
		if (! strcmp (var, strtable[i].name))
			*strtable[i].value = strcopy (value);
}

config (name)
char *name;
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
