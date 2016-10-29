/*
 * Copyright (C) 1991 Сергей Вакуленко, <vak@kiae.su>.
 * All rights reserved.
 */

# include <stdio.h>
# include "config.h"
# include "server.h"

extern char *malloc (), *strcopy (), *strcpy (), *strcat ();
extern long a2time ();

/*
 * Загрузка информации о лоте.
 */

struct lot *loadlot (name)
char *name;
{
	FILE *fd;
	char iname [20], host [100], info [512], t1 [20], t2 [20];
	struct lot *lot;

	strcpy (iname, name);
	strcat (iname, ".i");
	fd = fopen (iname, "r");
	if (! fd)
		return (0);

	lot = (struct lot *) malloc (sizeof (struct lot));
	if (! lot)
		fatal ("out of memory in loadlot");
	lot->name = strcopy (name);

	getstr (fd, host);
	getstr (fd, info);
	getstr (fd, t1);
	getstr (fd, t2);
	lot->host = strcopy (host);
	lot->info = strcopy (info);
	lot->t1 = a2time (t1);
	lot->t2 = a2time (t2);

	fclose (fd);
	return (lot);
}

/*
 * Освобождение памяти, занятой структурой lot.
 */

freelot (lot)
struct lot *lot;
{
	free (lot->host);
	free (lot->name);
	free (lot->info);
	free ((char *) lot);
}
