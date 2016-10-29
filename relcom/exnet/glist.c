/*
 * Copyright (C) 1991 Сергей Вакуленко, <vak@kiae.su>.
 * All rights reserved.
 */

# include <stdio.h>
# include "config.h"
# include "server.h"

extern char *malloc (), *strcopy (), *time2a ();
extern long a2time ();
extern FILE *f2open ();

/*
 * Загрузка общего списка лотов.
 */

struct glist *loadglist ()
{
	register struct glist *glp;
	register struct glistentry *p, **q;
	FILE *fd;
	char t0 [30];
	char line [512];
	char name [512];

	glp = (struct glist *) malloc (sizeof (struct glist));
	if (! glp)
		fatal ("out of memoty in loadglist");
	glp->list = 0;
	glp->changed = 0;

	fd = f2open (SERVDIR, "listfile", "r");
	if (! fd)
		return (glp);
	q = &glp->list;
	while (fgets (line, sizeof (line), fd)) {
		sscanf (line, "%s %s", t0, name);
		p = (struct glistentry *) malloc (sizeof (struct glistentry));
		if (! p)
			fatal ("out of memoty in loadglist.");
		p->t0 = a2time (t0);
		p->name = strcopy (name);
		p->next = 0;
		*q = p;
		q = &p->next;
	}
	fclose (fd);
	return (glp);
}

/*
 * Освобождение (памяти) общего списка лотов.
 */

freeglist (glp)
struct glist *glp;
{
	register struct glistentry *p, *q;

	if (p = glp->list)
		do {
			q = p;
			p = p->next;
			free (q->name);
			free ((char *) q);
		} while (p);
	free ((char *) glp);
}

/*
 * Запись общего списка лотов в файл.
 */

saveglist (glp)
struct glist *glp;
{
	register struct glistentry *p;
	FILE *fd;

	if (! glp->changed)
		return;
	fd = f2open (SERVDIR, "listfile", "w");
	if (! fd)
		fatal ("cannot create %s/listfile", SERVDIR);

	if (! glp->list)
		return;
	for (p=glp->list; p; p=p->next)
		fprintf (fd, "%s %s\n", time2a (p->t0), p->name);
	fclose (fd);
}

/*
 * Удаление лота из общего списка.
 */

deleteglist (glp, name)
struct glist *glp;
char *name;
{
	register struct glistentry *p, **q;

	if (! glp->list)
		return;
	q = &glp->list;
	do {
		p = *q;
		if (! strcmp (name, p->name)) {
			*q = p->next;
			free (p->name);
			free ((char *) p);
		} else
			q = &p->next;
	} while (*q);
	glp->changed = 1;
}

/*
 * Добление нового лота к общему списку.
 */

appendglist (name, t0)
char *name;
long t0;
{
	FILE *fd;

	fd = f2open (SERVDIR, "listfile", "a");
	if (! fd)
		fatal ("cannot create %s/listfile", SERVDIR);
	fprintf (fd, "%s %s\n", time2a (t0), name);
	fclose (fd);
}
