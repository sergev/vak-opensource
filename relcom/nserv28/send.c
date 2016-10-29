/*
 * $Header: send.c,v 2.0 92/08/05 08:43:09 vak Exp $
 * $Log:	send.c,v $
 * Revision 2.0  92/08/05  08:43:09  vak
 * 
 * 
 * Revision 1.2  92/06/19  22:11:30  usenet
 * Исправлено чтение заголовков - на предмет переполнения. Руднев.
 * 
 * Revision 1.2  92/06/19  22:08:25  usenet
 * News Server
 * 
 */
# include <stdio.h>
# include "config.h"
# include "header.h"
# include "groups.h"
# include "lang.h"

# define NADDRPERCMD 30

# define TMPFNAME "/tmp/NSpXXXXXX"

static char tmpattern [] = TMPFNAME;
static char *tempf;
static FILE *inputfd, *outputfd;

extern debug;

extern char *mktemp (), *gethline (), *strcpy (), *strncpy (), *strlower ();
extern char *groupclass (), *malloc (), *canondate ();
extern long filesize ();
extern FILE *popen ();

sendarticle (g, n, nusers, users)
long g, n;
char *users;
{
	char filename [512];
	register char *p, *s;
	register c;
	FILE *fd, *fd2;
	char *gname, *gclass;
	long filsiz;

	/* Ignore "Path:", "Lines:", "Sender:", "Xref:" */
	/* Update "Subject:" */
	/* Add "To:", "Sender:", "X-Class" */

	gname = groupname (g);
	gclass = groupclass (gname);
	if (! tempf)
		/* make temp file name */
		tempf = mktemp (tmpattern);

	strcpy (filename, NEWSSPOOLDIR);
	p = filename + strlen (filename);
	*p++ = '/';
	for (s=gname; *s; ++s)
		*p++ = *s=='.' ? '/' : *s;
	sprintf (p, "/%ld", n);

	fd = fopen (filename, "r");
	if (! fd) {
		error ("cannot read %s", filename);
		return (0);
	}

	fd2 = fopen (tempf, "w");
	if (! fd2) {
		error ("cannot create %s", tempf);
		fclose (fd);
		return (0);
	}

	/* Print article to temporary file */
	fprintf (fd2, "To: %s\n", nusers>1 ? "subscribers" : users);
	copyheader (fd, fd2);
	fprintf (fd2, "Sender: %s%s\n", gclass, MYDOMAIN);
	if (FHEADLINE) {
		fprintf (fd2, FHEADLINE);
		fprintf (fd2, "\n");
	}
	fprintf (fd2, "\n");

	while ((c = getc (fd)) >= 0)
		putc (c, fd2);

	fclose (fd2);
	fclose (fd);

	run ("%s -r %s%s %s < %s", FMAILCMD, gclass, MYDOMAIN, users, tempf);
	filsiz = filesize (filename);
	if (nusers > 1)
		messg ("article %s/%ld posted, %d by %ld bytes",
			gname, n, nusers, filsiz);
	else
		messg ("article %s/%ld posted to %s, %ld bytes",
			gname, n, users, filsiz);
	outstat (gclass, users, filsiz);
	unlink (tempf);
	return (1);
}

