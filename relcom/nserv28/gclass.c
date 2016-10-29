/*
 * $Header: gclass.c,v 2.0 92/08/05 08:42:47 vak Exp $
 * $Log:	gclass.c,v $
 * Revision 2.0  92/08/05  08:42:47  vak
 * 
 * 
 * Revision 1.3  92/06/19  22:19:54  usenet
 * Fix errors. Rudnev
 * 
 * Revision 1.2  92/06/19  22:11:32  usenet
 * 
 * Revision 1.2  92/06/19  22:08:28  usenet
 * News Server
 * 
 */
# include <stdio.h>
# include "config.h"

# define MAXC 40                        /* max number of classes */

struct ctab {
	char    *gclass;
	char    *gpat;
	int     patlen;
};

static struct ctab ctab [MAXC];
static nctab;
static initclass;

extern FILE *f2open ();
extern char *strcopy ();

# ifdef DEBUG
# define SERVDIR "/usr/spool/newsserv"
# define MYADDRESS "newsserv"

char *LOGFILE = "log";

quit ()
{
	exit (1);
}

/* VARARGS1 */

error (s, a, b, c, d)
char *s;
long a, b, c, d;
{
	fprintf (stderr, s, a, b, c, d);
	fprintf (stderr, "\n");
}

/* VARARGS1 */

messg (s, a, b, c, d)
char *s;
long a, b, c, d;
{
	fprintf (stderr, s, a, b, c, d);
	fprintf (stderr, "\n");
}

char *groupclass ();

main ()
{
	register i;
	register char **p;
	static char *gtab [] = {
		"relcom.ads",
		"relcom.ads.comp",
		"relcom.archives",
		"relcom.archives.d",
		"relcom.bbs",
		"relcom.commerce",
		"relcom.fido.su.c-c++",
		"relcom.fido.su.general",
		"relcom.fido.su.pol",
		"relcom.infomarket.quote",
		"relcom.infomarket.talk",
		"relcom.maps",
		"relcom.mn.economics",
		"relcom.mn.faxdigest",
		"relcom.mn.laws",
		"relcom.mn.newspaper",
		"relcom.msdos",
		"relcom.music",
		"relcom.netnews",
		"relcom.netnews.big",
		"relcom.penpals",
		"relcom.politics",
		"relcom.postf.business",
		"relcom.postf.exchange",
		"relcom.postf.military",
		"relcom.postf.news",
		"relcom.postmasters",
		"relcom.sources",
		"relcom.talk",
		"relcom.teleputing",
		"relcom.terms",
		"relcom.test",
		"relcom.wtc",
		"relcom.x",
		0,
	};

	ldclasstab ();
	for (i=0; i<nctab; ++i)
		printf ("%s\t%s\t%d\n", ctab[i].gclass,
			ctab[i].gpat, ctab[i].patlen);
	for (p=gtab; *p; ++p)
		printf ("%s\t%s\n", *p, groupclass (*p));
	return (0);
}
# endif

static setvalue (clss, group)
char *clss, *group;
{
	if (nctab >= MAXC) {
		error ("class table overflow: %s %s", clss, group);
		return;
	}
	ctab[nctab].gclass = strcopy (clss);
	if (strcmp (group, "all")) {
		ctab[nctab].gpat = strcopy (group);
		ctab[nctab].patlen = strlen (group);
	} else
		ctab[nctab].gpat = 0;
	++nctab;
}

ldclasstab (file)
char *file;
{
	FILE *fd;
	char line [512];
	char *c, *p, *q;

	initclass = 1; /* 2 раза не пытаться */
	fd = f2open (SERVDIR, file, "r");
	if (! fd)
		return;
	while (getstr (fd, line)) {
		c = line;
		while (*c==' ' || *c == '\t')
			++c;
		if (*c==0 || *c=='#')
			continue;
		p = c;
		while (*p && *p != ' ' && *p != '\t')
			++p;
		if (! *p)
			continue;
		*p++ = 0;
		for (;;) {
			while (*p == ' ' || *p == '\t')
				++p;
			if (! *p)
				break;
			q = p;
			while (*p && *p != ' ' && *p != '\t')
				++p;
			if (! *p) {
				setvalue (c, q);
				break;
			}
			*p++ = 0;
			setvalue (c, q);
		}
	}
	fclose (fd);
}

char *groupiclass (indx)
{
	if (indx < 0)
		return (MYADDRESS);
	return (ctab[indx].gclass);
}

char *groupclass (gname)
char *gname;
{
	register i, len;
	register struct ctab *p;

	if (! initclass) {
		ldclasstab ("classes");
	}
	len = strlen (gname);
	for (i=0, p=ctab; i<nctab; ++i, ++p) {
		if (! p->gpat)                /* pattern "all" */
			return (p->gclass);
		if (! strncmp (gname, p->gpat, p->patlen) &&
		    (len==p->patlen || gname[p->patlen]=='.'))
			return (p->gclass);
	}
	return (MYADDRESS);
}

int groupcindex (gname)
char *gname;
{
	register i, len;
	register struct ctab *p;

	if (! initclass) {
		ldclasstab ();
		initclass = 1;
	}
	len = strlen (gname);
	for (i=0, p=ctab; i<nctab; ++i, ++p) {
		if (! p->gpat)                /* pattern "all" */
			return (p - ctab);
		if (! strncmp (gname, p->gpat, p->patlen) &&
		    (len==p->patlen || gname[p->patlen]=='.'))
			return (p - ctab);
	}
	return (-1);
}