copyheader (fd, fd2)
FILE *fd, *fd2;
{
	register char *p;
	register c, ignore;

	/* Copy article header from fd to fd2 */
	/* Ignore "Path:", "Lines:", "Sender:", "Xref:", "To:" */
	/* Update "Subject:", "Date:" */

	for (;;) {
		/* get line from stdin */
		p = gethline (fd);

		/* end of header */
		if (! p)
			break;

		if (cmpfield (p, "path") || cmpfield (p, "lines") ||
		    /* cmpfield (p, "xref") || - убрали по общей просьбе */
		    cmpfield (p, "sender") ||
		    cmpfield (p, "to"))
			ignore = 1;
		else
			ignore = 0;

		if (! ignore) {
			if (cmpfield (p, "subject")) {
				register char *q;

				q = p + 8;      /* skip Subject: */
				while (*q==' ' || *q=='\t')
					++q;    /* skip leading blanks */
				if (! strncmp (q, "[NEWS]", 6) ||
				    ! strncmp (q, "[News]", 6)) {
					q += 6;
					while (*q==' ' || *q=='\t')
						++q;    /* skip blanks */
				}
				fputs ("Subject: [News] ", fd2);
				fputs (q, fd2);
			} else if (cmpfield (p, "date")) {
				fputs ("Date: ", fd2);
				fputs (canondate (p+5), fd2);
			} else if (cmpfield (p, "xref")) {
				fputs ("X-Ref:", fd2);
				fputs ((p+5), fd2);
			} else
				fputs (p, fd2);
			putc ('\n', fd2);
		}

		/* handle multiline header fields */
		c = getc (fd);
		while (c == ' ' || c == '\t') {
			register char *p2;

			p2 = gethline (fd);
			/* Если строка пустая, это - конец заголовка */
			if ( !p2)
			{
			       c = '\n';
			       break;
			}
			if (! ignore)
				fputs (p2, fd2);
			free (p2);

			c = getc (fd);
		}
		ungetc (c, fd);

		free (p);
	}
}

cmpfield (field, pattern)
char *field, *pattern;
{
	register char *p;
	char name [80];

	for (p=field; *p>' ' && *p<0177 && *p!=':'; ++p);
	strncpy (name, field, p-field);
	name [p-field] = 0;
	strlower (name);
	return (! strcmp (name, pattern));
}

sendbatch (fd, utab, nutab, gname, gclass)
FILE *fd;
char *gname, *gclass;
long *utab;
{
	char *buf, *b, *cmd;
	int i, nusers;
	FILE *pd, *bfd, *myfd;

	buf = malloc (NADDRPERCMD * 50);
	cmd = malloc (NADDRPERCMD * 50 + 200);
	if (! buf || ! cmd) {
		error ("out of memory in sendbatch");
		quit ();
	}
	nusers = nutab;
	myfd = fd;
	/*
	 * Try to compress the batch.
	 */
	if (! tempf)
		/* make temp file name */
		tempf = mktemp (tmpattern);
	bfd = fopen (tempf, "w+");
	if (bfd) {
		unlink (tempf);
		rewind (fd);
		if (fpack (fd, bfd) >= 0 && ftell (bfd) < ftell (fd)) {
			fflush (bfd);
			myfd = bfd;
		} else {
			fclose (bfd);
			bfd = 0;
		}
	}
	while (nutab) {
		b = buf;
		for (i=0; nutab>0 && i<NADDRPERCMD; ++i) {
			strcpy (b, username (*utab++));
			--nutab;
			b += strlen (b);
			*b++ = ' ';
		}
		*--b = 0;
		sprintf (cmd, "%s -r %s%s %s", BMAILCMD, gclass,
			MYDOMAIN, buf);
		if (debug)
			printf ("    %s\n", cmd);
		pd = popen (cmd, "w");
		if (! pd) {
			error ("cannot run %s", BMAILCMD);
			if (debug)
				printf ("    *** cannot run!\n");
			break;
		}
		fprintf (pd, "From: %s%s\n", gclass, MYDOMAIN);
		fprintf (pd, "To: subscribers\n");
		fprintf (pd, "Subject: %s\n", batch_of_articles);
		fprintf (pd, "Sender: %s%s\n", gclass, MYDOMAIN);
		fprintf (pd, "Date: %s\n", gendate ());
		if (BHEADLINE) {
			fprintf (pd, BHEADLINE);
			fprintf (pd, "\n");
		}
		fprintf (pd, "X-Batch: %s\n", bfd ? "compress" : "pack");
		fprintf (pd, "\n");
		rewind (myfd);
		ffcopy (myfd, pd);
		fflush (pd);
		pclose (pd);
		outstat (gclass, buf, ftell (myfd));
	}
	free (buf);
	free (cmd);
	if (debug)
		printf ("    batch %ld bytes\n", ftell (myfd));
	messg ("batch %s (%s) posted, %d by %ld bytes",
		gname, gclass, nusers, ftell (myfd));
	if (bfd)
		fclose (bfd);
}

send1batch (fd, address, gclass)
FILE *fd;
char *gclass, *address;
{
	char cmd [200];
	FILE *pd, *bfd, *myfd;

	sprintf (cmd, "%s -r %s%s %s", BMAILCMD, gclass,
		MYDOMAIN, address);
	if (debug)
		printf ("    %s\n", cmd);
	myfd = fd;
	/*
	 * Try to compress the batch.
	 */
	if (! tempf)
		/* make temp file name */
		tempf = mktemp (tmpattern);
	bfd = fopen (tempf, "w+");
	if (bfd) {
		unlink (tempf);
		rewind (fd);
		if (fpack (fd, bfd) >= 0 && ftell (bfd) < ftell (fd)) {
			fflush (bfd);
			myfd = bfd;
		} else {
			fclose (bfd);
			bfd = 0;
		}
	}
	pd = popen (cmd, "w");
	if (! pd) {
		error ("cannot run %s", BMAILCMD);
		if (debug)
			printf ("    *** cannot run!\n");
		if (bfd)
			fclose (bfd);
		return;
	}
	fprintf (pd, "From: %s%s\n", gclass, MYDOMAIN);
	fprintf (pd, "To: %s\n", address);
	fprintf (pd, "Subject: %s\n", batch_of_articles);
	fprintf (pd, "Sender: %s%s\n", gclass, MYDOMAIN);
	fprintf (pd, "Date: %s\n", gendate ());
	if (BHEADLINE) {
		fprintf (pd, BHEADLINE);
		fprintf (pd, "\n");
	}
	fprintf (pd, "X-Batch: %s\n", bfd ? "compress" : "pack");
	fprintf (pd, "\n");
	rewind (myfd);
	ffcopy (myfd, pd);
	fflush (pd);
	pclose (pd);

	messg ("batch %s posted to %s, %ld bytes",
		gclass, address, ftell (myfd));
	outstat (gclass, address, ftell (myfd));
	if (bfd)
		fclose (bfd);
}

batcharticle (g, n, fd, gclass)
long g, n;
FILE *fd;
char *gclass;
{
	char buf [512];
	register char *p, *s;
	FILE *afd;

	strcpy (buf, NEWSSPOOLDIR);
	p = buf + strlen (buf);
	*p++ = '/';
	for (s=groupname(g); *s; ++s)
		*p++ = *s=='.' ? '/' : *s;
	sprintf (p, "/%ld", n);

	afd = fopen (buf, "r");
	if (! afd)
		return;

	/* Print article to temporary file */
	fprintf (fd, "To: subscribers\n");
	copyheader (afd, fd);
	fprintf (fd, "Sender: %s%s\n", gclass, MYDOMAIN);
	fprintf (fd, "\n");

	while (fgets (buf, sizeof (buf), afd)) {
		if (!strncmp (buf, "From_", 5) || !strncmp (buf, "From ", 5))
			putc ('>', fd);
		fputs (buf, fd);
	}
	fclose (afd);
}

static get ()
{
	return (getc (inputfd));
}

static void put (c)
{
	putc (c, outputfd);
}

fpack (ifd, ofd)
FILE *ifd, *ofd;
{
	inputfd = ifd;
	outputfd = ofd;
	return (compack (2, get, put));
}

/*
 * Check if the article does exist.
 */
existarticle (g, n)
long g, n;
{
	char filename [512];
	register char *p, *s;

	/* Return 1 if article #n of group g does exist */

	strcpy (filename, NEWSSPOOLDIR);
	p = filename + strlen (filename);
	*p++ = '/';
	for (s=groupname(g); *s; ++s)
		*p++ = *s=='.' ? '/' : *s;
	sprintf (p, "/%ld", n);

	return (! access (filename, 4));        /* ok if we can read it */
}
